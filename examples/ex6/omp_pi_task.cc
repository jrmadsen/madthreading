//
//
//  This program will numerically compute the integral of
//
//                    4/(1+x*x)
//
//  from 0 to 1.  The value of this integral is pi -- which
//  is great since it gives us an easy way to check the answer.
//
//  This version of the program uses a divide and concquer algorithm
//  with tasks and taskwait.
//
//  History: Written by Tim Mattson, 10/2013
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

static ulong_t MIN_BLK;

//============================================================================//

double_t pi_comp(ulong_t Nstart, ulong_t Nfinish, double_t step)
{
    double_t sum = 0.0;
    double_t sum1 = 0.0;
    double_t sum2 = 0.0;

    if (Nfinish-Nstart < MIN_BLK)
    {
        pragma_simd()
        for (ulong_t i = Nstart; i < Nfinish; ++i)
        {
            double_t x = (i+0.5)*step;
            sum = sum + 4.0/(1.0+x*x);
        }
    }
    else
    {
        ulong_t iblk = Nfinish-Nstart;
        #pragma omp task shared(sum1)
            sum1 = pi_comp(Nstart,         Nfinish-iblk/2,step);
        #pragma omp task shared(sum2)
            sum2 = pi_comp(Nfinish-iblk/2, Nfinish,       step);
        #pragma omp taskwait
            sum = sum1 + sum2;
   }
   return sum;
}

//============================================================================//

int main (int, char** argv)
{
    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    atomic_double_t sum = 0.0;
    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);
    omp_set_num_threads(num_threads);
    MIN_BLK = num_steps / num_threads;

    //========================================================================//
    mad::timer t;
    t.start();

    #pragma omp parallel
    {
       #pragma omp single
       {
           sum = pi_comp(0, num_steps, step);
       }
    }

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

//============================================================================//
