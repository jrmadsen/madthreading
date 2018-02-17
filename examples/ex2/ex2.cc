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

typedef const double_type& cdoubleref;

//----------------------------------------------------------------------------//

int main(int, char**)
{
    ulong_type num_steps = GetEnvNumSteps(500000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_ts sum = 0.0;
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    thread_pool* tp = new thread_pool(num_threads);
    thread_manager* tm = new thread_manager(tp);

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
    auto accum = [&] (double_type& a, const double_type& b) { return a + b; };
    //------------------------------------------------------------------------//

    //========================================================================//
    mad::timer t;
    t.start();

    mad::task_group<double_type> tg(accum);
    tm->run_loop(tg, compute_block, 0, num_steps, num_threads);
    sum = tg.join();

    report(num_steps, step*sum, t.stop_and_return(), "mad_thread_pool");
    //========================================================================//

    delete tm;
}

