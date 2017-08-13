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

#include <madthreading/threading/thread_manager.hh>
#include <madthreading/threading/auto_lock.hh>
#include <madthreading/threading/threading.hh>

using namespace std::placeholders;

typedef std::mt19937_64 engine_type;

static ulong rseed = time(NULL);
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

template <typename _Tp>
void _run_itr(_Tp itr)
{
    static mad::mutex mutex;
    mad::auto_lock l(&mutex);
    tmcout << "Hello World! - iteration #" << *itr << std::endl;
}

//============================================================================//

void full_func(int a1, double a2, long a3, short a4, float a5, unsigned a6)
{
    static mad::mutex mutex;
    mad::auto_lock l(&mutex);
    tmcout << "Hello World! - " << a1 << ", " << a2 << ", "
           << a3 << ", " << a4 << ", " << a5 << ", " << a6 << std::endl;
}

//============================================================================//

int main(int argc, char** argv)
{
    int niter = 20;
    if(argc > 1)
        niter = atoi(argv[argc-1]);

    mad::thread_manager* tm
        = new mad::thread_manager(mad::thread_manager::GetEnvNumThreads(4));

    tmcout << "Number of threads: " << tm->size() << std::endl;
    static mad::mutex mutex;


    //========================================================================//
    // RUN #1
    //========================================================================//
    auto _run1 = [] (int n)
    {
        mad::auto_lock l(&mutex);
        tmcout << "Hello World! - iteration #" << n << std::endl;
    };
    //------------------------------------------------------------------------//
    tmcout << "\nRunning loop #1 (run_loop)..." << std::endl;
    tm->run_loop(_run1, 0, niter);
    tm->join();
    //========================================================================//


    //========================================================================//
    // RUN #2
    //========================================================================//
    mad::atomic<int> n(niter);
    //------------------------------------------------------------------------//
    auto _run2 = [&] ()
    {
        int _n = ++n;
        mad::auto_lock l(&mutex);
        tmcout << "Hello World! - iteration #" << _n << std::endl;
    };
    //------------------------------------------------------------------------//
    tmcout << "\nRunning loop #2 (exec)..." << std::endl;
    for(int i = 0; i < niter; ++i)
        tm->exec(_run2);
    tm->join();


    //========================================================================//
    // RUN #3
    //========================================================================//
    std::list<int> _list;
    fill(_list, 0, niter);
    tmcout << "\nRunning loop #3 (run_loop<list::iterator>)..." << std::endl;
    tm->run_loop(_run_itr<std::list<int>::iterator>, _list.begin(), _list.end());
    tm->join();


    //========================================================================//
    // RUN #4
    //========================================================================//
    std::deque<int> _deque;
    fill(_deque, niter, niter+niter);
    tmcout << "\nRunning loop #4 (run_loop<deque::iterator>)..." << std::endl;
    auto func = std::bind(_run_itr<std::deque<int>::iterator>, _1);
    tm->run_loop(func, _deque.begin(), _deque.end());
    tm->join();


    //========================================================================//
    // RUN #5
    //========================================================================//
    tmcout << "\nRunning #5 (> 4 arguments)..." << std::endl;
    auto arg4 = 2;
    auto arg5 = 4.5f;
    auto arg6 = 1;
    auto func1 = [arg4, arg5, arg6] (int arg1, double arg2, long arg3)
    {
          full_func(arg1, arg2, arg3, arg4, arg5, arg6);
    };
    auto func2 = std::bind(full_func, _1, _2, _3, arg4, arg5, arg6);
    tm->exec<void>(func1, 1, 1.3, 5L);
    tm->exec<void>(func2, 3, 4.1, 7L);
    tm->join();


#if defined(MAD_USE_CXX98)
#else
    //========================================================================//
    // RUN #6
    //========================================================================//
    auto _run6 = [&] (const double& d1, const double& d2,
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
    tmcout << "\nRunning exec #6 (6 params w/ C++11 variadic templates)..."
           << std::endl;
    auto prec = std::cout.precision();
    std::cout.precision(4);
    std::cout.setf(std::ios::fixed);
    for(int i = 0; i < niter; ++i)
        tm->exec(_run6,
                 canonical(), canonical(), canonical(),
                 canonical(), canonical(), canonical());
    tm->join();
    std::cout.precision(prec);
    std::cout.unsetf(std::ios::fixed);
#endif

    return 0;
}

