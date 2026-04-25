#include "simple_touch/control_value.h"

#include <cmath>

namespace simple_touch {

ControlValue::ControlValue(Touch& touch, float& value, float threshold,
                           float coeff)
    : touch_(touch),
      state_(State::kDetached),
      value_(value),
      coeff_(coeff),
      threshold_(threshold),
      slow_value_(value),
      slow_coeff_(coeff * 0.25f),
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
    idle_counter_ = kIdleTicks;
    if (blink_when_value_moves_) {
      touch_.led().Blink();
      blink_when_value_moves_ = false;
    }
  } else if (idle_counter_ > 0) {
    idle_counter_--;
  } else {
    blink_when_value_moves_ = true;
  }
}

}  // namespace simple_touch
