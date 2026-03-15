#include <daisy_seed.h>

#include "eurorack/plaits/dsp/engine/particle_engine.h"
#include "eurorack/stmlib/utils/buffer_allocator.h"
#include "simpletouch/touch.h"
#include "voice/controls.h"
#include "voice/voice.h"

using namespace daisy;
using namespace voice;

DaisySeed hw;
simpletouch::Touch touch;
Controls controls(touch);
plaits::ParticleEngine pe;
Voice v(pe);
uint32_t buffer_space[8192];
stmlib::BufferAllocator allocator(buffer_space, 8192 * 4);

int trigger_counter = 0;
int sample_rate = 48000;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  controls.Process();

  trigger_counter += size;
  int trigger_state;
  if (trigger_counter >= sample_rate) {
    trigger_state = plaits::TRIGGER_RISING_EDGE;
    trigger_counter = 0;
  } else {
    trigger_state = plaits::TRIGGER_LOW;
  }

  const plaits::EngineParameters params{.trigger = trigger_state,
                                        .note = controls.note(),
                                        .timbre = controls.timbre(),
                                        .morph = controls.morph(),
                                        .harmonics = controls.harmonics(),
                                        .accent = controls.accent()};

  v.Process(params, out[0], size);

  memcpy(out[1], out[0], size * sizeof(float));
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(4);

  touch.Init(hw);
  pe.Init(&allocator);
  v.Init();

  hw.StartAudio(AudioCallback);
  while (true) {
    System::Delay(100);
  }
}
