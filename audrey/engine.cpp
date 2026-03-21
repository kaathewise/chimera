// Copyright (c) 2022 Infrasonic Audio, LLC
// Ported from https://github.com/Synthux-Academy/AudreyTouch
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "audrey/engine.h"

#include "audrey/dsp_utils.h"
#include "simpletouch/memory/sdram_alloc.h"

namespace audrey {

using daisysp::fclamp;
using daisysp::fonepole;
using daisysp::mtof;
using daisysp::ReverbSc;

void Engine::Init(const float sample_rate) {
  using ED = EchoDelay<kMaxEchoDelaySamp>;
  using FB_DL = daisysp::DelayLine<float, kMaxFeedbackDelaySamp>;

  _env.Init(sample_rate);
  _env.SetMode(Envelope::Mode::ASR);
  _env.SetShape(0.0f);

  echo_delay_[0] = EchoDelayPtr(simpletouch::SDRAM::allocate<ED>());
  echo_delay_[1] = EchoDelayPtr(simpletouch::SDRAM::allocate<ED>());
  verb_ = VerbPtr(simpletouch::SDRAM::allocate<ReverbSc>());

  for (int i = 0; i < 2; i++) {
    strings_[i] =
        KarplusStringPtr(simpletouch::SDRAM::allocate<KarplusString>());
    fb_delayline_[i] =
        FeedbackDelayLinePtr(simpletouch::SDRAM::allocate<FB_DL>());
  }

  sample_rate_ = sample_rate;
  fb_delay_smooth_coef_ = onepole_coef(0.2f, sample_rate);

  noise_.Init();
  noise_.SetAmp(dbfs2lin(-90.0f));

  for (unsigned int i = 0; i < 2; i++) {
    strings_[i]->Init(sample_rate);
    strings_[i]->SetBrightness(0.98f);
    strings_[i]->SetFreq(mtof(40.0f));
    strings_[i]->SetDamping(0.4f);

    fb_delayline_[i]->Init();

    echo_delay_[i]->Init(sample_rate);
    echo_delay_[i]->SetDelayTime(5.0f, true);
    echo_delay_[i]->SetFeedback(0.5f);
    echo_delay_[i]->SetLagTime(0.5f);

    overdrive_[i].Init();
    overdrive_[i].SetDrive(0.4);
  }

  verb_->Init(sample_rate);
  verb_->SetFeedback(0.85f);
  verb_->SetLpFreq(12000.0f);

  fb_lpf_.Init(sample_rate);
  fb_lpf_.SetQ(0.9f);
  fb_lpf_.SetCutoff(18000.0f);

  fb_hpf_.Init(sample_rate);
  fb_hpf_.SetQ(0.9f);
  fb_hpf_.SetCutoff(60.f);
}

void Engine::Process(EngineParameters params, float in, float &outL,
                     float &outR) {
  // --- Update audio-rate-smoothed control params ---

  const float fb_delay_samp_target =
      DSY_CLAMP(params.feedback_delay * sample_rate_, 1.0f,
                static_cast<float>(kMaxFeedbackDelaySamp - 1));

  fonepole(fb_delay_samp_, fb_delay_samp_target, fb_delay_smooth_coef_);

  // --- Process Samples ---

  float inL, inR, sampL, sampR, echoL, echoR, verbL, verbR;
  const float noise_samp = noise_.Process();

  // ---> Feedback Loop

  // Get noise + feedback output
  inL = fb_delayline_[0]->Read(fb_delay_samp_) + noise_samp +
        (in * params.input_level);
  inR = fb_delayline_[1]->Read(daisysp::fmax(1.0f, fb_delay_samp_ - 4.f)) +
        noise_samp + (in * params.input_level);

  // Process through KS resonator
  const float freq = mtof(params.string_pitch);
  strings_[0]->SetFreq(freq);
  strings_[1]->SetFreq(freq);

  sampL = strings_[0]->Process(inL);
  sampR = strings_[1]->Process(inR);

  // Distort + Clip
  sampL = overdrive_[0].Process(sampL);
  sampR = overdrive_[1].Process(sampR);

  // Filter in feedback loop
  fb_lpf_.SetCutoff(params.feedback_lpf_cutoff);
  fb_hpf_.SetCutoff(params.feedback_hpf_cutoff);

  fb_lpf_.ProcessStereo(sampL, sampR);
  fb_hpf_.ProcessStereo(sampL, sampR);

  // ---> Reverb
  verb_->SetFeedback(params.reverb_feedback);
  verb_->Process(sampL, sampR, &verbL, &verbR);

  const float verb_mix = fclamp(params.reverb_mix, 0.0f, 1.0f);
  //       (sampL * (1.0f - verb_mix_)) + verbL * verb_mix_;
  //       sampL - sampL * verb_mix + verbL * verb_mix_;
  sampL -= (sampL - verbL) * verb_mix;
  sampR -= (sampR - verbR) * verb_mix;

  // ---> Resonator feedback

  const float fb_gain = dbfs2lin(params.feedback_gain);

  // Write back into delay with attenuation
  if (params.trigger == TriggerState::kRisingEdge) {
    _env.Trigger();
  } else if (params.trigger == TriggerState::kFallingEdge) {
    _env.Release();
  }
  _env.SetShape(params.shape);
  const float env = _env.Process();

  fb_delayline_[0]->Write(sampL * (fb_gain * (params.drone_mode ? 1.0f : env)));
  fb_delayline_[1]->Write(sampR * (fb_gain * (params.drone_mode ? 1.0f : env)));

  // ---> Echo Delay

  echo_delay_[0]->SetDelayTime(params.echo_delay_time);
  echo_delay_[1]->SetDelayTime(params.echo_delay_time);

  echo_delay_[0]->SetFeedback(params.echo_delay_feedback);
  echo_delay_[1]->SetFeedback(params.echo_delay_feedback);

  echoL = echo_delay_[0]->Process(sampL * params.echo_delay_send_amount);
  echoR = echo_delay_[1]->Process(sampR * params.echo_delay_send_amount);

  sampL = 0.5f * (sampL + echoL);
  sampR = 0.5f * (sampR + echoR);

  // ---> Output
  outL += sampL * params.output_level;
  outR += sampR * params.output_level;
}
}  // namespace audrey
