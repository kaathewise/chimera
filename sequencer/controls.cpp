#include "sequencer/controls.h"

namespace sequencer {

void Controls::Process() {
  deja_vu_.Process(touch_.knobs().s36().GetRawFloat());
  rate_.Process(touch_.knobs().s30().GetRawFloat());
  bias_.Process(touch_.knobs().s32().GetRawFloat());
  jitter_.Process(touch_.knobs().s31().GetRawFloat());
  loop_length_.Process(touch_.knobs().s37().GetRawFloat());
}

}  // namespace sequencer
