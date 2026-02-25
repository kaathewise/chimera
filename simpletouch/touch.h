#ifndef SIMPLETOUCH_TOUCH_H
#define SIMPLETOUCH_TOUCH_H

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"
#include "simpletouch/knobs.h"
#include "simpletouch/pads.h"
#include "simpletouch/switches.h"

namespace simpletouch {
using namespace daisy;

class Touch {
 public:
  Touch() = default;

  ~Touch() = default;

  void Init(DaisySeed &hw) {
    pads_.Init();
    knobs_.Init(hw);
    switches_.Init();

    hw.adc.Start();
  }

  Pads &pads() { return pads_; }

  Knobs &knobs() { return knobs_; }

  Switches &switches() { return switches_; }

 private:
  Knobs knobs_;
  Pads pads_;
  Switches switches_;

  DISALLOW_COPY_AND_ASSIGN(Touch);
};
}  // namespace simpletouch

#endif
