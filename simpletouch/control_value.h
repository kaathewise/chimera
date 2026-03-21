#ifndef SIMPLETOUCH_CONTROL_VALUE_H_
#define SIMPLETOUCH_CONTROL_VALUE_H_

#include <algorithm>
#include <cmath>

#include "simpletouch/touch.h"

namespace simpletouch {

class Touch;

class ControlValue {
 public:
  enum class State {
    kAttached,
    kDetached,
    kTryToAttach
  };

  ControlValue(Touch& touch, float initial, float threshold = 0.02f,
               float coeff = 0.02f)
      : touch_(touch),
        state_(State::kDetached),
        value_(initial),
        coeff_(coeff),
        threshold_(threshold),
        min_input_(1),
        max_input_(0),
        blink_when_attached_(true) {}

  ~ControlValue() = default;

  float Process(float input) {
    min_input_ = std::min(min_input_, input);
    max_input_ = std::max(max_input_, input);
    switch (state_) {
      case State::kAttached:
        value_ += (input - value_) * coeff_;
        if (blink_when_attached_ && (max_input_ - min_input_ > threshold_)) {
          touch_.led().Blink();
          blink_when_attached_ = false;
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
    state_ = State::kTryToAttach;
    blink_when_attached_ = true;
    min_input_ = 1;
    max_input_ = 0;
  }

  void Detach() { state_ = State::kDetached; }

  float Value() const { return value_; }

 private:
  Touch& touch_;
  State state_;
  float value_;
  float coeff_;
  float threshold_;
  float min_input_;
  float max_input_;
  bool blink_when_attached_;
};
}  // namespace simpletouch

#endif  // SIMPLETOUCH_CONTROL_VALUE_H_
