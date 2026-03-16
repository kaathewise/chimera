#include <daisy_seed.h>

#include "eurorack/plaits/dsp/engine/particle_engine.h"
#include "eurorack/stmlib/utils/buffer_allocator.h"
#include "simpletouch/touch.h"
#include "voice/simpletouch_controls.h"
#include "voice/voice.h"

using namespace daisy;
using namespace voice;

int trigger_counter = 0;
int sample_rate = 48000;

DaisySeed hw;
simpletouch::Touch touch;
SimpleTouchControls simpletouch_controls(touch);
plaits::ParticleEngine pe;
uint32_t buffer_space[8192];
float DSY_SDRAM_BSS delay_buffer[240000];
stmlib::BufferAllocator allocator(buffer_space, 8192 * 4);
stmlib::BufferAllocator delay_allocator(delay_buffer, sizeof(delay_buffer));
Voice v(pe);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.Process();
  simpletouch_controls.Process();

  trigger_counter += size;
  int trigger_state;
  if (trigger_counter >= sample_rate) {
    trigger_state = plaits::TRIGGER_RISING_EDGE;
    trigger_counter = 0;
  } else {
    trigger_state = plaits::TRIGGER_LOW;
  }

  const plaits::EngineParameters params{.trigger = trigger_state,
                                        .note = simpletouch_controls.note(),
                                        .timbre = simpletouch_controls.timbre(),
                                        .morph = simpletouch_controls.morph(),
                                        .harmonics = simpletouch_controls.harmonics(),
                                        .accent = simpletouch_controls.accent()};

  v.Process(params, simpletouch_controls.delay_time(),
            simpletouch_controls.delay_feedback(), out[0], size);

  memcpy(out[1], out[0], size * sizeof(float));
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(4);

  touch.Init(hw);
  pe.Init(&allocator);
  v.Init(sample_rate, &delay_allocator, 5);
  simpletouch_controls.Attach();

  DaisySeed::StartLog();

  hw.StartAudio(AudioCallback);
  while (true) {
    System::Delay(100);
  }
}
