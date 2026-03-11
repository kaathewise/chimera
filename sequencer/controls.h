#ifndef SEQUENCER_CONTROLS_H_
#define SEQUENCER_CONTROLS_H_

#include <cmath>

#include "common/control_value.h"
#include "eurorack/stmlib/stmlib.h"
#include "simpletouch/touch.h"

namespace sequencer {

using namespace common;

class Controls {
 public:
  explicit Controls(simpletouch::Touch& touch) : touch_(touch) {}

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

 private:
  simpletouch::Touch& touch_;

  ControlValue deja_vu_{.5f};
  ControlValue rate_{.5f};
  ControlValue bias_{.5f};
  ControlValue jitter_{.0f};
  ControlValue loop_length_{.5f};

  DISALLOW_COPY_AND_ASSIGN(Controls);
};

}  // namespace sequencer

#endif  // SEQUENCER_CONTROLS_H_
