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

#include <algorithm>
#include <iomanip>
#include <cmath>
#include <cassert>

#include "madthreading/macros.hh"
#include "madthreading/threading/threading.hh"
#include "madthreading/threading/task/task.hh"
#include "madthreading/threading/task/task_group.hh"
#include "madthreading/threading/thread_pool.hh"

#if defined(USE_OPENMP)
#   include <omp.h>
#endif

namespace mad
{

//============================================================================//

class thread_manager
{
public:
    typedef thread_manager                      this_type;
    typedef thread_pool                         pool_type;
    typedef pool_type::size_type                size_type;
    typedef std::vector<details::vtask*>        task_list_t;

public:
    // Constructor and Destructors
    thread_manager(thread_pool*&);
    virtual ~thread_manager();

private:
    // disable external copying and assignment
    thread_manager& operator=(const thread_manager&);
    thread_manager(const thread_manager&);

    static unsigned ncores() { return std::thread::hardware_concurrency(); }

public:
    /// get the singleton pointer
    static thread_manager* instance();

    /// function for returning the thread id
    template <typename _Tp>
    static long id(_Tp thread_self)
    {
        long _id = -1;
        static ::mad::mutex _mtx;
        auto_lock l(_mtx);
        thread_manager* _tm = thread_manager::instance();
        if(!_tm)
            return _id;
        if(_tm->m_pool->GetThreadIDs().find(thread_self) !=
           _tm->m_pool->GetThreadIDs().end())
            _id = _tm->m_pool->GetThreadIDs().find(thread_self)->second;
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
            return "";
        thread_manager* _tm = thread_manager::instance();
        if(!_tm)
            return "";
        short _w = 4;
        _w = std::min(_w, (short) std::ceil(std::log10(_tm->size())));
        std::stringstream ss;
        ss.fill('0');
        ss << std::setw(_w) << _id;
        return ss.str();
    }

private:
    static thread_manager* f_instance;
    static void check_instance(thread_manager*);

public:
    thread_pool* get_thread_pool() { return m_pool; }

public:
    //------------------------------------------------------------------------//
    /// return the number of threads in the thread pool
    size_type size() const { return m_pool->size(); }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // direct insertion of a task
    //------------------------------------------------------------------------//
    template <typename... _Args>
    void exec(task<_Args...>* _task)
    {
        typedef task<_Args...>            task_type;
        typedef std::shared_ptr<task_type>  task_pointer;

        m_pool->add_task(task_pointer(_task));
    }

