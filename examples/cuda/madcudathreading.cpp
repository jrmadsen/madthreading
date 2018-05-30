//
//
//

#include <iterator>
#include <list>
#include <vector>
#include <deque>
#include <algorithm>
#include <memory>
#include <functional>
#include <random>

#include <madthreading/types.hh>
#include <madthreading/threading/thread_manager.hh>
#include <madthreading/threading/auto_lock.hh>
#include <madthreading/threading/threading.hh>

#include <timemory/timer.hpp>
#include <timemory/auto_timer.hpp>
#include <timemory/manager.hpp>

#include "thread_context.hpp"
#include "particle.hpp"
#include "v3.hpp"
#include "particle_simulator.hpp"

using namespace std::placeholders;
using mad::ulong_t;

typedef std::mt19937_64 engine_type;

static ulong_t rseed = time(NULL);
static engine_type random_engine = engine_type(rseed);

//============================================================================//

#define print_here(str) printf("\n##### %s -- %s @ %s:%i\n\n", str, __FUNCTION__, __FILE__, __LINE__)

//============================================================================//

mad::task_group<int>
run_thread_context(int device_count, int num_threads, int num_iter)
{
    mad::thread_manager* tm = mad::thread_manager::instance();
    CUDAContextData cuda_data =
            get_cuda_context(device_count, num_threads, "kernelFunction");
    auto join = [] (int& ref, const int& i) { ref += i; return ref; };
    mad::task_group<int> tg(join);

    print_here("running task_group #1...");
    for(int i = 0; i < num_iter; ++i)
        tm->exec(tg, thread_proc, &(cuda_data.cpu[i % num_threads]));

    return tg;
}

//============================================================================//

mad::task_group<v3_data>
run_particle_simulator(int num_threads, int num_iter, int64_t n)
{
    mad::thread_manager* tm = mad::thread_manager::instance();
    auto join = [] (v3_data& ref, const v3_data& rhs)
    {
        ref.num_entries += rhs.num_entries;
        ref.total_distance += rhs.total_distance;
        return ref;
    };
    mad::task_group<v3_data> tg(join);

    print_here("running task_group #2...");
    for(int i = 0; i < num_iter; ++i)
        tm->exec(tg, simulate_particles, 500, n/num_iter, i % num_threads + 1);

    return tg;
}

//============================================================================//

int main(int argc, char** argv)
{
    int niter = 4;
    if(argc > 1)
        niter = atoi(argv[argc-1]);

    int device_count = 0;
    CUresult status;
    status = cuInit(0);

    if (CUDA_SUCCESS != status)
    {
        printf("CUDA init failed!\n");
        return status;
    }
    status = cuDeviceGetCount(&device_count);

    if(CUDA_SUCCESS != status)
    {
        printf("Failure finding CUDA devices!\n");
        return status;
    }

    printf("CUDA device count == %i\n", device_count);
    if(device_count == 0)
        return 0;

    auto hw_concurr = std::thread::hardware_concurrency();
    auto num_threads = mad::thread_manager::GetEnvNumThreads(2*hw_concurr);
    mad::thread_pool* tp = new mad::thread_pool(num_threads);
    mad::thread_manager* tm = new mad::thread_manager(tp);

    tmcout << "Number of threads: " << tm->size() << std::endl;

    get_args() = { argc, argv };

    int64_t npart = 5000000;
    print_here("submitting task_group #1...");
    mad::task_group<int> itg = run_thread_context(device_count, num_threads, niter);
    print_here("submitting task_group #2...");
    mad::task_group<v3_data> ptg = run_particle_simulator(num_threads, niter, npart);

    int answer = 0;
    v3_data v3_result;
    v3_result.num_entries = 0;
    v3_result.total_distance = v3(0.0, 0.0, 0.0);

    print_here("joining task_group #1...");
    answer = itg.join(answer);
    print_here("joining task_group #2...");
    v3_result = ptg.join(v3_result);

    float avgX = v3_result.total_distance.x /(float) npart;
    float avgY = v3_result.total_distance.y /(float) npart;
    float avgZ = v3_result.total_distance.z /(float) npart;
    float avgNorm = sqrt(avgX*avgX + avgY*avgY + avgZ*avgZ);
    printf(	"Moved %i particles 500 steps. Average distance traveled is |(%f, %f, %f)| = %f\n",
                    v3_result.num_entries, avgX, avgY, avgZ, avgNorm);
    tmcout << "Answer: " << answer << std::endl;

    delete tm;
    return 0;
}

