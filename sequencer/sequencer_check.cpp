#include <daisy_seed.h>
#include "controls.h"
#include "sequencer.h"
#include "../simpletouch/touch.h"

using namespace daisy;
using namespace sequencer;

constexpr auto kSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;
constexpr size_t kBlockSize = 4;

DaisySeed hw;
simpletouch::Touch touch;
Sequencer seq;
Controls controls(touch);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {

  bool gate[2];
  controls.Process();
  seq.Process(false, controls.deja_vu(), controls.rate(), controls.bias(), controls.jitter(), controls.loop_length(), gate);
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

    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, controls.deja_vu()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, controls.rate()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, controls.bias()));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, controls.jitter()));
    DaisySeed::Print("%d ", controls.loop_length());
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, *seq.ramps().master));
    DaisySeed::Print(FLT_FMT(5) " ", FLT_VAR(5, *seq.ramps().slave[0]));
    DaisySeed::PrintLine(FLT_FMT(5) " ", FLT_VAR(5, *seq.ramps().slave[1]));

    System::Delay(10);
  }
}
