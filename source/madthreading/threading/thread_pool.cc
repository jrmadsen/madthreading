// MIT License
//
// Copyright (c) 2017 Jonathan R. Madsen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

//
//
//
//
//
// created by jrmadsen on Wed Jul 22 09:15:04 2015
//
//
//
//


#include "madthreading/threading/thread_pool.hh"
#include "madthreading/allocator/allocator_list.hh"
#include "madthreading/threading/thread_manager.hh"
#include "madthreading/utility/fpe_detection.hh"
#include "timemory/utility.hpp"

#include <cstdlib>

#if !defined(WIN32) && !defined(__MACH__)
#define ALLOW_AFFINITY
#   include <pthread.h>
#endif

static mad::mutex io_mutex;

namespace mad
{

//============================================================================//

inline int ncores()
{
    return std::thread::hardware_concurrency();
}

//============================================================================//

static mad::mutex io_mutex;
static mad::mutex tid_mutex;
thread_pool::tid_type thread_pool::tids;

//============================================================================//

namespace state
{
static const int STARTED = 0;
static const int STOPPED = 1;
static const int NONINIT = 2;
}

//============================================================================//

std::string get_state(const int _state)
{
    std::stringstream ss;
    switch(_state)
    {
        case state::STARTED:
            ss << "started";
            break;
        case state::STOPPED:
            ss << "stopped";
            break;
        case state::NONINIT:
            ss << "not initialized";
            break;
        default:
            ss << "unknown state " << _state << " (> 2)";
            break;
    }
    return ss.str();
}

//============================================================================//
// static member function that calls the member function we want the thread to
// run
void thread_pool::start_thread(thread_pool* tp)
{

    if(mad::get_env<int32_t>("MAD_VERBOSE", 0) > 0)
    {
        mad::auto_lock lock(tid_mutex);
        tids[std::this_thread::get_id()] = tids.size();
    }
    tp->execute_thread();
}

//============================================================================//

thread_pool::thread_pool(const size_type& pool_size, bool _use_affinity)
: m_use_affinity(_use_affinity),
  m_alive_flag(false),
  m_pool_size(0),
  m_pool_state(state::NONINIT)
{
    this->initialize_threadpool(pool_size);
}

//============================================================================//

thread_pool::~thread_pool()
{
    // Release resources
    if (m_pool_state != state::STOPPED)
    {
        size_type ret = destroy_threadpool();
        while(ret > 0)
            ret = stop_thread();
    }

    // wait until thread pool is fully destroyed
    while(thread_pool::is_alive());
    // delete thread-local allocator and erase thread IDS
    if(mad::details::allocator_list_tl::get_allocator_list_if_exists())
    {
        auto _self = std::this_thread::get_id();
        ulong_t _id = mad::thread_pool::GetThreadIDs().find(_self)->second;
        if(tids.find(_self) != tids.end())
            tids.erase(tids.find(_self));
        mad::details::allocator_list_tl::get_allocator_list()->Destroy(_id, 1);
    }

}

//============================================================================//

bool thread_pool::is_initialized() const
{
    return !(m_pool_state == state::NONINIT);
}

//============================================================================//

void thread_pool::resize(size_type _n)
{
    if(_n == m_pool_size)
        return;
    initialize_threadpool(_n);
}

//============================================================================//

thread_pool::size_type
thread_pool::initialize_threadpool(size_type proposed_size)
{
    if(proposed_size == 1)
        return 1;

    m_alive_flag.store(true);

    //--------------------------------------------------------------------//
    // if started, stop some thread if smaller or return if equal
    if(m_pool_state == state::STARTED)
    {
        if(m_pool_size > proposed_size)
        {
            while(stop_thread() > proposed_size);
            return m_pool_size;
        }
        else if(m_pool_size == proposed_size)
            return m_pool_size;
    }

    //--------------------------------------------------------------------//
    // lock any existing threads in thread-pool
    mad::auto_lock _task_lock(m_task_lock);

    if(!m_alive_flag.load())
        m_pool_state = state::STARTED;

    for (size_type i = m_pool_size; i < proposed_size; i++)
    {
        // add the threads
        mad::thread* tid = new mad::thread;
        bool _add_thread = true;
        try
        {
            *tid = std::thread(thread_pool::start_thread, this);
        }
        catch(std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl; // issue creating thread
            _add_thread = false;
        }
        catch(std::bad_alloc& e)
        {
            std::cerr << e.what() << std::endl;
            _add_thread = false;
        }

#if defined(ALLOW_AFFINITY)
        if(m_use_affinity)
        {
            // Create a cpu_set_t object representing a set of CPUs. Clear it and mark
            // only CPU i as set.
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i, &cpuset);
            int rc = pthread_setaffinity_np(tid->native_handle(),
                                            sizeof(cpu_set_t), &cpuset);
            if (rc != 0)
                std::cerr << "Error calling pthread_setaffinity_np: " << rc
                          << std::endl;
        }
#endif

        // successful creation of thread
        if(_add_thread)
        {
            m_main_threads.push_back(std::move(tid));
            // list of joined thread booleans
            m_is_joined.push_back(false);
            // thread-local task queue
            m_thread_tasks.insert(std::make_pair(tid->get_id(), task_list_t()));
        }
    }
    //------------------------------------------------------------------------//

