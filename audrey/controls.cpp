#include "audrey/controls.h"

#include <daisysp.h>

#include "audrey/dsp_utils.h"

namespace audrey {

using daisy::DaisySeed;
using daisy::Random;
using daisy::Switch3;
using daisysp::fclamp;
using daisysp::fmap;
using daisysp::Mapping;
using daisysp::Oscillator;

void Controls::Init(DaisySeed &hw) {
  output_volume_cv_.Detach();
  envelope_shape_cv_.Detach();

  body_lfo_.Init(hw.AudioCallbackRate());
  body_lfo_.SetAmp(1.f);
  body_lfo_.SetWaveform(Oscillator::WAVE_RAMP);
  body_lfo_.SetFreq(1.0f);
}

void Controls::UpdateAudioRate(DaisySeed &hw) {
  fb_gain_ = fmap(FeedbackGainKnob().Process(), -60.0f, 12.0f);
  verb_mix_ = fmap(ReverbMixKnob().Process(), 0.0f, 1.0f);
  verb_feedback_ = fmap(ftension(ReverbSizeKnob().Process(), -3.0f), 0.2f, 1.0f);
  fb_lpf_cutoff_ = fmap(LPFKnob().Process(), 100.0f, 18000.0f, Mapping::LOG);
  fb_hpf_cutoff_ = fmap(HPFKnob().Process(), 10.0f, 4000.0f, Mapping::LOG);

  input_level_ = fmap(input_volume_cv_.Process(VolumeKnob().GetRawFloat()), 0.0f,
                      1.0f, Mapping::EXP);
  output_level_ = fmap(output_volume_cv_.Process(VolumeKnob().GetRawFloat()),
                       0.0f, 1.0f, Mapping::EXP);

  envelope_shape_ = envelope_shape_cv_.Process(EnvelopeBodyFader().GetRawFloat());

  float body_knob_val =
      1.0f - feedback_body_knob_cv_.Process(EnvelopeBodyFader().GetRawFloat());

  float body_val;
  if (LfoSwitch() == Switch3::POS_LEFT) {
    body_val = body_knob_val;
  } else {
    static float prev_osc = 0.0f;
    static float held_val = 0.0f;
    static float smoothed_val = 0.0f;

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
    if ((prev_osc < 0.0f && curr_osc >= 0.0f) ||
        (prev_osc > 0.0f && curr_osc <= 0.0f)) {
      held_val = Random::GetFloat(
          body_knob_val - (0.05f + (0.07f * (1.0f - body_knob_val))),
          body_knob_val + (0.05f + (0.07f * (1.0f - body_knob_val))));
    }

    smoothed_val += slew_rate * (held_val - smoothed_val);
    body_val = smoothed_val;
    prev_osc = curr_osc;
  }

  feedback_body_final_cv_.Process(fclamp(body_val, 0.0f, 1.0f));
  fb_delay_samp_target_ =
      fmap(feedback_body_final_cv_.Value(), 0.001f, 0.1f, Mapping::EXP) *
      hw.AudioSampleRate();

  float freq_shift = touch_.knobs().s36().Process() * 24.0f;
  current_note_base_ =
      fclamp(current_note_base_ + freq_shift + octave_shift_, 16.0f, 88.0f);
}

void Controls::UpdateSlowRate(DaisySeed &hw) {
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
    feedback_body_knob_cv_.Detach();
    envelope_shape_cv_.Attach();
  }

  if (touch_.pads().IsFallingEdge(11)) {
    envelope_shape_cv_.Detach();
    feedback_body_knob_cv_.Attach();
  }

  if (touch_.pads().IsRisingEdge(10)) {
    input_volume_cv_.Detach();
    output_volume_cv_.Attach();
  }

  if (touch_.pads().IsFallingEdge(10)) {
    output_volume_cv_.Detach();
    input_volume_cv_.Attach();
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

  bool note_touched = false;
  float base_note = 16.0f;
  for (int pad = 3; pad <= 9; ++pad) {
    if (touch_.pads().IsTouched(pad)) {
      int note_index = pad - 3;
      current_note_base_ = base_note + scales[scale_][note_index];
      note_touched = true;
      break;
    }
  }

  note_on_ = note_touched;

  hw.SetLed(drone_mode_ || note_touched);
}

EngineParameters Controls::GetEngineParameters() {
  EngineParameters params;
  params.string_pitch = current_note_base_;
  params.feedback_gain = fb_gain_;
  params.feedback_delay = fb_delay_samp_target_;
  params.feedback_lpf_cutoff = fb_lpf_cutoff_;
  params.feedback_hpf_cutoff = fb_hpf_cutoff_;
  params.echo_delay_time = echo_delay_time_;
  params.echo_delay_feedback = echo_delay_feedback_;
  params.echo_delay_send_amount = echo_send_;
  params.reverb_mix = verb_mix_;
  params.reverb_feedback = verb_feedback_;
  params.output_level = output_level_;
  params.input_level = input_level_;
  params.envelope_shape = envelope_shape_;
  params.drone_mode = drone_mode_;
  params.note_on = note_on_;
  return params;
}

}  // namespace audrey
