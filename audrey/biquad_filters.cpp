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

#include "audrey/biquad_filters.h"

#include <cmath>

#include "audrey/dsp_utils.h"

namespace audrey {

const BiquadSection::Coefficients BiquadSection::CalculateCoefficients(
    const FilterType type, const float sample_rate, const float cutoff_hz,
    const float q) {
  Coefficients coefs;
  float norm;
  const float K = audrey::tanf(PI_F * cutoff_hz * (1.0f / sample_rate));
  const float Ksq = K * K;

  float &b0 = coefs[0];
  float &b1 = coefs[1];
  float &b2 = coefs[2];
  float &a1 = coefs[3];
  float &a2 = coefs[4];

  switch (type) {
    case FilterType::HighPass:
      norm = 1.0f / (1.0f + (K / q) + Ksq);
      b0 = norm;
      b1 = -2.0f * b0;
      b2 = b0;
      a1 = 2.0f * (Ksq - 1.0f) * norm;
      a2 = (1.0f - (K / q) + Ksq) * norm;
      break;

    case FilterType::BandPass:
      norm = 1.0f / (1.0f + (K / q) + Ksq);
      b0 = (K / q) * norm;
      b1 = 0.0f;
      b2 = -b0;
      a1 = 2.0f * (Ksq - 1) * norm;
      a2 = (1.0f - (K / q) + Ksq) * norm;
      break;

    default:  // lowpass
      norm = 1.0f / (1.0f + (K / q) + Ksq);
      b0 = Ksq * norm;
      b1 = 2.0f * b0;
      b2 = b0;
      a1 = 2.0f * (Ksq - 1.0f) * norm;
      a2 = (1.0f - (K / q) + Ksq) * norm;
      break;
  }

  return coefs;
}

}  // namespace audrey
