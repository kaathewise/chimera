#pragma once

#include "../eurorack/marbles/random/t_generator.h"
#include "../eurorack/marbles/random/random_stream.h"
#include "../eurorack/marbles/random/random_generator.h"

namespace sequencer {

class Sequencer {
public:
    Sequencer() = default;
    void Init(float sampleRate);
    void Process(bool useExternalClock, bool reset, float dejaVu, float rate, float bias, float jitter, int loopLength, bool* gate);
    const marbles::Ramps& Ramps() const { return ramps_; }

private:
    marbles::TGenerator t_generator_;
    marbles::RandomStream random_stream_;
    marbles::RandomGenerator random_generator_;
    marbles::Ramps ramps_;
    float ramp_buffer_[4];
};

}
