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
// created by jrmadsen on Wed Jul 15 13:56:47 2015
//
//
//
//


#ifndef thread_manager_hh_
#define thread_manager_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG

%define MACRO(cl)
%pythoncode
%{
    def cl():
        return
%}
%enddef

%module thread_manager
%{
    #define SWIG_FILE_WITH_INIT
    #include "madthreading/allocator/allocator.hh"
    #include "madthreading/threading/thread_pool.hh"
    #include "madthreading/threading/task/task_tree.hh"
    #include "madthreading/threading/task/task_group.hh"
    #include "madthreading/threading/thread_manager.hh"
%}

%import "madthreading/threading/thread_pool.hh"
%import "madthreading/threading/task/task_tree.hh"
%import "madthreading/threading/task/task_group.hh"
%include "thread_manager.hh"

MACRO(tmid)
MACRO(_tid_)

#endif
//----------------------------------------------------------------------------//
#ifndef __inline__
#   ifdef SWIG
#       define __inline__ inline
#   else
#       define __inline__ inline __attribute__ ((always_inline))
#   endif
#endif
//----------------------------------------------------------------------------//

#include "madthreading/macros.hh"
#include "madthreading/threading/thread_pool.hh"
#include "madthreading/threading/task/task.hh"
#include "madthreading/threading/task/task_tree.hh"
#include "madthreading/threading/task/task_group.hh"
#include "madthreading/allocator/allocator.hh"

// task.hh defines mad::function and mad::bind if CXX11 or USE_BOOST

#include <algorithm>
#include <numeric>
#include <iomanip>
#include <cmath>
#include <cassert>

#if defined(USE_OPENMP)
#   include <omp.h>
#endif
namespace mad
{

//============================================================================//

namespace details
{

class thread_manager_data
{
public:
    typedef size_t size_type;

public:
    // Constructor and Destructors
    thread_manager_data()
    : m_size(0), m_tp(new mad::thread_pool)
    { }

    thread_manager_data(size_type _n)
    : m_size(0), m_tp(new mad::thread_pool(_n))
    {
        allocate_threads(_n);
    }

    virtual ~thread_manager_data()
    {
        delete_threads();
    }

public:
    //------------------------------------------------------------------------//
    size_type size() const { return m_size; }
    //------------------------------------------------------------------------//
    void delete_threads(const int& _n = 0)
    {
        int n_new_threads = 0;
        if(_n > 0)
            n_new_threads = m_tp->size() - _n;

        delete m_tp;
        m_tp = 0;

        if(n_new_threads > 0)
        {
            m_size = n_new_threads;
            m_tp = new mad::thread_pool(m_size);
        }
    }
    //------------------------------------------------------------------------//
    void allocate_threads(size_type _n)
    {
        if(_n != m_size)
        {
            m_tp->set_size(_n);
            m_size = m_tp->initialize_threadpool();
        }
    }
    //------------------------------------------------------------------------//

    mad::thread_pool* tp() { return m_tp; }
    const mad::thread_pool* tp() const { return m_tp; }

    //------------------------------------------------------------------------//

protected:
    size_type m_size;
    mad::thread_pool* m_tp;
};

} // namespace details

//============================================================================//

class thread_manager
{
public:
    typedef thread_manager this_type;
    typedef details::thread_manager_data data_type;
    typedef data_type::size_type size_type;
    typedef std::random_access_iterator_tag iterator_category;

public:
    // Constructor and Destructors
    thread_manager();
    thread_manager(size_type _n, bool _use_affinity = false);
    virtual ~thread_manager();

    thread_manager* clone(mad::task_group* = 0) const;

private:
    // disable external copying and assignment
    thread_manager& operator=(const thread_manager&);
    thread_manager(const thread_manager&);

public:
    /// get the singleton pointer
    static thread_manager* Instance();
    /// get the singleton pointer
    static thread_manager* instance();
    /// get the default task group
    static mad::task_group* default_task_group() { return m_default_group; }
    /// get the singleton pointer if exists and allocate nthreads in the
    /// thread pool.
    /// if an instance has not been created, create one with thread pool of
    /// nthreads
    static thread_manager* get_thread_manager(const uint32_t& nthreads,
                                              const int& verbose = 0);

