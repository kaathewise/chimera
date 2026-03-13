// Copyright 2026 Svyatoslav Usachev (kaathewise@gmail.com)
//
// Based on: https://github.com/pichenettes/eurorack/tree/master/marbles
// Original Copyright 2015 Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Generator for the T outputs.

#ifndef SEQUENCER_T_GENERATOR_H_
#define SEQUENCER_T_GENERATOR_H_

#include "eurorack/marbles/ramp/ramp.h"
#include "eurorack/marbles/ramp/ramp_divider.h"
#include "eurorack/marbles/ramp/ramp_generator.h"
#include "eurorack/marbles/random/random_sequence.h"
#include "eurorack/stmlib/dsp/hysteresis_quantizer.h"
#include "eurorack/stmlib/stmlib.h"

namespace sequencer {

using marbles::RampGenerator;
using marbles::RandomSequence;
using marbles::RandomStream;
using marbles::Ratio;

class SlaveRamp {
 public:
  SlaveRamp() = default;
  ~SlaveRamp() = default;

  void Init() {
    phase_ = 0.0f;
    max_phase_ = marbles::kMaxRampValue;
    ratio_ = 1.0f;
    target_ = 1.0f;
    bernoulli_ = false;
    must_complete_ = false;
  }

  void Reset() {
    Init();
    phase_ = 1.0f;
  }

  void Init(int pattern_length, Ratio ratio) {
    bernoulli_ = false;
    phase_ = 0.0f;
    max_phase_ = static_cast<float>(pattern_length) * marbles::kMaxRampValue;
    ratio_ = ratio.to_float();
    target_ = 1.0f;
  }

  void Init(bool must_complete, float expected_value) {
    bernoulli_ = true;
    if (must_complete_) {
      phase_ = 0.0f;
      ratio_ = 1.0f;
    }

    if (!must_complete) {
      ratio_ = (1.0f - phase_) * expected_value;
    } else {
      ratio_ = 1.0f - phase_;
    }
    must_complete_ = must_complete;
  }

  void Process(float frequency, float* phase, bool* trigger) {
    float output_phase;
    *trigger = false;
    if (bernoulli_) {
      float previous_phase = phase_;
      phase_ += frequency * ratio_;
      output_phase = phase_;
      if (output_phase >= 1.0f) {
        output_phase = 1.0f;
      }
      if (previous_phase < 1.0f && phase_ >= 1.0f) {
        *trigger = true;
      }
    } else {
      phase_ += frequency;
      if (phase_ >= max_phase_) {
        phase_ = max_phase_;
      }
      output_phase = phase_ * ratio_;
      if (output_phase > target_) {
        *trigger = true;
        target_ += 1.0f;
      }
      output_phase -= static_cast<float>(static_cast<int>(output_phase));
    }
    *phase = output_phase;
  }

 private:
  float phase_;
  float max_phase_;
  float ratio_;
  float target_;

  bool bernoulli_;
  bool must_complete_;

  DISALLOW_COPY_AND_ASSIGN(SlaveRamp);
};

enum TGeneratorModel {
  T_GENERATOR_MODEL_COMPLEMENTARY_BERNOULLI,
  T_GENERATOR_MODEL_CLUSTERS,
  T_GENERATOR_MODEL_DRUMS,

  T_GENERATOR_MODEL_INDEPENDENT_BERNOULLI,
  T_GENERATOR_MODEL_DIVIDER,
  T_GENERATOR_MODEL_THREE_STATES,

  T_GENERATOR_MODEL_MARKOV,
};

enum TGeneratorRange {
  T_GENERATOR_RANGE_0_25X,
  T_GENERATOR_RANGE_1X,
  T_GENERATOR_RANGE_4X,
};

const size_t kNumTChannels = 2;
const size_t kMarkovHistorySize = 16;
const size_t kNumDrumPatterns = 18;
const size_t kDrumPatternSize = 8;

struct DividerPattern {
  Ratio ratios[kNumTChannels];
  int32_t length;
};

struct Ramps {
  float master;
  float slave[kNumTChannels];
};

struct Triggers {
  bool master;
  bool slave[kNumTChannels];
};

const size_t kNumDividerPatterns = 17;
const size_t kNumInputDividerRatios = 9;

class TGenerator {
 public:
  TGenerator() = default;

  ~TGenerator() = default;

  void Init(RandomStream* random_stream, float sr);

  void Process();

  void set_model(TGeneratorModel model) { model_ = model; }

  void set_range(TGeneratorRange range) { range_ = range; }

  void set_frequency(float frequency) { frequency_ = frequency; }

  void set_bias(float bias) { bias_ = bias; }

  void set_jitter(float jitter) { jitter_ = jitter; }

  void set_deja_vu(float deja_vu) { sequence_.set_deja_vu(deja_vu); }

  void set_length(int length) { sequence_.set_length(length); }

  const Ramps& ramps() const { return ramps_; }

  const Triggers& triggers() const { return triggers_; }

 private:
  union RandomVector {
    struct {
      float u[kNumTChannels];
      float p;
      float jitter;
    } variables;
    float x[kNumTChannels + 2];
  };

  void ConfigureSlaveRamps(const RandomVector& v);
  int GenerateComplementaryBernoulli(const RandomVector& v);
  int GenerateIndependentBernoulli(const RandomVector& v);
  int GenerateThreeStates(const RandomVector& v);
  int GenerateDrums(const RandomVector& v);
  int GenerateMarkov(const RandomVector& v);
  void ScheduleOutputPulses(const RandomVector& v, int bitmask);

  float one_hertz_;

  TGeneratorModel model_;
  TGeneratorRange range_;

  float frequency_;
  float bias_;
  float jitter_;

  float master_phase_;
  float jitter_multiplier_;
  float phase_difference_;

  int32_t divider_pattern_length_;
  int32_t streak_counter_[kMarkovHistorySize];
  int32_t markov_history_[kMarkovHistorySize];
  int32_t markov_history_ptr_;
  size_t drum_pattern_step_;
  size_t drum_pattern_index_;

  RandomSequence sequence_;
  RampGenerator ramp_generator_;

  Ramps ramps_;
  Triggers triggers_;

  SlaveRamp slave_ramp_[kNumTChannels];

  stmlib::HysteresisQuantizer2 bias_quantizer_;

  static DividerPattern divider_patterns[kNumDividerPatterns];
  static DividerPattern fixed_divider_patterns[kNumDividerPatterns];
  static Ratio input_divider_ratios[kNumInputDividerRatios];
  static uint8_t drum_patterns[kNumDrumPatterns][kDrumPatternSize];

  DISALLOW_COPY_AND_ASSIGN(TGenerator);
};

}  // namespace sequencer

#endif  // SEQUENCER_T_GENERATOR_H_
