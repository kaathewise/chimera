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
        input_volume_(touch, 1.0f, 0.02f, 0.007f),
        output_volume_(touch, 0.5f, 0.02f, 0.007f),
        envelope_shape_(touch, 0.0f, 0.02f, 0.007f),
        feedback_body_knob_(touch, 1.0f, 0.02f, 0.00014f),
        feedback_body_final_(touch, 0.0f, 0.02f, 0.007f),
        fb_gain_(touch, 0.5f),
        verb_mix_(touch, 0.0f),
        verb_feedback_(touch, 0.85f),
        fb_lpf_cutoff_(touch, 1.0f),
        fb_hpf_cutoff_(touch, 0.3f),
        frequency_(touch, 0.0f) {}

  ~SimpletouchControls() = default;

  void Init(DaisySeed &hw);

  void Process();

  void UpdateSlowRate(DaisySeed &hw);

  EngineParameters GetEngineParameters();

  void Attach() {
    fb_gain_.Attach();
    verb_mix_.Attach();
    verb_feedback_.Attach();
    fb_lpf_cutoff_.Attach();
    fb_hpf_cutoff_.Attach();
    frequency_.Attach();
    input_volume_.Attach();
    feedback_body_knob_.Attach();
  }

  void Detach() {
    fb_gain_.Detach();
    verb_mix_.Detach();
    verb_feedback_.Detach();
    fb_lpf_cutoff_.Detach();
    fb_hpf_cutoff_.Detach();
    frequency_.Detach();
    input_volume_.Detach();
    output_volume_.Detach();
    envelope_shape_.Detach();
    feedback_body_knob_.Detach();
  }

 private:
  Touch &touch_;

  float sample_rate_ = 48000.0f;
  float current_note_base_ = 40.0f;
  float octave_shift_ = 0.0f;
  bool drone_mode_ = false;
  bool note_on_ = false;

  float echo_delay_time_ = 5.0f;
  float echo_delay_feedback_ = 0.5f;
  float echo_send_ = 0.0f;

  ControlValue input_volume_;
  ControlValue output_volume_;
  ControlValue envelope_shape_;
  ControlValue feedback_body_knob_;
  ControlValue feedback_body_final_;
  ControlValue fb_gain_;
  ControlValue verb_mix_;
  ControlValue verb_feedback_;
  ControlValue fb_lpf_cutoff_;
  ControlValue fb_hpf_cutoff_;
  ControlValue frequency_;

  int scale_ = 0;
  int range_ = 0;

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
