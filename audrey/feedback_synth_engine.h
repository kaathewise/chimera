
#ifndef AUDREY_FEEDBACK_SYNTH_ENGINE_H_
#define AUDREY_FEEDBACK_SYNTH_ENGINE_H_

#include <daisysp.h>

#include <memory>

#include "audrey/biquad_filters.h"
#include "audrey/echo_delay.h"
#include "audrey/env.h"
#include "audrey/karplus_string.h"

#ifdef __arm__
#include <dev/sdram.h>
#endif  // __arm__

namespace infrasonic {
namespace FeedbackSynth {

class Engine {
 public:
  Engine() = default;
  ~Engine() = default;

  void Init(const float sample_rate);

  void SetStringPitch(const float nn);

  void SetFeedbackGain(const float gain_dbfs);

  void SetFeedbackDelay(const float delay_s);
  void SetFeedbackLPFCutoff(const float cutoff_hz);
  void SetFeedbackHPFCutoff(const float cutoff_hz);

  void SetEchoDelayTime(const float echo_time);
  void SetEchoDelayFeedback(const float echo_fb);
  void SetEchoDelaySendAmount(const float echo_send);

  // Both range 0-1
  void SetReverbMix(const float mix);
  void SetReverbFeedback(const float time);

  void SetOutputLevel(const float level);
  void SetInputLevel(const float level);

  void NoteOn();
  void NoteOff();
  void SetShape(const float shape);
  void DroneMode(bool mode);

  void Process(float in, float &outL, float &outR);

 private:
  // long enough for 250ms at 48kHz
  static constexpr size_t kMaxFeedbackDelaySamp = 12000;
  // long enough for 5s at 48kHz
  static constexpr size_t kMaxEchoDelaySamp = 48000 * 5;

  float sample_rate_;
  float fb_gain_ = 0.0f;
  float echo_send_ = 0.0f;
  float verb_mix_ = 0.0f;
  float output_level_ = 0.5f;
  float input_level_ = 1.0f;
  bool drone = false;

  float fb_delay_smooth_coef_;
  float fb_delay_samp_ = 1000.f;
  float fb_delay_samp_target_ = 64.f;

  using KarplusStringPtr = std::unique_ptr<infrasonic::KarplusString>;
  KarplusStringPtr strings_[2];
  daisysp::WhiteNoise noise_;

  using FeedbackDelayLinePtr =
      std::unique_ptr<daisysp::DelayLine<float, kMaxFeedbackDelaySamp>>;
  FeedbackDelayLinePtr fb_delayline_[2];

  daisysp::Overdrive overdrive_[2];
  synthux::Envelope _env;

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

}  // namespace FeedbackSynth
}  // namespace infrasonic

#endif  // AUDREY_FEEDBACK_SYNTH_ENGINE_H_
