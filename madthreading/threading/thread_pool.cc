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
#include "madthreading/threading/task/task_tree.hh"
#include "madthreading/threading/thread_manager.hh"
#include "madthreading/utility/fpe_detection.hh"

#include <cstdlib>

// CTCAST --> CoreThread cast (needed for Apple)
#ifndef __APPLE__
#   define CTCAST(field) field
#else
#   define CTCAST(field) (unsigned long)(field)
#endif

static mad::mutex io_mutex;

namespace mad
{

//============================================================================//

inline int ncores()
{
    return Threading::GetNumberOfCores();
}

//============================================================================//

static mad::mutex tid_mutex;

//============================================================================//

thread_pool::tid_type thread_pool::tids;
volatile bool thread_pool::is_alive_flag = false;

//============================================================================//

namespace state
{
static const int STARTED = 0;
static const int STOPPED = 1;
static const int NONINIT = 2;
}

//============================================================================//

thread_pool::thread_pool(bool _use_affinity)
: m_use_affinity(_use_affinity),
  m_pool_size(Threading::GetNumberOfCores()),
  m_pool_state(state::NONINIT),
  m_task_lock(true), // recursive
  m_back_lock(true),
  m_back_task_to_do(NULL)
{
    m_task_lock.unlock();
    m_back_lock.unlock();

#ifdef VERBOSE_THREAD_POOL
    std::cout << "Constructing ThreadPool of size "
              << m_pool_size << std::endl;
#endif

#ifdef FPE_DEBUG
        fpe_settings::fpe_set ops;
        ops.insert(fpe::underflow);
        ops.insert(fpe::overflow);
        EnableInvalidOperationDetection(ops);
#endif

}

//============================================================================//

thread_pool::thread_pool(size_type pool_size, bool _use_affinity)
: m_use_affinity(_use_affinity),
  m_pool_size(pool_size),
  m_pool_state(state::NONINIT),
  m_task_lock(true), // recursive
  m_back_lock(true),
  m_back_task_to_do(NULL)
{
    m_task_lock.unlock();
    m_back_lock.unlock();

#ifdef VERBOSE_THREAD_POOL
    std::cout << "Constructing ThreadPool of size " << m_pool_size << std::endl;
#endif

#ifdef FPE_DEBUG
        fpe_settings::fpe_set ops;
        ops.insert(fpe::underflow);
        ops.insert(fpe::overflow);
        EnableInvalidOperationDetection(ops);
#endif

}

//============================================================================//

thread_pool::~thread_pool()
{
    // Release resources
    if (m_pool_state != state::STOPPED)
        destroy_threadpool();

    // delete all background tasks
    for(TaskMap_t::iterator itr = m_back_tasks.begin();
        itr != m_back_tasks.end(); ++itr)
        delete itr->second;

    // wait until thread pool is fully destroyed
    while(mad::thread_pool::is_alive());
    // delete thread-local allocator and erase thread IDS
    if(mad::details::allocator_list_tl::get_allocator_list_if_exists())
    {
        ulong_type _self = CORETHREADSELFINT();
        ulong_type _id = mad::thread_pool::GetThreadIDs().find(_self)->second;
        if(tids.find(_self) != tids.end())
            tids.erase(tids.find(_self));
        mad::details::allocator_list_tl::get_allocator_list()->Destroy(_id, 1);
    }

    m_task_lock.unlock();
    m_back_lock.unlock();
}

//============================================================================//

bool thread_pool::is_initialized() const
{
    return !(m_pool_state == state::NONINIT);
}

//============================================================================//

// We can't pass a non-static member function to CORETHREADCREATE.
// So created the static member function that calls the member function
// we want to run in the thread.
void* thread_pool::start_thread(void* arg)
{
    {
        mad::Lock_t lock(&tid_mutex);
        tids[CORETHREADSELFINT()] = tids.size();
#ifdef VERBOSE_THREAD_POOL
        std::cout << "--> [MAIN THREAD QUEUE] thread ids size: " << tids.size()
                  << std::endl;
#endif
    }
    thread_pool* tp = (thread_pool*) arg;
    tp->execute_thread();
    return NULL;
}

//============================================================================//

void* thread_pool::start_background(void* arg)
{
    { // a background thread is not in thread pool
        mad::Lock_t lock(&tid_mutex);
        tids[CORETHREADSELFINT()] = tids.size();
#ifdef VERBOSE_THREAD_POOL
        std::cout << "--> [BACK THREAD QUEUE] thread ids size: "
                  << tids.size() << std::endl;
#endif
    }
    thread_pool* tp = (thread_pool*) arg;
    tp->background_thread();
    return NULL;
}

//============================================================================//

int thread_pool::initialize_threadpool()
{
#ifndef ENABLE_THREADING
    return 1;
#endif

    if(m_pool_size == 1)
        return 1;

    is_alive_flag = true;

#ifdef VERBOSE_THREAD_POOL
    std::cout << "--> Creating " << m_pool_size
              << " threads ... " << m_main_threads.size() << " already exist"
              << std::endl;
#endif

    //--------------------------------------------------------------------//
    // destroy any existing thread pool
    if(m_pool_state == state::STARTED)
        destroy_threadpool();

    //--------------------------------------------------------------------//
    m_pool_state = state::STARTED;

    for (size_type i = 0; i < m_pool_size; i++)
    {
        // add the threads
        CoreThread tid;
        bool _add_thread = true;
        try
        {
            // assign to core is affinity is set
            if(m_use_affinity)
            {
                CORETHREADCREATEID(&tid,
                                   thread_pool::start_thread,
                                   (void*)(this),
                                   (abs(ncores() - (int)i - 1))%ncores());
            } else
            {
                CORETHREADCREATE(&tid,
                                 thread_pool::start_thread,
                                 (void*)(this));
            }
        } catch(std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl; // issue creating thread
            _add_thread = false;
        } catch(std::bad_alloc& e)
        {
            std::cerr << e.what() << std::endl;
            _add_thread = false;
        }

        // successful creation of thread
        if(_add_thread)
        {
            m_main_threads.push_back(tid);
            // TODO: FIGURE THIS OUT
            m_is_joined.push_back(false);
        }
    }
    //------------------------------------------------------------------------//

    m_pool_size = m_main_threads.size();

#ifdef VERBOSE_THREAD_POOL
    std::cout << "--> " << m_pool_size
              << " threads created by the thread pool" << std::endl;
#endif

    // thread pool size doesn't match with join vector
    // this will screw up joining later
    if(m_is_joined.size() != m_main_threads.size())
    {
        std::stringstream ss;
        ss << "thread_pool::initialize_threadpool - boolean is_joined vector "
           << "is a different size than threads vector: " << m_is_joined.size()
           << " vs. " << m_main_threads.size() << " (tid: "
           << CORETHREADSELFINT() << ")";

        throw std::runtime_error(ss.str());
    }

    return m_main_threads.size();
}

//============================================================================//

int thread_pool::destroy_threadpool()
{
#ifndef ENABLE_THREADING
    return 0;
#endif

    if(!is_alive_flag)
        return 0;

    // Note: this is not for synchronization, its for thread communication!
    // destroy_threadpool() will only be called from the main thread, yet
    // the modified m_pool_state may not show up to other threads until its
    // modified in a lock!
    //------------------------------------------------------------------------//
    {
        mad::auto_lock l(m_task_lock);
        m_pool_state = state::STOPPED;
    }

    //------------------------------------------------------------------------//
    // notify all threads we are shutting down
    m_task_cond.broadcast();
    m_back_cond.broadcast();
    //------------------------------------------------------------------------//

    if(m_is_joined.size() != m_main_threads.size())
    {
        std::stringstream ss;
        ss << "   thread_pool::destroy_thread_pool - boolean is_joined vector "
           << "is a different size than threads vector: " << m_is_joined.size()
           << " vs. " << m_main_threads.size() << " (tid: "
           << CORETHREADSELFINT() << ")";

        throw std::runtime_error(ss.str());
    }

    for (size_type i = 0; i < m_is_joined.size(); i++)
    {
        //--------------------------------------------------------------------//
        // if its joined already, nothing else needs to be done
        if(m_is_joined.at(i))
            continue;

        //--------------------------------------------------------------------//
        // erase thread from thread ID list
        if(tids.find(CTCAST(m_main_threads[i])) != tids.end())
            tids.erase(tids.find(CTCAST(m_main_threads[i])));

        //--------------------------------------------------------------------//
        // set cancellation state
        CORETHREADCANCEL(m_main_threads[i]);

        //--------------------------------------------------------------------//
        // it's joined
        m_is_joined.at(i) = true;

        //--------------------------------------------------------------------//
        // try waking up a bunch of threads that are still waiting
        m_task_cond.broadcast();
        //--------------------------------------------------------------------//
    }

    for (size_type i = 0; i < m_back_threads.size(); i++)
    {
        //--------------------------------------------------------------------//
        // erase background threads
        if(tids.find(CTCAST(m_back_threads[i])) != tids.end())
            tids.erase(tids.find(CTCAST(m_back_threads[i])));

        //--------------------------------------------------------------------//
        // set cancellation state
        CORETHREADCANCEL(m_back_threads[i]);

        //--------------------------------------------------------------------//
        // try waking up a bunch of threads that are still waiting
        m_back_cond.broadcast();
        //--------------------------------------------------------------------//
    }

#ifdef VERBOSE_THREAD_POOL
    std::cout << "--> " << m_pool_size
              << " threads exited from the thread pool" << std::endl;
#endif

    // clean up
    m_main_threads.clear();
    m_back_threads.clear();
    m_is_joined.clear();

    if(m_task_lock.is_locked())
        m_task_lock.unlock();
    if(m_back_lock.is_locked())
        m_back_lock.unlock();

    is_alive_flag = false;

    return 0;
}

//============================================================================//

void* thread_pool::execute_thread()
{
    while(true)
    {
        //--------------------------------------------------------------------//
        // Try to pick a task
        m_task_lock.lock();
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
            m_task_cond.wait(m_task_lock.base_mutex_ptr());
        }

        // If the thread was waked to notify process shutdown, return from here
        if (m_pool_state == state::STOPPED)
        {
            // has exited.
            m_task_lock.unlock();
            //----------------------------------------------------------------//
            if(mad::details::allocator_list_tl::get_allocator_list_if_exists()
               && tids.find(CORETHREADSELFINT()) != tids.end())
                mad::details::allocator_list_tl::get_allocator_list()
                        ->Destroy(tids.find(CORETHREADSELFINT())->second, 1);
            //----------------------------------------------------------------//
            CORETHREADEXIT(NULL);
        }

        // get the task
        vtask* task = m_main_tasks.front();
        m_main_tasks.pop_front();
        //--------------------------------------------------------------------//

        // Unlock
        m_task_lock.unlock();
        //--------------------------------------------------------------------//

        // get the task group
        task_group* tg = task->group();
        //--------------------------------------------------------------------//

        // execute the task
        run(task);
        //--------------------------------------------------------------------//

        // decrement the task group
        tg->task_count() -= 1;
        /*long_type tc = (tg->task_count() -= 1);
        {
            mad::auto_lock l(io_mutex);
            tmcout << "Task count (-) : " << tc << std::endl;
        }*/
        tg->join_lock().lock();
        tg->join_cond().signal();
        tg->join_lock().unlock();
        //--------------------------------------------------------------------//
    }
    return NULL;
}

