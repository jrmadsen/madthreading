//
//  This program will numerically compute the integral of
//
//                    4/(1+x*x)
//
//  from 0 to 1.  The value of this integral is pi -- which
//  is great since it gives us an easy way to check the answer.
//
//  The program was parallelized using OpenMP by adding just
//  four lines
//
//  (1) A line to include omp.h -- the include file that
//  contains OpenMP's function prototypes and constants.
//
//  (2) A pragma that tells OpenMP to create a team of threads
//
//  (3) A pragma to cause one of the threads to print the
//  number of threads being used by the program.
//
//  (4) A pragma to split up loop iterations among the team
//  of threads.  This pragma includes 2 clauses to (1) create a
//  private variable and (2) to cause the threads to compute their
//  sums locally and then combine their local sums into a
//  single global value.
//
//  History: Written by Tim Mattson, 11/99.
//           Edited by Jonathan Madsen, 7/2017
//

#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <iostream>
#include <iomanip>
#include <thread>

#include <madthreading/types.hh>
#include <madthreading/utility/timer.hh>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

using namespace mad;

//============================================================================//

int main (int, char** argv)
{
    ulong_type num_steps = GetEnvNumSteps(500000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    omp_set_num_threads(num_threads);
    double_type sum = 0.0;

    //========================================================================//
    timer::timer t;

    #pragma omp parallel
    {
        #pragma omp for reduction(+:sum)
        for(ulong_type i = 0; i < num_steps; ++i)
        {
            double_type x = (i-0.5)*step;
            sum = sum + 4.0/(1.0+x*x);
        }
    }

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_type pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

//============================================================================//
