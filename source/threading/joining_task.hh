//
//
//
//
//
// created by jrmadsen on Wed Jul 22 09:15:04 2015
//
//
//
//


#ifndef joining_task_hh_
#define joining_task_hh_

#ifdef SWIG
%module joining_task
%{
    #include "joining_task.hh"
%}
#endif

#include "task.hh"

namespace mad
{
//----------------------------------------------------------------------------//

template <typename _Ret>
class joiner
{
public:
    typedef _Ret result_type;
    FUNCTION_TYPEDEF_2(return_func_type,
                       result_type,
                       result_type,
                       result_type);

public:
    joiner(return_func_type rfunc)
    : m_return_func_ptr(rfunc) { }

public:
    operator result_type() { return m_result; }

    result_type operator()(result_type r)
    { return m_return_func_ptr(m_result, r); }

    result_type operator=(result_type r) { return (m_result = r); }

protected:
    return_func_type m_return_func_ptr;
    result_type m_result;

};

//============================================================================//

template <typename _Ret, typename _Arg1, typename _Arg2 = void,
          typename _Arg3 = void>
class joining_task : public vtask
{
public:
    typedef _Arg1 arg_type_1;
    typedef _Arg2 arg_type_2;
    typedef _Arg3 arg_type_3;
    typedef _Ret result_type;
    FUNCTION_TYPEDEF_3(function_type,
                       result_type,
                       arg_type_1,
                       arg_type_2,
                       arg_type_3);

    typedef          joiner<result_type>            joiner_type;
    typedef typename joiner_type::return_func_type  return_func_type;

public:
    // pass a free function pointer
    joining_task(function_type fn_ptr,
                 arg_type_1 arg_1,
                 arg_type_2 arg_2,
                 arg_type_3 arg_3,
                 return_func_type rfunc)
    : joiner_type(rfunc),
      m_fn_ptr(fn_ptr), m_arg_1(arg_1),
      m_arg_2(arg_2), m_arg_3(arg_3)
    {}

    virtual ~joining_task() { }

public:
    virtual void operator()()
    {
        task_joiner(CALL_FUNCTION(m_fn_ptr)(m_arg_1, m_arg_2, m_arg_3));
    }

    virtual void* get() const { return NULL; }

    template <unsigned N, typename _Tp> void set(_Tp _arg)
    {
        switch (N)
        {
            case 0:
                m_arg_1 = _arg;
                break;
            case 1:
                m_arg_2 = _arg;
                break;
            case 2:
                m_arg_2 = _arg;
                break;
            default:
                break;
        }
    }

private:
    function_type m_fn_ptr;
    arg_type_1 m_arg_1;
    arg_type_2 m_arg_2;
    arg_type_3 m_arg_3;

public:
    static joiner_type task_joiner;

};

//============================================================================//

template <typename _Ret, typename _Arg1, typename _Arg2>
class joining_task<_Ret, _Arg1, _Arg2, void> : public vtask
{
public:
    typedef _Arg1 arg_type_1;
    typedef _Arg2 arg_type_2;
    typedef _Ret result_type;
    FUNCTION_TYPEDEF_2(function_type,
                       result_type,
                       arg_type_1,
                       arg_type_2);

    typedef          joiner<result_type>            joiner_type;
    typedef typename joiner_type::return_func_type  return_func_type;

public:
    // pass a free function pointer
    joining_task(function_type fn_ptr,
                 arg_type_1 arg_1, arg_type_2 arg_2)
    : m_fn_ptr(fn_ptr), m_arg_1(arg_1), m_arg_2(arg_2)
    {}

    virtual ~joining_task() { }

public:
    virtual void operator()()
    {
        task_joiner(CALL_FUNCTION(m_fn_ptr)(m_arg_1, m_arg_2));
    }

    virtual void* get() const { return NULL; }

    template <unsigned N, typename _Tp> void set(_Tp _arg)
    {
        switch (N)
        {
            case 0:
                m_arg_1 = _arg;
                break;
            case 1:
                m_arg_2 = _arg;
                break;
            default:
                break;
        }
    }

private:
    function_type m_fn_ptr;
    arg_type_1 m_arg_1;
    arg_type_2 m_arg_2;

public:
    static joiner_type task_joiner;

};

//============================================================================//

template <typename _Ret, typename _Arg>
class joining_task<_Ret, _Arg, void, void> : public vtask
{
public:
    typedef _Arg argument_type;
    typedef _Ret result_type;
    FUNCTION_TYPEDEF_1(function_type,
                       result_type,
                       argument_type);

    typedef          joiner<result_type>            joiner_type;
    typedef typename joiner_type::return_func_type  return_func_type;

public:
    // pass a free function pointer
    joining_task(function_type fn_ptr, argument_type arg)
    : m_fn_ptr(fn_ptr), m_arg(arg)
    {}

    virtual ~joining_task() { }

public:
    virtual void operator()()
    {
        task_joiner(CALL_FUNCTION(m_fn_ptr)(m_arg));
    }

    virtual void* get() const { return NULL; }

    template <typename _Tp, typename N>  void set(_Tp _arg) { m_arg = _arg; }

private:
    function_type m_fn_ptr;
    argument_type m_arg;

public:
    static joiner_type task_joiner;

};

//============================================================================//

template <typename _Ret, typename _Arg1, typename _Arg2, typename _Arg3>
typename joining_task<_Ret, _Arg1, _Arg2, _Arg3>::joiner_type
joining_task<_Ret, _Arg1, _Arg2, _Arg3>::task_joiner;

//============================================================================//

} // namespace mad

#endif
