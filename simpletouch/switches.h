#ifndef SIMPLETOUCH_SWITCHES_H_
#define SIMPLETOUCH_SWITCHES_H_

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"

namespace simpletouch {

using daisy::Switch3;

class Switches {
 public:
  Switches() = default;

  ~Switches() = default;

  void Init();

  int s7s8();

  int s9s10();

 private:
  Switch3 switch_7_8_;
  Switch3 switch_9_10_;

  DISALLOW_COPY_AND_ASSIGN(Switches);
};
}  // namespace simpletouch

#endif  // SIMPLETOUCH_SWITCHES_H_
