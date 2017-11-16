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

#include "madthreading/macros.hh"
#include "madthreading/threading/threading.hh"
#include "madthreading/threading/auto_lock.hh"
#include "madthreading/allocator/allocator.hh"
#include "madthreading/threading/task/task_group.hh"

#include <functional>
#include <utility>
#include <tuple>
#include <cstddef>
#include <string>
#include <array>
#include <future>
#include <thread>


namespace mad
{

//============================================================================//
/// \brief vtask is the abstract class stored in thread_pool
class vtask : public mad::Allocator
{
public:
    typedef vtask*          iterator;
    typedef const vtask*    const_iterator;
    typedef size_t          size_type;

public:
    vtask(task_group* tg);
    virtual ~vtask() { }

public:
    iterator begin() { return this; }
    iterator end() { return this+1; }
    const_iterator begin() const { return this; }
    const_iterator end() const { return this+1; }
    size_type size() const { return 1; }

public:
    virtual void operator()() = 0;

    virtual void* get() const { return nullptr; }
    virtual void set_result(void*)
    {
        std::runtime_error("Bad function call vtask::set_result(void*)");
    }

    template <typename... _Tp>
    void set(_Tp...) { }

public:
    // get the task group
    task_group*&      group()       { return m_group; }
    const task_group* group() const { return m_group; }

protected:
    void _check_group();

protected:
    task_group* m_group;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <typename _Ret, typename... _Args>
class task : public vtask
{
public:
    typedef _Ret                                result_type;
    typedef std::function<_Ret(_Args...)>       function_type;
    typedef std::future<result_type>            future_type;
    typedef std::packaged_task<_Ret()>          packaged_task_type;

public:
    // pass a free function pointer
    task(task_group* tg, function_type fn_ptr, _Args... args)
    : vtask(tg),
      m_function(fn_ptr),
      m_ptask(std::bind(m_function, std::forward<_Args>(std::move(args))...)),
      m_result(new result_type())
    { }

    virtual ~task() { delete m_result; }

    template <typename... _Tp>
    void set(_Tp... args)
    {
        m_ptask = packaged_task_type(
                      std::bind(m_function,
                                std::forward<_Args>(std::move(args))...));
    }

    virtual void* get() const { return (void*) m_result; }
    virtual void set_result(void* ptr) { *m_result = *(result_type*) ptr; }

public:
    virtual void operator()()
    {
        m_future = m_ptask.get_future();
        m_ptask();
        *m_result = m_future.get();
    }

private:
    function_type                   m_function;
    packaged_task_type              m_ptask;
    mutable future_type             m_future;
    result_type*                    m_result;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <typename... _Args>
class task<void, _Args...> : public vtask
{
public:
    typedef void                            _Ret;
    typedef _Ret                            result_type;
    typedef std::function<_Ret(_Args...)>   function_type;
    typedef std::packaged_task<_Ret()>      packaged_task_type;

public:
    // pass a free function pointer
    task(task_group* tg, function_type fn_ptr, _Args... args)
    : vtask(tg),
      m_function(fn_ptr),
      m_ptask(std::bind(m_function, std::forward<_Args>(std::move(args))...))
    { }

    virtual ~task() { }

    template <typename... _Tp>
    void set(_Tp... args)
    {
        m_ptask = packaged_task_type(
                      std::bind(m_function,
                                std::forward<_Args>(std::move(args))...));

    }

public:
    virtual void operator()()
    {
        m_ptask();
    }

private:
    function_type       m_function;
    packaged_task_type  m_ptask;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <>
class task<void> : public vtask
{
public:
    typedef void _Ret;
    typedef _Ret result_type;
    typedef std::function<_Ret()> function_type;

public:
    // pass a free function pointer
    task(task_group* tg, function_type fn_ptr)
    : vtask(tg),
      m_function(fn_ptr)
    { }

    virtual ~task() { }

public:
    virtual void operator()()
    {
        m_function();
    }

private:
    function_type           m_function;
};

//============================================================================//


} // namespace mad


#endif
