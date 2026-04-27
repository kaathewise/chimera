#include <daisy_seed.h>

#include "audrey/engine.h"
#include "audrey/simple_touch_controls.h"
#include "simple_touch/simple_touch.h"

using daisy::AudioHandle;
using daisy::DaisySeed;
using daisy::SaiHandle;
using daisysp::Limiter;
using simple_touch::SimpleTouch;

SimpleTouch::Config config{
  .daisy_sample_rate = SaiHandle::Config::SampleRate::SAI_48KHZ,
  .block_size = 4
};
SimpleTouch touch(config);
audrey::Engine engine;
audrey::SimpleTouchControls controls(touch);
Limiter limiter[2];

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  touch.led().Process();
  controls.Process();
  const audrey::EngineParameters parameters = controls.GetEngineParameters();
  std::fill_n(OUT_L, size, 0.0f);
  std::fill_n(OUT_R, size, 0.0f);

  for (size_t i = 0; i < size; i++) {
    engine.Process(parameters, IN_L[i], OUT_L[i], OUT_R[i]);
  }
  limiter[0].ProcessBlock(OUT_L, size, 0.7f);
  limiter[1].ProcessBlock(OUT_R, size, 0.7f);
}

int main() {
  touch.Init();
  engine.Init(config.AudioSampleRate());
  controls.Init();
  controls.Attach();

  for (auto &lim : limiter) {
    lim.Init();
  }

  touch.hw().StartAudio(AudioCallback);

  while (true) {
    touch.pads().Process();
    controls.UpdateSlowRate();
    daisy::System::Delay(10);
  }
}
