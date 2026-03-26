#include <daisy_seed.h>

#include "audrey/engine.h"
#include "audrey/simpletouch_controls.h"
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
static audrey::SimpletouchControls controls(touch);
static Limiter limiter[2];

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.led().Process();
  controls.Process();
  const audrey::EngineParameters parameters = controls.GetEngineParameters();
  std::fill(OUT_L, OUT_L + size, 0.0f);
  std::fill(OUT_R, OUT_R + size, 0.0f);

  for (size_t i = 0; i < size; i++) {
    engine.Process(parameters, IN_L[i], OUT_L[i], OUT_R[i]);
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
  controls.Init();
  controls.Attach();

  for (auto &lim : limiter) {
    lim.Init();
  }

  hw.StartAudio(AudioCallback);

  while (true) {
    touch.pads().Process();
    controls.UpdateSlowRate();
    hw.DelayMs(4);
  }
}
