//
//
// created by jmadsen on Wed Mar  9 01:11:17 2016
//
//
//
//

#include <UnitTest++.h>

#ifndef do_pragma
#   define do_pragma(x) _Pragma(#x)
#endif

//============================================================================//
//  OpenMP
//============================================================================//

#if defined(USE_OPENMP) && !defined(__INTEL_COMPILER)
#   include <omp.h>
#   ifndef pragma_simd
#       define pragma_simd(args) do_pragma(omp simd args)
#   endif
#else
#   ifndef pragma_simd
#       define pragma_simd(args) {;}
#   endif
#endif

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include <madthreading/threading/thread_pool.hh>
#include <madthreading/utility/constants.hh>

using namespace mad;
using namespace std;

#define CheckTol    1.0e-7
#define NUM_STEPS   500000000UL

static mad::mutex _mutex;

//============================================================================//
// T1
TEST(Test_1_pi_pool)
{
    std::cout << "Running Test_1_pi_pool..." << std::endl;
    uint64_t num_steps = NUM_STEPS;
    double_t step = 1.0/static_cast<double_t>(num_steps);
    atomic_double_t sum = 0.0;

    uint64_t num_threads = std::thread::hardware_concurrency();
    thread_pool* tp = new thread_pool(num_threads);
    thread_manager* tm = new thread_manager(tp);

    //------------------------------------------------------------------------//
    auto compute_block = [&sum, step] (const ulong_t& s, const ulong_t& e)
    {
        auto x = [step] (const ulong_t& i) { return (i-0.5)*step; };
        double_t tl_sum = 0.0;
        pragma_simd()
        for(ulong_t i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        sum += tl_sum;
    };
    //------------------------------------------------------------------------//
    mad::task_group<void> tg;
    tm->run_loop(tg, compute_block, 0, num_steps, num_threads);
    tg.join();

    CHECK_CLOSE(step*sum, dat::PI, CheckTol);

    delete tm;
}

//============================================================================//

TEST(Test_2_Hello_World)
{
    //------------------------------------------------------------------------//
    uint64_t num_threads = std::thread::hardware_concurrency();
    thread_pool* tp = new thread_pool(num_threads);
    thread_manager* tm = new thread_manager(tp);
    int niter = 20;
    //------------------------------------------------------------------------//
    auto _run1 = [] (int n)
    {
        mad::auto_lock l(_mutex);
        tmcout << "Hello World! - iteration #" << n << std::endl;
    };
    //------------------------------------------------------------------------//
    tmcout << "\nRunning loop #1 (run_loop)..." << std::endl;
    mad::task_group<void> tg1;
    tm->run_loop(tg1, _run1, 0, niter);
    tg1.join();

    delete tm;
}

//============================================================================//

TEST(Test_3_Hello_World_2)
{
    //------------------------------------------------------------------------//
    uint64_t num_threads = std::thread::hardware_concurrency();
    thread_pool* tp = new thread_pool(num_threads);
    thread_manager* tm = new thread_manager(tp);
    int niter = 20;
    //------------------------------------------------------------------------//
    mad::atomic<int> n(niter);
    //------------------------------------------------------------------------//
    auto _run2 = [&] ()
    {
        int _n = ++n;
        mad::auto_lock l(_mutex);
        tmcout << "Hello World! - iteration #" << _n << std::endl;
    };
    //------------------------------------------------------------------------//
    tmcout << "\nRunning loop #2 (exec)..." << std::endl;
    mad::task_group<void> tg2;
    for(int i = 0; i < niter; ++i)
        tm->exec(tg2, _run2);
    tg2.join();

    delete tm;
}

//============================================================================//
