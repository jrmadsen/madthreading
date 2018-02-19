//
//
//	Multithreading example using TBB with C++11
//		- TBB with C++11 allows the use of lambdas
//
//

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>

using namespace mad;


//============================================================================//

int main(int, char** argv)
{
    typedef tbb::blocked_range<ulong_t> range_t;
    typedef range_t::const_iterator const_iterator;

    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);
    double_t sum = 0.0;

    tbb::task_scheduler_init init(num_threads);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_t& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x] (const range_t& r, double_t tl_sum)
    {
        for(const_iterator itr = r.begin(); itr != r.end(); ++itr)
            tl_sum += 4.0/(1.0 + x(itr)*x(itr));
        return tl_sum;
    };
    //------------------------------------------------------------------------//
    auto join = [] (double_t _x, double_t _y) { return _x+_y; };
    //------------------------------------------------------------------------//

    //========================================================================//
    mad::timer t;
    t.start();

    sum = tbb::parallel_reduce(range_t(0, num_steps), sum, compute_block, join);

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

