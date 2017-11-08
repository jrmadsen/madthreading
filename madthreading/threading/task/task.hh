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

#define FUNCTION_TYPEDEF_0(alias, ret) \
    typedef function<ret()> alias
#define FUNCTION_TYPEDEF_1(alias, ret, arg) \
    typedef function<ret(arg)> alias
#define FUNCTION_TYPEDEF_2(alias, ret, arg1, arg2) \
    typedef function<ret(arg1, arg2)> alias
#define FUNCTION_TYPEDEF_3(alias, ret, arg1, arg2, arg3) \
    typedef function<ret(arg1, arg2, arg3)> alias
#define CALL_FUNCTION(ptr2fn) ptr2fn

namespace mad
{
using std::function;
using std::bind;
using std::tuple;
}

namespace mad
{
//----------------------------------------------------------------------------//

namespace details
{
//----------------------------------------------------------------------------//
template <typename _Tp, _Tp... _Ints>
struct integer_sequence
{ };
//----------------------------------------------------------------------------//
template <typename _Sp>
struct next_integer_sequence;
//----------------------------------------------------------------------------//
template <typename _Tp, _Tp... _Ints>
struct next_integer_sequence<integer_sequence<_Tp, _Ints...>>
{
    using type = integer_sequence<_Tp, _Ints..., sizeof...(_Ints)>;
};
//----------------------------------------------------------------------------//
template <typename _Tp, _Tp _I, _Tp _N>
struct make_int_seq_impl;
//----------------------------------------------------------------------------//
template <typename _Tp, _Tp _N>
using make_integer_sequence = typename make_int_seq_impl<_Tp, 0, _N>::type;
//----------------------------------------------------------------------------//
template <typename _Tp, _Tp _I, _Tp _N>
struct make_int_seq_impl
{
    using type = typename next_integer_sequence<
        typename make_int_seq_impl<_Tp, _I+1, _N>::type>::type;
};
//----------------------------------------------------------------------------//
template <typename _Tp, _Tp _N>
struct make_int_seq_impl<_Tp, _N, _N>
{
    using type = integer_sequence<_Tp>;
};
//----------------------------------------------------------------------------//
template <std::size_t... _Ints>
using index_sequence = integer_sequence<std::size_t, _Ints...>;
//----------------------------------------------------------------------------//
template <std::size_t _N>
using make_index_sequence = make_integer_sequence<std::size_t, _N>;
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//

template <std::size_t... _Indices>
struct indices
{ };

//----------------------------------------------------------------------------//

template <std::size_t _N, std::size_t... _I>
struct build_indices : build_indices<_N-1, _N-1, _I...>
{ };

//----------------------------------------------------------------------------//

template <std::size_t... _I>
struct build_indices<0, _I...> : indices<_I...>
{ };

//----------------------------------------------------------------------------//

template <typename _Ret, typename _Func, typename _ArgTuple,
          std::size_t... _Indices>
_Ret _call_ret(const _Func& func, _ArgTuple&& args, const indices<_Indices...>&)
{
   return std::move(func(std::get<_Indices>(std::forward<_ArgTuple>(args))...));
}

//----------------------------------------------------------------------------//

template <typename _Func, typename _ArgTuple, std::size_t... _Indices>
void _call_void(const _Func& func, _ArgTuple&& args, const indices<_Indices...>&)
{
   func(std::get<_Indices>(std::forward<_ArgTuple>(args))...);
}

//----------------------------------------------------------------------------//

template <typename _Ret, typename _Func, typename _ArgTuple>
_Ret call_ret(const _Func& func, _ArgTuple&& args)
{
    typedef typename std::remove_reference<_ArgTuple>::type tuple_t;
    const build_indices<std::tuple_size<tuple_t>::value> indices = {};
    return std::move(_call_ret<_Ret>(func, std::move(args), indices));
}

//----------------------------------------------------------------------------//

template <typename _Func, typename _ArgTuple>
void call_void(const _Func& func, _ArgTuple&& args)
{
    typedef typename std::remove_reference<_ArgTuple>::type tuple_t;
    const build_indices<std::tuple_size<tuple_t>::value> indices = {};
    _call_void(func, std::move(args), indices);
}

//----------------------------------------------------------------------------//
} // namespace details

//============================================================================//
/// \brief vtask is the abstract class stored in thread_pool
class vtask //: public mad::Allocator
{
public:
    typedef vtask*          iterator;
    typedef const vtask*    const_iterator;
    typedef size_t          size_type;

public:
    vtask(task_group* tg, void* result = 0);
    virtual ~vtask() { }

public:
    iterator begin() { return this; }
    iterator end() { return this+1; }
    const_iterator begin() const { return this; }
    const_iterator end() const { return this+1; }
    size_type size() const { return 1; }

public:
    virtual void operator()() = 0;

