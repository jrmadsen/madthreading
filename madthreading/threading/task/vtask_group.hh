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
// created by jmadsen on Sat Jul 15 18:43:49 2017
//

#ifndef vtask_group_hh_
#define vtask_group_hh_

#include "madthreading/threading/threading.hh"
#include "madthreading/threading/task/vtask.hh"
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
#include <cstdint>
#include <unordered_map>
#include <map>

//----------------------------------------------------------------------------//

namespace mad
{

class thread_pool;

namespace details
{

//----------------------------------------------------------------------------//

class vtask_group
{
public:
    typedef mad::details::vtask                     task_type;
    typedef std::size_t                             size_type;
    typedef mad::mutex                              lock_t;
    typedef long_ts                                 task_count_type;
    typedef mad::condition                          condition_t;
    typedef std::shared_ptr<task_type>              task_pointer;

public:
    // Constructor and Destructors
    vtask_group(thread_pool* tp = nullptr);
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    virtual ~vtask_group();

public:
    // wait to finish
    void wait();
    
    // get the task count
    task_count_type& task_count() { return m_task_count; }
    const task_count_type& task_count() const { return m_task_count; }

    // get the locks/conditions
    lock_t& task_lock()      { return m_task_lock; }
    condition_t& task_cond() { return m_task_cond; }

    const lock_t& task_lock() const      { return m_task_lock; }
    const condition_t& task_cond() const { return m_task_cond; }

    const uint64_t& id() const { return m_id; }
    void set_pool(thread_pool* tp) { m_pool = tp; }

    thread_pool*& pool()       { return m_pool; }
    thread_pool*  pool() const { return m_pool; }

protected:
    // check if any tasks are still pending
    int pending() { return m_task_count; }
    
protected:
    // Private variables
    task_count_type m_task_count;
    uint64_t        m_id;
    thread_pool*    m_pool;
    condition_t     m_task_cond;
    lock_t          m_task_lock;

protected:
    enum class state : int
    {
        STARTED = 0,
        STOPPED = 1,
        NONINIT = 2
    };
};

} // namespace details

} // namespace mad


#endif
