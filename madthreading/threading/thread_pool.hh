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


#ifndef thread_pool_hh_
#define thread_pool_hh_

#include "madthreading/threading/threading.hh"
#include "madthreading/threading/mutex.hh"
#include "madthreading/threading/condition.hh"
#include "madthreading/allocator/allocator.hh"
#include "madthreading/atomics/atomic.hh"
#include "madthreading/threading/task/task_group.hh"
#include "madthreading/threading/task/task.hh"
#include "madthreading/threading/task/task_tree.hh"
#include "madthreading/types.hh"

#include <iostream>
#include <deque>
#include <vector>
#include <map>
#include <queue>
#include <stack>

namespace mad
{

class thread_pool
{
public:
    typedef long                                            long_type;
    typedef unsigned long                                   ulong_type;
    typedef mad::vtask                                      task_type;
    typedef std::size_t                                     size_type;
    typedef std::vector<std::thread*>                       ThreadContainer_t;
    typedef std::deque<task_type*, Allocator_t(task_type*)> TaskContainer_t;
    typedef std::vector<bool, Allocator_t(bool)>            JoinContainer_t;
    typedef mutex                                           Lock_t;
    typedef ulong_ts                                        task_count_type;
    typedef volatile int                                    pool_state_type;
    typedef mad::condition                                  Condition_t;
    typedef std::map<void*, task_type*>                     TaskMap_t;
    typedef std::map<void*, volatile bool>                  JoinMap_t;
    typedef std::map<std::thread::id, ulong_type>           tid_type;

public:
    // Constructor and Destructors
    // affinity assigns threads to certain cores
    explicit thread_pool(bool _use_affinity = false);
    thread_pool(size_type pool_size, bool _use_affinity = false);
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    virtual ~thread_pool();

public:
    // Public functions
    int initialize_threadpool(); // start the threads
    int destroy_threadpool(); // destroy the threads

public:
    // add tasks for threads to process
    int add_task(task_type* task);
    // add tasks quickly
    //int fast_add_tasks(task_type* task);
    // add a generic container with iterator
    template <typename Container_t>
    int add_tasks(Container_t&);
    // add a queue of tasks
    template <typename _Tp>
    int add_tasks(std::queue<_Tp*>);
    // add a stack of tasks
    template <typename _Tp>
    int add_tasks(std::stack<_Tp*>);
    // add tasks from task tree
    template <typename _Tp, typename _A1,
              typename _A2, typename _TpJ>
    int add_tasks(task_tree_node<_Tp, _A1, _A2, _TpJ>*);

public:
    // background tasks are task that you don't call join() on
    // and are executed several times, e.g. generate random number
    // task is identified by a unique pointer -- typically
    // "this" in class method
    int add_background_task(void*, task_type*);
    // signal a thread to wake up and execute the task
    void signal_background(void*);
    // set the task argument to a value and signal a thread to wake up
    // and execute
    template <typename... _Tp>
    void signal_background(void*, _Tp...);
    // check is a background task is finished computing
    volatile bool& is_done(void* ptr) { return m_back_done.find(ptr)->second; }
    // get the pool state
    const pool_state_type& state() const { return m_pool_state; }

public:
    // see how many main task threads there are
    size_type size() const { return m_pool_size; }
    // set the thread pool size
    void set_size(size_type _n) { m_pool_size = _n; }
    // affinity assigns threads to cores, only affects threads when
    // first initialized
    void use_affinity(bool _val) { m_use_affinity = _val; }

public:
    // read FORCE_NUM_THREADS environment variable
    static long_type GetEnvNumThreads(long_type _default = -1);
    static const tid_type& GetThreadIDs() { return tids; }
    static volatile bool& is_alive() { return is_alive_flag; }

protected:
    void* execute_thread(); // function thread sits in
    void  background_thread(); // function background threads sit in
    void  run(task_type*&);
    bool  is_initialized() const;

protected:
    // called in THREAD INIT
    static void* start_thread(void* arg);
    static void* start_background(void* arg);

private:
    // Private variables
    // random
    bool m_use_affinity;
    size_type m_pool_size;
    pool_state_type m_pool_state;

    // locks
    Lock_t m_task_lock;
    Lock_t m_back_lock;

