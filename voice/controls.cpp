#include "voice/controls.h"

namespace voice {

void Controls::Process() {
  note_.Process(touch_.knobs().s30().GetRawFloat());
  harmonics_.Process(touch_.knobs().s31().GetRawFloat());
  timbre_.Process(touch_.knobs().s32().GetRawFloat());
  morph_.Process(touch_.knobs().s33().GetRawFloat());
  accent_.Process(touch_.knobs().s34().GetRawFloat());
  overdrive_.Process(touch_.knobs().s36().GetRawFloat());
}

}  // namespace voice
