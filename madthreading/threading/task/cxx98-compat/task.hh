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

#ifdef CXX11
    #include <functional>
    namespace mad
    {
        using std::function;
        using std::bind;
    }
#elif defined(USE_BOOST)
    #include <boost/function.hpp>
    #include <boost/bind.hpp>
    namespace mad
    {
        using boost::function;
        using boost::bind;
    }
#endif

namespace mad
{
//----------------------------------------------------------------------------//

#if defined(CXX11) || defined(USE_BOOST)
    #define FUNCTION_TYPEDEF_0(alias, ret) \
        typedef function<ret()> alias
    #define FUNCTION_TYPEDEF_1(alias, ret, arg) \
        typedef function<ret(arg)> alias
    #define FUNCTION_TYPEDEF_2(alias, ret, arg1, arg2) \
        typedef function<ret(arg1, arg2)> alias
    #define FUNCTION_TYPEDEF_3(alias, ret, arg1, arg2, arg3) \
        typedef function<ret(arg1, arg2, arg3)> alias
    #define CALL_FUNCTION(ptr2fn) ptr2fn
#else
    #define FUNCTION_TYPEDEF_0(alias, ret) \
        typedef ret(*alias)()
    #define FUNCTION_TYPEDEF_1(alias, ret, arg) \
        typedef ret(*alias)(arg)
    #define FUNCTION_TYPEDEF_2(alias, ret, arg1, arg2) \
        typedef ret(*alias)(arg1, arg2)
    #define FUNCTION_TYPEDEF_3(alias, ret, arg1, arg2, arg3) \
        typedef ret(*alias)(arg1, arg2, arg3)
    #define CALL_FUNCTION(ptr2fn) (*ptr2fn)
#endif
//----------------------------------------------------------------------------//

//============================================================================//
/// \brief vtask is the abstract class stored in thread_pool
class vtask //: public mad::Allocator
{
public:
    typedef vtask*          iterator;
    typedef const vtask*    const_iterator;
    typedef size_t          size_type;

public:
    vtask(task_group* tg,
          void* result = 0, void* arg1 = 0,
          void* arg2 = 0, void* arg3 = 0);
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

    template <unsigned N, typename _Tp>
    void set(_Tp val)
    {
        if(N > 3)
            throw std::out_of_range("vtask::set<N> argument array is only "
                                    "of size 3 -- " + n2s(N) + " -- out "
                                    "of bounds");
        //_Tp* _ptr = new _Tp(val);
        _Tp& _ref = *((_Tp*)(m_arg_array[N]));
        _ref = val;
        //*(m_arg_array)[N] = val;
    }

    // don't save the task even if it has a return value
    bool is_stored_elsewhere() const { return m_is_stored_elsewhere; }
    void is_stored_elsewhere(bool val) { m_is_stored_elsewhere = val; }
    // always delete the task, even if it has a result value
    bool force_delete() const { return m_force_delete; }
    void force_delete(bool val) { m_force_delete = val; }

public:
    void set_result(void* val) { m_result = val; }
    void set_arg_array(void* arg1 = 0, void* arg2 = 0, void* arg3 = 0)
    {
        m_arg_array[0] = arg1;
        m_arg_array[1] = arg2;
        m_arg_array[2] = arg3;
    }

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
    void*       m_arg_array[3];

private:
    template <typename _Tp>
    std::string n2s(_Tp val)
    {
        std::stringstream ss;
        ss << val; return ss.str();
    }
};

//============================================================================//

/// \brief The task class is supplied to thread_pool. This task type is used
/// in conjunction with free functions and/or static member functions
/// or, if Boost is enabled, can be used with member functions
/// via boost::bind
///
/// \details For a given thread_pool (tp) and given function
/// [void doSomething(int*) ], you create a new task, e.g.
///     int* x = new int(5);
///     task<void, int> t = new task<void, int>(&doSomething, x);
///     tp.add_task(t);
/// The thread_pool will push the task back into a deque and have a thread
/// run it when available
///
/// There are two partial specializations for (1) when the function return type
/// is void [ "task <void, _Arg>" and "void doSomething(_Arg*)" ] and for (2)
/// when the function return type is void and the function takes no parameters
/// [ "task <void, void>" and "void doSomething()" ]
///
template <typename _Ret, typename _Arg1, typename _Arg2 = void,
          typename _Arg3 = void>
class task : public vtask
{
public:
    typedef _Arg1 argument_type_1;
    typedef _Arg2 argument_type_2;
    typedef _Arg3 argument_type_3;
    typedef _Ret result_type;
    FUNCTION_TYPEDEF_3(function_type,
                       result_type,
                       argument_type_1,
                       argument_type_2,
                       argument_type_3);

public:
    // pass a free function pointer
    task(task_group* tg,
         function_type fn_ptr, argument_type_1 arg_1,
         argument_type_2 arg_2, argument_type_3 arg_3)
    : vtask(tg, new _Ret),
      m_fn_ptr(fn_ptr), m_arg_1(arg_1),
      m_arg_2(arg_2), m_arg_3(arg_3)
    {
        _check_group();
        set_arg_array(&m_arg_1, &m_arg_2, &m_arg_3);
    }

