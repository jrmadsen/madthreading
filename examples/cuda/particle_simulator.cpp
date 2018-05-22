
#include "particle_simulator.hpp"
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "particle.hpp"
#include "advance_particles.hpp"

v3 simulate_particles(int64_t n, int16_t stream)
{
    cudaError_t error;
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        printf("0 %s\n", cudaGetErrorString(error));
        exit(1);
    }

    particle* pArray = new particle[n];
    void* devPArray = NULL;
    cudaMalloc(&devPArray, n*sizeof(particle));
    cudaDeviceSynchronize();
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        printf("1 %s\n", cudaGetErrorString(error));
        exit(1);
    }

    cudaMemcpy(devPArray, pArray, n*sizeof(particle), cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        printf("2 %s\n",cudaGetErrorString(error));
        exit(1);
    }

    for(int i=0; i<500; i++)
    {
        float dt = (*rng::instance())(); // Random distance each step
        //advance_particles<<< 1 +  n/256, 256>>>(dt,
        //                                       (particle*) devPArray,
        //                                       n);
        error = cudaGetLastError();
        if (error != cudaSuccess)
        {
            printf("3 %s\n",cudaGetErrorString(error));
            exit(1);
        }

        cudaDeviceSynchronize();
    }
    cudaMemcpy(pArray, devPArray, n*sizeof(particle), cudaMemcpyDeviceToHost);

    v3 totalDistance(0,0,0);
    v3 temp;
    for(int i=0; i<n; i++)
    {
        temp = pArray[i].getTotalDistance();
        totalDistance.x += temp.x;
        totalDistance.y += temp.y;
        totalDistance.z += temp.z;
    }
    return totalDistance;
}
