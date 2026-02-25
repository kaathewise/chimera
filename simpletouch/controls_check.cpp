#include <daisy_seed.h>

#include "simpletouch/touch.h"

using namespace daisy;
using namespace simpletouch;

constexpr auto kSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;
constexpr size_t kBlockSize = 4;

DaisySeed hw;
Touch touch;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  for (auto &knob : touch.knobs().knobs()) {
    knob.Process();
  }
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(kSampleRate);
  hw.SetAudioBlockSize(kBlockSize);

  touch.Init(hw);

  DaisySeed::StartLog();

  hw.StartAudio(AudioCallback);

  while (true) {
    for (auto &knob : touch.knobs().knobs()) {
      DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, knob.Value()));
    }

    touch.pads().Process();

    DaisySeed::Print("%d ", touch.pads().Touched());

    for (uint8_t i = 0; i < 12; i++) {
      DaisySeed::Print("%d %d ", touch.pads().GetBaseline(i),
                       touch.pads().GetValue(i));
    }

    DaisySeed::PrintLine("%d %d", touch.switches().s7s8(),
                         touch.switches().s9s10());
    System::Delay(10);
  }
}
