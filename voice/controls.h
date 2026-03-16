#ifndef VOICE_CONTROLS_H_
#define VOICE_CONTROLS_H_

#include "common/control_value.h"
#include "eurorack/stmlib/stmlib.h"
#include "simpletouch/touch.h"

namespace voice {

using namespace common;

class Controls {
 public:
  explicit Controls(simpletouch::Touch& touch) : touch_(touch) {}

  void Process();

  float note() const { return 24.0f + note_.Value() * 72.0f; }
  float harmonics() const { return harmonics_.Value(); }
  float timbre() const { return timbre_.Value(); }
  float morph() const { return morph_.Value(); }
  float accent() const { return accent_.Value(); }

  void Attach() {
    note_.Attach();
    harmonics_.Attach();
    timbre_.Attach();
    morph_.Attach();
    accent_.Attach();
  }

  void Detach() {
    note_.Detach();
    harmonics_.Detach();
    timbre_.Detach();
    morph_.Detach();
    accent_.Detach();
  }

 private:
  simpletouch::Touch& touch_;

  ControlValue note_{.5f};
  ControlValue harmonics_{.5f};
  ControlValue timbre_{.5f};
  ControlValue morph_{.5f};
  ControlValue accent_{.8f};

  DISALLOW_COPY_AND_ASSIGN(Controls);
};

}  // namespace voice

#endif  // VOICE_CONTROLS_H_
