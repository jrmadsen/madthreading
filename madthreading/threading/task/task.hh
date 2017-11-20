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
// created by jrmadsen on Wed Jul 22 09:15:04 2015
//
//
//
//

#ifndef task_hh_
#define task_hh_

#include "madthreading/threading/task/vtask.hh"
#include "madthreading/threading/task/task_group.hh"
#include <stdexcept>

#define forward_args_t(_Args, _args) std::forward<_Args>(std::move(_args))...

namespace mad
{

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <typename _Ret, typename... _Args>
class packaged_task : public details::vtask
{
public:
    typedef _Ret                            result_type;
    typedef std::function<_Ret(_Args...)>   function_type;
    typedef std::promise<result_type>       promise_type;
    typedef std::future<result_type>        future_type;
    typedef std::packaged_task<_Ret()>      packaged_task_type;
    typedef task_group<_Ret>                task_group_type;

public:
    // pass a free function pointer
    packaged_task(function_type func, _Args... args)
    : details::vtask(nullptr),
      m_ptask(std::bind(func, forward_args_t(_Args, args)))
    { }

    virtual ~packaged_task() { }

public:
    // execution operator
    virtual void operator()() { m_ptask(); }

    future_type get_future() { return m_ptask.get_future(); }

private:
    packaged_task_type      m_ptask;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <typename _Ret, typename... _Args>
class task : public details::vtask
{
public:
    typedef _Ret                            result_type;
    typedef std::function<_Ret(_Args...)>   function_type;
    typedef std::promise<result_type>       promise_type;
    typedef std::future<result_type>        future_type;
    typedef std::packaged_task<_Ret()>      packaged_task_type;
    typedef task_group<_Ret>                task_group_type;

public:
    // pass a free function pointer
    task(task_group_type* tg, function_type func, _Args... args)
    : details::vtask(tg),
      m_ptask(std::bind(func, forward_args_t(_Args, args)))
    {
        tg->add(m_ptask.get_future());
    }

    virtual ~task() { }

public:
    // execution operator
    virtual void operator()()
    {
        m_ptask();
    }

private:
    packaged_task_type      m_ptask;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <>
class task<void> : public details::vtask
{
public:
    typedef void                                _Ret;
    typedef _Ret                                result_type;
    typedef std::function<result_type()>        function_type;
    typedef std::promise<result_type>           promise_type;
    typedef std::future<result_type>            future_type;
    typedef std::packaged_task<result_type()>   packaged_task_type;
    typedef task_group<result_type>             task_group_type;

public:
    // pass a free function pointer
    task(task_group_type* tg, function_type func)
    : details::vtask(tg),
      m_ptask(func)
    {
        tg->add(m_ptask.get_future());
    }

    virtual ~task() { }

public:
    // execution operator
    virtual void operator()()
    {
        m_ptask();
    }

private:
    packaged_task_type      m_ptask;
};

//============================================================================//


} // namespace mad


#endif
