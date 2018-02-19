//
//
//

#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>

using namespace mad;

#include "../Common.hh"

typedef const double_t& cdoubleref;

//----------------------------------------------------------------------------//

int main(int, char**)
{
    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    atomic_double_t sum = 0.0;
    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);
    thread_pool* tp = new thread_pool(num_threads);
    thread_manager* tm = new thread_manager(tp);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_t& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x] (const ulong_t& s, const ulong_t& e)
    {
        double_t tl_sum = 0.0;
        #pragma omp simd
        for(ulong_t i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        return tl_sum;
    };
    //------------------------------------------------------------------------//
    auto accum = [&] (double_t& a, const double_t& b) { return a + b; };
    //------------------------------------------------------------------------//

    //========================================================================//
    mad::timer t;
    t.start();

    mad::task_group<double_t> tg(accum);
    tm->run_loop(tg, compute_block, 0, num_steps, num_threads);
    sum = tg.join();

    report(num_steps, step*sum, t.stop_and_return(), "mad_thread_pool");
    //========================================================================//

    delete tm;
}

