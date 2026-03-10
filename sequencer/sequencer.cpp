#include "sequencer.h"

namespace sequencer {

void Sequencer::Init(float sampleRate) {
    random_generator_.Init(0); // Initialize with a seed
    random_stream_.Init(&random_generator_);
    t_generator_.Init(&random_stream_, sampleRate);

    // Initialize ramps pointers
    ramps_.master = &ramp_buffer_[0];
    ramps_.external = &ramp_buffer_[1];
    ramps_.slave[0] = &ramp_buffer_[2];
    ramps_.slave[1] = &ramp_buffer_[3];
}

void Sequencer::Process(float dejaVu, float frequency, float bias, float jitter, int loopLength, bool* gate) {
    t_generator_.set_deja_vu(dejaVu);
    t_generator_.set_frequency(frequency);
    t_generator_.set_bias(bias);
    t_generator_.set_jitter(jitter);
    t_generator_.set_length(loopLength);

    t_generator_.Process(ramps_, gate);
}

}
