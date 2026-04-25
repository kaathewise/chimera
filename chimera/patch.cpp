#include "chimera/patch.h"

#include <daisy_seed.h>

#include "eurorack/stmlib/utils/buffer_allocator.h"

namespace chimera {

void Patch::Init(daisy::DaisySeed hw) {
  stmlib::BufferAllocator allocator(buffer_space_, sizeof(buffer_space_));
  particle_engine_.Init(&allocator);

  voice_.Init(hw.AudioSampleRate());
  sequencer_.Init(hw.AudioCallbackRate());
  audrey_.Init(hw.AudioSampleRate());

  audrey_simple_touch_controls_.Init();

  sequencer_simple_touch_controls_.Attach();
}

void Patch::Process(daisy::AudioHandle::InputBuffer in,
                    daisy::AudioHandle::OutputBuffer out, size_t size) {
  sequencer_simple_touch_controls_.Process();
  voice_simple_touch_controls_.Process();
  audrey_simple_touch_controls_.Process();

  sequencer_.Process(sequencer_simple_touch_controls_.deja_vu(),
                     sequencer_simple_touch_controls_.rate(),
                     sequencer_simple_touch_controls_.bias(),
                     sequencer_simple_touch_controls_.jitter(),
                     sequencer_simple_touch_controls_.loop_length());

  const auto& triggers = sequencer_.triggers();

  int trigger_state =
      triggers.slave[0] ? plaits::TRIGGER_RISING_EDGE : plaits::TRIGGER_LOW;

  const plaits::EngineParameters params{
      .trigger = trigger_state,
      .note = voice_simple_touch_controls_.note(),
      .timbre = voice_simple_touch_controls_.timbre(),
      .morph = voice_simple_touch_controls_.morph(),
      .harmonics = voice_simple_touch_controls_.harmonics(),
      .accent = voice_simple_touch_controls_.accent()};

  voice_.Process(params, voice_simple_touch_controls_.delay_time(),
                 voice_simple_touch_controls_.delay_feedback(), out[0], size);

  memcpy(out[1], out[0], size * sizeof(float));

  const audrey::EngineParameters audrey_params =
      audrey_simple_touch_controls_.GetEngineParameters();
  for (size_t i = 0; i < size; i++) {
    audrey_.Process(audrey_params, IN_L[i], OUT_L[i], OUT_R[i]);
  }

  limiter_[0].ProcessBlock(OUT_L, size, 0.7f);
  limiter_[1].ProcessBlock(OUT_R, size, 0.7f);
}

void Patch::UpdateSlowRate() {
  const auto& pads = touch_.pads();

  if (pads.IsTouched(10)) {
    if (pads.IsRisingEdge(3)) {
      sequencer_simple_touch_controls_.Attach();
      voice_simple_touch_controls_.Detach();
      audrey_simple_touch_controls_.Detach();
    } else if (pads.IsRisingEdge(4)) {
      voice_simple_touch_controls_.Attach();
      sequencer_simple_touch_controls_.Detach();
      audrey_simple_touch_controls_.Detach();
    } else if (pads.IsRisingEdge(5)) {
      audrey_simple_touch_controls_.Attach();
      voice_simple_touch_controls_.Detach();
      sequencer_simple_touch_controls_.Detach();
    }
  }

  audrey_simple_touch_controls_.UpdateSlowRate();
}

}  // namespace chimera
