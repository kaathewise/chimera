#include <daisy_seed.h>

#include "audrey/controls.h"
#include "audrey/engine.h"
#include "simpletouch/touch.h"

using daisy::AudioHandle;
using daisy::DaisySeed;
using daisy::SaiHandle;
using daisysp::Limiter;
using simpletouch::Touch;

static const auto kSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;
static const size_t kBlockSize = 4;

static DaisySeed hw;
static Touch touch;
static audrey::Engine engine;
static audrey::Controls controls(engine, touch);
static Limiter limiter[2];

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  controls.UpdateAudioRate(hw);
  for (size_t i = 0; i < size; i++) {
    engine.Process(IN_L[i], OUT_L[i], OUT_R[i]);
  }
  limiter[0].ProcessBlock(OUT_L, size, 0.7f);
  limiter[1].ProcessBlock(OUT_R, size, 0.7f);
}

int main() {
  hw.Init();
  hw.SetAudioSampleRate(kSampleRate);
  hw.SetAudioBlockSize(kBlockSize);

  touch.Init(hw);
  engine.Init(hw.AudioSampleRate());
  controls.Init(hw);

  for (auto &lim : limiter) {
    lim.Init();
  }

  hw.StartAudio(AudioCallback);

  while (true) {
    controls.UpdateSlowRate(hw);
    hw.DelayMs(4);
  }
}
