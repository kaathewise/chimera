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

#ifndef AUDREY_KARPLUS_STRING_H_
#define AUDREY_KARPLUS_STRING_H_

#include <daisysp.h>

#include "audrey/dc_block.h"

namespace audrey {
/**
 * Modified version of KarplusString class from DaisySP:
 *  - Increase delay line length for very low pitches
 *  - Remove nonlinearity processing (not needed for feedback synth)
 *  - Re-namespaced to infrasonic
 *
 *  Original code licensed as MIT Copyright (c) 2020 Electrosmith, Corp.
 *  (see DaisySP/LICENSE)
 */
class KarplusString {
 public:
  KarplusString() {}
  ~KarplusString() {}

  /** Initialize the module.
      \param sample_rate Audio engine sample rate
  */
  void Init(float sample_rate);

  /** Clear the delay line */
  void Reset();

  /** Get the next floating point sample
      \param in Signal to excite the string.
  */
  float Process(float in);

  /** Set the string frequency.
      \param freq Frequency in Hz
  */
  void SetFreq(float freq);

  /** Set the string's overall brightness
      \param Works 0-1.
  */
  void SetBrightness(float brightness);

  /** Set the string's decay time.
      \param damping Works 0-1.
  */
  void SetDamping(float damping);

 private:
  static constexpr size_t kDelayLineSize = 8192;

  float ProcessInternal(float in);

  daisysp::DelayLine<float, kDelayLineSize> string_;

  float frequency_, brightness_, damping_;

  float sample_rate_;

  daisysp::Tone iir_damping_filter_;
  audrey::DCBlock dc_blocker_;
  daisysp::CrossFade crossfade_;

  // Very crappy linear interpolation upsampler used for low pitches that
  // do not fit the delay line. Rarely used.
  float src_phase_;
  float out_sample_[2];
};
}  // namespace audrey

#endif  // AUDREY_KARPLUS_STRING_H_