//============================================================================//

void thread_pool::signal_background(void* ptr)
{

#ifndef ENABLE_THREADING
    m_back_done.find(ptr)->second = false;
    (*m_back_tasks[ptr])();
    m_back_done.find(ptr)->second = true;
    return;
#endif

    if(m_back_tasks.find(ptr) == m_back_tasks.end())
        return;

    m_back_lock.lock();
    m_back_task_to_do = m_back_tasks[ptr];
    m_back_pointer = ptr;
    m_back_done.find(ptr)->second = false;
    m_back_cond.signal();
    m_back_lock.unlock();
}

//============================================================================//

void thread_pool::background_thread()
{
    vtask* task = NULL;
    void*  task_key = 0;

    while(m_pool_state == state::NONINIT)
        m_back_cond.wait(m_back_lock.base_mutex_ptr());

    while(true)
    {
        //--------------------------------------------------------------------//
        // Try to pick a task
        m_back_lock.lock();
        //--------------------------------------------------------------------//

        // We need to put condition.wait() in a loop for two reasons:
        // 1. There can be spurious wake-ups (due to signal/ENITR)
        // 2. When mutex is released for waiting, another thread can be waken up
        //    from a signal/broadcast and that thread can mess up the condition.
        //    So when the current thread wakes up the condition may no longer be
        //    actually true!
        while ((m_pool_state != state::STOPPED) && !m_back_task_to_do)
        {
            // Wait until there is a task in the queue
            // Unlock mutex while wait, then lock it back when signaled
            m_back_cond.wait(m_back_lock.base_mutex_ptr());
        }

        // If the thread was waked to notify process shutdown, return from here
        if (m_pool_state == state::STOPPED)
        {
            //m_has_exited.
            m_back_lock.unlock();
            //----------------------------------------------------------------//
            if(mad::details::allocator_list::get_allocator_list_if_exists() &&
               tids.find(CORETHREADSELFINT()) != tids.end())
                mad::details::allocator_list::get_allocator_list()
                        ->Destroy(tids.find(CORETHREADSELFINT())->second, 1);
            //----------------------------------------------------------------//
            CORETHREADEXIT(NULL);
        }

        task_key = m_back_pointer;
        task = m_back_task_to_do;
        m_back_task_to_do = NULL;
        //--------------------------------------------------------------------//
        // Unlock
        m_back_lock.unlock();
        //--------------------------------------------------------------------//

        if(!task)
        {
            typedef TaskMap_t::iterator _iterator;
            _iterator itr = m_back_tasks.begin();
            std::cerr << "Background task has a null pointer. "
                      << "Num background tasks: "
                      << m_back_tasks.size()
                      << " \t " << itr->first
                      << " \t " << itr->second
                      << " \t " << task_key
                      << " \t " << task
                      << std::endl;
            continue;
        }

        // execute the task
        (*task)();

        // notify that it is done
        {
            mad::auto_lock l(m_back_lock);
            m_back_done.find(task_key)->second = true;
        }

        task = NULL;
        task_key = NULL;
        //--------------------------------------------------------------------//
    }
    //return NULL;
}

