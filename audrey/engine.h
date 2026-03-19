
#ifndef AUDREY_ENGINE_H_
#define AUDREY_ENGINE_H_

#include <daisysp.h>

#include <memory>

#include "audrey/biquad_filters.h"
#include "audrey/echo_delay.h"
#include "audrey/env.h"
#include "audrey/karplus_string.h"

#ifdef __arm__
#include <dev/sdram.h>
#endif  // __arm__

namespace audrey {

enum class TriggerState { RISING_EDGE, FALLING_EDGE, UNKNOWN };

struct EngineParameters {
  float string_pitch;
  float feedback_gain;
  float feedback_delay;
  float feedback_lpf_cutoff;
  float feedback_hpf_cutoff;
  float echo_delay_time;
  float echo_delay_feedback;
  float echo_delay_send_amount;
  float reverb_mix;
  float reverb_feedback;
  float output_level;
  float input_level;
  float shape;
  bool drone_mode;
  TriggerState trigger;
};

class Engine {
 public:
  Engine() = default;
  ~Engine() = default;

  void Init(const float sample_rate);

  void Process(EngineParameters params, float in, float &outL, float &outR);

 private:
  // long enough for 250ms at 48kHz
  static constexpr size_t kMaxFeedbackDelaySamp = 12000;
  // long enough for 5s at 48kHz
  static constexpr size_t kMaxEchoDelaySamp = 48000 * 5;

  float sample_rate_;

  float fb_delay_smooth_coef_;
  float fb_delay_samp_ = 1000.f;

  using KarplusStringPtr = std::unique_ptr<KarplusString>;
  KarplusStringPtr strings_[2];
  daisysp::WhiteNoise noise_;

  using FeedbackDelayLinePtr =
      std::unique_ptr<daisysp::DelayLine<float, kMaxFeedbackDelaySamp>>;
  FeedbackDelayLinePtr fb_delayline_[2];

  daisysp::Overdrive overdrive_[2];
  Envelope _env;

  LPF12 fb_lpf_;
  HPF12 fb_hpf_;

  using VerbPtr = std::unique_ptr<daisysp::ReverbSc>;
  VerbPtr verb_;

  using EchoDelayPtr = std::unique_ptr<EchoDelay<kMaxEchoDelaySamp>>;
  EchoDelayPtr echo_delay_[2];

  Engine(const Engine &other) = delete;
  Engine(Engine &&other) = delete;
  Engine &operator=(const Engine &other) = delete;
  Engine &operator=(Engine &&other) = delete;
};

}  // namespace audrey

#endif  // AUDREY_ENGINE_H_
