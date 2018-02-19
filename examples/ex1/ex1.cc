//
//
//

#include <iterator>
#include <list>
#include <vector>
#include <deque>
#include <algorithm>
#include <memory>
#include <functional>
#include <random>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include <madthreading/threading/auto_lock.hh>
#include <madthreading/threading/threading.hh>
#include <timemory/auto_timer.hpp>
#include <timemory/timing_manager.hpp>

using namespace std::placeholders;
using mad::ulong_t;

typedef std::mt19937_64 engine_type;

static ulong_t rseed = time(NULL);
static engine_type random_engine = engine_type(rseed);

//============================================================================//

template <typename _Tp>
_Tp& fill(_Tp& _array, typename _Tp::value_type _b, typename _Tp::value_type _e)
{
    typedef typename _Tp::value_type value_type;
    std::vector<value_type> _v;
    for(auto itr = _b; itr != _e; ++itr)
        _v.push_back(itr);
    _array.clear();
    std::copy(_v.begin(), _v.end(), std::inserter(_array, _array.begin()));
    return _array;
}

//============================================================================//

int64_t fibonacci(int64_t n)
{
    if(n < 2)
        return n;
    return fibonacci(n-2) + fibonacci(n-1);
}

//============================================================================//

template <typename _Tp>
void _run_itr(_Tp itr)
{
    static mad::mutex mutex;
    mad::auto_lock l(mutex);
    tmcout << "Hello World! - iteration #" << *itr << std::endl;
}

//============================================================================//

void full_func(int a1, double a2, long a3, short a4, float a5, unsigned a6)
{
    static mad::mutex mutex;
    mad::auto_lock l(mutex);
    tmcout << "Hello World! - " << a1 << ", " << a2 << ", "
           << a3 << ", " << a4 << ", " << a5 << ", " << a6 << std::endl;
}

//============================================================================//

int main(int argc, char** argv)
{
    int niter = 20;
    if(argc > 1)
        niter = atoi(argv[argc-1]);

    auto num_threads = mad::thread_manager::GetEnvNumThreads(4);
    mad::thread_pool* tp = new mad::thread_pool(num_threads);
    mad::thread_manager* tm = new mad::thread_manager(tp);

    tmcout << "Number of threads: " << tm->size() << std::endl;
    static mad::mutex mutex;


    //========================================================================//
    // RUN #1
    //========================================================================//
    auto _run1 = [] (int n)
    {
        mad::auto_lock l(mutex);
        tmcout << "Hello World! - iteration #" << n << std::endl;
    };
    //------------------------------------------------------------------------//
    tmcout << "\nRunning loop #1 (run_loop)..." << std::endl;
    mad::task_group<void> tg1;
    tm->run_loop(tg1, _run1, 0, niter);
    tg1.join();
    //========================================================================//


    //========================================================================//
    // RUN #2
    //========================================================================//
    mad::atomic<int> n(niter);
    //------------------------------------------------------------------------//
    auto _run2 = [&] ()
    {
        int _n = ++n;
        mad::auto_lock l(mutex);
        tmcout << "Hello World! - iteration #" << _n << std::endl;
    };
    //------------------------------------------------------------------------//
    tmcout << "\nRunning loop #2 (exec)..." << std::endl;
    mad::task_group<void> tg2;
    for(int i = 0; i < niter; ++i)
        tm->exec(tg2, _run2);
    tg2.join();

    //========================================================================//
    // RUN #3
    //========================================================================//
    auto _run3 = [&] (const double& d1, const double& d2,
                      const double& d3, const double& d4,
                      const double& d5, const double& d6)
    {
       mad::auto_lock l(mutex);
       tmcout << "Random: "
              << d1 << ", " << d2 << ", " << d3 << ", "
              << d4 << ", " << d5 << ", " << d6 << std::endl;
    };
    //------------------------------------------------------------------------//
    auto canonical = [&] ()
    {
        return std::generate_canonical<double,
                std::numeric_limits<double>::digits>(random_engine);
    };
    //------------------------------------------------------------------------//
    tmcout << "\nRunning exec #3 (6 params w/ C++11 variadic templates)..."
           << std::endl;
    auto prec = std::cout.precision();
    std::cout.precision(4);
    std::cout.setf(std::ios::fixed);
    mad::task_group<void> tg3;
    for(int i = 0; i < niter; ++i)
        tm->exec(tg3, _run3,
                 canonical(), canonical(), canonical(),
                 canonical(), canonical(), canonical());
    tg3.join();
    std::cout.precision(prec);
    std::cout.unsetf(std::ios::fixed);

    //========================================================================//
    // RUN #4
    //========================================================================//
    tmcout << "\nRunning exec #4..."
           << std::flush;
    mad::task_group<int64_t> tg4([](int64_t& a, const int64_t& b) { return a + b; });
    for(int i = 0; i < niter; ++i)
        tm->exec(tg4, fibonacci, 44);
    {
        TIMEMORY_AUTO_TIMER("fibonacci_calculation");
        auto sum = tg4.join();
        std::cout << " sum = " << sum << std::endl;
    }

    mad::timing_manager::instance()->report();

    delete tm;
    return 0;
}

