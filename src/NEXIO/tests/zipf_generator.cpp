#include "data_distribution.h"
#include <algorithm>
#include <cassert>


// zipfian
TraceZipfian::TraceZipfian(int seed, uint64_t minimum, uint64_t maximum):
    Trace(seed), range_(maximum) {
    gi_ = new GenInfo();
    struct GenInfo_Zipfian * const gz = &(gi_->gen.zipfian);
    
    const uint64_t items = maximum - minimum + 1;
    gz->nr_items = items;
    gz->base = minimum;
    gz->zipfian_constant = ZIPFIAN_CONSTANT;
    gz->theta = ZIPFIAN_CONSTANT;
    gz->zeta2theta = Zeta(2, ZIPFIAN_CONSTANT);
    gz->alpha = 1.0 / (1.0 - ZIPFIAN_CONSTANT);
    double zetan = Zeta(items, ZIPFIAN_CONSTANT);
    gz->zetan = zetan;
    gz->eta = (1.0 - std::pow(2.0 / (double)items, 1.0 - ZIPFIAN_CONSTANT)) / (1.0 - (gz->zeta2theta / zetan));
    gz->countforzeta = items;
    gz->min = minimum;
    gz->max = maximum;

    gi_->type = GEN_ZIPFIAN;
}

double TraceZipfian::Zeta(const uint64_t n, const double theta) {
    // assert(theta == zetalist_theta);
    const uint64_t zlid0 = n / zetalist_step;
    const uint64_t zlid = (zlid0 > zetalist_count) ? zetalist_count : zlid0;
    const double sum0 = zetalist_double[zlid];
    const uint64_t start = zlid * zetalist_step;
    const uint64_t count = n - start;
    assert(n > start);
    const double sum1 = ZetaRange(start, count, theta);
    return sum0 + sum1;
}

double TraceZipfian::ZetaRange(const uint64_t start, const uint64_t count, const double theta) {
    double sum = 0.0;
    if (count > 0x10000000) {
        fprintf(stderr, "zeta_range would take a long time... kill me or wait\n");
    }
    for (uint64_t i = 0; i < count; i++) {
        sum += (1.0 / pow((double)(start + i + 1), theta));
    }
    return sum;
}

uint64_t TraceZipfian::FNVHash64(const uint64_t value) {
    uint64_t hashval = FNV_OFFSET_BASIS_64;
    uint64_t val = value;
    for (int i = 0; i < 8; i++)
    {
        const uint64_t octet=val & 0x00ff;
        val = val >> 8;
        // FNV-1a
        hashval = (hashval ^ octet) * FNV_PRIME_64;
    }
    return hashval;
}

uint64_t TraceZipfian::NextRaw() {
// simplified: no increamental update
    const GenInfo_Zipfian *gz = &(gi_->gen.zipfian);
    const double u = RandomDouble();
    const double uz = u * gz->zetan;
    if (uz < 1.0) {
        return gz->base + 0lu;
    } else if (uz < (1.0 + pow(0.5, gz->theta))) {
        return gz->base + 1lu;
    }
    const double x = ((double)gz->nr_items) * pow(gz->eta * (u - 1.0) + 1.0, gz->alpha);
    const uint64_t ret = gz->base + (uint64_t)x;
    return ret;
}

uint64_t TraceZipfian::Next() {
    // ScrambledZipfian. scatters the "popular" items across the itemspace.
    const uint64_t z = NextRaw();
    const uint64_t xz = gi_->gen.zipfian.min + (FNVHash64(z) % gi_->gen.zipfian.nr_items);
    return xz % range_;
}
