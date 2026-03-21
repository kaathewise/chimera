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

#ifndef AUDREY_DSP_UTILS_H_
#define AUDREY_DSP_UTILS_H_

#include <Utility/dsp.h>

#include <cmath>
#ifdef __arm__
#include <arm_math.h>
#endif  // __arm__

namespace audrey {

inline float dbfs2lin(float dbfs) { return daisysp::pow10f(dbfs * 0.05f); }

inline float lin2dbfs(float lin) { return daisysp::fastlog10f(lin) * 20.0f; }

// Coefficient for one pole smoothing filter based on Tau time constant for
// `time_s`
inline float onepole_coef(float time_s, float sample_rate) {
  if (time_s <= 0.0f || sample_rate <= 0.0f) {
    return 1.0f;
  }
  return daisysp::fmin(1.0f / (time_s * sample_rate), 1.0f);
}

inline float onepole_coef_t60(float time_s, float sample_rate) {
  return onepole_coef(time_s * 0.1447597f, sample_rate);
}

inline float ftension(const float in, const float factor) {
  if (factor == 0.0f) return in;
  const float denom = expm1f(factor);
  return expm1f(in * factor) / denom;
}

inline float tanf(const float x) {
#ifdef __arm__
  return std::tan(x);
  // float s, c;
  // arm_sin_cos_f32(x, &s, &c);
  // return s / c;
#else
  return std::tanf(x);
#endif  // __arm__
}

}  // namespace audrey

#endif  // AUDREY_DSP_UTILS_H_
