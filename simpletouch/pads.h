#ifndef SIMPLETOUCH_PADS_H
#define SIMPLETOUCH_PADS_H

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"

namespace simpletouch {
using namespace daisy;

class Pads {
 public:
  Pads() = default;

  ~Pads() = default;

  void Init() { mpr_.Init(Mpr121I2C::Config()); }

  void Process() {
    prev_state_ = state_;
    state_ = mpr_.Touched();
  }

  bool IsTouched(uint8_t pad) const { return state_ & (1 << pad); }

  uint16_t GetValue(uint8_t pad) { return mpr_.FilteredData(pad); }

  uint16_t GetBaseline(uint8_t pad) { return mpr_.BaselineData(pad); }

  bool HasTouch() const { return state_ > 0; }

  bool IsRisingEdge(uint8_t pad) const {
    return state_ & ~prev_state_ & (1 << pad);
  }

  bool IsFallingEdge(uint8_t pad) const {
    return ~state_ & prev_state_ & (1 << pad);
  }

  uint16_t Touched() const { return state_; }

 private:
  uint16_t state_{0}, prev_state_{0};
  Mpr121I2C mpr_;

  DISALLOW_COPY_AND_ASSIGN(Pads);
};
}  // namespace simpletouch

#endif
