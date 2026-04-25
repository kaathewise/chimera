#ifndef SEQUENCER_SIMPLE_TOUCH_CONTROLS_H_
#define SEQUENCER_SIMPLE_TOUCH_CONTROLS_H_

#include <cmath>

#include "eurorack/stmlib/stmlib.h"
#include "simple_touch/control_value.h"
#include "simple_touch/simple_touch.h"

namespace sequencer {

using simple_touch::ControlValue;
using simple_touch::SimpleTouch;

struct PersistentSettings {
  uint32_t magic;
  float deja_vu;
  float rate;
  float bias;
  float jitter;
  float loop_length;
};

class SimpleTouchControls {
 public:
  explicit SimpleTouchControls(SimpleTouch& touch);

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
  SimpleTouch& touch_;

  ControlValue deja_vu_;
  ControlValue rate_;
  ControlValue bias_;
  ControlValue jitter_;
  ControlValue loop_length_;

  DISALLOW_COPY_AND_ASSIGN(SimpleTouchControls);
};

}  // namespace sequencer

#endif  // SEQUENCER_SIMPLE_TOUCH_CONTROLS_H_