    m_pool_size = m_main_threads.size();

    if(mad::get_env<int32_t>("MAD_VERBOSE", 0) > 0)
        tmcout << "--> " << m_pool_size
               << " threads created by the thread pool" << std::endl;

    // thread pool size doesn't match with join vector
    // this will screw up joining later
    if(m_is_joined.size() != m_main_threads.size())
    {
        std::stringstream ss;
        ss << "thread_pool::initialize_threadpool - boolean is_joined vector "
           << "is a different size than threads vector: " << m_is_joined.size()
           << " vs. " << m_main_threads.size() << " (tid: "
           << std::this_thread::get_id() << ")";

        throw std::runtime_error(ss.str());
    }

    return m_main_threads.size();
}

//============================================================================//

thread_pool::size_type
thread_pool::destroy_threadpool()
{
    if(!m_alive_flag.load())
        return 0;

    // Note: this is not for synchronization, its for thread communication!
    // destroy_threadpool() will only be called from the main thread, yet
    // the modified m_pool_state may not show up to other threads until its
    // modified in a lock!
    //------------------------------------------------------------------------//
    {
        mad::auto_lock _task_lock(m_task_lock);
        m_pool_state = state::STOPPED;
        // notify all threads we are shutting down
        m_task_cond.notify_all();
    }

    if(m_is_joined.size() != m_main_threads.size())
    {
        std::stringstream ss;
        ss << "   thread_pool::destroy_thread_pool - boolean is_joined vector "
           << "is a different size than threads vector: " << m_is_joined.size()
           << " vs. " << m_main_threads.size() << " (tid: "
           << std::this_thread::get_id() << ")";

        throw std::runtime_error(ss.str());
    }

    for (size_type i = 0; i < m_is_joined.size(); i++)
    {
        //--------------------------------------------------------------------//
        // if its joined already, nothing else needs to be done
        if(m_is_joined.at(i))
            continue;

        //--------------------------------------------------------------------//
        // join
        if(!(std::this_thread::get_id() == m_main_threads[i]->get_id()))
            m_main_threads[i]->join();

        //--------------------------------------------------------------------//
        // erase thread from thread ID list
        if(tids.find(m_main_threads[i]->get_id()) != tids.end())
            tids.erase(tids.find(m_main_threads[i]->get_id()));

        //--------------------------------------------------------------------//
        // it's joined
        m_is_joined.at(i) = true;

        //--------------------------------------------------------------------//
        // try waking up a bunch of threads that are still waiting
        m_task_cond.notify_all();
        //--------------------------------------------------------------------//
    }

    if(mad::get_env<int32_t>("MAD_VERBOSE", 0) > 0)
        tmcout << "--> " << m_pool_size
               << " threads exited from the thread pool" << std::endl;

    // clean up
    for(auto& itr : m_main_threads)
        delete itr;

    m_main_threads.clear();
    m_is_joined.clear();

    m_alive_flag.store(false);

    return 0;
}

//============================================================================//

thread_pool::size_type
thread_pool::stop_thread()
{
    if(!m_alive_flag.load() || m_pool_size == 0)
        return 0;

    //------------------------------------------------------------------------//
    // notify all threads we are shutting down
    m_task_lock.lock();
    m_is_stopped.push_back(true);
    m_task_cond.notify_one();
    m_task_lock.unlock();
    //------------------------------------------------------------------------//

    // lock up the task queue
    mad::auto_lock _task_lock(m_task_lock);

    while(!m_stop_threads.empty())
    {
        // get the thread
        mad::thread* t = m_stop_threads.back();
        // let thread finish
        t->join();
        // remove from stopped
        m_stop_threads.pop_back();
        // remove from main
        for(auto itr = m_main_threads.begin(); itr != m_main_threads.end(); ++itr)
            if((*itr)->get_id() == t->get_id())
            {
                m_main_threads.erase(itr);
                break;
            }
        // remove from join list
        m_is_joined.pop_back();
        // delete thread
        delete t;
    }

    m_pool_size = m_main_threads.size();
    return m_main_threads.size();
}

