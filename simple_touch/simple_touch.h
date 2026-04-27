#ifndef SIMPLE_TOUCH_SIMPLE_TOUCH_H_
#define SIMPLE_TOUCH_SIMPLE_TOUCH_H_

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"
#include "simple_touch/knobs.h"
#include "simple_touch/led.h"
#include "simple_touch/pads.h"
#include "simple_touch/switches.h"

namespace simple_touch {

using daisy::DaisySeed;

class SimpleTouch {
 public:
  struct Config {
    daisy::SaiHandle::Config::SampleRate daisy_sample_rate;
    size_t block_size;

    float AudioSampleRate() const {
      switch (daisy_sample_rate) {
        case daisy::SaiHandle::Config::SampleRate::SAI_8KHZ:
          return 8000.f;
        case daisy::SaiHandle::Config::SampleRate::SAI_16KHZ:
          return 16000.f;
        case daisy::SaiHandle::Config::SampleRate::SAI_32KHZ:
          return 32000.f;
        case daisy::SaiHandle::Config::SampleRate::SAI_48KHZ:
          return 48000.f;
        case daisy::SaiHandle::Config::SampleRate::SAI_96KHZ:
          return 96000.f;
        default:
          return 48000.f;
      }
    }

    float AudioCallbackRate() const { return AudioSampleRate() / block_size; }
  };

  explicit SimpleTouch(Config config): config_(config) {}

  ~SimpleTouch() = default;

  void Init() {
    hw_.SetAudioSampleRate(config_.daisy_sample_rate);
    hw_.SetAudioBlockSize(config_.block_size);
    hw_.Init();

    pads_.Init();
    knobs_.Init(hw_);
    switches_.Init();
    led_.Init(hw_);

    hw_.adc.Start();
  }

  void Process() { led_.Process(); }

  Config const &config() const { return config_; }

  DaisySeed &hw() { return hw_; }

  Pads &pads() { return pads_; }

  Knobs &knobs() { return knobs_; }

  Switches &switches() { return switches_; }

  Led &led() { return led_; }

 private:
  DaisySeed hw_;
  Config config_;
  Knobs knobs_;
  Pads pads_;
  Switches switches_;
  Led led_;

  DISALLOW_COPY_AND_ASSIGN(SimpleTouch);
};
}  // namespace simple_touch

#endif  // SIMPLE_TOUCH_SIMPLE_TOUCH_H_
