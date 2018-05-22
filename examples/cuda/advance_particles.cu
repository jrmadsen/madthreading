
#include "particle.hpp"
#include <stdlib.h>
#include <stdio.h>

__global__ void advance_particles(float dt, particle* pArray, int nParticles)
{
    int idx = threadIdx.x + blockIdx.x*blockDim.x;
    if(idx < nParticles)
        pArray[idx].advance(dt);
}
