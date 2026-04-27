#include <daisy_seed.h>

#include "chimera/patch.h"
#include "simple_touch/simple_touch.h"

using chimera::Patch;
using daisy::AudioHandle;
using daisy::DaisySeed;
using daisy::SaiHandle;
using daisy::System;
using simple_touch::SimpleTouch;

SimpleTouch::Config config{
  .daisy_sample_rate = SaiHandle::Config::SampleRate::SAI_48KHZ,
  .block_size = 4
};
SimpleTouch touch(config);
Patch patch(touch);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.Process();
  patch.Process(in, out, size);
}

int main() {
  touch.Init();
  patch.Init(touch.hw());

  DaisySeed::StartLog();

  touch.hw().StartAudio(AudioCallback);

  while (true) {
    touch.pads().Process();
    patch.UpdateSlowRate();
    System::Delay(10);
  }
}