    virtual ~task() { delete m_result; }

public:
    virtual void operator()()
    {
        *(_Ret*)(m_result) = CALL_FUNCTION(m_fn_ptr)(m_arg_1, m_arg_2, m_arg_3);
    }

protected:
    using vtask::m_group;

private:
    function_type   m_fn_ptr;
    argument_type_1 m_arg_1;
    argument_type_2 m_arg_2;
    argument_type_3 m_arg_3;

};

//============================================================================//

template <typename _Ret, typename _Arg1, typename _Arg2>
class task<_Ret, _Arg1, _Arg2, void> : public vtask
{
public:
    typedef _Arg1 argument_type_1;
    typedef _Arg2 argument_type_2;
    typedef _Ret result_type;
    FUNCTION_TYPEDEF_2(function_type,
                       result_type,
                       argument_type_1,
                       argument_type_2);

public:
    // pass a free function pointer
    task(task_group* tg,
         function_type fn_ptr, argument_type_1 arg_1, argument_type_2 arg_2)
    : vtask(tg, new _Ret),
      m_fn_ptr(fn_ptr), m_arg_1(arg_1), m_arg_2(arg_2)
    {
        _check_group();
        set_arg_array(&m_arg_1, &m_arg_2, NULL);
    }

public:
    virtual void operator()()
    {
        *(_Ret*)(m_result) = CALL_FUNCTION(m_fn_ptr)(m_arg_1, m_arg_2);
    }

protected:
    using vtask::m_group;

private:
    function_type   m_fn_ptr;
    argument_type_1 m_arg_1;
    argument_type_2 m_arg_2;

};

//============================================================================//

/// \brief The task class is supplied to thread_pool.
///
/// specialization RET<ARG>
///

template <typename _Ret, typename _Arg1>
class task<_Ret, _Arg1, void, void> : public vtask
{
public:
    typedef _Arg1 argument_type_1;
    typedef _Ret result_type;
    FUNCTION_TYPEDEF_1(function_type,
                       result_type,
                       argument_type_1);

public:
    // pass a free function pointer
    task(task_group* tg,
         function_type fn_ptr, argument_type_1 arg1)
    : vtask(tg, new _Ret),
      m_fn_ptr(fn_ptr), m_arg_1(arg1)
    {
        _check_group();
        set_arg_array(&m_arg_1, NULL, NULL);
    }

public:
    virtual void operator()()
    {
        *(_Ret*)(m_result) = CALL_FUNCTION(m_fn_ptr)(m_arg_1);
    }

protected:
    using vtask::m_group;

private:
    function_type m_fn_ptr;
    argument_type_1 m_arg_1;

};

//============================================================================//

/// \brief Partial specialization of task for when the function return type
/// is void [ "task <void, _Arg>" and function is "void doSomething(_Arg*)" ]
template <typename _Arg1, typename _Arg2, typename _Arg3>
class task<void, _Arg1, _Arg2, _Arg3> : public vtask
{
public:
    typedef _Arg1 argument_type_1;
    typedef _Arg2 argument_type_2;
    typedef _Arg3 argument_type_3;
    typedef void result_type;
    FUNCTION_TYPEDEF_3(function_type,
                       result_type,
                       argument_type_1,
                       argument_type_2,
                       argument_type_3);

public:
    // pass a free function pointer
    task(task_group* tg,
         function_type fn_ptr, argument_type_1 arg1,
         argument_type_2 arg2, argument_type_3 arg3)
    : vtask(tg), m_fn_ptr(fn_ptr), m_arg_1(arg1), m_arg_2(arg2), m_arg_3(arg3)
    {
        _check_group();
        set_arg_array(&m_arg_1, &m_arg_2, &m_arg_3);
    }

public:
    virtual void operator()()
    {
        CALL_FUNCTION(m_fn_ptr)(m_arg_1, m_arg_2, m_arg_3);
    }

protected:
    using vtask::m_group;

private:
    function_type   m_fn_ptr;
    argument_type_1 m_arg_1;
    argument_type_2 m_arg_2;
    argument_type_3 m_arg_3;

};

//============================================================================//

/// \brief Partial specialization of task for when the function return type
/// is void [ "task <void, _Arg>" and function is "void doSomething(_Arg*)" ]
template <typename _Arg1, typename _Arg2>
class task<void, _Arg1, _Arg2, void> : public vtask
{
public:
    typedef _Arg1 argument_type_1;
    typedef _Arg2 argument_type_2;
    typedef void result_type;
    FUNCTION_TYPEDEF_2(function_type,
                       result_type,
                       argument_type_1,
                       argument_type_2);

public:
    // pass a free function pointer
    task(task_group* tg,
         function_type fn_ptr, argument_type_1 arg1, argument_type_2 arg2)
    : vtask(tg), m_fn_ptr(fn_ptr), m_arg_1(arg1), m_arg_2(arg2)
    {
        _check_group();
        set_arg_array(&m_arg_1, &m_arg_2, NULL);
    }

public:
    virtual void operator()()
    {
        CALL_FUNCTION(m_fn_ptr)(m_arg_1, m_arg_2);
    }

protected:
    using vtask::m_group;

private:
    function_type   m_fn_ptr;
    argument_type_1 m_arg_1;
    argument_type_2 m_arg_2;

};

//============================================================================//

/// \brief Partial specialization of task for when the function return type
/// is void [ "task <void, _Arg>" and function is "void doSomething(_Arg*)" ]
template <typename _Arg1>
class task<void, _Arg1, void, void> : public vtask
{
public:
    typedef _Arg1 argument_type_1;
    typedef void result_type;
    FUNCTION_TYPEDEF_1(function_type,
                       result_type,
                       argument_type_1);

public:
    // pass a free function pointer
    task(task_group* tg,
         function_type fn_ptr, argument_type_1 arg1)
    : vtask(tg), m_fn_ptr(fn_ptr), m_arg_1(arg1)
    {
        _check_group();
        set_arg_array(&m_arg_1, NULL, NULL);
    }

public:
    virtual void operator()()
    {
        CALL_FUNCTION(m_fn_ptr)(m_arg_1);
    }

protected:
    using vtask::m_group;

private:
    function_type   m_fn_ptr;
    argument_type_1 m_arg_1;

};

//============================================================================//

/// \brief Partial specialization of task for when the function return type is
/// void and the function takes no parameters [ "task <void, void>" and the
/// function is "void doSomething()" ]
template <>
class task<void, void, void, void> : public vtask
{
public:
    typedef void argument_type;
    typedef void result_type;
    FUNCTION_TYPEDEF_0(function_type,
                       result_type);

public:
    // pass a free function pointer
    task(task_group* tg, function_type fn_ptr)
    : vtask(tg), m_fn_ptr(fn_ptr)
    { _check_group(); }

public:
    virtual void operator()()
    {
        CALL_FUNCTION(m_fn_ptr)();
    }

protected:
    using vtask::m_group;

private:
    function_type m_fn_ptr;

};

//============================================================================//


} // namespace mad

#endif
