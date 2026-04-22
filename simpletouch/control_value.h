#ifndef SIMPLETOUCH_CONTROL_VALUE_H_
#define SIMPLETOUCH_CONTROL_VALUE_H_

#include <cmath>

#include "simpletouch/touch.h"

namespace simpletouch {

class Touch;

class ControlValue {
 public:
  enum class State { kAttached, kDetached, kTryToAttach };

  static float constexpr kCallbackRate = 12000.f;

  static float constexpr kSmoothingCoefficient =
      6.9f /
      (0.02f * kCallbackRate);  // Within 0.1% in 0.02s, 12KHz callback rate

  static float constexpr kMovementDetectionThreshold = 0.0001f;

  static float constexpr kIdleTicks = kCallbackRate * 5;  // 5 seconds

  ControlValue(Touch& touch, float& value, float threshold = 0.02f,
               float coeff = kSmoothingCoefficient)
      : touch_(touch),
        state_(State::kDetached),
        value_(value),
        coeff_(coeff),
        threshold_(threshold),
        slow_value_(value),
        slow_coeff_(coeff * 0.25f),
        blink_when_value_moves_(false) {}

  ~ControlValue() = default;

  float Process(float input) {
    switch (state_) {
      case State::kAttached:
        value_ += (input - value_) * coeff_;
        slow_value_ += (input - slow_value_) * slow_coeff_;
        if (fabs(value_ - slow_value_) > kMovementDetectionThreshold) {
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

  void Attach() {
    if (state_ != State::kDetached) {
      return;
    }
    state_ = State::kTryToAttach;
    blink_when_value_moves_ = true;
  }

  void Detach() { state_ = State::kDetached; }

  float Value() const { return value_; }

 private:
  Touch& touch_;
  State state_;
  float& value_;
  float coeff_;
  float threshold_;
  float slow_value_;
  float slow_coeff_;
  float idle_counter_;
  bool blink_when_value_moves_;
};
}  // namespace simpletouch

#endif  // SIMPLETOUCH_CONTROL_VALUE_H_
