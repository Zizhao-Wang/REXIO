#include "data_distribution.h"

// uniform
TraceUniform::TraceUniform(int seed, uint64_t minimum, uint64_t maximum):
    Trace(seed) {
    gi_ = new GenInfo();
    gi_->gen.uniform.min = minimum;
    gi_->gen.uniform.max = maximum;
    gi_->gen.uniform.interval = (double)(maximum - minimum);
    gi_->type = GEN_UNIFORM;
}

uint64_t TraceUniform::Next() {
    const uint64_t off = (uint64_t)(RandomDouble() * gi_->gen.uniform.interval);
    return gi_->gen.uniform.min + off;
}
