//
//
//	Multithreading example using C++11 threads
//
//

#include <iostream>
#include <iomanip>
#include <thread>

#include <madthreading/types.hh>
#include <madthreading/utility/timer.hh>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

using namespace mad;

typedef atomic<double_type>     atomic_double_type;

int main(int, char** argv)
{

    ulong_type num_steps = GetEnvNumSteps(500000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);
    atomic_double_type sum = 0.0;
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_type& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x, &sum] (const ulong_type& s, const ulong_type& e)
    {
        double_type tl_sum = 0.0;
        pragma_simd()
        for(ulong_type i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//

    //========================================================================//
    timer::timer t;

    std::vector<std::thread> threads(num_threads);
    ulong_type diff = num_steps/num_threads;
    for(ulong_type i = 0; i < threads.size(); ++i)
    {
        ulong_type _f = diff*i; // first
        ulong_type _l = _f + diff; // last
        if(i+1 == threads.size())
            _l = num_steps;

        std::thread thread(compute_block, _f, _l);
        threads[i] = std::move(thread);
    }

    for(ulong_type i = 0; i < threads.size(); ++i)
    {
        threads[i].join();
    }

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_type pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