    inline void* get() const { return m_result; }

    // don't save the task even if it has a return value
    bool is_stored_elsewhere() const { return m_is_stored_elsewhere; }
    void is_stored_elsewhere(bool val) { m_is_stored_elsewhere = val; }
    // always delete the task, even if it has a result value
    bool force_delete() const { return m_force_delete; }
    void force_delete(bool val) { m_force_delete = val; }

    template <typename... _Tp>
    void set(_Tp...) { }

public:
    void set_result(void* val) { m_result = val; }

    // get the task group
    task_group*&      group()       { return m_group; }
    const task_group* group() const { return m_group; }

protected:
    void _check_group();

protected:
    task_group* m_group;
    bool        m_force_delete;
    bool        m_is_stored_elsewhere;
    void*       m_result;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <typename _Ret, typename... _Args>
class task : public vtask
{
public:
    typedef _Ret result_type;
    typedef mad::function<_Ret(_Args...)> function_type;

public:
    // pass a free function pointer
    task(task_group* tg, function_type fn_ptr, _Args... args)
    : vtask(tg, (void*)(new _Ret)),
      m_fn_ptr(fn_ptr),
      m_args(std::forward<_Args>(std::move(args))...)
    {
        _check_group();
    }

    virtual ~task()
    {
        if(m_result)
        {
            delete (_Ret*) m_result;
            m_result = 0;
        }
    }

    #define is_same_t(_Tp, _Up) std::is_same<_Tp, _Up>::value

    template <bool _Bp, typename _Tp = void>
    using enable_if_t = typename std::enable_if<_Bp, _Tp>::type;

    template <typename... _Tp>
    void set(_Tp... args)
    {
        m_args = std::tuple<_Args...>(std::forward<_Args>(std::move(args))...);
    }

public:
    //#define is_same_t(_Tp, _Up) std::is_same<_Tp, _Up>::value
    //template <bool _Bp, typename _Tp = void>
    //using enable_if_t = typename std::enable_if<_Bp, _Tp>::type;
    virtual void operator()()
    {
        *((_Ret*)(m_result)) = details::call_ret<_Ret>(m_fn_ptr, m_args);
    }

private:
    function_type           m_fn_ptr;
    std::tuple<_Args...>    m_args;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <typename... _Args>
class task<void, _Args...> : public vtask
{
public:
    typedef void _Ret;
    typedef _Ret result_type;
    typedef mad::function<_Ret(_Args...)> function_type;

public:
    // pass a free function pointer
    task(task_group* tg, function_type fn_ptr, _Args... args)
    : vtask(tg),
      m_fn_ptr(fn_ptr),
      m_args(std::forward<_Args>(std::move(args))...)
    {
        _check_group();
    }

    virtual ~task() { }

    template <typename... _Tp>
    void set(_Tp... args)
    {
        m_args = std::tuple<_Args...>(std::forward<_Args>(std::move(args))...);
    }

public:
    virtual void operator()()
    {
        details::call_void(m_fn_ptr, m_args);
    }

private:
    function_type           m_fn_ptr;
    std::tuple<_Args...>    m_args;
};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
template <>
class task<void> : public vtask
{
public:
    typedef void _Ret;
    typedef _Ret result_type;
    typedef mad::function<_Ret()> function_type;

public:
    // pass a free function pointer
    task(task_group* tg, function_type fn_ptr)
    : vtask(tg),
      m_fn_ptr(fn_ptr)
    {
        _check_group();
    }

    virtual ~task() { }

public:
    virtual void operator()()
    {
        m_fn_ptr();
    }

private:
    function_type           m_fn_ptr;
};

//============================================================================//


} // namespace mad

#endif
