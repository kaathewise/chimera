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
    strings_[i] = KarplusStringPtr(
        simpletouch::SDRAM::allocate<KarplusString>());
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

void Engine::Process(const EngineParameters &params, float in, float &outL,
                     float &outR) {
  // Update internal parameters from struct
  const auto freq = mtof(params.string_pitch);
  strings_[0]->SetFreq(freq);
  strings_[1]->SetFreq(freq);

  fb_lpf_.SetCutoff(params.feedback_lpf_cutoff);
  fb_hpf_.SetCutoff(params.feedback_hpf_cutoff);

  echo_delay_[0]->SetDelayTime(params.echo_delay_time);
  echo_delay_[1]->SetDelayTime(params.echo_delay_time);
  echo_delay_[0]->SetFeedback(params.echo_delay_feedback);
  echo_delay_[1]->SetFeedback(params.echo_delay_feedback);

  verb_->SetFeedback(params.reverb_feedback);

  _env.SetShape(params.envelope_shape);

  static bool prev_note_on = false;
  if (params.note_on && !prev_note_on) {
    _env.Trigger();
  } else if (!params.note_on && prev_note_on) {
    _env.Release();
  }
  prev_note_on = params.note_on;

  // --- Update audio-rate-smoothed control params ---

  fonepole(fb_delay_samp_, params.feedback_delay, fb_delay_smooth_coef_);

  // --- Process Samples ---

  float inL, inR, sampL, sampR, echoL, echoR, verbL, verbR;
  const float noise_samp = noise_.Process();
  auto env = _env.Process();

  // ---> Feedback Loop

  // Get noise + feedback output
  inL =
      fb_delayline_[0]->Read(fb_delay_samp_) + noise_samp + (in * params.input_level);
  inR = fb_delayline_[1]->Read(daisysp::fmax(1.0f, fb_delay_samp_ - 4.f)) +
        noise_samp + (in * params.input_level);

  // Process through KS resonator
  sampL = strings_[0]->Process(inL);
  sampR = strings_[1]->Process(inR);

  // Distort + Clip
  sampL = overdrive_[0].Process(sampL);
  sampR = overdrive_[1].Process(sampR);

  // Filter in feedback loop
  fb_lpf_.ProcessStereo(sampL, sampR);
  fb_hpf_.ProcessStereo(sampL, sampR);

  // ---> Reverb

  verb_->Process(sampL, sampR, &verbL, &verbR);

  sampL -= (sampL - verbL) * params.reverb_mix;
  sampR -= (sampR - verbR) * params.reverb_mix;

  // ---> Resonator feedback

  // Write back into delay with attenuation
  fb_delayline_[0]->Write(sampL * (params.feedback_gain * (params.drone_mode ? 1.0f : env)));
  fb_delayline_[1]->Write(sampR * (params.feedback_gain * (params.drone_mode ? 1.0f : env)));

  // ---> Echo Delay

  echoL = echo_delay_[0]->Process(sampL * params.echo_delay_send_amount);
  echoR = echo_delay_[1]->Process(sampR * params.echo_delay_send_amount);

  sampL = 0.5f * (sampL + echoL);
  sampR = 0.5f * (sampR + echoR);

  // ---> Output
  outL = sampL * params.output_level;
  outR = sampR * params.output_level;
}

}  // namespace audrey
