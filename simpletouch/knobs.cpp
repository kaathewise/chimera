#include "simpletouch/knobs.h"

#include <daisy_seed.h>

namespace simpletouch {

using daisy::AdcChannelConfig;
using daisy::DaisySeed;

void Knobs::Init(DaisySeed &hw) {
  AdcChannelConfig cfg[kKnobCount];
  cfg[0].InitSingle(daisy::seed::A0);
  cfg[1].InitSingle(daisy::seed::A1);
  cfg[2].InitSingle(daisy::seed::A2);
  cfg[3].InitSingle(daisy::seed::A3);
  cfg[4].InitSingle(daisy::seed::A4);
  cfg[5].InitSingle(daisy::seed::A5);
  cfg[6].InitSingle(daisy::seed::A6);
  cfg[7].InitSingle(daisy::seed::A7);
  hw.adc.Init(cfg, kKnobCount);

  for (auto i = 0; i < kKnobCount; i++) {
    knobs_[i].Init(hw.adc.GetPtr(i), hw.AudioCallbackRate());
  }
}
}  // namespace simpletouch
