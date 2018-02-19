//
//
//	Multithreading example using custom MT API
//		- API uses a thread-pool
//		- compute_block function is wrapped into simple "task" classes
//		  distributing work equally among threads
//
//

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

using namespace mad;

//============================================================================//

int main(int, char** argv)
{
    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    atomic_double_t sum = 0.0;
    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);
    thread_pool* tp = new thread_pool(num_threads);
    thread_manager* tm = new thread_manager(tp);
    task_group<void> tg;

    auto compute_block = [&sum, step] (const ulong_t& s, const ulong_t& e)
    {
        auto x = [step] (const ulong_t& i) { return (i-0.5)*step; };
        double_t tl_sum = 0.0;
        pragma_simd()
        for(ulong_t i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        sum += tl_sum;
    };

    //========================================================================//
    mad::timer t;
    t.start();

    tm->run_loop(tg, compute_block, 0, num_steps, num_threads);
    tg.join();

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * sum;

    delete tm;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

//========================================================================//
