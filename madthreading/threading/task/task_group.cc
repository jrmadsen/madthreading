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


#include "madthreading/threading/task/task_group.hh"
#include "madthreading/threading/thread_pool.hh"
#include "madthreading/threading/task/task.hh"
#include "madthreading/threading/thread_manager.hh"
#include "madthreading/utility/fpe_detection.hh"

namespace mad
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

task_group::task_group(thread_pool* tp)
: m_task_count(0),
  m_id(m_group_count++),
  m_pool(tp),
  m_save_lock(),
  m_join_lock()
{
    m_save_lock.unlock();
    m_join_lock.unlock();
}

//============================================================================//

task_group::~task_group()
{
    m_join_lock.unlock();
    m_save_lock.unlock();
}

//============================================================================//

void task_group::join()
{
#ifdef VERBOSE_THREAD_POOL
    std::cout << "Joining " << pending() << " tasks..." << std::endl;
    std::cout << std::boolalpha << "is alive: " << is_alive_flag << std::endl;
#endif

    // return if thread pool isn't built
    if(!m_pool->is_alive())
        return;

    while (m_pool->state() != state::STOPPED)
    {
        m_join_lock.lock();

        while(pending() > 0 && m_pool->state() != state::STOPPED)
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
            // if not locked, we need to lock it
            /*m_join_lock.lock();
            // Wait until signaled that a task has been competed
            // Unlock mutex while wait, then lock it back when signaled
            m_join_cond.timed_wait(m_join_lock, 5.0);*/
            m_join_cond.wait(m_join_lock);
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

    m_join_lock.unlock();
    m_save_lock.unlock();
}

//============================================================================//

int task_group::save_task(vtask* task)
{
    mad::auto_lock l(m_save_lock);
    // TODO: put a limit on how many tasks can be added at most
    m_save_tasks.push_back(task);

    return 0;
}

//============================================================================//

} // namespace mad
