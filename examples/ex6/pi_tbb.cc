//
//
//	Multithreading example using custom MT API
//		- TBB with C++98 requires writing functor classes/structs
//		  which are must more tedious to implement than C++11 versions
//		  using lambdas
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

class ComputePi
{
public:
    typedef tbb::blocked_range<ulong_t> range_t;
    typedef range_t::const_iterator const_iterator;

public:
    // primary constructor
    ComputePi(double_t step)
    : m_step(step), m_sum(0.0) { }

    // split copy constructor required by tbb::parallel_reduce
    ComputePi(ComputePi& rhs, splitter)
    : m_step(rhs.m_step), m_sum(0.0) { }

    // for computing integral
    _inline_ double_t x(ulong_t i) const { return (i-0.5)*m_step; }

    _inline_ void operator()(const range_t& range)
    {
        for(const_iterator itr = range.begin(); itr != range.end(); ++itr)
        {
            //double _x = x(itr);
            m_sum += 4.0/(1.0 + x(itr)*x(itr));
        }
    }

    // called at end of tbb::parallel_reduce
    _inline_ void join(ComputePi& rhs) { m_sum += rhs.m_sum; }

    // access sum
    double_t& sum() { return m_sum; }
    const double_t& sum() const { return m_sum; }

private:
    double_t m_step;
    double_t m_sum;
};

//============================================================================//

int main(int, char** argv)
{
    typedef ComputePi::range_t range_t;

    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);

    tbb::task_scheduler_init init(num_threads);

    //========================================================================//
    mad::timer t;
    t.start();

    ComputePi pi_body(step);

    tbb::parallel_reduce(range_t(0, num_steps),
                         pi_body,
                         tbb::auto_partitioner());

    report(num_steps, step*pi_body.sum(), t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * pi_body.sum();
    return (fabs(pi - M_PI) > PI_EPSILON);
}

