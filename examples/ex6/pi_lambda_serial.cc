//
//
//	Serial example (for comparison with MT examples) using C++11 lambdas
//
//

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

using namespace mad;

int main(int, char** argv)
{
    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    double_t sum = 0.0;

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_t& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//

    //========================================================================//
    mad::timer t;
    t.start();

    pragma_simd()
    for(ulong_t i = 0; i < num_steps; ++i)
    {
        sum += 4.0/(1.0 + x(i)*x(i));
    }

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

