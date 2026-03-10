#include "controls.h"

namespace sequencer {

void Controls::Process() {
  // Process all knobs first
  for (auto &knob : touch_.knobs().knobs()) {
    knob.Process();
  }

  deja_vu_ = touch_.knobs().s36().Value();
  rate_ = touch_.knobs().s30().Value();
  bias_ = touch_.knobs().s32().Value();
  jitter_ = touch_.knobs().s31().Value();
  float loop_length_param = touch_.knobs().s37().Value();

  // Map loop length parameter to integer range [1, 16]
  loop_length_ = static_cast<int>(loop_length_param * 15.0f) + 1;
}

}  // namespace sequencer
