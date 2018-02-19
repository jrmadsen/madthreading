//
//
//	Multithreading example using C++11 threads
//
//

#include <iostream>
#include <iomanip>
#include <thread>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

using namespace mad;

typedef atomic<double_t>     atomic_double_t;

int main(int, char** argv)
{

    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    atomic_double_t sum = 0.0;
    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_t& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x, &sum] (const ulong_t& s, const ulong_t& e)
    {
        double_t tl_sum = 0.0;
        pragma_simd()
        for(ulong_t i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//

    //========================================================================//
    mad::timer t;
    t.start();

    std::vector<std::thread> threads(num_threads);
    ulong_t diff = num_steps/num_threads;
    for(ulong_t i = 0; i < threads.size(); ++i)
    {
        ulong_t _f = diff*i; // first
        ulong_t _l = _f + diff; // last
        if(i+1 == threads.size())
            _l = num_steps;

        std::thread thread(compute_block, _f, _l);
        threads[i] = std::move(thread);
    }

    for(ulong_t i = 0; i < threads.size(); ++i)
    {
        threads[i].join();
    }

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

