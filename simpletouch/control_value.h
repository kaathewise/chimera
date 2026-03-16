#ifndef SIMPLETOUCH_CONTROL_VALUE_H_
#define SIMPLETOUCH_CONTROL_VALUE_H_

#include <cmath>

#include "touch.h"

namespace simpletouch {

class Touch;

class ControlValue {
 public:
  enum State {
    kStateAttached,
    kStateDetached,
    kStateTryToAttach,
    kStateSeeking
  };

  ControlValue(Touch& touch, float initial, float threshold = 0.05f, float coeff = 0.02f)
      : touch_(touch),
        state_(kStateDetached),
        value_(initial),
        coeff_(coeff),
        threshold_(threshold) {}

  ~ControlValue() = default;

  float Process(float input) {
    switch (state_) {
      case kStateAttached:
        value_ += (input - value_) * coeff_;
        break;
      case kStateTryToAttach:
        if (fabs(input - value_) < threshold_) {
          state_ = kStateAttached;
        } else {
          state_ = kStateSeeking;
        }
        break;
      case kStateSeeking:
        if (fabs(input - value_) < threshold_) {
          state_ = kStateAttached;
          touch_.led().Blink();
        }
        break;
      case kStateDetached:
        break;
      default:
        break;
    }

    return value_;
  }

  void Attach() { state_ = kStateTryToAttach; }

  void Detach() { state_ = kStateDetached; }

  float Value() const { return value_; }

 private:
  Touch& touch_;
  State state_;
  float value_;
  float coeff_;
  float threshold_;
};
}  // namespace simpletouch

#endif  // SIMPLETOUCH_CONTROL_VALUE_H_
