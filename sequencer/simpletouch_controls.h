#ifndef SEQUENCER_SIMPLETOUCH_CONTROLS_H_
#define SEQUENCER_SIMPLETOUCH_CONTROLS_H_

#include <cmath>

#include "eurorack/stmlib/stmlib.h"
#include "simpletouch/control_value.h"
#include "simpletouch/touch.h"

namespace sequencer {

using simpletouch::ControlValue;
using simpletouch::Touch;

class SimpleTouchControls {
 public:
  explicit SimpleTouchControls(Touch& touch)
      : touch_(touch),
        deja_vu_(touch, .5f),
        rate_(touch, .5f),
        bias_(touch, .5f),
        jitter_(touch, .0f),
        loop_length_(touch, .5f) {}

  void Process();

  float deja_vu() const {
    return fabs(deja_vu_.Value() - .5f) < .05f ? .5f : deja_vu_.Value();
  }

  float rate() const { return exp2f(rate_.Value() * 10.0f) / 10.0f; }

  float bias() const { return bias_.Value(); }

  float jitter() const { return jitter_.Value(); }

  int loop_length() const {
    return static_cast<int>(loop_length_.Value() * 15.0f) + 1;
  }

  void Attach() {
    deja_vu_.Attach();
    rate_.Attach();
    bias_.Attach();
    jitter_.Attach();
    loop_length_.Attach();
  }

  void Detach() {
    deja_vu_.Detach();
    rate_.Detach();
    bias_.Detach();
    jitter_.Detach();
    loop_length_.Detach();
  }

 private:
  Touch& touch_;

  ControlValue deja_vu_;
  ControlValue rate_;
  ControlValue bias_;
  ControlValue jitter_;
  ControlValue loop_length_;

  DISALLOW_COPY_AND_ASSIGN(SimpleTouchControls);
};

}  // namespace sequencer

#endif  // SEQUENCER_SIMPLETOUCH_CONTROLS_H_
