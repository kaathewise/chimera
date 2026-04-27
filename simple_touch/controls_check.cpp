#include <daisy_seed.h>

#include "simple_touch/control_value.h"
#include "simple_touch/simple_touch.h"

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
float val;
simple_touch::ControlValue cv(touch, val);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  for (auto &knob : touch.knobs().knobs()) {
    knob.Process();
  }

  cv.Process(touch.knobs().s30().GetRawFloat());

  touch.led().Process();
}

int main() {
  touch.Init();

  DaisySeed::StartLog();

  touch.hw().StartAudio(AudioCallback);

  cv.Attach();

  while (true) {
    for (auto &knob : touch.knobs().knobs()) {
      DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, knob.Value()));
    }

    touch.pads().Process();

    DaisySeed::Print("%d ", touch.pads().SimpleTouched());

    for (uint8_t i = 0; i < 12; i++) {
      DaisySeed::Print("%d %d ", touch.pads().GetBaseline(i),
                       touch.pads().GetValue(i));
    }

    DaisySeed::PrintLine("%d %d", touch.switches().left(),
                         touch.switches().right());
    System::Delay(10);
  }
}
