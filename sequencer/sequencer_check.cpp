#include <daisy_seed.h>

#include "sequencer/simpletouch_controls.h"
#include "sequencer/sequencer.h"
#include "simpletouch/touch.h"

using namespace daisy;
using namespace sequencer;

constexpr auto kSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;
constexpr size_t kBlockSize = 4;

DaisySeed hw;
simpletouch::Touch touch;
Sequencer seq;
SimpleTouchControls simpletouch_controls(touch);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.Process();
  simpletouch_controls.Process();
  seq.Process(simpletouch_controls.deja_vu(), simpletouch_controls.rate(), simpletouch_controls.bias(),
              simpletouch_controls.jitter(), simpletouch_controls.loop_length());
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(kSampleRate);
  hw.SetAudioBlockSize(kBlockSize);

  touch.Init(hw);
  seq.Init(hw.AudioCallbackRate());

  DaisySeed::StartLog();

  hw.StartAudio(AudioCallback);

  while (true) {
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, simpletouch_controls.deja_vu()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, simpletouch_controls.rate()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, simpletouch_controls.bias()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, simpletouch_controls.jitter()));
    DaisySeed::Print("%d ", simpletouch_controls.loop_length());
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, seq.ramps().master));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, seq.ramps().slave[0]));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, seq.ramps().slave[1]));
    DaisySeed::PrintLine("");

    System::Delay(10);
  }
}
