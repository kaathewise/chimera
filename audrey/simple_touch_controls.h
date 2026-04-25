#ifndef AUDREY_SIMPLE_TOUCH_CONTROLS_H_
#define AUDREY_SIMPLE_TOUCH_CONTROLS_H_

#include <daisy_seed.h>

#include "audrey/engine.h"
#include "simple_touch/control_value.h"
#include "simple_touch/touch.h"

namespace audrey {

using daisy::AnalogControl;
using daisy::DaisySeed;
using daisysp::Oscillator;
using simple_touch::ControlValue;
using simple_touch::Touch;

struct PersistentSettings {
  uint32_t magic;
  float input_volume;
  float output_volume;
  float envelope_shape;
  float feedback_body_knob;
  float frequency;
  float feedback_gain;
  float lpf;
  float hpf;
  float reverb_mix;
  float reverb_size;
  float echo_delay_time;
  float echo_delay_feedback;
  float echo_delay_send_amount;
};

class SimpleTouchControls {
public:
  explicit SimpleTouchControls(Touch &touch);

  ~SimpleTouchControls() = default;

  void Init();

  void Process();

  void UpdateSlowRate();

  void Attach();
  void Detach();

  EngineParameters GetEngineParameters() const;

private:
  Touch &touch_;
  bool attached_{false};
  int scale_{0};

  TriggerState trigger_{TriggerState::kUnknown};

  float current_note_base_{40};
  float octave_shift_{0};
  bool drone_mode_{false};

  float prev_osc_{0};
  float held_val_{0};
  float smoothed_val_{0};
  float feedback_body_{0};

  ControlValue input_volume_;
  ControlValue output_volume_;
  ControlValue envelope_shape_;
  ControlValue feedback_body_knob_;
  ControlValue frequency_;
  ControlValue feedback_gain_;
  ControlValue lpf_;
  ControlValue hpf_;
  ControlValue reverb_mix_;
  ControlValue reverb_size_;
  ControlValue echo_delay_time_;
  ControlValue echo_delay_feedback_;
  ControlValue echo_delay_send_amount_;

  Oscillator body_lfo_;

  AnalogControl &FrequencyFader() const { return touch_.knobs().s36(); }
  AnalogControl &FeedbackGainKnob() const { return touch_.knobs().s30(); }
  AnalogControl &VolumeKnob() const { return touch_.knobs().s31(); }
  AnalogControl &ReverbMixKnob() const { return touch_.knobs().s32(); }
  AnalogControl &ReverbSizeKnob() const { return touch_.knobs().s33(); }
  AnalogControl &LPFKnob() const { return touch_.knobs().s34(); }
  AnalogControl &HPFKnob() const { return touch_.knobs().s35(); }
  AnalogControl &BodyFader() const { return touch_.knobs().s37(); }

  int LfoSwitch() const { return touch_.switches().right(); }
};
}  // namespace audrey

#endif  // AUDREY_SIMPLE_TOUCH_CONTROLS_H_
