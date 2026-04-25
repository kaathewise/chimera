#ifndef SIMPLE_TOUCH_TOUCH_H_
#define SIMPLE_TOUCH_TOUCH_H_

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"
#include "simple_touch/knobs.h"
#include "simple_touch/led.h"
#include "simple_touch/pads.h"
#include "simple_touch/switches.h"

namespace simple_touch {

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
}  // namespace simple_touch

#endif  // SIMPLE_TOUCH_TOUCH_H_
