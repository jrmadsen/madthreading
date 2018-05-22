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


#ifndef thread_pool_hh_
#define thread_pool_hh_

#include <iostream>
#include <deque>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <atomic>
#include <cstdint>

#include "madthreading/threading/threading.hh"
#include "madthreading/threading/mutex.hh"
#include "madthreading/threading/condition.hh"
#include "madthreading/allocator/allocator.hh"
#include "madthreading/atomics/atomic.hh"
#include "madthreading/threading/task/vtask_group.hh"
#include "madthreading/threading/task/vtask.hh"
#include "madthreading/types.hh"

namespace mad
{

class thread_pool
{
public:
    typedef details::vtask                      task_type;
    typedef std::size_t                         size_type;
    typedef std::shared_ptr<task_type>          task_pointer;
    typedef std::vector<thread*>                ThreadContainer_t;
    typedef std::deque<task_pointer>            task_list_t;
    typedef std::vector<bool>                   BoolContainer_t;
    typedef mutex                               lock_t;
    typedef atomic_ulong_t                      task_count_type;
    typedef atomic_int_t                        pool_state_type;
    typedef condition                           condition_t;
    typedef std::map<thread::id, uint64_t>      tid_type;
    typedef std::map<thread::id, task_list_t>   thread_task_list_t;

public:
    // Constructor and Destructors
    thread_pool(const size_type& pool_size, bool _use_affinity = false);
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    virtual ~thread_pool();

public:
    // Public functions
    size_type initialize_threadpool(size_type); // start the threads
    size_type destroy_threadpool(); // destroy the threads
    size_type stop_thread();

public:
    // add tasks for threads to process
    size_type add_task(task_pointer task);
    size_type add_thread_task(thread::id id, task_pointer task);
    // add a generic container with iterator
    template <typename Container_t>
    size_type add_tasks(Container_t&);

    thread* get_thread(size_type _n) const;
    thread* get_thread(std::thread::id id) const;

public:
    // get the pool state
    const pool_state_type& state() const { return m_pool_state; }
    // see how many main task threads there are
    size_type size() const { return m_pool_size; }
    // set the thread pool size
    void resize(size_type _n);
    // affinity assigns threads to cores, assignment at constructor
    bool using_affinity() const { return m_use_affinity; }
    bool is_alive() { return m_alive_flag.load(); }

public:
    // read FORCE_NUM_THREADS environment variable
    static int64_t GetEnvNumThreads(int64_t _default = -1);
    static const tid_type& GetThreadIDs() { return tids; }

protected:
    void  execute_thread(); // function thread sits in
    void  background_thread(); // function background threads sit in
    void  run(task_pointer);
    bool  is_initialized() const;

protected:
    // called in THREAD INIT
    static void start_thread(thread_pool* arg);

private:
    // Private variables
    // random
    bool m_use_affinity;
    std::atomic<bool> m_alive_flag;
    size_type m_pool_size;
    pool_state_type m_pool_state;

    // locks
    lock_t m_task_lock;
    // conditions
    condition_t m_task_cond;

    // containers
    BoolContainer_t     m_is_joined;      // join list
    BoolContainer_t     m_is_stopped;     // lets thread know to stop
    ThreadContainer_t   m_main_threads;   // storage for active threads
    ThreadContainer_t   m_stop_threads;   // storage for stopped threads
    task_list_t         m_main_tasks;     // storage for tasks
    thread_task_list_t  m_thread_tasks;   // storage for thread-specific tasks

private:
    // Private static variables
    static tid_type tids;

private:
    thread_pool(const thread_pool&)
    : m_use_affinity(false),
      m_alive_flag(false),
      m_pool_size(0),
      m_pool_state(0),
      m_task_lock(), // recursive
      m_task_cond(),
      m_is_joined(BoolContainer_t()),
      m_is_stopped(BoolContainer_t()),
      m_main_threads(ThreadContainer_t()),
      m_stop_threads(ThreadContainer_t()),
      m_main_tasks(task_list_t()),
      m_thread_tasks(thread_task_list_t())
    {
        throw std::runtime_error(__FUNCTION__);
    }

    thread_pool& operator=(const thread_pool&)
    {
        throw std::runtime_error(__FUNCTION__);
        return *this;
    }

};

//----------------------------------------------------------------------------//
inline thread*
thread_pool::get_thread(size_type _n) const
{
    return (_n < m_main_threads.size()) ? m_main_threads[_n] : nullptr;
}
//----------------------------------------------------------------------------//
inline thread*
thread_pool::get_thread(std::thread::id id) const
{
    for(const auto& itr : m_main_threads)
        if(itr->get_id() == id)
            return itr;
    return nullptr;
}
//----------------------------------------------------------------------------//
template <typename Container_t>
thread_pool::size_type
thread_pool::add_tasks(Container_t& c)
{

    if(!m_alive_flag) // if we haven't built thread-pool, just execute
    {
        for(auto& itr : c)
            run(itr);
        return 0;
    }

    m_task_lock.lock();

    // TODO: put a limit on how many tasks can be added at most
    for(auto& itr : c)
    {
        itr->task_count() += 1;
        m_main_tasks.push_back(std::move(itr));
    }
    c.clear();

    // wake up one thread that is waiting for a task to be available
    if(c.size() < this->size())
    {
        for(size_type i = 0; i < c.size(); ++i)
            m_task_cond.notify_one();
    }
    else
        m_task_cond.notify_all();

    m_task_lock.unlock();

    return c.size();
}
//----------------------------------------------------------------------------//

} // namespace mad

#endif
