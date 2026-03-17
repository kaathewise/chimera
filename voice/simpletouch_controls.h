#ifndef VOICE_SIMPLETOUCH_CONTROLS_H_
#define VOICE_SIMPLETOUCH_CONTROLS_H_

#include "eurorack/stmlib/stmlib.h"
#include "simpletouch/control_value.h"
#include "simpletouch/touch.h"

namespace voice {

using simpletouch::ControlValue;
using simpletouch::Touch;

class SimpleTouchControls {
 public:
  explicit SimpleTouchControls(Touch& touch, float max_delay_time = 5.f)
      : touch_(touch),
        max_delay_time_(max_delay_time),
        note_(touch, .5f),
        harmonics_(touch, .5f),
        timbre_(touch, .5f),
        morph_(touch, .5f),
        accent_(touch, .8f),
        delay_time_(touch, .2f),
        delay_feedback_(touch, .0f) {}

  void Process();

  float note() const { return 24.0f + note_.Value() * 72.0f; }
  float harmonics() const { return harmonics_.Value(); }
  float timbre() const { return timbre_.Value(); }
  float morph() const { return morph_.Value(); }
  float accent() const { return accent_.Value(); }
  float delay_time() const { return delay_time_.Value() * max_delay_time_; }
  float delay_feedback() const { return delay_feedback_.Value(); }

  void Attach() {
    note_.Attach();
    harmonics_.Attach();
    timbre_.Attach();
    morph_.Attach();
    accent_.Attach();
    delay_time_.Attach();
    delay_feedback_.Attach();
  }

  void Detach() {
    note_.Detach();
    harmonics_.Detach();
    timbre_.Detach();
    morph_.Detach();
    accent_.Detach();
    delay_time_.Detach();
    delay_feedback_.Detach();
  }

 private:
  Touch& touch_;

  float max_delay_time_;

  ControlValue note_;
  ControlValue harmonics_;
  ControlValue timbre_;
  ControlValue morph_;
  ControlValue accent_;
  ControlValue delay_time_;
  ControlValue delay_feedback_;

  DISALLOW_COPY_AND_ASSIGN(SimpleTouchControls);
};

}  // namespace voice

#endif  // VOICE_SIMPLETOUCH_CONTROLS_H_
