#ifndef SIMPLE_TOUCH_CONTROL_VALUE_H_
#define SIMPLE_TOUCH_CONTROL_VALUE_H_

#include "simple_touch/simple_touch.h"

namespace simple_touch {

class SimpleTouch;

class ControlValue {
 public:
  enum class State { kAttached, kDetached, kTryToAttach };

  static float constexpr kCallbackRate = 12000.f;

  static float constexpr kSmoothingCoefficient =
      6.9f /
      (0.02f * kCallbackRate);  // Within 0.1% in 0.02s, 12KHz callback rate

  static float constexpr kMovementDetectionThreshold = 0.0001f;

  static float constexpr kIdleTicks = kCallbackRate * 5;  // 5 seconds

  ControlValue(SimpleTouch& touch, float& value, float threshold = 0.02f,
               float coeff = kSmoothingCoefficient);

  ~ControlValue() = default;

  float Process(float input);

  void Attach();

  void Detach();

  float Value() const { return value_; }

 private:
  SimpleTouch& touch_;
  State state_;
  float& value_;
  float coeff_;
  float threshold_;
  float slow_value_;
  float slow_coeff_;
  float idle_counter_;
  bool blink_when_value_moves_;

  void DetectValueMovement_();
};
}  // namespace simple_touch

#endif  // SIMPLE_TOUCH_CONTROL_VALUE_H_
