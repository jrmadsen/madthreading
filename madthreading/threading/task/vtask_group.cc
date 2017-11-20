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
// created by jmadsen on Sat Jul 15 18:43:49 2017
//


#include "madthreading/threading/task/vtask_group.hh"
#include "madthreading/threading/thread_pool.hh"
#include "madthreading/threading/task/task.hh"
#include "madthreading/threading/thread_manager.hh"
#include "madthreading/utility/fpe_detection.hh"

namespace mad
{

namespace details
{

//============================================================================//

namespace state
{
static const int STARTED = 0;
static const int STOPPED = 1;
static const int NONINIT = 2;
}

//============================================================================//

static ulong_ts m_group_count = 0;

//============================================================================//

vtask_group::vtask_group(thread_pool* tp)
: m_task_count(0),
  m_id(m_group_count++),
  m_pool(tp),
  m_task_lock()
{
    if(!m_pool)
        m_pool = mad::thread_manager::instance()->thread_pool();
}

//============================================================================//

vtask_group::~vtask_group()
{ }

//============================================================================//

void vtask_group::wait()
{
#ifdef VERBOSE_THREAD_POOL
    std::cout << "Joining " << pending() << " tasks..." << std::endl;
    std::cout << std::boolalpha << "is alive: " << is_alive_flag << std::endl;
#endif

    // return if thread pool isn't built
    if(!m_pool->is_alive())
        return;

    while ((int) m_pool->state() != (int) state::STOPPED)
    {
        m_task_lock.lock();

        while(pending() > 0 && (int) m_pool->state() != (int) state::STOPPED)
        {
            #if defined(DEBUG)
            long_type ntasks = pending();
            if(false)
            {
                static mad::mutex _mutex;
                mad::auto_lock l(_mutex);
                tmcout << "# of tasks: " << ntasks << std::endl;
            }
            #endif
            // Wait until signaled that a task has been competed
            // Unlock mutex while wait, then lock it back when signaled
            m_task_cond.wait(m_task_lock);
        }

        // if pending is not greater than zero, we are joined
        if(!(0 < pending()))
            break;
    }

    if(m_task_count > 0)
    {
        long_type ntask = m_task_count;
        std::stringstream ss;
        ss << "\bError! Join operation failure! " << ntask << " tasks still "
           << "are running!" << std::endl;
        throw std::runtime_error(ss.str().c_str());
    }

    m_task_lock.unlock();
}

//============================================================================//

} // namespace details

} // namespace mad