    // conditions
    Condition_t m_task_cond;
    Condition_t m_back_cond;

    // containers
    ThreadContainer_t m_main_threads;   // storage for threads
    ThreadContainer_t m_back_threads;
    TaskContainer_t   m_main_tasks;     // storage for tasks
    JoinContainer_t   m_is_joined;
    JoinMap_t         m_back_done;

    // background
    void*             m_back_pointer;
    task_type*        m_back_task_to_do;
    TaskMap_t         m_back_tasks;

    static tid_type tids;
    static volatile bool is_alive_flag;

private:
    thread_pool(const thread_pool&)
    : m_use_affinity(false),
      m_pool_size(0),
      m_pool_state(0),
      m_task_lock(), // recursive
      m_back_lock(),
      m_task_cond(),
      m_back_cond(),
      m_main_threads(ThreadContainer_t()),
      m_back_threads(ThreadContainer_t()),
      m_main_tasks(TaskContainer_t()),
      m_is_joined(JoinContainer_t()),
      m_back_done(JoinMap_t()),
      m_back_pointer(nullptr),
      m_back_task_to_do(nullptr),
      m_back_tasks(TaskMap_t())
    { }

    thread_pool& operator=(const thread_pool&) { return *this; }

};

//----------------------------------------------------------------------------//
#include "task/task_group.hh"
//----------------------------------------------------------------------------//
template <typename Container_t>
int thread_pool::add_tasks(Container_t& c)
{

    if(!is_alive_flag) // if we haven't built thread-pool, just execute
    {
        for(auto& itr : c)
            run(itr);
        return 0;
    }

    m_task_lock.lock();

    // if the thread pool hasn't been initialize, initialize it
    if(!is_initialized())
        initialize_threadpool();

    // TODO: put a limit on how many tasks can be added at most
    for(auto& itr : c)
    {
        itr->group()->task_count() += 1;
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
template <typename _Tp>
int thread_pool::add_tasks(std::queue<_Tp*> c)
{
    int n = 0;
    while(!c.empty())
    {
        this->add_task(c.front());
        c.pop();
        ++n;
    }
    return n;
}
//----------------------------------------------------------------------------//
template <typename _Tp>
int thread_pool::add_tasks(std::stack<_Tp*> c)
{
    int n = 0;
    while(!c.empty())
    {
        this->add_task(c.top());
        c.pop();
        ++n;
    }
    return n;
}
//----------------------------------------------------------------------------//
template <typename _Tp, typename _A1, typename _A2, typename _TpJ>
int thread_pool::add_tasks(task_tree_node<_Tp, _A1, _A2, _TpJ>* node)
{
    // if we haven't built thread-pool, just execute
    if(!is_alive_flag)
    {
        if(node->left())
            add_tasks(node->left());
        vtask* vnode = node;
        run(vnode);
        if(node->right())
            add_tasks(node->right());
        return 1;
    }

    // thread-pool has been built
    if(node->left())
        add_tasks(node->left());

    // do outside of lock because is thread-safe and needs to be updated as
    // soon as possible
    node->group()->task_count() += 1;

    m_task_lock.lock();
    // if the thread pool hasn't been initialize, initialize it
    if(!is_initialized())
        initialize_threadpool();

    m_main_tasks.push_back(node);
    m_task_lock.unlock();

    if(node->right())
        add_tasks(node->right());

    m_task_cond.notify_all();

    return 1;
}
//----------------------------------------------------------------------------//
template <typename... _Tp>
void thread_pool::signal_background(void* ptr, _Tp... _args)
{
    if(m_back_tasks.find(ptr) == m_back_tasks.end())
        return;
        //throw std::runtime_error("invalid signal to background task");

    // acquire lock, assign task-to-do, signal a thread to wake up.
    // thread just woken up will acquire lock, get pointer value of
    // task-to-do, and then set task-to-do pointer to nullptr
    m_back_lock.lock();
    m_back_task_to_do = m_back_tasks[ptr];
    m_back_task_to_do->set(_args...);
    m_back_pointer = ptr;
    m_back_done[ptr] = false;
    m_back_cond.notify_one();
    m_back_lock.unlock();
}
//----------------------------------------------------------------------------//

} // namespace mad

#endif
