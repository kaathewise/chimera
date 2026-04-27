#include <daisy_seed.h>

#include "eurorack/plaits/dsp/engine/particle_engine.h"
#include "eurorack/stmlib/utils/buffer_allocator.h"
#include "simple_touch/simple_touch.h"
#include "voice/simple_touch_controls.h"
#include "voice/voice.h"

using daisy::AudioHandle;
using daisy::DaisySeed;
using daisy::SaiHandle;
using daisy::System;
using voice::SimpleTouchControls;
using voice::Voice;

int trigger_counter = 0;

simple_touch::SimpleTouch::Config config{
  .daisy_sample_rate = SaiHandle::Config::SampleRate::SAI_48KHZ,
  .block_size = 4
};
simple_touch::SimpleTouch touch(config);
SimpleTouchControls simple_touch_controls(touch);
plaits::ParticleEngine pe;
uint32_t buffer_space[8192];
stmlib::BufferAllocator allocator(buffer_space, 8192 * 4);
Voice v(pe);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.Process();
  simple_touch_controls.Process();

  trigger_counter += size;
  int trigger_state;
  if (trigger_counter >= config.AudioSampleRate()) {
    trigger_state = plaits::TRIGGER_RISING_EDGE;
    trigger_counter = 0;
  } else {
    trigger_state = plaits::TRIGGER_LOW;
  }

  const plaits::EngineParameters params{
      .trigger = trigger_state,
      .note = simple_touch_controls.note(),
      .timbre = simple_touch_controls.timbre(),
      .morph = simple_touch_controls.morph(),
      .harmonics = simple_touch_controls.harmonics(),
      .accent = simple_touch_controls.accent()};

  v.Process(params, simple_touch_controls.delay_time(),
            simple_touch_controls.delay_feedback(), out[0], size);

  memcpy(out[1], out[0], size * sizeof(float));
}

int main() {
  touch.Init();
  pe.Init(&allocator);
  v.Init(config.AudioSampleRate());
  simple_touch_controls.Attach();

  DaisySeed::StartLog();

  touch.hw().StartAudio(AudioCallback);
  while (true) {
    System::Delay(100);
  }
}
