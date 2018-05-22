
#include "particle_simulator.hpp"
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "particle.hpp"
#include "advance_particles.hpp"
#include <thread>
#include <string>
#include <sstream>

std::string get_context_info()
{
    std::stringstream ss;
    ss << "<CUDA Device=" << 0
       << ", Context=N/A"
       << ", Thread=" << std::this_thread::get_id()
       << ">";
    return ss.str();
}

v3_data simulate_particles(int32_t nitr, int64_t n, int16_t stream)
{
    printf("%s - %s() Launched...\n",
           get_context_info().c_str(), __FUNCTION__);

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
    //cudaDeviceSynchronize();
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        printf("2 %s\n",cudaGetErrorString(error));
        exit(1);
    }

    printf("\t%s - %s() running kernel...\n",
           get_context_info().c_str(), __FUNCTION__);
    for(int i=0; i < nitr; ++i)
    {
        float dt = (*rng::instance())(); // Random distance each step
        advance_particles<<< 1 +  n/64, 64, stream>>>(
                dt, (particle*) devPArray, n);
        error = cudaGetLastError();
        if (error != cudaSuccess)
        {
            printf("3 %s\n",cudaGetErrorString(error));
            exit(1);
        }

        //cudaDeviceSynchronize();
    }
    printf("\t%s - %s() finished kernel...\n",
           get_context_info().c_str(), __FUNCTION__);

    cudaMemcpy(pArray, devPArray, n*sizeof(particle), cudaMemcpyDeviceToHost);

    v3 total_distance(0,0,0);
    v3 temp;
    for(int i=0; i<n; i++)
    {
        temp = pArray[i].getTotalDistance();
        total_distance.x += temp.x;
        total_distance.y += temp.y;
        total_distance.z += temp.z;
    }

    printf("%s - %s() Finished...\n",
           get_context_info().c_str(), __FUNCTION__);

    v3_data _data;
    _data.num_entries = n;
    _data.total_distance = total_distance;

    return _data;
}
