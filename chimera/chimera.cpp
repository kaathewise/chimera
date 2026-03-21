#include <daisy_seed.h>

#include "chimera/patch.h"
#include "simpletouch/touch.h"

using chimera::Patch;
using daisy::AudioHandle;
using daisy::DaisySeed;
using daisy::SaiHandle;
using daisy::System;

DaisySeed hw;
simpletouch::Touch touch;
Patch patch(touch);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.Process();
  patch.Process(in, out, size);
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(4);

  touch.Init(hw);
  patch.Init(hw);

  DaisySeed::StartLog();

  hw.StartAudio(AudioCallback);

  while (true) {
    touch.pads().Process();
    patch.UpdateSlowRate();
    System::Delay(10);
  }
}
