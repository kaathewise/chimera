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

#ifndef AUDREY_ECHO_DELAY_H_
#define AUDREY_ECHO_DELAY_H_

#include <daisysp.h>

#include "audrey/biquad_filters.h"
#include "audrey/dsp_utils.h"

namespace audrey {

/**
 * @brief
 * Tape-ish echo delay.
 *   - Feedback is unbounded, but signal is soft-clipped
 *   - Output is full-wet, should be mixed with dry signal externally
 *
 * @tparam MaxLength Max length of delay in samples
 */
template <size_t MaxLength>
class EchoDelay {
 public:
  EchoDelay() {}
  ~EchoDelay() {}

  void Init(float sample_rate) {
    sample_rate_ = sample_rate;
    delayLine_.Init();
    bpf_.Init(sample_rate);
    bpf_.SetParams(800.0f, 1.0f);
  }

  /**
   * @brief Set the approximate lag time (smoothing) for delay time changes, in
   * seconds
   */
  void SetLagTime(const float time_s) {
    delay_smooth_coef_ = onepole_coef(time_s, sample_rate_);
  }

  /**
   * @brief Set the Delay Time in seconds
   *
   * @param time_s Delay time in seconds. Will be truncated to MaxLength.
   * @param immediately If true, sets delay time immediately with no smoothing.
   */
  void SetDelayTime(const float time_s, bool immediately = false) {
    delay_time_target_ = time_s;
    if (immediately) delay_time_current_ = time_s;
  }

  /**
   * @brief
   * Set the feedback amount (linear multiplier).
   * This can be >1 in magnitude for saturated swells, or negative.
   *
   * NOTE: This is not internally smoothed. Use external smoothing if desired.
   *
   * @param feedback
   */
  void SetFeedback(const float feedback) { feedback_ = feedback; }

  inline float Process(const float in) {
    float out;
    daisysp::fonepole(delay_time_current_, delay_time_target_,
                      delay_smooth_coef_);
    delayLine_.SetDelay(delay_time_current_ * sample_rate_);
    out = delayLine_.Read();
    out = bpf_.Process(out);
    out = daisysp::SoftClip(out);
    delayLine_.Write(out * feedback_ + in);
    return out;
  }

 private:
  EchoDelay(const EchoDelay &other) = delete;
  EchoDelay(EchoDelay &&other) = delete;
  EchoDelay &operator=(const EchoDelay &other) = delete;
  EchoDelay &operator=(EchoDelay &&other) = delete;

  float sample_rate_;
  float delay_time_current_;
  float delay_time_target_;
  float delay_smooth_coef_;

  float feedback_;

  daisysp::DelayLine<float, MaxLength> delayLine_;
  BPF12 bpf_;
};

}  // namespace audrey

#endif  // AUDREY_ECHO_DELAY_H_
