#include "voice/simple_touch_controls.h"

namespace voice {

__attribute__((
    section(".backup_sram"))) static PersistentSettings persistent_settings;

SimpleTouchControls::SimpleTouchControls(SimpleTouch& touch, float max_delay_time)
    : touch_(touch),
      max_delay_time_(max_delay_time),
      note_(touch, persistent_settings.note),
      harmonics_(touch, persistent_settings.harmonics),
      timbre_(touch, persistent_settings.timbre),
      morph_(touch, persistent_settings.morph),
      accent_(touch, persistent_settings.accent),
      delay_time_(touch, persistent_settings.delay_time),
      delay_feedback_(touch, persistent_settings.delay_feedback) {
  constexpr uint32_t kMagic = 0x564F4943;  // "VOIC"
  if (persistent_settings.magic != kMagic) {
    persistent_settings.magic = kMagic;
    persistent_settings.note = .5f;
    persistent_settings.harmonics = .5f;
    persistent_settings.timbre = .5f;
    persistent_settings.morph = .5f;
    persistent_settings.accent = .8f;
    persistent_settings.delay_time = .2f;
    persistent_settings.delay_feedback = .0f;
  }
}

void SimpleTouchControls::Process() {
  note_.Process(touch_.knobs().s30().GetRawFloat());
  harmonics_.Process(touch_.knobs().s31().GetRawFloat());
  timbre_.Process(touch_.knobs().s32().GetRawFloat());
  morph_.Process(touch_.knobs().s33().GetRawFloat());
  accent_.Process(touch_.knobs().s34().GetRawFloat());
  delay_feedback_.Process(touch_.knobs().s35().GetRawFloat());
  delay_time_.Process(touch_.knobs().s37().GetRawFloat());
}

}  // namespace voice
