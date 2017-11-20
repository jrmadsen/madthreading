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

    // finish execution
    virtual void wait() const
    {
        std::stringstream ss;
        ss << "Invalid call to mad::packaged_task<>::wait(). "
           << "Instead, call mad::packaged_task<>::get_future() and then "
           << "get() on the future";
        throw std::runtime_error(ss.str().c_str());
    }

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
      m_retrieved(false),
      m_function(func),
      m_ptask(std::bind(m_function, forward_args_t(_Args, args))),
      m_group(tg)
    {
        if(m_group)
            m_group->add(this, m_promise.get_future());
    }

    virtual ~task() { }

public:
    // execution operator
    virtual void operator()()
    {
        m_future = m_ptask.get_future();
        m_ptask();
        m_retrieved = false;
    }

    // finish execution
    virtual void wait() const
    {
        if(!m_retrieved)
            m_promise.set_value(m_future.get());
        m_retrieved = true;
    }

public:
    // set arguments
    template <typename... _Tp>
    void set(_Tp... args)
    {   m_ptask = packaged_task_type(std::bind(m_function,
                                               forward_args_t(_Args, args))); }

private:
    mutable bool            m_retrieved;
    mutable promise_type    m_promise;
    mutable future_type     m_future;
    function_type           m_function;
    packaged_task_type      m_ptask;
    task_group_type*        m_group;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <typename... _Args>
class task<void, _Args...> : public details::vtask
{
public:
    typedef void                                _Ret;
    typedef _Ret                                result_type;
    typedef std::function<_Ret(_Args...)>       function_type;
    typedef std::promise<result_type>           promise_type;
    typedef std::future<result_type>            future_type;
    typedef std::packaged_task<result_type()>   packaged_task_type;
    typedef task_group<result_type>             task_group_type;

public:
    // pass a free function pointer
    task(task_group_type* tg, function_type func, _Args... args)
    : details::vtask(tg),
      m_retrieved(false),
      m_function(func),
      m_ptask(std::bind(m_function, forward_args_t(_Args, args))),
      m_group(tg)
    {
        if(m_group)
            m_group->add(this, m_promise.get_future());
    }

    virtual ~task() { }

public:
    // execution operator
    virtual void operator()()
    {
        m_future = m_ptask.get_future();
        m_ptask();
        m_retrieved = false;
    }

    // finish execution
    virtual void wait() const
    {
        if(!m_retrieved)
            m_future.get();
        m_retrieved = true;
    }

    // set arguments
    template <typename... _Tp>
    void set(_Tp... args)
    {   m_ptask = packaged_task_type(std::bind(m_function,
                                               forward_args_t(_Args, args))); }

private:
    mutable bool            m_retrieved;
    mutable promise_type    m_promise;
    mutable future_type     m_future;
    function_type           m_function;
    packaged_task_type      m_ptask;
    task_group_type*        m_group;
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
      m_retrieved(false),
      m_ptask(func),
      m_group(tg)
    {
        if(m_group)
            m_group->add(this, m_promise.get_future());
    }

    virtual ~task() { }

public:
    // execution operator
    virtual void operator()()
    {
        m_future = m_ptask.get_future();
        m_ptask();
        m_retrieved = false;
    }

    // finish execution
    virtual void wait() const
    {
        if(!m_retrieved)
            m_future.get();
        m_retrieved = true;
    }

private:
    mutable bool            m_retrieved;
    mutable promise_type    m_promise;
    mutable future_type     m_future;
    function_type           m_function;
    packaged_task_type      m_ptask;
    task_group_type*           m_group;
};

//============================================================================//


} // namespace mad


#endif
