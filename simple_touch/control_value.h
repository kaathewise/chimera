#ifndef SIMPLE_TOUCH_CONTROL_VALUE_H_
#define SIMPLE_TOUCH_CONTROL_VALUE_H_

#include "simple_touch/simple_touch.h"

namespace simple_touch {

class SimpleTouch;

class ControlValue {
 public:
  enum class State { kAttached, kDetached, kTryToAttach };

  // Within 0.1% in 0.02s, will be multiplied by callback rate.
  static float constexpr kSmoothingCoefficient = 6.9f / 0.02;

  static float constexpr kMovementDetectionThreshold = 0.0001f;

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
  uint32_t idle_time_;
  uint32_t idle_time_threshold_;
  bool blink_when_value_moves_;

  void DetectValueMovement_();
};
}  // namespace simple_touch

#endif  // SIMPLE_TOUCH_CONTROL_VALUE_H_
