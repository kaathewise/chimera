#include "chimera/patch.h"

namespace chimera {

void Patch::Init(float sample_rate) {
  stmlib::BufferAllocator allocator(buffer_space_, sizeof(buffer_space_));
  particle_engine_.Init(&allocator);
  voice_.Init();
  sequencer_.Init(sample_rate);
  sequencer_controls_.Attach();
  voice_controls_.Detach();
}

void Patch::Process(daisy::AudioHandle::InputBuffer in,
                         daisy::AudioHandle::OutputBuffer out, size_t size) {
  sequencer_controls_.Process();
  voice_controls_.Process();

  sequencer_.Process(sequencer_controls_.deja_vu(), sequencer_controls_.rate(),
                     sequencer_controls_.bias(), sequencer_controls_.jitter(),
                     sequencer_controls_.loop_length());

  const auto& triggers = sequencer_.triggers();

  // 3. The synth should take the first trigger from the sequencer and use it
  // to trigger a ParticleEngine voice.
  int trigger_state =
      triggers.slave[0] ? plaits::TRIGGER_RISING_EDGE : plaits::TRIGGER_LOW;

  const plaits::EngineParameters params{.trigger = trigger_state,
                                        .note = voice_controls_.note(),
                                        .timbre = voice_controls_.timbre(),
                                        .morph = voice_controls_.morph(),
                                        .harmonics = voice_controls_.harmonics(),
                                        .accent = voice_controls_.accent()};

  voice_.Process(params, out, size);
}

void Patch::UpdateControls() {
  const auto& pads = touch_.pads();

  if (pads.IsTouched(10)) {
    if (pads.IsRisingEdge(3)) {
      if (control_target_ != CONTROL_TARGET_SEQUENCER) {
        control_target_ = CONTROL_TARGET_SEQUENCER;
        sequencer_controls_.Attach();
        voice_controls_.Detach();
      }
    } else if (pads.IsRisingEdge(4)) {
      if (control_target_ != CONTROL_TARGET_VOICE) {
        control_target_ = CONTROL_TARGET_VOICE;
        voice_controls_.Attach();
        sequencer_controls_.Detach();
      }
    }
  }
}

}  // namespace chimera
