#ifndef AUDREY_SIMPLETOUCH_CONTROLS_H_
#define AUDREY_SIMPLETOUCH_CONTROLS_H_

#include <daisy_seed.h>

#include "audrey/engine.h"
#include "simpletouch/control_value.h"
#include "simpletouch/touch.h"

namespace audrey {

using daisy::AnalogControl;
using daisy::DaisySeed;
using daisysp::Oscillator;
using simpletouch::ControlValue;
using simpletouch::Touch;

class SimpletouchControls {
 public:
  explicit SimpletouchControls(Touch &touch)
      : touch_(touch),
        input_volume_cv_(touch, 0.5f, 0.02f, 0.007f),
        output_volume_cv_(touch, 0.5f, 0.02f, 0.007f),
        envelope_shape_cv_(touch, 0.0f, 0.02f, 0.007f),
        feedback_body_knob_cv_(touch, 0.0f, 0.02f, 0.00014f),
        feedback_body_final_cv_(touch, 0.0f, 0.02f, 0.007f) {
    current_note_base_ = 40.0f;
    octave_shift_ = 0.0f;
    drone_mode_ = false;
    echo_delay_time_ = 5.0f;
    echo_delay_feedback_ = 0.5f;
    echo_delay_send_amount_ = 0.0f;
    prev_osc_ = 0.0f;
    held_val_ = 0.0f;
    smoothed_val_ = 0.0f;
  }

  ~SimpletouchControls() = default;

  void Init(DaisySeed &hw);

  void Process();

  void UpdateSlowRate(DaisySeed &hw);

  EngineParameters GetEngineParameters();

 private:
  Touch &touch_;

  float current_note_base_;
  float octave_shift_;
  bool drone_mode_;

  float echo_delay_time_;
  float echo_delay_feedback_;
  float echo_delay_send_amount_;

  float prev_osc_;
  float held_val_;
  float smoothed_val_;

  ControlValue input_volume_cv_;
  ControlValue output_volume_cv_;
  ControlValue envelope_shape_cv_;
  ControlValue feedback_body_knob_cv_;
  ControlValue feedback_body_final_cv_;

  int scale_;
  int range_;

  TriggerState trigger_;

  Oscillator body_lfo_;

  AnalogControl &FrequencyFader() const { return touch_.knobs().s36(); }
  AnalogControl &FeedbackGainKnob() const { return touch_.knobs().s30(); }
  AnalogControl &VolumeKnob() const { return touch_.knobs().s31(); }
  AnalogControl &ReverbMixKnob() const { return touch_.knobs().s32(); }
  AnalogControl &ReverbSizeKnob() const { return touch_.knobs().s33(); }
  AnalogControl &LPFKnob() const { return touch_.knobs().s34(); }
  AnalogControl &HPFKnob() const { return touch_.knobs().s35(); }
  AnalogControl &EnvelopeBodyFader() const { return touch_.knobs().s37(); }

  int RangeSwitch() const { return touch_.switches().s7s8(); }
  int LfoSwitch() const { return touch_.switches().s9s10(); }
};
}  // namespace audrey

#endif  // AUDREY_SIMPLETOUCH_CONTROLS_H_
