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

#ifndef VOICE_VOICE_H
#define VOICE_VOICE_H

#include "eurorack/plaits/dsp/engine/engine.h"
#include "eurorack/plaits/dsp/envelope.h"
#include "eurorack/plaits/dsp/fx/low_pass_gate.h"
#include "eurorack/stmlib/dsp/limiter.h"
#include "eurorack/stmlib/stmlib.h"

namespace voice {

class ChannelPostProcessor {
 public:
  ChannelPostProcessor() = default;
  ~ChannelPostProcessor() = default;

  void Init() {
    lpg_.Init();
    Reset();
  }

  void Reset() { limiter_.Init(); }

  void Process(float gain, bool bypass_lpg, float low_pass_gate_gain,
               float low_pass_gate_frequency, float low_pass_gate_hf_bleed,
               float* in_out, size_t size) {
    if (gain < 0.0f) {
      limiter_.Process(-gain, in_out, size);
    }
    const float post_gain = (gain < 0.0f ? 1.0f : gain);
    if (!bypass_lpg) {
      lpg_.Process(post_gain * low_pass_gate_gain, low_pass_gate_frequency,
                   low_pass_gate_hf_bleed, in_out, size);
    } else {
      for (size_t i = 0; i < size; ++i) {
        in_out[i] *= post_gain;
      }
    }
  }

 private:
  stmlib::Limiter limiter_;
  plaits::LowPassGate lpg_;

  DISALLOW_COPY_AND_ASSIGN(ChannelPostProcessor);
};

class Voice {
 public:
  Voice(plaits::Engine& engine) : engine_(engine) {}

  ~Voice() = default;

  void Init();

  void Process(const plaits::EngineParameters& parameters, float** out,
               size_t size);

 private:
  plaits::Engine& engine_;

  plaits::DecayEnvelope decay_envelope_;
  plaits::LPGEnvelope lpg_envelope_;

  ChannelPostProcessor post_processor_[2];

  DISALLOW_COPY_AND_ASSIGN(Voice);
};
}  // namespace voice
#endif  // VOICE_VOICE_H
