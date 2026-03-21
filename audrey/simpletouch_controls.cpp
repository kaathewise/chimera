#include "audrey/simpletouch_controls.h"

#include <daisysp.h>

namespace audrey {

using daisy::DaisySeed;
using daisy::Random;
using daisy::Switch3;
using daisysp::fclamp;
using daisysp::fmap;
using daisysp::Mapping;
using daisysp::Oscillator;

void SimpletouchControls::Init() {
  output_volume_.Detach();
  envelope_shape_.Detach();

  body_lfo_.Init(48000.0f);
  body_lfo_.SetAmp(1.f);
  body_lfo_.SetWaveform(Oscillator::WAVE_RAMP);
  body_lfo_.SetFreq(1.0f);
}

void SimpletouchControls::Process() {
  input_volume_.Process(VolumeKnob().GetRawFloat());
  output_volume_.Process(VolumeKnob().GetRawFloat());
  envelope_shape_.Process(EnvelopeBodyFader().GetRawFloat());
  frequency_.Process(FrequencyFader().GetRawFloat());
  feedback_gain_.Process(FeedbackGainKnob().GetRawFloat());
  lpf_.Process(LPFKnob().GetRawFloat());
  hpf_.Process(HPFKnob().GetRawFloat());
  reverb_mix_.Process(ReverbMixKnob().GetRawFloat());
  reverb_size_.Process(ReverbSizeKnob().GetRawFloat());

  float body_knob_val =
      1 - feedback_body_knob_.Process(EnvelopeBodyFader().GetRawFloat());

  float body_val;
  if (LfoSwitch() == Switch3::POS_LEFT) {
    body_val = body_knob_val;
  } else {
    float slew_rate;
    if (LfoSwitch() == Switch3::POS_CENTER) {
      body_lfo_.SetFreq(0.01f + ((1.0f - body_knob_val) * 0.5f));
      slew_rate = 0.0001f;  // lower is slower
    } else {
      // POS_RIGHT
      body_lfo_.SetFreq(1.0f + ((1.0f - body_knob_val) * 7.0f));
      slew_rate = 0.08f;  // lower is slower
    }

    float curr_osc = body_lfo_.Process();
    if ((prev_osc_ < 0.0f && curr_osc >= 0.0f) ||
        (prev_osc_ > 0.0f && curr_osc <= 0.0f)) {
      held_val_ = Random::GetFloat(
          body_knob_val - (0.05f + (0.07f * (1.0f - body_knob_val))),
          body_knob_val + (0.05f + (0.07f * (1.0f - body_knob_val))));
    }

    smoothed_val_ += slew_rate * (held_val_ - smoothed_val_);
    body_val = smoothed_val_;
    prev_osc_ = curr_osc;
  }

  feedback_body_final_.Process(fclamp(body_val, 0.0f, 1.0f));
}

void SimpletouchControls::UpdateSlowRate(DaisySeed &hw) {
  if (!attached_) {
    return;
  }

  // Note: range is currently unused
  if (RangeSwitch() == Switch3::POS_LEFT) {
    range_ = 0;
  } else if (RangeSwitch() == Switch3::POS_CENTER) {
    range_ = 1;
  } else {
    // POS_RIGHT
    range_ = 2;
  }

  touch_.pads().Process();

  if (touch_.pads().IsRisingEdge(11)) {
    feedback_body_knob_.Detach();
    envelope_shape_.Attach();
  }

  if (touch_.pads().IsFallingEdge(11)) {
    envelope_shape_.Detach();
    feedback_body_knob_.Attach();
  }

  if (touch_.pads().IsRisingEdge(10)) {
    input_volume_.Detach();
    output_volume_.Attach();
  }

  if (touch_.pads().IsFallingEdge(10)) {
    output_volume_.Detach();
    input_volume_.Attach();
  }

  if (touch_.pads().IsTouched(11)) {
    if (touch_.pads().IsRisingEdge(0)) {
      scale_ = (scale_ + 1) % 3;
    }

    if (touch_.pads().IsRisingEdge(2)) {
      drone_mode_ = !drone_mode_;
    }
  } else {
    if (touch_.pads().IsRisingEdge(0)) {
      octave_shift_ -= 12.0f;
      if (octave_shift_ < -12.0f) octave_shift_ = -12.0f;
    }

    if (touch_.pads().IsRisingEdge(2)) {
      octave_shift_ += 12.0f;
      if (octave_shift_ > 48.0f) octave_shift_ = 48.0f;
    }
  }

  static const int scales[][8] = {{0, 2, 4, 5, 9, 12, 14},
                                  {0, 5, 6, 9, 10, 12, 13},
                                  {0, 2, 3, 7, 9, 12, 14}};

  static bool prev_note_touched = false;
  bool note_touched = false;
  for (int pad = 3; pad <= 9; ++pad) {
    if (touch_.pads().IsTouched(pad)) {
      int note_index = pad - 3;
      float base_note = 16.0f;

      current_note_base_ = base_note + scales[scale_][note_index];
      note_touched = true;
      break;
    }
  }

  if (note_touched && !prev_note_touched) {
    trigger_ = TriggerState::RISING_EDGE;
  } else if (!note_touched && prev_note_touched) {
    trigger_ = TriggerState::FALLING_EDGE;
  } else {
    trigger_ = TriggerState::UNKNOWN;
  }

  prev_note_touched = note_touched;

  hw.SetLed(drone_mode_ || note_touched);
}

void SimpletouchControls::Attach() {
  attached_ = true;
  input_volume_.Attach();
  output_volume_.Attach();
  envelope_shape_.Attach();
  feedback_body_knob_.Attach();
  feedback_body_final_.Attach();
  frequency_.Attach();
  feedback_gain_.Attach();
  lpf_.Attach();
  hpf_.Attach();
  reverb_mix_.Attach();
  reverb_size_.Attach();
}

void SimpletouchControls::Detach() {
  attached_ = false;
  input_volume_.Detach();
  output_volume_.Detach();
  envelope_shape_.Detach();
  feedback_body_knob_.Detach();
  feedback_body_final_.Detach();
  frequency_.Detach();
  feedback_gain_.Detach();
  lpf_.Detach();
  hpf_.Detach();
  reverb_mix_.Detach();
  reverb_size_.Detach();
}

EngineParameters SimpletouchControls::GetEngineParameters() {
  EngineParameters p;
  p.string_pitch =
      fclamp(current_note_base_ + frequency_.Value() * 24.0f + octave_shift_,
             16.0f, 88.0f);
  p.feedback_gain = fmap(feedback_gain_.Value(), -60.0f, 12.0f);
  p.feedback_delay =
      fmap(feedback_body_final_.Value(), 0.001f, 0.1f, Mapping::EXP);
  p.feedback_lpf_cutoff = fmap(lpf_.Value(), 100.0f, 18000.0f, Mapping::LOG);
  p.feedback_hpf_cutoff = fmap(hpf_.Value(), 10.0f, 4000.0f, Mapping::LOG);
  p.echo_delay_time = echo_delay_time_;
  p.echo_delay_feedback = echo_delay_feedback_;
  p.echo_delay_send_amount = echo_delay_send_amount_;
  p.reverb_mix = fmap(reverb_mix_.Value(), 0.0f, 1.0f);
  p.reverb_feedback = fmap(ftension(reverb_size_.Value(), -3.0f), 0.2f, 1.0f);
  p.output_level = fmap(output_volume_.Value(), 0.0f, 1.0f, Mapping::EXP);
  p.input_level = fmap(input_volume_.Value(), 0.0f, 1.0f, Mapping::EXP);
  p.shape = envelope_shape_.Value();
  p.drone_mode = drone_mode_;
  p.trigger = trigger_;
  return p;
}

}  // namespace audrey
