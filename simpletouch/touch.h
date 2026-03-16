#ifndef SIMPLETOUCH_TOUCH_H
#define SIMPLETOUCH_TOUCH_H

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"
#include "simpletouch/knobs.h"
#include "simpletouch/led_ui.h"
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
    ui_.Init(hw);

    hw.adc.Start();
  }

  Pads &pads() { return pads_; }

  Knobs &knobs() { return knobs_; }

  Switches &switches() { return switches_; }

  LedUI &led() { return ui_; }

 private:
  Knobs knobs_;
  Pads pads_;
  Switches switches_;
  LedUI ui_;

  DISALLOW_COPY_AND_ASSIGN(Touch);
};
}  // namespace simpletouch

#endif
