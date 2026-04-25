#include <daisy_seed.h>

#include "eurorack/plaits/dsp/engine/particle_engine.h"
#include "eurorack/stmlib/utils/buffer_allocator.h"
#include "simple_touch/touch.h"
#include "voice/simple_touch_controls.h"
#include "voice/voice.h"

using daisy::AudioHandle;
using daisy::DaisySeed;
using daisy::SaiHandle;
using daisy::System;
using voice::SimpleTouchControls;
using voice::Voice;

int trigger_counter = 0;
int sample_rate = 48000;

DaisySeed hw;
simple_touch::Touch touch;
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
  if (trigger_counter >= sample_rate) {
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
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(4);

  touch.Init(hw);
  pe.Init(&allocator);
  v.Init(sample_rate);
  simple_touch_controls.Attach();

  DaisySeed::StartLog();

  hw.StartAudio(AudioCallback);
  while (true) {
    System::Delay(100);
  }
}
