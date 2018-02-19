//
//
//	Multithreading example using pure pthreads
//
//

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include "../Common.hh"

#define CORETHREADCREATE( worker , func , arg )  { \
                pthread_attr_t attr; \
                pthread_attr_init(&attr); \
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); \
                int ret = pthread_create( worker, &attr, func , arg ); \
                if (ret != 0) \
                { \
                    std::stringstream msg; \
                    msg << "pthread_create() failed: " << ret << std::endl; \
                    throw std::runtime_error(msg.str()); \
                } \
            }
#define CORETHREADJOIN(worker) pthread_join(worker, NULL)

using namespace mad;

//============================================================================//

struct ProxyStruct
{
    typedef ulong_t arg_type;
    typedef const arg_type& cr_arg_type;
    typedef double_t return_type;
    typedef std::function<return_type(cr_arg_type, cr_arg_type)> function_type;
    typedef std::pair<arg_type, arg_type> range_type;

    function_type m_func;
    range_type m_range;
    return_type m_value;

    ProxyStruct(function_type f, range_type r)
    : m_func(f), m_range(r), m_value(0.0) { }
};

//============================================================================//

void* proxy(void* threadargs)
{
    ProxyStruct* args = (ProxyStruct*)(threadargs);

    args->m_value = args->m_func(args->m_range.first, args->m_range.second);

    return (void*)(args);
}

//============================================================================//

int main(int, char** argv)
{
    typedef ProxyStruct::range_type range_t;

    ulong_t num_steps = GetEnvNumSteps(500000000UL);
    double_t step = 1.0/static_cast<double_t>(num_steps);
    double_t sum = 0.0;
    ulong_t num_threads = thread_manager::GetEnvNumThreads(1);

    //------------------------------------------------------------------------//
    auto x = [step] (const ulong_t& i) { return (i-0.5)*step; };
    //------------------------------------------------------------------------//
    auto compute_block = [x] (const ulong_t& s, const ulong_t& e)
    {
        double_t tl_sum = 0.0;
        pragma_simd()
        for(ulong_t i = s; i < e; ++i)
            tl_sum += 4.0/(1.0 + x(i)*x(i));
        return tl_sum;
    };
    //------------------------------------------------------------------------//

    //========================================================================//
    mad::timer t;
    t.start();

    std::vector<ProxyStruct*> proxies(num_threads, 0);
    std::vector<pthread_t> threads(num_threads);
    ulong_t diff = num_steps/num_threads;
    for(ulong_t i = 0; i < threads.size(); ++i)
    {
        ulong_t _f = diff*i; // first
        ulong_t _l = _f + diff; // last
        if(i+1 == threads.size())
            _l = num_steps;

        ProxyStruct* proxy_struct
            = new ProxyStruct(compute_block, range_t(_f, _l));

        pthread_t thread;
        proxies[i] = proxy_struct;
        CORETHREADCREATE(&thread, proxy, (void*)(proxies[i]));
        threads[i] = std::move(thread);
    }

    for(ulong_t i = 0; i < threads.size(); ++i)
    {
        CORETHREADJOIN(threads[i]);
    }

    for(ulong_t i = 0; i < proxies.size(); ++i)
    {
        sum += proxies.at(i)->m_value;
    }

    report(num_steps, step*sum, t.stop_and_return(), argv[0]);
    //========================================================================//

    double_t pi = step * sum;
    return (fabs(pi - M_PI) > PI_EPSILON);
}

