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
// created by jrmadsen on Wed Jul 15 14:45:53 2015
//
//
//
//


#include "madthreading/threading/thread_manager.hh"

namespace mad
{

//============================================================================//

thread_manager* thread_manager::fgInstance = nullptr;

//============================================================================//

thread_manager* thread_manager::Instance() { return fgInstance; }

//============================================================================//

thread_manager* thread_manager::instance() { return fgInstance; }

//============================================================================//

task_group* thread_manager::m_default_group = nullptr;

//============================================================================//

void thread_manager::check_instance(thread_manager* local_instance)
{
    static std::string exist_msg
            = "Instance of singleton \"thread_manager\" already exists!";
    static std::string null_msg
            = "Local instance to \"thread_manager\" is a null pointer!";

    if(fgInstance)
        throw std::runtime_error(exist_msg);
    else if(!local_instance)
        throw std::runtime_error(null_msg);
    else
        fgInstance = local_instance;

    if(m_default_group)
        delete m_default_group;

    m_default_group = new task_group(fgInstance->m_data->tp());
}

//============================================================================//

thread_manager* thread_manager::get_thread_manager(const uint32_t& nthreads,
                                                   const int& verbose)
{
    mad::thread_manager* tm = mad::thread_manager::instance();
    if(!tm)
    {
        if(verbose > 0)
            tmcout << "Allocating thread manager with " << nthreads
                   << " thread(s)..." << std::endl;
        tm = new thread_manager(nthreads);
    }
    else if(tm->size() != nthreads)
    {
        if(verbose > 0)
            tmcout << "thread_manager exists - Allocating " << nthreads
                   << "..." << std::endl;
        tm->allocate_threads(nthreads);
    }
    else
    {
        if(verbose > 0)
            tmcout << "Using existing thread_manager with "
                   << tm->size() << " thread(s)..." << std::endl;
    }
    return tm;
}

//============================================================================//

thread_manager::size_type
thread_manager::max_threads = 4*std::thread::hardware_concurrency();

//============================================================================//

thread_manager::thread_manager()
: m_is_clone(false),
  m_data(new data_type),
  m_current_group(nullptr)
{
    check_instance(this);
}

//============================================================================//

thread_manager::thread_manager(size_type _n, bool _use_affinity)
: m_is_clone(false),
  m_data(new data_type(_n)),
  m_current_group(nullptr)
{
    check_instance(this);
    this->use_affinity(_use_affinity);
}

//============================================================================//

thread_manager::~thread_manager()
{
    if(!m_is_clone)
    {
        if(m_current_group != m_default_group)
            delete m_current_group;
        m_current_group = nullptr;
        delete m_default_group;
        m_default_group = nullptr;
        finalize();
        delete m_data;
        m_data = nullptr;
        fgInstance = nullptr;
    }
    else
    {
        delete m_current_group;
    }
}

//============================================================================//

thread_manager::thread_manager(const thread_manager& rhs)
: m_is_clone(true),
  m_data(rhs.m_data),
  m_current_group(rhs.m_current_group)
{
    if(!m_default_group)
        m_default_group = new task_group(m_data->tp());
}

//============================================================================//

thread_manager& thread_manager::operator=(const thread_manager& rhs)
{
    if(this == &rhs)
        return *this;

    m_is_clone = rhs.m_is_clone;
    m_data = rhs.m_data;
    m_current_group = rhs.m_current_group;

    if(!m_default_group)
        m_default_group = new task_group(m_data->tp());

    return *this;
}

//============================================================================//

thread_manager* thread_manager::clone(task_group* tg) const
{
    thread_manager* rhs = new thread_manager(*this);
    if(!tg)
        tg = new task_group(rhs->m_data->tp());
    rhs->m_is_clone = true;
    rhs->m_data = m_data;
    rhs->m_current_group = tg;
    return rhs;
}
//============================================================================//

} // namespace mad
