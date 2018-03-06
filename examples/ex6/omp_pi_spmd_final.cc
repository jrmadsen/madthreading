//
//
//  NAME:   PI SPMD final version without false sharing
//
//  This program will numerically compute the integral of
//
//                    4/(1+x*x)
//
//  from 0 to 1.  The value of this integral is pi -- which
//  is great since it gives us an easy way to check the answer.
//
//  The program was parallelized using OpenMP and an SPMD
//  algorithm.  The following OpenMP specific lines were
//  added:
//
//  (1) A line to include omp.h -- the include file that
//  contains OpenMP's function prototypes and constants.
//
//  (2) A pragma that tells OpenMP to create a team of threads
//  with an integer variable i being created for each thread.
//
//  (3) two function calls: one to get the thread ID (ranging
//  from 0 to one less than the number of threads), and the other
//  returning the total number of threads.
//
//  (4) A "single" construct so only one thread prints the number
//  of threads.
//
//  (5) A cyclic distribution of the loop by changing loop control
//  expressions to run from the thread ID incremented by the number
//  of threads.  Local sums accumlated into sum[id].
//
//  (6) A barrier to make sure everyone's done.
//
//  (7) A single construct so only one thread combines the local
//  sums into a single global sum.
//
//  Note that this program avoids the false sharing problem
//  by storing partial sums into a private scalar.
//
//  History: Written by Tim Mattson, 11/1999.
//           Edited by Jonathan Madsen, 7/2017
//

#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

using namespace mad;

//============================================================================//

int main (int, char** argv)
{
    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    double_t full_sum = 0.0;

    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);
    omp_set_num_threads(num_threads);

    //========================================================================//
    mad::timer t;
    t.start();

    ulong_t i;
    #pragma omp parallel private(i)
    {
        ulong_t id = omp_get_thread_num();
        ulong_t nthreads = omp_get_num_threads();
        double_t partial_sum = 0.0;

        pragma_simd()
        for(i = id; i < num_steps; i += nthreads)
        {
            double_t x = (i+0.5)*step;
            partial_sum += + 4.0/(1.0+x*x);
        }
        #pragma omp critical
            full_sum += partial_sum;
    }

    report(num_steps, step*full_sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * full_sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

//============================================================================//
