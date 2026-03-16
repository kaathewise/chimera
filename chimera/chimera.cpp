#include <daisy_seed.h>

#include "chimera/patch.h"
#include "simpletouch/touch.h"

using namespace daisy;
using namespace chimera;

DaisySeed hw;
simpletouch::Touch touch;
Patch patch(touch);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  patch.Process(in, out, size);
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(4);

  touch.Init(hw);
  patch.Init(hw);

  hw.StartAudio(AudioCallback);

  while (true) {
    touch.pads().Process();
    patch.UpdateSimpleTouchControls();
    System::Delay(10);
  }
}
