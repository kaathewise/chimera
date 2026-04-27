#include "simple_touch/control_value.h"

#include <cmath>

namespace simple_touch {

ControlValue::ControlValue(SimpleTouch& touch, float& value, float threshold,
                           float coeff)
    : touch_(touch),
      state_(State::kDetached),
      value_(value),
      coeff_(coeff / touch.config().AudioCallbackRate()),
      threshold_(threshold),
      slow_value_(value),
      slow_coeff_(coeff_ * 0.25f),
      idle_time_threshold_(5 * static_cast<uint32_t>(touch.config().AudioCallbackRate())),
      blink_when_value_moves_(true) {
}

float ControlValue::Process(float input) {
  switch (state_) {
    case State::kAttached:
      value_ += (input - value_) * coeff_;
      slow_value_ += (input - slow_value_) * slow_coeff_;
      DetectValueMovement_();
      break;
    case State::kTryToAttach:
      if (std::fabs(input - value_) < threshold_) {
        state_ = State::kAttached;
      }
      break;
    default:
      break;
  }

  return value_;
}

void ControlValue::Attach() {
  if (state_ != State::kDetached) {
    return;
  }
  state_ = State::kTryToAttach;
}

void ControlValue::Detach() { state_ = State::kDetached; }

void ControlValue::DetectValueMovement_() {
  if (std::fabs(value_ - slow_value_) > kMovementDetectionThreshold) {
    idle_time_ = 0;
    if (blink_when_value_moves_) {
      touch_.led().Blink();
      blink_when_value_moves_ = false;
    }
  } else if (idle_time_ > idle_time_threshold_) {
    blink_when_value_moves_ = true;
  } else {
    idle_time_++;
  }
}

}  // namespace simple_touch
