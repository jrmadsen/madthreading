//
//
//
//
//
// created by jmadsen on Fri Aug  7 09:44:32 2015
//
//
//
//


#include "allocator_list.hh"
#include "allocator.hh"
#include "../threading/thread_pool.hh"

#include <iostream>
#include <iomanip>

namespace mad
{
namespace details
{

//============================================================================//

allocator_list* allocator_list::m_allocator_list = 0;

//============================================================================//

allocator_list::allocator_list()
{  }

//============================================================================//

allocator_list::~allocator_list()
{
    m_allocator_list = 0;
}

//============================================================================//

allocator_list* allocator_list::get_allocator_list()
{
    if(!m_allocator_list)
        m_allocator_list = new allocator_list;
    return m_allocator_list;
}

//============================================================================//

allocator_list* allocator_list::get_allocator_list_if_exists()
{
    return m_allocator_list;
}

//============================================================================//

void allocator_list::Register(allocator_base* alloc)
{
    m_list.push_back(alloc);
}

//============================================================================//

void allocator_list::Destroy(size_type nstat, int verbose)
{
    unsigned i = 0, j = 0;
    double mem = 0.0, tmem = 0;

    if(!(m_list.size() > 0))
        return;

    if(verbose > 0)
    {
        std::cout << "================= Deleting memory pools ================="
                  << std::endl;
        std::cout << "Thread ID : "
                  << mad::thread_pool::GetThreadIDs()
                     .find(CORETHREADSELFINT())->second
                  << ", nstat = " << nstat
                  << std::endl;
    }

    for(list_type::iterator itr = m_list.begin(); itr != m_list.end(); ++itr)
    {
        if(!*itr)
            continue;
        std::cout << "Pointer : " << (*itr) << " / " << m_list.size() << std::endl;
        mem = (*itr)->get_allocated_size();
        if(i < nstat)
        {
            i++;
            tmem += mem;
            (*itr)->reset_storage();
            continue;
        }
        j++;
        tmem += mem;
        if(verbose > 1)
        {
          std::cout << "Pool ID '" << (*itr)->get_pool_type() << "', size : "
                    << std::setprecision(3) << mem/1048576
                    << std::setprecision(6) << " MB" << std::endl;
        }
        (*itr)->reset_storage();
        delete *itr;
        *itr = 0;
    }

    if(verbose > 0)
    {
        std::cout << "Number of memory pools allocated: " << size()
                  << "; of which, static: " << i << std::endl;
        std::cout << "Dynamic pools deleted: " << j
                  << " / Total memory freed: " << std::setprecision(2)
                  << tmem/1048576 << std::setprecision(6) << " MB" << std::endl;
        std::cout << "========================================================="
                  << std::endl;
    }

    m_list.clear();
}

//============================================================================//

allocator_list::size_type allocator_list::size() const
{
    return m_list.size();
}
//============================================================================//



//============================================================================//
//============================================================================//
// thread-local version
//============================================================================//
//============================================================================//

ThreadLocal allocator_list_tl* allocator_list_tl::m_allocator_list_tl = 0;

//============================================================================//

allocator_list_tl::allocator_list_tl()
{  }

//============================================================================//

allocator_list_tl::~allocator_list_tl()
{
    m_allocator_list_tl = 0;
}

//============================================================================//

allocator_list_tl* allocator_list_tl::get_allocator_list()
{
    if(!m_allocator_list_tl)
        m_allocator_list_tl = new allocator_list_tl;
    return m_allocator_list_tl;
}

//============================================================================//

allocator_list_tl* allocator_list_tl::get_allocator_list_if_exists()
{
    return m_allocator_list_tl;
}

//============================================================================//

void allocator_list_tl::Register(allocator_base* alloc)
{
    m_list.push_back(alloc);
}

//============================================================================//

void allocator_list_tl::Destroy(size_type nstat, int verbose)
{
    unsigned i = 0, j = 0;
    double mem = 0.0, tmem = 0;

    if(!(m_list.size() > 0))
        return;

    if(verbose > 0)
    {
        std::cout << "================= Deleting memory pools ================="
                  << std::endl;
        std::cout << "Thread ID : "
                  << mad::thread_pool::GetThreadIDs()
                     .find(CORETHREADSELFINT())->second
                  << ", nstat = " << nstat
                  << std::endl;
    }

    for(list_type::iterator itr = m_list.begin(); itr != m_list.end(); ++itr)
    {
        if(!*itr)
            continue;
        std::cout << "Pointer : " << (*itr) << " / " << m_list.size() << std::endl;
        mem = (*itr)->get_allocated_size();
        if(i < nstat)
        {
            i++;
            tmem += mem;
            (*itr)->reset_storage();
            continue;
        }
        j++;
        tmem += mem;
        if(verbose > 1)
        {
          std::cout << "Pool ID '" << (*itr)->get_pool_type() << "', size : "
                    << std::setprecision(3) << mem/1048576
                    << std::setprecision(6) << " MB" << std::endl;
        }
        (*itr)->reset_storage();
        delete *itr;
        *itr = 0;
    }

    if(verbose > 0)
    {
        std::cout << "Number of memory pools allocated: " << size()
                  << "; of which, static: " << i << std::endl;
        std::cout << "Dynamic pools deleted: " << j
                  << " / Total memory freed: " << std::setprecision(2)
                  << tmem/1048576 << std::setprecision(6) << " MB" << std::endl;
        std::cout << "========================================================="
                  << std::endl;
    }

    m_list.clear();
}

//============================================================================//

allocator_list_tl::size_type allocator_list_tl::size() const
{
    return m_list.size();
}
//============================================================================//

} // namespace details
} // namespace mad
