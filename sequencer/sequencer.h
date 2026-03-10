#ifndef SEQUENCER_SEQUENCER_H_
#define SEQUENCER_SEQUENCER_H_

#include "t_generator.h"
#include "../eurorack/marbles/random/random_stream.h"
#include "../eurorack/marbles/random/random_generator.h"

namespace sequencer {

class Sequencer {
public:
    Sequencer() = default;
    void Init(float sampleRate);
    void Process(float dejaVu, float frequency, float bias, float jitter, int loopLength, bool* gate);
    const Ramps& ramps() const { return ramps_; }

private:
    TGenerator t_generator_;
    marbles::RandomStream random_stream_;
    marbles::RandomGenerator random_generator_;
    Ramps ramps_;
    float ramp_buffer_[3];
};

}

#endif  // SEQUENCER_SEQUENCER_H_
