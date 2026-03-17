#include "chimera/patch.h"

#include <daisy_seed.h>

#include "eurorack/stmlib/utils/buffer_allocator.h"

namespace chimera {

void Patch::Init(daisy::DaisySeed hw) {
  stmlib::BufferAllocator allocator(buffer_space_, sizeof(buffer_space_));
  particle_engine_.Init(&allocator);

  voice_.Init(hw.AudioSampleRate());

  sequencer_.Init(hw.AudioCallbackRate());
  sequencer_simpletouch_controls_.Attach();
  voice_simpletouch_controls_.Detach();
}

void Patch::Process(daisy::AudioHandle::InputBuffer in,
                    daisy::AudioHandle::OutputBuffer out, size_t size) {
  sequencer_simpletouch_controls_.Process();
  voice_simpletouch_controls_.Process();

  sequencer_.Process(sequencer_simpletouch_controls_.deja_vu(),
                     sequencer_simpletouch_controls_.rate(),
                     sequencer_simpletouch_controls_.bias(),
                     sequencer_simpletouch_controls_.jitter(),
                     sequencer_simpletouch_controls_.loop_length());

  const auto& triggers = sequencer_.triggers();

  // 3. The synth should take the first trigger from the sequencer and use it
  // to trigger a ParticleEngine voice.
  int trigger_state =
      triggers.slave[0] ? plaits::TRIGGER_RISING_EDGE : plaits::TRIGGER_LOW;

  const plaits::EngineParameters params{
      .trigger = trigger_state,
      .note = voice_simpletouch_controls_.note(),
      .timbre = voice_simpletouch_controls_.timbre(),
      .morph = voice_simpletouch_controls_.morph(),
      .harmonics = voice_simpletouch_controls_.harmonics(),
      .accent = voice_simpletouch_controls_.accent()};

  voice_.Process(params, voice_simpletouch_controls_.delay_time(),
                 voice_simpletouch_controls_.delay_feedback(), out[0], size);

  memcpy(out[1], out[0], size * sizeof(float));
}

void Patch::UpdateSimpleTouchControls() {
  const auto& pads = touch_.pads();

  if (pads.IsTouched(10)) {
    if (pads.IsRisingEdge(3)) {
      if (control_target_ != CONTROL_TARGET_SEQUENCER) {
        control_target_ = CONTROL_TARGET_SEQUENCER;
        sequencer_simpletouch_controls_.Attach();
        voice_simpletouch_controls_.Detach();
      }
    } else if (pads.IsRisingEdge(4)) {
      if (control_target_ != CONTROL_TARGET_VOICE) {
        control_target_ = CONTROL_TARGET_VOICE;
        voice_simpletouch_controls_.Attach();
        sequencer_simpletouch_controls_.Detach();
      }
    }
  }
}

}  // namespace chimera
