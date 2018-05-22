#include "particle.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <cuda.h>
#include <cuda_runtime_api.h>

__global__ void advance_particles(float dt, particle* pArray, int nParticles);
