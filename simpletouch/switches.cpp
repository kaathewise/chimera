#include "simpletouch/switches.h"

namespace simpletouch {

void Switches::Init() {
  switch_7_8_.Init(daisy::seed::D7, daisy::seed::D6);
  switch_9_10_.Init(daisy::seed::D9, daisy::seed::D8);
}

int Switches::left() { return switch_7_8_.Read(); }

int Switches::right() { return switch_9_10_.Read(); }
}  // namespace simpletouch
