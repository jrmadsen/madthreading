//
// created by jrmadsen on Tue Apr  3 18:11:12 2018
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
#include <timemory/manager.hpp>

const int64_t cxxfib = 40;

int64_t fibonacci(int64_t n)
{
    return (n < 2) ? n : (fibonacci(n-1) + fibonacci(n-2));
}

int main(int argc, char** argv)
{
    int niter = 100;
    if(argc > 1)
        niter = atoi(argv[argc-1]);

    auto num_threads = mad::thread_manager::GetEnvNumThreads(4);
    mad::thread_pool* tp = new mad::thread_pool(num_threads);
    mad::thread_manager* tm = new mad::thread_manager(tp);

    tmcout << argv[0] << " - Number of threads: " << tm->size() << std::endl;
    typedef std::shared_future<int64_t> future_t;

    mad::timer t;
    t.start();

    std::vector<future_t> futures;
    futures.reserve(niter);

    for(int i = 0; i < niter; ++i)
        futures.push_back(tm->async<int64_t>(fibonacci, cxxfib));

    int64_t sum = 0;
    for(auto& itr : futures)
        sum += itr.get();

    t.stop();
    tmcout << "\"" << argv[0] << "\" execution time: " << t << std::endl;
    delete tm;

    return 0;
}