//============================================================================//

void thread_pool::run(vtask*& task)
{
    // execute task
    (*task)();

    if(task->force_delete()) // should we delete
    {
        delete task;
        task = 0;
    } else
    {
        // does the task store anything?
        if(task->get() && !task->is_stored_elsewhere())
            task->group()->save_task(task);
        else if(!task->is_stored_elsewhere())
        {
            delete task;
            task = 0;
        }
    }

}

//============================================================================//

int thread_pool::add_task(vtask* task)
{

#ifdef VERBOSE_THREAD_POOL
    std::cout << "Adding task..." << std::endl;
#endif

#ifndef ENABLE_THREADING
    run(task);
    return 0;
#endif

    if(!is_alive_flag) // if we haven't built thread-pool, just execute
    {
        run(task);
        return 0;
    }

    // do outside of lock because is thread-safe and needs to be updated as
    // soon as possible
    task->group()->task_count() += 1;
    /*long_type tc = (task->group()->task_count() += 1);
    {
        mad::auto_lock l(io_mutex);
        tmcout << "Task count (+) : " << tc << std::endl;
    }*/

    m_task_lock.lock();

    // if the thread pool hasn't been initialize, initialize it
    if(m_pool_state == state::NONINIT)
        initialize_threadpool();

    // TODO: put a limit on how many tasks can be added at most
    m_main_tasks.push_back(task);

    // wake up one thread that is waiting for a task to be available
    m_task_cond.signal();

    m_task_lock.unlock();

    return 0;
}

