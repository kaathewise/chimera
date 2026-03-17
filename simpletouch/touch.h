#ifndef SIMPLETOUCH_TOUCH_H_
#define SIMPLETOUCH_TOUCH_H_

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"
#include "simpletouch/knobs.h"
#include "simpletouch/led.h"
#include "simpletouch/pads.h"
#include "simpletouch/switches.h"

namespace simpletouch {

using daisy::DaisySeed;

class Touch {
 public:
  Touch() = default;

  ~Touch() = default;

  void Init(DaisySeed &hw) {
    pads_.Init();
    knobs_.Init(hw);
    switches_.Init();
    led_.Init(hw);

    hw.adc.Start();
  }

  void Process() { led_.Process(); }

  Pads &pads() { return pads_; }

  Knobs &knobs() { return knobs_; }

  Switches &switches() { return switches_; }

  Led &led() { return led_; }

 private:
  Knobs knobs_;
  Pads pads_;
  Switches switches_;
  Led led_;

  DISALLOW_COPY_AND_ASSIGN(Touch);
};
}  // namespace simpletouch

#endif  // SIMPLETOUCH_TOUCH_H_