    /// function for returning the thread id
    template <typename _Tp>
    static long id(_Tp thread_self)
    {
        long _id = -1;
        static CoreMutex _mtx;
        auto_lock l(&_mtx);
        thread_manager* _tm = thread_manager::Instance();
        if(!_tm)
            return _id;
        if(_tm->m_data->tp()->GetThreadIDs().find(thread_self) !=
           _tm->m_data->tp()->GetThreadIDs().end())
            _id = _tm->m_data->tp()->GetThreadIDs().find(thread_self)->second;
        return _id;
    }

    /// function for returning the thread id in string format
    template <typename _Tp>
    static std::string sid(_Tp thread_self)
    {
        long _id = thread_manager::id(thread_self);
        if(_id < 0)
            return "";
        std::stringstream ss;
        ss.fill('0');
        ss << "[" << std::setw(4) << _id << "] ";
        return ss.str();
    }

    /// function for returning the thread id in string format
    template <typename _Tp>
    static std::string id_string(_Tp thread_self)
    {
        long _id = thread_manager::id(thread_self);
        if(_id < 0)
            return "0";
        thread_manager* _tm = thread_manager::Instance();
        short _w = 4;
        _w = std::min(_w, (short) std::ceil(std::log10(_tm->size())));
        std::stringstream ss;
        ss.fill('0');
        ss << std::setw(_w) << _id;
        return ss.str();
    }

private:
    static thread_manager* fgInstance;
    static void check_instance(thread_manager*);

public:
    // Public functions
    void use_affinity(bool _val) { m_data->tp()->use_affinity(_val); }

