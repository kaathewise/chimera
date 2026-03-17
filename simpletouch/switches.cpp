#include "simpletouch/switches.h"

namespace simpletouch {

void Switches::Init() {
  switch_7_8_.Init(daisy::seed::D7, daisy::seed::D6);
  switch_9_10_.Init(daisy::seed::D9, daisy::seed::D8);
}

int Switches::s7s8() { return switch_7_8_.Read(); }

int Switches::s9s10() { return switch_9_10_.Read(); }
}  // namespace simpletouch
