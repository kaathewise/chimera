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
  patch.ProcessAudio(in, out, size);
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(4);

  touch.Init(hw);
  patch.Init(hw.AudioCallbackRate());

  hw.StartAudio(AudioCallback);

  while (true) {
    touch.pads().Process();
    patch.UpdateControls();
    System::Delay(10);
  }
}
