//
//
//	Serial example (for comparison with MT examples)
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
    ulong_type num_steps = GetEnvNumSteps(500000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_type sum = 0.0;

    //========================================================================//
    mad::timer t;
    t.start();

    pragma_simd()
    for(ulong_type i = 0; i < num_steps; ++i)
    {
        double_type x = (i-0.5)*step;
        sum += 4.0/(1.0 + x*x);
    }

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_type pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

