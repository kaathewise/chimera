#ifndef CHIMERA_PATCH_H_
#define CHIMERA_PATCH_H_

#include <daisy_seed.h>

#include "eurorack/plaits/dsp/engine/particle_engine.h"
#include "sequencer/controls.h"
#include "sequencer/sequencer.h"
#include "simpletouch/touch.h"
#include "voice/controls.h"
#include "voice/voice.h"

namespace chimera {

class Patch {
 public:
  Patch(simpletouch::Touch& touch)
      : touch_(touch),
        sequencer_controls_(touch),
        voice_controls_(touch),
        voice_(particle_engine_) {}

  void Init(daisy::DaisySeed hw);
  void Process(daisy::AudioHandle::InputBuffer in,
                    daisy::AudioHandle::OutputBuffer out, size_t size);
  void UpdateControls();

 private:
  enum ControlTarget {
    CONTROL_TARGET_SEQUENCER,
    CONTROL_TARGET_VOICE,
  };

  simpletouch::Touch& touch_;
  sequencer::Sequencer sequencer_;
  sequencer::Controls sequencer_controls_;

  voice::Voice voice_;
  voice::Controls voice_controls_;
  plaits::ParticleEngine particle_engine_;

  ControlTarget control_target_ = CONTROL_TARGET_SEQUENCER;

  uint32_t buffer_space_[8192];

  DISALLOW_COPY_AND_ASSIGN(Patch);
};

}  // namespace chimera

#endif  // CHIMERA_PATCH_H_
