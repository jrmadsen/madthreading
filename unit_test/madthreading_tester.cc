//
//
// created by jmadsen on Wed Mar  9 01:11:17 2016
//
//
//
//

#include <UnitTest++/UnitTest++.h>

#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <madthreading/types.hh>
#include <madthreading/utility/timer.hh>
#include <madthreading/threading/thread_manager.hh>
#include <madthreading/utility/constants.hh>

using namespace mad;
using namespace std;

#define CheckTol    1.0e-7
#define NUM_STEPS   500000000UL

//============================================================================//
// T1
TEST(Test_1_pi_pool)
{
    ulong_type num_steps = NUM_STEPS;
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_ts sum = 0.0;
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    thread_manager* tm = thread_manager::get_thread_manager(num_threads);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_type& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x] (const ulong_type& s, const ulong_type& e)
    {
        double_type tl_sum = 0.0;
        #pragma omp simd
        for(ulong_type i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        return tl_sum;
    };
    //------------------------------------------------------------------------//
    auto join = [&sum] (double_type tl_sum)
    {
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//

    tm->run_loop<double_type>(compute_block, 0, num_steps, num_threads);
    tm->join<double_type>(join);

    CHECK_CLOSE(step*sum, dat::PI, CheckTol);
}

//============================================================================//
// T2
TEST(Test_2_pi_pool_joiner_A)
{
    ulong_type num_steps = NUM_STEPS;
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_ts sum = 0.0;
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    thread_manager* tm = thread_manager::get_thread_manager(num_threads);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_type& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x] (const ulong_type& s, const ulong_type& e)
    {
        double_type tl_sum = 0.0;
        #pragma omp simd
        for(ulong_type i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        return tl_sum;
    };
    //------------------------------------------------------------------------//
    auto join = [&sum] (double_type tl_sum)
    {
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//

    tm->run_loop<double_type>(compute_block, 0, num_steps, num_threads,
                              join, 0.0);

    CHECK_CLOSE(step*sum, dat::PI, CheckTol);
}

//============================================================================//
// T3
TEST(Test_3_pi_pool_joiner_B)
{
    ulong_type num_steps = NUM_STEPS;
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_ts sum = 0.0;
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    thread_manager* tm = thread_manager::get_thread_manager(num_threads);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_type& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x] (const ulong_type& s, const ulong_type& e)
    {
        double_type tl_sum = 0.0;
        #pragma omp simd
        for(ulong_type i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        return tl_sum;
    };
    //------------------------------------------------------------------------//
    auto join = [&sum] (double_type tl_sum)
    {
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//

    tm->run_loop<double_type>(compute_block, 0, num_steps,
                              num_threads*4, join, 0.0);

    CHECK_CLOSE(step*sum, dat::PI, CheckTol);
}

