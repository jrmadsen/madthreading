//
//
//
//
//
// created by jmadsen on Sat Jul 15 18:43:49 2017
//
//
//
//

#ifndef task_group_hh_
#define task_group_hh_

#include "madthreading/threading/threading.hh"
#include "madthreading/threading/mutex.hh"
#include "madthreading/threading/condition.hh"
#include "madthreading/allocator/allocator.hh"
#include "madthreading/atomics/atomic.hh"
#include "madthreading/types.hh"

#include <iostream>
#include <deque>
#include <vector>
#include <map>
#include <queue>
#include <stack>

//----------------------------------------------------------------------------//

namespace mad
{

class thread_pool;
class vtask;

//----------------------------------------------------------------------------//

class task_group
{
public:
    typedef long                                            long_type;
    typedef unsigned long                                   ulong_type;
    typedef mad::vtask                                      task_type;
    typedef std::size_t                                     size_type;
    typedef std::deque<task_type*, Allocator_t(task_type*)> TaskContainer_t;
    typedef mad::mutex                                      Lock_t;
    typedef long_ts                                         task_count_type;
    typedef volatile int                                    pool_state_type;
    typedef mad::condition                                  Condition_t;
    typedef TaskContainer_t::iterator                       iterator;
    typedef TaskContainer_t::const_iterator                 const_iterator;

public:
    // Constructor and Destructors
    task_group(thread_pool* tp);
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    virtual ~task_group();

public:
    // wait for threads to finish tasks
    void join();

    // get the task count
    task_count_type& task_count() { return m_task_count; }
    const task_count_type& task_count() const { return m_task_count; }

    // get the locks/conditions
    Lock_t& join_lock()      { return m_join_lock; }
    Lock_t& save_task()      { return m_save_lock; }
    Condition_t& join_cond() { return m_join_cond; }

    const Lock_t& join_lock() const      { return m_join_lock; }
    const Lock_t& save_task() const      { return m_save_lock; }
    const Condition_t& join_cond() const { return m_join_cond; }

    // Get tasks with non-void return types
    TaskContainer_t& get_saved_tasks() { return m_save_tasks; }
    const TaskContainer_t& get_saved_tasks() const { return m_save_tasks; }

    // iterate over tasks with return type
    iterator begin()                { return m_save_tasks.begin(); }
    iterator end()                  { return m_save_tasks.end(); }
    const_iterator begin() const    { return m_save_tasks.begin(); }
    const_iterator end()   const    { return m_save_tasks.end(); }
    const_iterator cbegin() const   { return m_save_tasks.begin(); }
    const_iterator cend()   const   { return m_save_tasks.end(); }

    // save a task with a return type
    int save_task(task_type* task);

    const ulong_type& id() const { return m_id; }
    void set_pool(thread_pool* tp) { m_pool = tp; }

    thread_pool*& pool()       { return m_pool; }
    thread_pool*  pool() const { return m_pool; }

protected:
    // check if any tasks are still pending
    int pending() { return m_task_count; }

private:
    // Private variables
    task_count_type     m_task_count;
    ulong_type          m_id;
    thread_pool*        m_pool;
    Condition_t         m_join_cond;
    Lock_t              m_save_lock;
    Lock_t              m_join_lock;
    TaskContainer_t     m_save_tasks;

};

//----------------------------------------------------------------------------//

} // namespace mad

#endif