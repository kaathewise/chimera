// Copyright (c) 2022 Infrasonic Audio, LLC
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

#ifndef AUDREY_ENGINE_H_
#define AUDREY_ENGINE_H_

#include <daisysp.h>

#include <memory>

#include "audrey/biquad_filters.h"
#include "audrey/echo_delay.h"
#include "audrey/env.h"
#include "audrey/karplus_string.h"

namespace audrey {

enum class TriggerState { kUnknown, kRisingEdge, kFallingEdge };

struct EngineParameters {
  float string_pitch;
  float feedback_gain;
  float feedback_delay;
  float feedback_lpf_cutoff;
  float feedback_hpf_cutoff;
  float echo_delay_time;
  float echo_delay_feedback;
  float echo_delay_send_amount;
  float reverb_mix;
  float reverb_feedback;
  float output_level;
  float input_level;
  float shape;
  bool drone_mode;
  TriggerState trigger;
};

class Engine {
 public:
  Engine() = default;
  ~Engine() = default;

  void Init(float sample_rate);

  void Process(EngineParameters params, float in, float &outL, float &outR);

 private:
  // long enough for 250ms at 48kHz
  static constexpr size_t kMaxFeedbackDelaySamp = 12000;
  // long enough for 5s at 48kHz
  static constexpr size_t kMaxEchoDelaySamp = 48000 * 5;

  float sample_rate_;

  float fb_delay_smooth_coef_;
  float fb_delay_samp_ = 1000.f;

  using KarplusStringPtr = std::unique_ptr<KarplusString>;
  KarplusStringPtr strings_[2];
  daisysp::WhiteNoise noise_;

  using FeedbackDelayLinePtr =
      std::unique_ptr<daisysp::DelayLine<float, kMaxFeedbackDelaySamp>>;
  FeedbackDelayLinePtr fb_delayline_[2];

  daisysp::Overdrive overdrive_[2];
  Envelope _env;

  LPF12 fb_lpf_;
  HPF12 fb_hpf_;

  using VerbPtr = std::unique_ptr<daisysp::ReverbSc>;
  VerbPtr verb_;

  using EchoDelayPtr = std::unique_ptr<EchoDelay<kMaxEchoDelaySamp>>;
  EchoDelayPtr echo_delay_[2];

  Engine(const Engine &other) = delete;
  Engine(Engine &&other) = delete;
  Engine &operator=(const Engine &other) = delete;
  Engine &operator=(Engine &&other) = delete;
};

}  // namespace audrey

#endif  // AUDREY_ENGINE_H_
