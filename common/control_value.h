#ifndef COMMON_CONTROL_VALUE_H
#define COMMON_CONTROL_VALUE_H

#include <cmath>

namespace common {

class ControlValue {
public:
  enum State {
    kStateAttached,
    kStateDetached,
    kStateTryToAttach,
  };

  ControlValue(float initial, float threshold = 0.05f, float coeff = 0.02f):
    state_(kStateAttached),
    value_(initial),
    coeff_(coeff),
    threshold_(threshold) {}

  ~ControlValue() = default;

  float Process(float input) {
    if (state_ == kStateTryToAttach) {
      if (fabs(input - value_) < threshold_) {
        state_ = kStateAttached;
      }
    }

    if (state_ == kStateAttached) {
      value_ += (input - value_) * coeff_;
    }

    return value_;
  }

  void Attach() {
    state_ = kStateTryToAttach;
  }

  void Detach() {
    state_ = kStateDetached;
  }

  float Value() const { return value_; }

private:
  State state_;
  float value_;
  float coeff_;
  float threshold_;
};
}

#endif
