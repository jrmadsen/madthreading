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

thread_manager* thread_manager::fgInstance = 0;

//============================================================================//

thread_manager* thread_manager::Instance() { return fgInstance; }

//============================================================================//

task_group* thread_manager::m_default_group = new task_group(NULL);

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

    m_default_group->set_pool(fgInstance->m_data->tp());
}

//============================================================================//

thread_manager::size_type
thread_manager::max_threads = Threading::GetNumberOfCores();

//============================================================================//

thread_manager::thread_manager()
: m_data(new data_type),
  m_current_group(0),
  m_is_clone(false)
{
    check_instance(this);
}

//============================================================================//

thread_manager::thread_manager(size_type _n, bool _use_affinity)
: m_data(new data_type(_n)),
  m_current_group(0),
  m_is_clone(false)
{
    check_instance(this);
    this->use_affinity(_use_affinity);
}

//============================================================================//

thread_manager::~thread_manager()
{
    if(!m_is_clone)
    {
        finalize();
        delete m_data;
        m_data = 0;
        fgInstance = 0;
    }
    else
    {
        delete m_current_group;
    }
}

//============================================================================//

thread_manager::thread_manager(const thread_manager& rhs)
: m_data(rhs.m_data),
  m_current_group(rhs.m_current_group),
  m_is_clone(true)
{ }

//============================================================================//

thread_manager& thread_manager::operator=(const thread_manager& rhs)
{
    if(this == &rhs)
        return *this;

    m_data = rhs.m_data;
    m_current_group = rhs.m_current_group;
    return *this;
}

//============================================================================//

thread_manager* thread_manager::clone(task_group* tg) const
{
    thread_manager* rhs = new thread_manager(*this);
    if(!tg)
        tg = new task_group(rhs->m_data->tp());
    rhs->m_current_group = tg;
    return rhs;
}
//============================================================================//

} // namespace mad