    void SetMaxThreads(const size_type& _n)   { max_threads = _n; }
    void set_max_threads(const size_type& _n) { max_threads = _n; }
    size_type GetMaxThreads()   const         { return max_threads; }
    size_type get_max_threads() const         { return max_threads; }

public:
    //------------------------------------------------------------------------//
    /// return the number of threads in the thread pool
    size_type size() const { return m_data->size(); }
    //------------------------------------------------------------------------//
    /// allocate the number of threads
    void allocate_threads(size_type _n) { m_data->allocate_threads(_n); }
    //------------------------------------------------------------------------//

private:
    mad::task_group* set_task_group(mad::task_group* tg = NULL)
    {
        if(tg && tg != m_current_group && tg != m_default_group)
        {
            std::stringstream ss;
            ss << "\n";
            ss << mad::thread_manager::sid(CORETHREADSELFINT());
            ss << "WARNING! ";
            ss << "Leaked <task_group> object! If a new task group was "
               << "explicitly created, use thread_manager::clone(task_group*) "
               << "to get a new pointer to a thread_manager holding the "
               << "task_group.\n";
            std::cerr << ss.str() << std::endl;
        }

        m_current_group = (tg && tg != m_default_group) ? tg : m_default_group;

        assert(m_current_group != 0);

        return m_current_group;
    }

public:
#if defined(MAD_USE_CXX98)
    //------------------------------------------------------------------------//
    // public exec functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func,
              typename _Arg1, typename _Arg2, typename _Arg3>
    __inline__
    void exec(_Func function, _Arg1 argument1, _Arg2 argument2, _Arg3 argument3,
              mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg1, _Arg2, _Arg3> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function, argument1, argument2, argument3);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg1, typename _Arg2>
    __inline__
    void exec(_Func function, _Arg1 argument1, _Arg2 argument2,
              mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg1, _Arg2> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function, argument1, argument2);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg>
    __inline__
    void exec(_Func function, _Arg argument,
              mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function, argument);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename _Arg>
    __inline__
    void exec(_Func function, _Arg argument,
              mad::task_group* tg = 0)
    {
        typedef task<void, _Arg> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function, argument);
        m_data->tp()->add_task(t);
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Func>
    __inline__
    void exec(_Func function,
              mad::task_group* tg = 0)
    {
        typedef task<void, void> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
#else // defined(MAD_USE_CXX98)
    //------------------------------------------------------------------------//
    // public exec functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename... _Args>
    __inline__
    void exec(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<_Ret, _Args...> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function, args...);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename... _Args>
    __inline__
    void exec(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<void, _Args...> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function, args...);
        m_data->tp()->add_task(t);
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    __inline__
    void exec(mad::task_group* tg, _Func function)
    {
        typedef task<_Ret> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func>
    __inline__
    void exec(mad::task_group* tg, _Func function)
    {
        typedef task<void> task_type;

        tg = set_task_group(tg);
        task_type* t = new task_type(tg, function);
        m_data->tp()->add_task(t);
    }
#endif
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    // public exec functions using default task group
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename... _Args>
    __inline__
    void exec(_Func function, _Args... args)
    {
        typedef task<_Ret, _Args...> task_type;

        mad::task_group* tg = set_task_group();
        task_type* t = new task_type(tg, function, args...);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename... _Args>
    __inline__
    void exec(_Func function, _Args... args)
    {
        typedef task<void, _Args...> task_type;

        mad::task_group* tg = set_task_group();
        task_type* t = new task_type(tg, function, args...);
        m_data->tp()->add_task(t);
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    __inline__
    void exec(_Func function)
    {
        typedef task<_Ret> task_type;

        mad::task_group* tg = set_task_group();
        task_type* t = new task_type(tg, function);
        m_data->tp()->add_task(t);
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func>
    __inline__
    void exec(_Func function)
    {
        typedef task<void> task_type;

        mad::task_group* tg = set_task_group();
        task_type* t = new task_type(tg, function);
        m_data->tp()->add_task(t);
    }
#endif
    //------------------------------------------------------------------------//
#endif // defined(MAD_USE_CXX98)

public:
#if defined(MAD_USE_CXX98)
    //------------------------------------------------------------------------//
    // public run functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg1, typename _Arg2,
              typename _Arg3>
    __inline__
    void run(_Func function, _Arg1 arg1, _Arg2 arg2, _Arg3 arg3,
             mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg1, _Arg2, _Arg3> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, arg1, arg2, arg3);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename _Arg1, typename _Arg2, typename _Arg3>
    __inline__
    void run(_Func function, _Arg1 arg1, _Arg2 arg2, _Arg3 arg3,
             mad::task_group* tg = 0)
    {
        typedef task<void, _Arg1, _Arg2, _Arg3> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, arg1, arg2, arg3);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg1, typename _Arg2>
    __inline__
    void run(_Func function, _Arg1 arg1, _Arg2 arg2,
             mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg1, _Arg2> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, arg1, arg2);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename _Arg1, typename _Arg2>
    __inline__
    void run(_Func function, _Arg1 arg1, _Arg2 arg2,
             mad::task_group* tg = 0)
    {
        typedef task<void, _Arg1, _Arg2> task_type;

        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, arg1, arg2);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg>
    __inline__
    void run(_Func function, _Arg argument,
             mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, argument);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename _Arg>
    __inline__
    void run(_Func function, _Arg argument,
             mad::task_group* tg = 0)
    {
        typedef task<void, _Arg> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, argument);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Func>
    __inline__
    void run(_Func function,
             mad::task_group* tg = 0)
    {
        typedef task<void, void> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#else // defined(MAD_USE_CXX98)
    //------------------------------------------------------------------------//
    // public run functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename... _Args>
    __inline__
    void run(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<_Ret, _Args...> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, args...);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename... _Args>
    __inline__
    void run(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<void, _Args...> task_type;

        tg = set_task_group(tg);
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, args...);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    __inline__
    void run(mad::task_group* tg, _Func function)
    {
        typedef task<_Ret> task_type;

        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func>
    __inline__
    void run(mad::task_group* tg, _Func function)
    {
        typedef task<void> task_type;

        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    // public run functions using default task group
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename... _Args>
    __inline__
    void run(_Func function, _Args... args)
    {
        typedef task<_Ret, _Args...> task_type;

        mad::task_group* tg = set_task_group();
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, args...);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename... _Args>
    __inline__
    void run(_Func function, _Args... args)
    {
        typedef task<void, _Args...> task_type;

        mad::task_group* tg = set_task_group();
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function, args...);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    __inline__
    void run(_Func function)
    {
        typedef task<_Ret> task_type;

        mad::task_group* tg = set_task_group();
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func>
    __inline__
    void run(_Func function)
    {
        typedef task<void> task_type;

        mad::task_group* tg = set_task_group();
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//
#endif // defined(MAD_USE_CXX98)

public:
    //------------------------------------------------------------------------//
    // public run_loop functions
    //------------------------------------------------------------------------//
    template <typename _Func, typename InputIterator>
    __inline__
    void run_loop(_Func function, InputIterator _s, InputIterator _e,
                  mad::task_group* tg = 0)
    {
        typedef task<void, InputIterator> task_type;

        tg = set_task_group(tg);
        for(InputIterator itr = _s; itr != _e; ++itr)
        {
            task_type* t = new task_type(tg, function, itr);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
    // Specialization for above when run_loop(func, 0, container->size())
    // is called. Generally, the "0" is defaulted to a signed type
    // so template deduction fails
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg1, typename _Arg>
    __inline__
    void run_loop(_Func function, const _Arg1& _s, const _Arg& _e,
                  mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg> task_type;

        tg = set_task_group(tg);
        for(size_type i = _s; i < _e; ++i)
        {
            task_type* t = new task_type(tg, function, i);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Ret, typename _Func, typename InputIterator>
    __inline__
    void run_loop(_Func function, InputIterator _s, InputIterator _e,
                  mad::task_group* tg = 0)
    {
        typedef task<_Ret, InputIterator> task_type;

        tg = set_task_group(tg);
        for(InputIterator itr = _s; itr != _e; ++itr)
        {
            task_type* t = new task_type(tg, function, itr);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
    // Specialization for above when run_loop(func, 0, container->size())
    // is called. Generally, the "0" is defaulted to a signed type
    // so template deduction fails
    //------------------------------------------------------------------------//
    template <typename _Func, typename _Arg1, typename _Arg>
    __inline__
    void run_loop(_Func function, const _Arg1& _s, const _Arg& _e,
                  mad::task_group* tg = 0)
    {
        typedef task<void, _Arg> task_type;

        tg = set_task_group(tg);
        for(size_type i = _s; i < _e; ++i)
        {
            task_type* t = new task_type(tg, function, i);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public run_loop (in chunks) functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg1, typename _Arg>
    __inline__
    void run_loop(_Func function,
                  const _Arg1& _s,
                  const _Arg& _e,
                  unsigned long chunks,
                  mad::task_group* tg = 0)
    {
        typedef task<_Ret, _Arg, _Arg> task_type;

        tg = set_task_group(tg);
        _Arg _grainsize = (chunks == 0) ? size() : chunks;
        _Arg _diff = (_e - _s)/_grainsize;
        size_type _n = _grainsize;
        for(size_type i = 0; i < _n; ++i)
        {
            _Arg _f = _diff*i; // first
            _Arg _l = _f + _diff; // last
            if(i+1 == _n)
                _l = _e;
            task_type* t = new task_type(tg, function, _f, _l);
            m_data->tp()->add_task(t);
        }
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Func, typename _Arg1, typename _Arg>
    __inline__
    void run_loop(_Func function, const _Arg1& _s, const _Arg& _e,
                  unsigned long chunks,
                  mad::task_group* tg = 0)
    {
        typedef task<void, _Arg, _Arg> task_type;

        tg = set_task_group(tg);
        _Arg _grainsize = (chunks == 0) ? size() : chunks;
        _Arg _diff = (_e - _s)/_grainsize;
        size_type _n = _grainsize;
        for(size_type i = 0; i < _n; ++i)
        {
            _Arg _f = _diff*i; // first
            _Arg _l = _f + _diff; // last
            if(i+1 == _n)
                _l = _e;
            task_type* t = new task_type(tg,
                                         function, _f, _l);
            m_data->tp()->add_task(t);
        }
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Ret,
              typename _Func,
              typename _Arg1, typename _Arg,
              typename _Tp,   typename _Join>
    __inline__
    void
    run_loop(_Func function, const _Arg1& _s, const _Arg& _e,
             unsigned long chunks, _Join _operator, _Tp identity,
             mad::task_group* tg = 0)
    {
#ifdef MAD_USE_CXX98
        typedef task_tree_node<_Ret, _Arg, _Arg, void>  task_tree_node_type;
#else
        typedef task_tree_node<_Ret, _Arg, _Arg>        task_tree_node_type;
#endif
        typedef task<_Ret, _Arg, _Arg>                  task_type;
        typedef task_tree<task_tree_node_type>          task_tree_type;

        tg = set_task_group(tg);
        _Arg _grainsize = (chunks == 0) ? size() : chunks;
        _Arg _diff = (_e - _s)/_grainsize;
        size_type _n = _grainsize;

        task_tree_type* tree = new task_tree_type;
        task_tree_node_type* tree_node = 0;
        std::deque<task_type*> _tasks;
        std::deque<task_tree_node_type*> _nodes;
        for(size_type i = 0; i < _n; ++i)
        {
            _Arg _f = _diff*i; // first
            _Arg _l = _f + _diff; // last
            if(i+1 == _n)
                _l = _e;

            _tasks.push_back(new task_type(tg, function, _f, _l));
            tree_node = new task_tree_node_type(tg, _operator,
                                                _tasks.back(),
                                                identity, tree->root());
            _nodes.push_back(tree_node);
            if(i == 0)
                tree->set_root(tree_node);

            tree->insert(tree->root(), tree_node);
        }
        m_data->tp()->add_tasks(tree->root());
        m_current_group->join();

        typedef typename std::deque<task_type*>::iterator            task_deque_itr;
        typedef typename std::deque<task_tree_node_type*>::iterator  node_deque_itr;
        typedef typename mad::task_group::iterator          tgrp_saved_itr;
        //for(node_deque_itr itr = _nodes.begin(); itr != _nodes.end(); ++itr)
        //    delete *itr;
        for(task_deque_itr titr = _tasks.begin(); titr != _tasks.end(); ++titr)
        {
            tgrp_saved_itr itr = (*titr)->group()->get_saved_tasks().begin();
            for(; itr != (*titr)->group()->get_saved_tasks().end(); ++itr)
                delete *itr;
            (*titr)->group()->get_saved_tasks().clear();
        }
        delete tree;
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public run in background functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg, typename _Func>
    __inline__
    void add_background_task(void* _id, _Func function, _Arg argument)
    {
        typedef task<_Ret, _Arg> task_type;

        task_type* t = new task_type(m_current_group,
                                     function, argument);
        m_data->tp()->add_background_task(_id, t);
    }
    //------------------------------------------------------------------------//
#ifndef SWIG
    template <typename _Arg, typename _Func>
    __inline__
    void add_background_task(void* _id, _Func function, _Arg argument)
    {
        typedef task<void, _Arg> task_type;

        task_type* t = new task_type(m_current_group,
                                     function, argument);
        m_data->tp()->add_background_task(_id, t);
    }
#endif
    //------------------------------------------------------------------------//
    template <typename _Func>
    __inline__
    void add_background_task(void* _id, _Func function)
    {
#ifdef MAD_USE_CXX98
        typedef task<void, void> task_type;
#else
        typedef task<void> task_type;
#endif

        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(m_current_group,
                                         function);
            m_data->tp()->add_background_task(_id, t);
        }
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // background utility functions
    //------------------------------------------------------------------------//
    // in a class, call thread_manager::Instance()->signal_background(this);
    // to wake up a thread to complete the background task
    // use case: a dedicated thread for generate random numbers
    //------------------------------------------------------------------------//
    __inline__
    void signal_background(void* _id)
    {
        m_data->tp()->signal_background(_id);
    }
    //------------------------------------------------------------------------//
    // same as above but set argument to "val"
    //------------------------------------------------------------------------//
    template <typename _Tp>
    __inline__
    void signal_background(void* _id, _Tp val)
    {
        m_data->tp()->signal_background<_Tp>(_id, val);
    }
    //------------------------------------------------------------------------//
    // check if background is done
    // example:
    //      while(!thread_manager::Instance()->is_done(this);
    //------------------------------------------------------------------------//
    __inline__
    volatile bool& is_done(void* _id)
    {
        return m_data->tp()->is_done(_id);
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public join functions
    //------------------------------------------------------------------------//
    // wait for threads to finish tasks given in run(...)
    //------------------------------------------------------------------------//
    __inline__
    void join()
    {
        m_current_group->join();
    }
    //------------------------------------------------------------------------//
    // for tasks that return values
    // there is probably a more flexible way to do this but it will do for now
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _List>
    __inline__
    _Ret join(_Ret _def,
              _Ret(*_operator)(_List&, _Ret))
    {
        typedef _List return_container;

        m_current_group->join();

        return_container ret_data;
        for(mad::task_group::iterator itr = m_current_group->begin();
            itr != m_current_group->end(); ++itr)
        {
            ret_data.insert(ret_data.end(),
                            *(static_cast<_Ret*>((*itr)->get())));
            delete *itr;
        }
        m_current_group->get_saved_tasks().clear();
        return _operator(ret_data, _def);
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    __inline__
    void join(_Func _operator)
    {
        m_current_group->join();

        for(mad::task_group::iterator itr = m_current_group->begin();
            itr != m_current_group->end(); ++itr)
        {
            _operator(*(static_cast<_Ret*>((*itr)->get())));
            delete *itr;
        }
        m_current_group->get_saved_tasks().clear();
    }

    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // kill all the threads. Usually wont be called directly
    //------------------------------------------------------------------------//
    void finalize()
    {
        m_data->tp()->destroy_threadpool();
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // looks for environment variable FORCE_NUM_THREADS
    static int GetEnvNumThreads(int _default = -1)
    {
        return mad::thread_pool::GetEnvNumThreads(_default);
    }
    //------------------------------------------------------------------------//
    static int get_env_num_threads(int _default = -1)
    {
        return mad::thread_pool::GetEnvNumThreads(_default);
    }
    //------------------------------------------------------------------------//

public:
    // Protected functions
    template <typename _Ret>
    static __inline__
    _Ret sum_function(std::vector<_Ret>& _data, _Ret _def = _Ret())
    {
        return std::accumulate(_data.begin(), _data.end(), _def);
    }


protected:
    // Protected variables
    static size_type        max_threads;
    static mad::task_group* m_default_group;
    data_type*              m_data;
    mad::task_group*        m_current_group;
    bool                    m_is_clone;
};

//============================================================================//

} // namespace mad

#define tmid    mad::thread_manager::sid      (CORETHREADSELFINT())
#define _tid_   mad::thread_manager::id       (CORETHREADSELFINT())
#define tmidstr mad::thread_manager::id_string(CORETHREADSELFINT())

#include <iostream>

#ifdef ENABLE_THREADING
#   define tmcout std::cout  << tmid
#   define tmcerr std::cerr  << tmid
#   define tmwout std::wcout << tmid
#   define tmwerr std::wcerr << tmid
#else
#   define tmcout std::cout
#   define tmcerr std::cerr
#   define tmwout std::wcout
#   define tmwerr std::wcerr
#endif

#ifdef SWIG
%template(id)           mad::thread_manager::id<unsigned long>;
%template(sid)          mad::thread_manager::sid<unsigned long>;
%template(id_string)    mad::thread_manager::id_string<unsigned long>;
#endif
//----------------------------------------------------------------------------//

#endif
