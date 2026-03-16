// Copyright 2026 Svyatoslav Usachev (kaathewise@gmail.com)
//
// Based on: https://github.com/pichenettes/eurorack/tree/master/plaits
// Original Copyright 2016 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
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
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Main synthesis voice.
#include "voice/voice.h"

#include "eurorack/plaits/dsp/dsp.h"
#include "eurorack/plaits/dsp/engine/particle_engine.h"
#include "eurorack/stmlib/dsp/units.h"

namespace voice {

void Voice::Init(float sample_rate, stmlib::BufferAllocator* allocator, float max_delay_time) {
  sample_rate_ = sample_rate;
  engine_.post_processing_settings.already_enveloped = false;
  engine_.post_processing_settings.out_gain = -2.0f;

  decay_envelope_.Init();
  lpg_envelope_.Init();
  post_processor_.Init();
  delay_line_.Init(allocator, sample_rate_ * max_delay_time);
}

void Voice::Process(const plaits::EngineParameters& parameters,
                    float delay_time, float delay_feedback,
                    float* out, size_t size) {
  bool already_enveloped = engine_.post_processing_settings.already_enveloped;

  if (parameters.trigger & plaits::TRIGGER_RISING_EDGE) {
    decay_envelope_.Trigger();
    lpg_envelope_.Trigger();
  }

  engine_.Render(parameters, out, aux_buffer, size, &already_enveloped);

  float decay_value = parameters.accent;
  const float short_decay = (200.0f * static_cast<float>(size)) /
                            plaits::kSampleRate *
                            stmlib::SemitonesToRatio(-96.0f * decay_value);

  decay_envelope_.Process(short_decay * 2.0f);

  const float hf = 0.5f;
  const float decay_tail =
      (20.0f * static_cast<float>(size)) / plaits::kSampleRate *
          stmlib::SemitonesToRatio(-72.0f * decay_value + 12.0f * hf) -
      short_decay;

  const float attack = plaits::NoteToFrequency(parameters.note) *
                       static_cast<float>(size) * 2.0f;
  lpg_envelope_.ProcessPing(attack, short_decay, decay_tail, hf);

  const plaits::PostProcessingSettings& pp_s = engine_.post_processing_settings;
  bool lpg_bypass = already_enveloped;

  post_processor_.Process(pp_s.out_gain, lpg_bypass, lpg_envelope_.gain(),
                               lpg_envelope_.frequency(),
                               lpg_envelope_.hf_bleed(), out, size);

  float delay_time_samples = delay_time * sample_rate_;
  for (size_t i = 0; i < size; ++i) {
    out[i] += delay_line_.Read(delay_time_samples) * delay_feedback;
    delay_line_.Write(out[i]);
  }
}

}  // namespace voice
