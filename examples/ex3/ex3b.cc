//
//
//

#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <madthreading/timer.hh>
#include <madthreading/threading/thread_manager.hh>

using namespace mad;

#include "../Common.hh"

//----------------------------------------------------------------------------//

int main(int argc, char** argv)
{
    ulong_type num_steps = GetEnvNumSteps(500000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_ts sum = 0.0;
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    thread_manager* tm = new thread_manager(num_threads);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_type& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x] (const ulong_type& s, const ulong_type& e)
    {
        double_type tl_sum = 0.0;
        #pragma omp simd
        for(ulong_type i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        return tl_sum;
    };
    //------------------------------------------------------------------------//
    auto join = [&sum] (double_type tl_sum)
    {
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//

    //========================================================================//
    timer::timer t;

    tm->run_loop<double_type>(compute_block, 0, num_steps,
                              num_threads*4, join, 0.0);

    report(num_steps, step*sum, t.stop_and_return(), "pthread_pool_joiner_b");
    //========================================================================//
}
