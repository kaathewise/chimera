#ifndef CHIMERA_PATCH_H_
#define CHIMERA_PATCH_H_

#include <daisy_seed.h>

#include "dev/sdram.h"
#include "eurorack/plaits/dsp/engine/particle_engine.h"
#include "sequencer/sequencer.h"
#include "sequencer/simpletouch_controls.h"
#include "simpletouch/touch.h"
#include "voice/simpletouch_controls.h"
#include "voice/voice.h"

namespace chimera {

class Patch {
 public:
  explicit Patch(simpletouch::Touch& touch)
      : touch_(touch),
        sequencer_simpletouch_controls_(touch),
        voice_simpletouch_controls_(touch),
        voice_(particle_engine_) {}

  void Init(daisy::DaisySeed hw);
  void Process(daisy::AudioHandle::InputBuffer in,
               daisy::AudioHandle::OutputBuffer out, size_t size);
  void UpdateSimpleTouchControls();

 private:
  enum ControlTarget {
    CONTROL_TARGET_SEQUENCER,
    CONTROL_TARGET_VOICE,
  };

  simpletouch::Touch& touch_;
  sequencer::SimpleTouchControls sequencer_simpletouch_controls_;
  voice::SimpleTouchControls voice_simpletouch_controls_;

  plaits::ParticleEngine particle_engine_;
  voice::Voice voice_;
  sequencer::Sequencer sequencer_;

  ControlTarget control_target_ = CONTROL_TARGET_SEQUENCER;

  uint32_t buffer_space_[8192];

  static float DSY_SDRAM_BSS delay_buffer[240000];

  DISALLOW_COPY_AND_ASSIGN(Patch);
};

}  // namespace chimera

#endif  // CHIMERA_PATCH_H_
