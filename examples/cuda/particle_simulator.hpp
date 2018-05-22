
#include "particle.hpp"
#include "advance_particles.hpp"
#include "randomize.hpp"
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cstdint>

struct v3_data
{
    int num_entries;
    v3 total_distance;
};

v3_data simulate_particles(int32_t nitr, int64_t n, int16_t stream);
