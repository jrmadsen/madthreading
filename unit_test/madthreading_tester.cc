//
//
// created by jmadsen on Wed Mar  9 01:11:17 2016
//
//
//
//

#include <UnitTest++.h>

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
    //std::cout << "Running Test_1_pi_pool..." << std::endl;
    ulong_type num_steps = NUM_STEPS;
    double_type step = 1.0/static_cast<double_type>(num_steps);
    double_ts sum = 0.0;
    ulong_type num_threads = 4;
    thread_manager* tm = new thread_manager(num_threads);

    //------------------------------------------------------------------------//
    auto compute_block = [&sum, step] (const ulong_type& s, const ulong_type& e)
    {
        auto x = [step] (const ulong_type& i) { return (i-0.5)*step; };
        double_type tl_sum = 0.0;
        pragma_simd
        for(ulong_type i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//
    mad::task_group<void> tg;
    tm->run_loop(&tg, compute_block, 0, num_steps, num_threads);
    tg.join();

    CHECK_CLOSE(step*sum, dat::PI, CheckTol);
}

//============================================================================//