//============================================================================//

void thread_pool::execute_thread()
{
    // thread-local task pointer
    task_pointer task = nullptr;

    // threads stay in this loop forever until thread-pool destroyed
    while(true)
    {
        //--------------------------------------------------------------------//
        // Try to pick a task
        mad::auto_lock _task_lock(m_task_lock);
        //--------------------------------------------------------------------//

        // We need to put condition.wait() in a loop for two reasons:
        // 1. There can be spurious wake-ups (due to signal/ENITR)
        // 2. When mutex is released for waiting, another thread can be waken up
        //    from a signal/broadcast and that thread can mess up the condition.
        //    So when the current thread wakes up the condition may no longer be
        //    actually true!
        while ((m_pool_state != state::STOPPED) && (m_main_tasks.empty()))
        {
            // Wait until there is a task in the queue
            // Unlock mutex while wait, then lock it back when signaled
            m_task_cond.wait(_task_lock);
            //G4CONDITIONWAIT( &m_task_cond, &_task_lock );
        }

        // If the thread was waked to notify process shutdown, return from here
        if (m_pool_state == state::STOPPED)
        {                
            // has exited.
            return;
        }

        // single thread stoppage
        if(m_is_stopped.size() > 0)
        {
            if(m_is_stopped.back())
                m_stop_threads.push_back(get_thread(std::this_thread::get_id()));
            m_is_stopped.pop_back();
            _task_lock.unlock();
            return;
        }

        // check thread-specific queue first and process all tasks
        if(m_thread_tasks[std::this_thread::get_id()].size() > 0)
        {
            // the queue is thread-local
            _task_lock.unlock();
            while(!m_thread_tasks[std::this_thread::get_id()].empty())
            {
                // get the task
                task = m_thread_tasks[std::this_thread::get_id()].front();
                m_thread_tasks[std::this_thread::get_id()].pop_front();
                //------------------------------------------------------------//

                // execute the task
                run(task);
                //------------------------------------------------------------//
            }
            // before we proceed to check m_main_tasks we need to acquire lock
            // released above
            _task_lock.lock();
        }

        // the global queue of tasks
        if(m_main_tasks.size() > 0)
        {
            // get the task
            task = m_main_tasks.front();
            m_main_tasks.pop_front();
            //----------------------------------------------------------------//

            // Unlock
            _task_lock.unlock();
            //----------------------------------------------------------------//

            // execute the task
            run(task);
            //----------------------------------------------------------------//
        }
        else // no main tasks, unlock and continue to beginning of loop
        {
            _task_lock.unlock();
        }
    }
}

//============================================================================//

void thread_pool::run(task_pointer task)
{
    if(!task.get())
        return;
    // execute task
    (*task)();

    task->operator--();
}

//============================================================================//

thread_pool::size_type
thread_pool::add_task(task_pointer task)
{

    if(!m_alive_flag.load()) // if we haven't built thread-pool, just execute
    {
        run(task_pointer(task));
        return 0;
    }

    // do outside of lock because is thread-safe and needs to be updated as
    // soon as possible
    task->operator++();

    m_task_lock.lock();

    // TODO: put a limit on how many tasks can be added at most
    m_main_tasks.push_back(task);

    // wake up one thread that is waiting for a task to be available
    m_task_cond.notify_one();

    m_task_lock.unlock();

    return 1;
}

//============================================================================//

thread_pool::size_type
thread_pool::add_thread_task(mad::thread::id id, task_pointer task)
{

    if(!m_alive_flag.load()) // if we haven't built thread-pool, just execute
    {
        run(task_pointer(task));
        return 0;
    }

    // do outside of lock because is thread-safe and needs to be updated as
    // soon as possible
    task->operator++();

    m_task_lock.lock();

    // TODO: put a limit on how many tasks can be added at most
    m_thread_tasks[id].push_back(task);

    // wake up one thread that is waiting for a task to be available
    m_task_cond.notify_all();

    m_task_lock.unlock();

    return 1;
}

//============================================================================//

int64_t thread_pool::GetEnvNumThreads(int64_t _default)
{
    char* env_nthreads;
    env_nthreads = getenv("MAD_NUM_THREADS");

    if(!env_nthreads) // this is legacy
        env_nthreads = getenv("FORCE_NUM_THREADS");

    if(env_nthreads)
    {
        std::string str_nthreads = std::string(env_nthreads);
        std::istringstream iss(str_nthreads);
        int64_t _n = 0;
        iss >> _n;
        return _n;
    }

    if(_default > 0)
        return _default;
    return 0;
}

//============================================================================//

} // namespace mad

