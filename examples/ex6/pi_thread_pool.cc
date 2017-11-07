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
#include <madthreading/utility/timer.hh>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

using namespace mad;

//============================================================================//

int main(int argc, char** argv)
{
    ulong_type num_steps = GetEnvNumSteps(500000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_ts sum = 0.0;
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    thread_manager* tm = new thread_manager(num_threads);

    auto compute_block = [&sum, step] (const ulong_type& s, const ulong_type& e)
    {
        auto x = [step] (const ulong_type& i) { return (i-0.5)*step; };
        double_type tl_sum = 0.0;
        pragma_simd
        for(ulong_type i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        sum += tl_sum;
    };

    //========================================================================//
    timer::timer t;

    tm->run_loop(compute_block, 0, num_steps, num_threads);
    tm->join();

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_type pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

//========================================================================//
