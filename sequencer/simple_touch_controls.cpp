#include "sequencer/simple_touch_controls.h"

namespace sequencer {

__attribute__((
    section(".backup_sram"))) static PersistentSettings persistent_settings;

SimpleTouchControls::SimpleTouchControls(SimpleTouch& touch)
    : touch_(touch),
      deja_vu_(touch, persistent_settings.deja_vu),
      rate_(touch, persistent_settings.rate),
      bias_(touch, persistent_settings.bias),
      jitter_(touch, persistent_settings.jitter),
      loop_length_(touch, persistent_settings.loop_length) {
  constexpr uint32_t kMagic = 0x53455155;  // "SEQU"
  if (persistent_settings.magic != kMagic) {
    persistent_settings.magic = kMagic;
    persistent_settings.deja_vu = .5f;
    persistent_settings.rate = .5f;
    persistent_settings.bias = .5f;
    persistent_settings.jitter = .0f;
    persistent_settings.loop_length = .5f;
  }
}

void SimpleTouchControls::Process() {
  deja_vu_.Process(touch_.knobs().s36().GetRawFloat());
  rate_.Process(touch_.knobs().s30().GetRawFloat());
  bias_.Process(touch_.knobs().s32().GetRawFloat());
  jitter_.Process(touch_.knobs().s31().GetRawFloat());
  loop_length_.Process(touch_.knobs().s37().GetRawFloat());
}

}  // namespace sequencer
