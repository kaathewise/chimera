#ifndef AUDREY_CONTROL_VALUE_H_
#define AUDREY_CONTROL_VALUE_H_

#include <daisy_seed.h>
#include <daisysp.h>

#include "audrey/dsp_utils.h"

using daisy::DaisySeed;
using daisysp::fonepole;

namespace audrey {

/**
 * A value that can be detached from tracking the input. When attached back it
 * will only continue tracking once the input appears within the given threshold
 * from the stored value. This is useful for having different params mapped to
 * the same knob.
 *
 * If you don't need this functionality, consider using plain AnalogControl as
 * it has built-in smoothing.
 */
class ControlValue {
 public:
  enum State {
    kStateAttached,
    kStateDetached,
    kStateTryToAttach,
  };

  ControlValue() = default;

  ~ControlValue() = default;

  void Init(const DaisySeed &hw, float initial, float slew_time,
            float threshold = 0.02f) {
    value_ = initial;
    state_ = kStateAttached;
    coef_ = audrey::onepole_coef_t60(slew_time, hw.AudioCallbackRate());
    threshold_ = threshold;
  }

  float Process(float input) {
    if (state_ == kStateTryToAttach) {
      if (fabs(input - value_) < threshold_) {
        state_ = kStateAttached;
      }
    }

    if (state_ == kStateAttached) {
      fonepole(value_, input, coef_);
    }

    return value_;
  }

  void Attach() { state_ = kStateTryToAttach; }

  void Detach() { state_ = kStateDetached; }

  float Value() const { return value_; }

 private:
  State state_;
  float value_;
  float coef_;
  float threshold_;
};
}  // namespace audrey

#endif  // AUDREY_CONTROL_VALUE_H_
