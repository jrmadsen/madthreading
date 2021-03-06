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

#include "madthreading/macros.hh"
#include "madthreading/threading/threading.hh"
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

private:
    thread_manager_data& operator=(const thread_manager_data& rhs)
    {
        if(this == &rhs)
            return *this;

        m_size = rhs.m_size;
        m_tp = rhs.m_tp;

        return *this;
    }

    thread_manager_data(const thread_manager_data& rhs)
    : m_size(rhs.m_size),
      m_tp(rhs.m_tp)
    { }

protected:
    size_type m_size;
    mad::thread_pool* m_tp;
};

} // namespace details

//============================================================================//

class thread_manager
{
public:
    typedef thread_manager                      this_type;
    typedef details::thread_manager_data        data_type;
    typedef data_type::size_type                size_type;
    typedef std::random_access_iterator_tag     iterator_category;
    typedef std::vector<vtask*>                 task_list_t;

public:
    // Constructor and Destructors
    thread_manager();
    thread_manager(size_type _n, bool _use_affinity = true);
    virtual ~thread_manager();

private:
    // disable external copying and assignment
    thread_manager& operator=(const thread_manager&);
    thread_manager(const thread_manager&);

public:
    /// get the singleton pointer
    static thread_manager* Instance();
    /// get the singleton pointer
    static thread_manager* instance();
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
        static mad::mutex _mtx;
        auto_lock l(_mtx);
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
            return "";
        thread_manager* _tm = thread_manager::Instance();
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
    static thread_manager* fgInstance;
    static void check_instance(thread_manager*);

public:
    // Public functions
    void use_affinity(bool _val) { m_data->tp()->use_affinity(_val); }

    void SetMaxThreads(const size_type& _n)   { max_threads = _n; }
    void set_max_threads(const size_type& _n) { max_threads = _n; }
    size_type GetMaxThreads()   const         { return max_threads; }
    size_type get_max_threads() const         { return max_threads; }
    mad::thread_pool* thread_pool() const     { return m_data->tp(); }

public:
    //------------------------------------------------------------------------//
    /// return the number of threads in the thread pool
    size_type size() const { return m_data->size(); }
    //------------------------------------------------------------------------//
    /// allocate the number of threads
    void allocate_threads(size_type _n) { m_data->allocate_threads(_n); }
    //------------------------------------------------------------------------//

private:
    #define is_same_t(_Tp, _Up) std::is_same<_Tp, _Up>::value
    #define is_integral_t(_Tp) std::is_integral<_Tp>::value
    #define is_iterator_t(_Tp) std::is_same<typename std::iterator_traits<_Tp>::value_type, int>::value
    #define enable_if_t(_Bp, _Tp) typename std::enable_if<_Bp, _Tp>::type
    //template <bool _Bp, typename _Tp = void>
    //using enable_if_t = typename std::enable_if<_Bp, _Tp>::type;

    template <typename _Tp, enable_if_t(is_iterator_t(_Tp), int) = 0>
    size_type distance(_Tp lhs, _Tp rhs) const
    {
        return std::distance(lhs, rhs);
    }

