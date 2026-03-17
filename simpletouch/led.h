//
// Created by Svyatoslav Usachev on 16/03/2026.
//

#ifndef SIMPLETOUCH_LED_H_
#define SIMPLETOUCH_LED_H_

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"

namespace simpletouch {

using daisy::DaisySeed;

class Led {
 public:
  Led() = default;

  ~Led() = default;

  void Init(DaisySeed &hw) { hw_ = &hw; }

  void Blink() {
    hw_->SetLed(true);
    timer_ = static_cast<uint16_t>(.25f * hw_->AudioCallbackRate());
  }

  void Process() {
    if (timer_ > 0) {
      timer_--;
      if (timer_ == 0) {
        hw_->SetLed(false);
      }
    }
  }

 private:
  uint16_t timer_{0};

  DaisySeed *hw_;

  DISALLOW_COPY_AND_ASSIGN(Led);
};

}  // namespace simpletouch

#endif  // SIMPLETOUCH_LED_H_
