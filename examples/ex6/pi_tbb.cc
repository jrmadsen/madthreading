//
//
//	Multithreading example using TBB with C++98
//		- TBB with C++98 requires writing functor classes/structs
//		  which are must more tedious to implement than C++11 versions
//		  using lambdas
//
//

#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <madthreading/utility/timer.hh>
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
    typedef tbb::blocked_range<ulong_type> range_t;
    typedef range_t::const_iterator const_iterator;

public:
    // primary constructor
    ComputePi(double_type step)
    : m_step(step), m_sum(0.0) { }

    // split copy constructor required by tbb::parallel_reduce
    ComputePi(ComputePi& rhs, splitter)
    : m_step(rhs.m_step), m_sum(0.0) { }

    // for computing integral
    __attribute__((always_inline))
    inline double_type x(ulong_type i) const { return (i-0.5)*m_step; }

    __attribute__((always_inline))
    inline void operator()(const range_t& range)
    {
        for(const_iterator itr = range.begin(); itr != range.end(); ++itr)
        {
            //double _x = x(itr);
            m_sum += 4.0/(1.0 + x(itr)*x(itr));
        }
    }

    // called at end of tbb::parallel_reduce
    __attribute__((always_inline))
    inline void join(ComputePi& rhs) { m_sum += rhs.m_sum; }

    // access sum
    double_type& sum() { return m_sum; }
    const double_type& sum() const { return m_sum; }

private:
    double_type m_step;
    double_type m_sum;
};

//============================================================================//

int main(int argc, char** argv)
{
    typedef ComputePi::range_t range_t;

    ulong_type num_steps = GetEnvNumSteps(500000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);

    tbb::task_scheduler_init init(num_threads);

    //========================================================================//
    timer::timer t;

    ComputePi pi_body(step);

    tbb::parallel_reduce(range_t(0, num_steps),
                         pi_body,
                         tbb::auto_partitioner());

    report(num_steps, step*pi_body.sum(), t.stop_and_return(), argv[0]);
    //========================================================================//

    double_type pi = step * pi_body.sum();
    return (fabs(pi - M_PI) > PI_EPSILON);
}

