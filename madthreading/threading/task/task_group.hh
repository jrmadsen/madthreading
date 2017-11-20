//
//
//
//
//
// created by jmadsen on Sat Jul 15 18:43:49 2017
//
//
//
//

#ifndef task_group_hh_
#define task_group_hh_

#include "madthreading/threading/task/vtask_group.hh"

#include <future>

//----------------------------------------------------------------------------//

namespace mad
{

class thread_pool;

//----------------------------------------------------------------------------//

template <typename _Tp,typename _Arg = _Tp>
class task_group : public mad::details::vtask_group
{
public:
    template <typename _Key, typename _Type>
    using map_type = std::unordered_map<_Key, _Type>;

    typedef _Tp                                     result_type;
    typedef task_group<_Tp>                         this_type;
    typedef std::promise<_Tp>                       promise_type;
    typedef std::future<_Tp>                        future_type;
    typedef std::tuple<bool, future_type, _Tp*>     data_type;
    typedef map_type<task_type*, data_type>         task_list_t;
    typedef typename task_list_t::iterator          iterator;
    typedef typename task_list_t::const_iterator    const_iterator;
    typedef std::deque<future_type>                 future_list_t;
    typedef std::function<_Tp(_Tp&, _Arg)>          function_type;

public:
    // Constructor and Destructors
    template <typename _Func>
    task_group(_Func _join, thread_pool* tp = nullptr);
    ~task_group();

public:
    // Get tasks with non-void return types
    task_list_t& get_tasks() { return m_task_list; }
    const task_list_t& get_tasks() const { return m_task_list; }

    // iterate over tasks with return type
    iterator begin()                { return m_task_list.begin(); }
    iterator end()                  { return m_task_list.end(); }
    const_iterator begin() const    { return m_task_list.begin(); }
    const_iterator end()   const    { return m_task_list.end(); }
    const_iterator cbegin() const   { return m_task_list.begin(); }
    const_iterator cend()   const   { return m_task_list.end(); }

    //------------------------------------------------------------------------//
    // add task
    this_type& add(task_type* _t, future_type&& _f);
    //------------------------------------------------------------------------//
    // wait to finish
    _Tp join(_Tp accum = _Tp());

protected:
    //------------------------------------------------------------------------//
    // used by vtask_group
    virtual void wait_internal() final;
    //------------------------------------------------------------------------//
    // get a specific task
    _Arg get(details::vtask* ptr);

protected:
    // Protected variables
    task_list_t         m_task_list;
    promise_type        m_promise;
    function_type       m_join_function;

};

//----------------------------------------------------------------------------//
// specialization for void type
template <>
class task_group<void, void> : public mad::details::vtask_group
{
public:
    template <typename _Key, typename _Type>
    using map_type = std::unordered_map<_Key, _Type>;

    typedef void                                        result_type;
    typedef task_group<void>                            this_type;
    typedef std::promise<void>                          promise_type;
    typedef std::future<void>                           future_type;
    typedef std::tuple<bool, future_type>               data_type;
    typedef map_type<task_type*, data_type>             task_list_t;
    typedef typename task_list_t::iterator              iterator;
    typedef typename task_list_t::const_iterator        const_iterator;
    typedef std::deque<future_type>                     future_list_t;

public:
    // Constructor and Destructors
    task_group(thread_pool* tp = nullptr) : details::vtask_group(tp) { }
    ~task_group()
    {
        for(auto& itr : m_task_list)
            delete itr.first;
    }

public:
    // Get tasks with non-void return types
    task_list_t& get_tasks() { return m_task_list; }
    const task_list_t& get_tasks() const { return m_task_list; }

    // iterate over tasks with return type
    iterator begin()                { return m_task_list.begin(); }
    iterator end()                  { return m_task_list.end(); }
    const_iterator begin() const    { return m_task_list.begin(); }
    const_iterator end()   const    { return m_task_list.end(); }
    const_iterator cbegin() const   { return m_task_list.begin(); }
    const_iterator cend()   const   { return m_task_list.end(); }

    //------------------------------------------------------------------------//
    // add task
    this_type& add(task_type* _t, future_type&& _f)
    {
        m_task_list[_t] = data_type(false, std::move(_f));
        return *this;
    }
    //------------------------------------------------------------------------//
    // wait to finish
    void join() { this->wait(); }

protected:
    //------------------------------------------------------------------------//
    // used by vtask_group
    virtual void wait_internal() final
    {
        for(auto& itr : m_task_list)
            itr.first->wait();
    }
    //------------------------------------------------------------------------//
    // get specific task
    void get(details::vtask* ptr)
    {
        data_type& _data = m_task_list[ptr];
        if(!std::get<0>(_data))
        {
            ptr->wait();
            std::get<1>(_data).get();
            std::get<0>(_data) = true;
        }
    }

protected:
    // Private variables
    task_list_t         m_task_list;
    promise_type        m_promise;

};

//----------------------------------------------------------------------------//

} // namespace mad

#include "madthreading/threading/task/task_group.tcc"

#endif
