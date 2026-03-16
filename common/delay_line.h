// Copyright 2014 Emilie Gillet.
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
// Delay line (Fork with dynamic allocation).

#ifndef COMMON_DELAY_LINE_H_
#define COMMON_DELAY_LINE_H_

#include <algorithm>

#include "eurorack/stmlib/dsp/dsp.h"
#include "eurorack/stmlib/stmlib.h"
#include "eurorack/stmlib/utils/buffer_allocator.h"

namespace common {

class DelayLine {
 public:
  DelayLine() { }
  ~DelayLine() { }
  
  void Init(stmlib::BufferAllocator* allocator, size_t size) {
    line_ = allocator->Allocate<float>(size);
    size_ = size;
    Reset();
  }

  void Reset() {
    if (line_) {
      std::fill(&line_[0], &line_[size_], 0.0f);
    }
    delay_ = 1;
    write_ptr_ = 0;
  }
  
  inline void set_delay(size_t delay) {
    delay_ = delay;
  }

  inline void Write(const float sample) {
    if (!line_) return;
    line_[write_ptr_] = sample;
    write_ptr_ = (write_ptr_ - 1 + size_) % size_;
  }
  
  inline const float Allpass(const float sample, size_t delay, const float coefficient) {
    if (!line_) return sample;
    float read = line_[(write_ptr_ + delay) % size_];
    float write = sample + coefficient * read;
    Write(write);
    return -write * coefficient + read;
  }

  inline const float WriteRead(const float sample, float delay) {
    Write(sample);
    return Read(delay);
  }
  
  inline const float Read() const {
    if (!line_) return 0.0f;
    return line_[(write_ptr_ + delay_) % size_];
  }
  
  inline const float Read(size_t delay) const {
    if (!line_) return 0.0f;
    return line_[(write_ptr_ + delay) % size_];
  }

  inline const float Read(float delay) const {
    if (!line_) return 0.0f;
    MAKE_INTEGRAL_FRACTIONAL(delay)
    const float a = line_[(write_ptr_ + delay_integral) % size_];
    const float b = line_[(write_ptr_ + delay_integral + 1) % size_];
    return a + (b - a) * delay_fractional;
  }
  
  inline const float ReadHermite(float delay) const {
    if (!line_) return 0.0f;
    MAKE_INTEGRAL_FRACTIONAL(delay)
    int32_t t = (write_ptr_ + delay_integral + size_);
    const float xm1 = line_[(t - 1) % size_];
    const float x0 = line_[(t) % size_];
    const float x1 = line_[(t + 1) % size_];
    const float x2 = line_[(t + 2) % size_];
    const float c = (x1 - xm1) * 0.5f;
    const float v = x0 - x1;
    const float w = c + v;
    const float a = w + v + (x2 - x0) * 0.5f;
    const float b_neg = w + a;
    const float f = delay_fractional;
    return (((a * f) - b_neg) * f + c) * f + x0;
  }

 private:
  size_t write_ptr_;
  size_t delay_;
  size_t size_;
  float* line_;
  
  DISALLOW_COPY_AND_ASSIGN(DelayLine);
};

}  // namespace common

#endif  // COMMON_DELAY_LINE_H_
