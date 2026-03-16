//
// Created by Svyatoslav Usachev on 16/03/2026.
//

#ifndef SIMPLETOUCH_LED_H
#define SIMPLETOUCH_LED_H

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"

namespace simpletouch {
using namespace daisy;

class LedUI {
public:
  LedUI() = default;

  ~LedUI() = default;

  void Init(DaisySeed &hw) {
    hw_ = &hw;
  }

  void Blink() {
    hw_->SetLed(true);
    turn_off_at_ = System::GetNow() + 500;
  }

  void Process() {
    if (turn_off_at_ > 0) {
      if (System::GetNow() > turn_off_at_) {
        hw_->SetLed(false);
        turn_off_at_ = -1;
      }
    }
  }

private:
  float turn_off_at_ {-1};

  DaisySeed *hw_;

  DISALLOW_COPY_AND_ASSIGN(LedUI);
};

}

#endif  // CHIMERA_LED_H
