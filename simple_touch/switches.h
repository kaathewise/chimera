#ifndef SIMPLE_TOUCH_SWITCHES_H_
#define SIMPLE_TOUCH_SWITCHES_H_

#include <daisy_seed.h>

#include "eurorack/stmlib/stmlib.h"

namespace simple_touch {

using daisy::Switch3;

class Switches {
 public:
  Switches() = default;

  ~Switches() = default;

  void Init();

  int left();

  int right();

 private:
  Switch3 switch_7_8_;
  Switch3 switch_9_10_;

  DISALLOW_COPY_AND_ASSIGN(Switches);
};
}  // namespace simple_touch

#endif  // SIMPLE_TOUCH_SWITCHES_H_
