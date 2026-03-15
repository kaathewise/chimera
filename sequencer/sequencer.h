#ifndef SEQUENCER_SEQUENCER_H_
#define SEQUENCER_SEQUENCER_H_

#include "eurorack/marbles/random/random_generator.h"
#include "eurorack/marbles/random/random_stream.h"
#include "eurorack/stmlib/stmlib.h"
#include "sequencer/t_generator.h"

namespace sequencer {

class Sequencer {
 public:
  Sequencer() = default;
  void Init(float callbackRate);
  void Process(float dejaVu, float frequency, float bias, float jitter,
               int loopLength);
  const Ramps& ramps() const { return t_generator_.ramps(); }
  const Triggers& triggers() const { return t_generator_.triggers(); }

 private:
  TGenerator t_generator_;
  marbles::RandomStream random_stream_;
  marbles::RandomGenerator random_generator_;

  DISALLOW_COPY_AND_ASSIGN(Sequencer);
};

}  // namespace sequencer

#endif  // SEQUENCER_SEQUENCER_H_