    //------------------------------------------------------------------------//
    // direct insertion of a packaged_task
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename... _Args>
    std::future<_Ret> async(_Func func, _Args... args)
    {
        typedef packaged_task<_Ret, _Ret, _Args...>     task_type;
        typedef std::shared_ptr<task_type>              task_pointer;

        task_pointer _ptask(new task_type(func, std::forward<_Args>(args)...));
        m_pool->add_task(_ptask);
        return _ptask->get_future();
    }

public:
    //------------------------------------------------------------------------//
    // public exec functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg, typename _Func, typename... _Args>
    void exec(task_group<_Ret, _Arg>& tg, _Func func, _Args... args)
    {
        typedef task<_Ret, _Arg, _Args...>    task_type;
        typedef std::shared_ptr<task_type>      task_pointer;

        m_pool->add_task(tg.store(task_pointer(new task_type(tg, func,
                                            std::forward<_Args>(args)...))));
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg, typename _Func>
    void exec(task_group<_Ret, _Arg>& tg, _Func func)
    {
        typedef task<_Ret, _Arg>                task_type;
        typedef std::shared_ptr<task_type>  task_pointer;

        m_pool->add_task(tg.store(task_pointer(new task_type(tg, func))));
    }
    //------------------------------------------------------------------------//
    // public thread-specific exec functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg, typename _Func, typename... _Args>
    void exec(task_group<_Ret, _Arg>& tg, thread::id tid, _Func func, _Args... args)
    {
        typedef task<_Ret, _Arg, _Args...>    task_type;
        typedef std::shared_ptr<task_type>      task_pointer;

        m_pool->add_thread_task(tid,
                                tg.store(task_pointer(new task_type(tg, func,
                                            std::forward<_Args>(args)...))));
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg, typename _Func>
    void exec(task_group<_Ret, _Arg>& tg, thread::id tid, _Func func)
    {
        typedef task<_Ret, _Arg>              task_type;
        typedef std::shared_ptr<task_type>      task_pointer;

        m_pool->add_thread_task(tid,
                            tg.store(task_pointer(new task_type(tg, func))));
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public run_loop functions
    //------------------------------------------------------------------------//
    // when run_loop(func, 0, container->size())
    // is called. Generally, the "0" is defaulted to a signed type
    // so template deduction fails
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg,
              typename _Func, typename _Arg1, typename _Arg2>
    void run_loop(task_group<_Ret, _Arg>& tg,
                  _Func func, const _Arg1& _s, const _Arg2& _e)
    {
        typedef task<_Ret, _Arg> task_type;
        typedef std::shared_ptr<task_type> task_pointer;
        for(size_type i = _s; i < _e; ++i)
            m_pool->add_task(tg.store(task_pointer(new task_type(tg,
                                                                func, i))));
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg,
              typename _Func, typename InputIterator>
    void run_loop(task_group<_Ret, _Arg>& tg,
                  _Func func, InputIterator _s, InputIterator _e)
    {
        typedef task<_Ret, _Arg, InputIterator> task_type;
        typedef std::shared_ptr<task_type> task_pointer;
        for(InputIterator itr = _s; itr != _e; ++itr)
            m_pool->add_task(tg.store(task_pointer(new task_type(tg,
                                                                  func, itr))));
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg,
              typename _Func, typename _Arg1, typename _Arg2>
    void run_loop(task_group<_Ret, _Arg>& tg,
                  _Func func,
                  const _Arg1& _s,
                  const _Arg2& _e,
                  uint64_t chunks)
    {
        typedef task<_Ret, _Arg, _Arg2, _Arg2> task_type;
        typedef std::shared_ptr<task_type> task_pointer;

        _Arg2 _grainsize = (chunks == 0) ? size() : chunks;
        _Arg2 _diff = (_e - _s)/_grainsize;
        size_type _n = _grainsize;
        for(size_type i = 0; i < _n; ++i)
        {
            _Arg2 _f = _diff*i; // first
            _Arg2 _l = _f + _diff; // last
            if(i+1 == _n)
                _l = _e;
            m_pool->add_task(tg.store(task_pointer(new task_type(tg,
                                                            func, _f, _l))));
        }
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // kill all the threads. Usually wont be called directly
    //------------------------------------------------------------------------//
    void finalize()
    {
        m_pool->destroy_threadpool();
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // looks for environment variable FORCE_NUM_THREADS
    static int64_t GetEnvNumThreads(int64_t _default = -1)
    {
        return thread_pool::GetEnvNumThreads(_default);
    }
    //------------------------------------------------------------------------//
    static int64_t get_env_num_threads(int64_t _default = -1)
    {
        return thread_pool::GetEnvNumThreads(_default);
    }
    //------------------------------------------------------------------------//
    /// get the singleton pointer if exists and allocate nthreads in the
    /// thread pool.
    /// if an instance has not been created, create one with thread pool of
    /// nthreads
    static thread_manager* get_thread_manager(const int64_t& nthread
                                              = get_env_num_threads(),
                                              const int& verbose = 0);

protected:
    // Protected variables
    thread_pool*& m_pool;

};

//============================================================================//

} // namespace mad

#define tmid    mad::thread_manager::sid      (std::this_thread::get_id())
#define _tid_   mad::thread_manager::id       (std::this_thread::get_id())
#define tmidstr mad::thread_manager::id_string(std::this_thread::get_id())

#include <iostream>

#define tmcout std::cout  << tmid
#define tmcerr std::cerr  << tmid
#define tmwout std::wcout << tmid
#define tmwerr std::wcerr << tmid

//----------------------------------------------------------------------------//

#endif