//============================================================================//

int thread_pool::add_background_task(void* ptr, vtask* task)
{
#ifndef ENABLE_THREADING
    m_back_tasks[ptr] = task;
    m_back_done.insert(std::pair<void*, bool>(ptr, true));
    return 1;
#endif

    CoreThread tid;
    bool _add_thread = true;
    try
    {
        // assign to core if affinity is set to on
        if(m_use_affinity)
        {
            CORETHREADCREATEID(&tid,
                               thread_pool::start_background,
                               (void*)(this),
                               (abs(ncores() -
                                    (int)m_back_threads.size() - 1))%ncores());
        } else
        {
            CORETHREADCREATE(&tid,
                             thread_pool::start_background,
                             (void*)(this));
        }
    } catch(std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        _add_thread = false;
    } catch(std::bad_alloc& e)
    {
        std::cerr << e.what() << std::endl;
        _add_thread = false;
    }

    // successful creation, identifiable via pointer
    if(_add_thread)
    {
        m_back_threads.push_back(tid);
        m_back_tasks[ptr] = task;
        m_back_done.insert(std::pair<void*, bool>(ptr, true));
    }

    return m_back_threads.size();
}

//============================================================================//

long_type thread_pool::GetEnvNumThreads(long_type _default)
{
    char* env_nthreads;
    env_nthreads = getenv("MAD_NUM_THREADS");

    if(!env_nthreads) // this is legacy
        env_nthreads = getenv("FORCE_NUM_THREADS");

    if(env_nthreads)
    {
        std::string str_nthreads = std::string(env_nthreads);
        std::istringstream iss(str_nthreads);
        long_type _n = 0;
        iss >> _n;
        return _n;
    }

    if(_default > 0)
        return _default;
    return 0;
}

//============================================================================//

} // namespace mad

