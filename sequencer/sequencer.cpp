#include "sequencer/sequencer.h"

namespace sequencer {

void Sequencer::Init(float callbackRate) {
  random_generator_.Init(0);  // Initialize with a seed
  random_stream_.Init(&random_generator_);
  t_generator_.Init(&random_stream_, callbackRate);
}

void Sequencer::Process(float dejaVu, float frequency, float bias, float jitter,
                        int loopLength) {
  t_generator_.set_deja_vu(dejaVu);
  t_generator_.set_frequency(frequency);
  t_generator_.set_bias(bias);
  t_generator_.set_jitter(jitter);
  t_generator_.set_length(loopLength);

  t_generator_.Process();
}

}  // namespace sequencer