    template <typename _Tp, enable_if_t(is_integral_t(_Tp), int) = 0>
    size_type distance(_Tp lhs, _Tp rhs) const
    {
        return rhs - lhs;
    }

public:
    //------------------------------------------------------------------------//
    // public exec functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename... _Args>
    _inline_
    void exec(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<_Ret, _Args...> task_type;
        m_data->tp()->add_task(new task_type(tg, function, args...));
    }
    //------------------------------------------------------------------------//
    template <typename _Func, typename... _Args>
    _inline_
    void exec(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<void, _Args...> task_type;
        m_data->tp()->add_task(new task_type(tg, function, args...));
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    _inline_
    void exec(mad::task_group* tg, _Func function)
    {
        typedef task<_Ret> task_type;
        m_data->tp()->add_task(new task_type(tg, function));
    }
    //------------------------------------------------------------------------//
    template <typename _Func>
    _inline_
    void exec(mad::task_group* tg, _Func function)
    {
        typedef task<void> task_type;
        m_data->tp()->add_task(new task_type(tg, function));
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public run functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename... _Args>
    _inline_
    void run(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<_Ret, _Args...> task_type;
        task_list_t _tasks(size(), nullptr);
        for(size_type i = 0; i < size(); ++i)
            _tasks[i] = new task_type(tg, function, args...);
        m_data->tp()->add_tasks(_tasks);
    }
    //------------------------------------------------------------------------//
    template <typename _Func, typename... _Args>
    _inline_
    void run(mad::task_group* tg, _Func function, _Args... args)
    {
        typedef task<void, _Args...> task_type;
        task_list_t _tasks(size(), nullptr);
        for(size_type i = 0; i < size(); ++i)
            _tasks[i] = new task_type(tg, function, args...);
        m_data->tp()->add_tasks(_tasks);
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    _inline_
    void run(mad::task_group* tg, _Func function)
    {
        typedef task<_Ret> task_type;
        task_list_t _tasks(size(), nullptr);
        for(size_type i = 0; i < size(); ++i)
            _tasks[i] = new task_type(tg, function);
        m_data->tp()->add_tasks(_tasks);
    }
    //------------------------------------------------------------------------//
    template <typename _Func>
    _inline_
    void run(mad::task_group* tg, _Func function)
    {
        typedef task<void> task_type;

        task_list_t _tasks(size(), nullptr);
        for(size_type i = 0; i < size(); ++i)
            _tasks[i] = new task_type(tg, function);
        m_data->tp()->add_tasks(_tasks);
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public run_loop functions
    //------------------------------------------------------------------------//
    template <typename _Func, typename InputIterator>
    _inline_
    void run_loop(mad::task_group* tg,
                  _Func function, InputIterator _s, InputIterator _e)
    {
        typedef task<void, InputIterator> task_type;
        for(InputIterator itr = _s; itr != _e; ++itr)
            m_data->tp()->add_task(new task_type(tg, function, itr));
    }
    //------------------------------------------------------------------------//
    // Specialization for above when run_loop(func, 0, container->size())
    // is called. Generally, the "0" is defaulted to a signed type
    // so template deduction fails
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg1, typename _Arg>
    _inline_
    void run_loop(mad::task_group* tg,
                  _Func function, const _Arg1& _s, const _Arg& _e)
    {
        typedef task<_Ret, _Arg> task_type;
        for(size_type i = _s; i < _e; ++i)
            m_data->tp()->add_task(new task_type(tg, function, i));
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename InputIterator>
    _inline_
    void run_loop(mad::task_group* tg,
                  _Func function, InputIterator _s, InputIterator _e)
    {
        typedef task<_Ret, InputIterator> task_type;
        for(InputIterator itr = _s; itr != _e; ++itr)
            m_data->tp()->add_task(new task_type(tg, function, itr));
    }
    //------------------------------------------------------------------------//
    // Specialization for above when run_loop(func, 0, container->size())
    // is called. Generally, the "0" is defaulted to a signed type
    // so template deduction fails
    //------------------------------------------------------------------------//
    template <typename _Func, typename _Arg1, typename _Arg>
    _inline_
    void run_loop(mad::task_group* tg,
                  _Func function, const _Arg1& _s, const _Arg& _e)
    {
        typedef task<void, _Arg> task_type;
        for(size_type i = _s; i < _e; ++i)
            m_data->tp()->add_task(new task_type(tg, function, i));
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public run_loop (in chunks) functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func, typename _Arg1, typename _Arg>
    _inline_
    void run_loop(mad::task_group* tg,
                  _Func function,
                  const _Arg1& _s,
                  const _Arg& _e,
                  unsigned long chunks)
    {
        typedef task<_Ret, _Arg, _Arg> task_type;

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
    template <typename _Func, typename _Arg1, typename _Arg>
    _inline_
    void run_loop(mad::task_group* tg,
                  _Func function, const _Arg1& _s, const _Arg& _e,
                  unsigned long chunks)
    {
        typedef task<void, _Arg, _Arg> task_type;

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
    //------------------------------------------------------------------------//
    template <typename _Ret,
              typename _Func,
              typename _Arg1, typename _Arg,
              typename _Tp,   typename _Join>
    _inline_
    void
    run_loop(mad::task_group* tg,
             _Func function, const _Arg1& _s, const _Arg& _e,
             unsigned long chunks, _Join _operator, _Tp identity)
    {
        typedef task_tree_node<_Ret, _Arg, _Arg>        task_tree_node_type;
        typedef task<_Ret, _Arg, _Arg>                  task_type;
        typedef task_tree<task_tree_node_type>          task_tree_type;

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
        tg->join();
        delete tree;
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public run in background functions
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Arg, typename _Func>
    _inline_
    void add_background_task(mad::task_group* tg,
                             void* _id, _Func function, _Arg argument)
    {
        typedef task<_Ret, _Arg> task_type;
        task_type* t = new task_type(tg, function, argument);
        m_data->tp()->add_background_task(_id, t);
    }
    //------------------------------------------------------------------------//
    template <typename _Arg, typename _Func>
    _inline_
    void add_background_task(mad::task_group* tg,
                             void* _id, _Func function, _Arg argument)
    {
        typedef task<void, _Arg> task_type;
        task_type* t = new task_type(tg, function, argument);
        m_data->tp()->add_background_task(_id, t);
    }
    //------------------------------------------------------------------------//
    template <typename _Func>
    _inline_
    void add_background_task(mad::task_group* tg,
                             void* _id, _Func function)
    {
        typedef task<void> task_type;
        for(size_type i = 0; i < size(); ++i)
        {
            task_type* t = new task_type(tg, function);
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
    _inline_
    void signal_background(void* _id)
    {
        m_data->tp()->signal_background(_id);
    }
    //------------------------------------------------------------------------//
    // same as above but set argument to "val"
    //------------------------------------------------------------------------//
    template <typename _Tp>
    _inline_
    void signal_background(void* _id, _Tp val)
    {
        m_data->tp()->signal_background<_Tp>(_id, val);
    }
    //------------------------------------------------------------------------//
    // check if background is done
    // example:
    //      while(!thread_manager::Instance()->is_done(this);
    //------------------------------------------------------------------------//
    _inline_
    volatile bool& is_done(void* _id)
    {
        return m_data->tp()->is_done(_id);
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    // public join functions
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    // for tasks that return values
    // there is probably a more flexible way to do this but it will do for now
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _List>
    _inline_
    _Ret join(mad::task_group* tg,
              _Ret _def,
              _Ret(*_operator)(_List&, _Ret))
    {
        typedef _List return_container;

        tg->join();
        return_container ret_data;
        for(auto itr = tg->begin(); itr != tg->end(); ++itr)
            ret_data.insert(ret_data.end(),
                            *(static_cast<_Ret*>((*itr)->get())));
        return _operator(ret_data, _def);
    }
    //------------------------------------------------------------------------//
    template <typename _Ret, typename _Func>
    _inline_
    void join(mad::task_group* tg,
              _Func _operator)
    {
        tg->join();
        for(auto itr = tg->begin(); itr != tg->end(); ++itr)
            _operator(*(static_cast<_Ret*>((*itr)->get())));
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
    static _inline_
    _Ret sum_function(std::vector<_Ret>& _data, _Ret _def = _Ret())
    {
        return std::accumulate(_data.begin(), _data.end(), _def);
    }

protected:
    // Protected variables
    static size_type                    max_threads;
    data_type*                          m_data;
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
