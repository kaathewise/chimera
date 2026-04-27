#include <daisy_seed.h>

#include "sequencer/sequencer.h"
#include "sequencer/simple_touch_controls.h"
#include "simple_touch/simple_touch.h"
#include "third_party/libDaisy/src/per/sai.h"

using daisy::AudioHandle;
using daisy::DaisySeed;
using daisy::SaiHandle;
using daisy::System;
using sequencer::Sequencer;
using sequencer::SimpleTouchControls;
using simple_touch::SimpleTouch;

SimpleTouch::Config config{
  .daisy_sample_rate = SaiHandle::Config::SampleRate::SAI_48KHZ,
  .block_size = 4
};
SimpleTouch touch(config);
Sequencer seq;
SimpleTouchControls simple_touch_controls(touch);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.Process();
  simple_touch_controls.Process();
  seq.Process(simple_touch_controls.deja_vu(), simple_touch_controls.rate(),
              simple_touch_controls.bias(), simple_touch_controls.jitter(),
              simple_touch_controls.loop_length());
}

int main() {
  touch.Init();
  seq.Init(config.AudioCallbackRate());
  simple_touch_controls.Attach();

  DaisySeed::StartLog();

  touch.hw().StartAudio(AudioCallback);

  while (true) {
    DaisySeed::Print(FLT_FMT(5) " ",
                     FLT_VAR(5, simple_touch_controls.deja_vu()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, simple_touch_controls.rate()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, simple_touch_controls.bias()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, simple_touch_controls.jitter()));
    DaisySeed::Print("%d ", simple_touch_controls.loop_length());
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, seq.ramps().master));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, seq.ramps().slave[0]));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, seq.ramps().slave[1]));
    DaisySeed::PrintLine("");

    System::Delay(10);
  }
}
