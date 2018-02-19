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

//============================================================================//

mad::thread_manager* mad::thread_manager::f_instance = nullptr;

//============================================================================//

mad::thread_manager* mad::thread_manager::instance()
{
    if(!f_instance)
    {
        auto nthreads = std::thread::hardware_concurrency();
        std::cout << "Allocating mad::thread_manager with " << nthreads
                  << " thread(s)..." << std::endl;
        auto tp = new thread_pool(nthreads);
        new thread_manager(tp);
    }
    return f_instance;
}

//============================================================================//

void mad::thread_manager::check_instance(thread_manager* local_instance)
{
    static std::string exist_msg
            = "Instance of singleton \"mad::thread_manager\" already exists!";
    static std::string null_msg
            = "Local instance to \"mad::thread_manager\" is a null pointer!";

    if(f_instance)
        throw std::runtime_error(exist_msg);
    else if(!local_instance)
        throw std::runtime_error(null_msg);
    else
        f_instance = local_instance;
}

//============================================================================//

mad::thread_manager*
mad::thread_manager::get_thread_manager(const int64_t& nthreads,
                                        const int& verbose)
{
    mad::thread_manager* tm = f_instance;
    if(!tm)
    {
        if(verbose > 0)
            std::cout << "Allocating mad::thread_manager with " << nthreads
                      << " thread(s)..." << std::endl;
        auto tp = new thread_pool(nthreads);
        tm = new thread_manager(tp);
    }
    else
    {
        if(verbose > 0)
            tmcout << "Using existing mad::thread_manager with "
                   << tm->size() << " thread(s)..." << std::endl;
    }
    return tm;
}

//============================================================================//

mad::thread_manager::thread_manager(thread_pool*& _pool)
: m_pool(_pool)
{
    check_instance(this);
}

//============================================================================//

mad::thread_manager::~thread_manager()
{
    delete m_pool;
    m_pool = nullptr;
    f_instance = nullptr;
}

//============================================================================//

mad::thread_manager::thread_manager(const thread_manager& rhs)
: m_pool(rhs.m_pool)
{ }

//============================================================================//

mad::thread_manager& mad::thread_manager::operator=(const thread_manager& rhs)
{
    if(this == &rhs)
        return *this;

    m_pool = rhs.m_pool;

    return *this;
}

//============================================================================//
