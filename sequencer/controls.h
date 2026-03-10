#pragma once

#include "../simpletouch/touch.h"

namespace sequencer {

class Controls {
 public:
  Controls(simpletouch::Touch& touch)
      : touch_(touch) {}

  void Process();

  float deja_vu() const { return deja_vu_; }
  float rate() const { return rate_; }
  float bias() const { return bias_; }
  float jitter() const { return jitter_; }
  int loop_length() const { return loop_length_; }

 private:
  simpletouch::Touch& touch_;
  float deja_vu_;
  float rate_;
  float bias_;
  float jitter_;
  int loop_length_;
};

}  // namespace sequencer
