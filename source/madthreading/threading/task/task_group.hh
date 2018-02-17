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

template <typename _Tp, typename _Arg = _Tp>
class task_group : public details::vtask_group
{
public:
    template <typename _Type>
    using list_type = std::deque<_Type>;

    typedef typename std::remove_const<
    typename std::remove_reference<_Arg>::type>::type _ArgT;
    typedef _Tp                                     result_type;
    typedef task_group<_Tp, _Arg>                  this_type;
    typedef std::promise<_ArgT>                     promise_type;
    typedef std::future<_ArgT>                      future_type;
    typedef std::packaged_task<_ArgT()>             packaged_task_type;
    typedef std::tuple<bool, future_type, _ArgT>    data_type;
    typedef list_type<data_type>                    task_list_t;
    typedef typename task_list_t::iterator          iterator;
    typedef typename task_list_t::const_iterator    const_iterator;
    typedef std::function<_Tp(_Tp&, _Arg)>          function_type;

public:
    // Constructor
    template <typename _Func>
    task_group(_Func _join, thread_pool* tp = nullptr);
    // Destructor
    virtual ~task_group();

    // delete copy-construct
    task_group(const this_type&) = delete;
    // define move-construct
    task_group(this_type&& rhs)
    : m_task_list(std::move(rhs.m_task_list)),
      m_promise(std::move(rhs.m_promise)),
      m_join_function(std::move(rhs.m_join_function))
    { }

    // delete copy-assign
    this_type& operator=(const this_type& rhs) = delete;
    // define move-assign
    this_type& operator=(this_type&& rhs)
    {
        if(this != &rhs)
        {
            m_task_list = std::move(rhs.m_task_list);
            m_promise = std::move(rhs.m_promise);
            m_join_function = std::move(rhs.m_join_function);
        }
        return *this;
    }

public:
    // set the join function
    template <typename _Func> void set_join_function(_Func);
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
    void add(future_type&& _f);
    //------------------------------------------------------------------------//
    // wait to finish
    _Tp join(_Tp accum = _Tp());
    //------------------------------------------------------------------------//
    // clear the task result history
    void clear() { m_task_list.clear(); vtask_group::clear(); }

protected:
    //------------------------------------------------------------------------//
    // get a specific task
    _Arg get(data_type& _data);

protected:
    // Protected variables
    task_list_t         m_task_list;
    promise_type        m_promise;
    function_type       m_join_function;

};

//----------------------------------------------------------------------------//
// specialization for void type
template <>
class task_group<void, void> : public details::vtask_group
{
public:
    template <typename _Type>
    using list_type = std::deque<_Type>;

    typedef void                                    result_type;
    typedef task_group<void, void>                 this_type;
    typedef std::promise<void>                      promise_type;
    typedef std::future<void>                       future_type;
    typedef std::packaged_task<void()>              packaged_task_type;
    typedef std::tuple<bool, future_type>           data_type;
    typedef list_type<data_type>                    task_list_t;
    typedef typename task_list_t::iterator          iterator;
    typedef typename task_list_t::const_iterator    const_iterator;

public:
    // Constructor
    task_group(thread_pool* tp = nullptr) : vtask_group(tp) { }
    // Destructor
    virtual ~task_group() { }

    // delete copy-construct
    task_group(const this_type&) = delete;
    // define move-construct
    task_group(this_type&& rhs)
    : m_task_list(std::move(rhs.m_task_list))
    { }

    // delete copy-assign
    this_type& operator=(const this_type& rhs) = delete;
    // define move-assign
    this_type& operator=(this_type&& rhs)
    {
        if(this != &rhs)
            m_task_list = std::move(rhs.m_task_list);
        return *this;
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
    this_type& add(future_type&& _f)
    {
        m_task_list.push_back(data_type(false, std::move(_f)));
        return *this;
    }
    //------------------------------------------------------------------------//
    // wait to finish
    void join()
    {
        this->wait();
        for(auto& itr : *this)
            this->get(itr);

        if(m_clear_freq.load() > 0 &&
           (++m_clear_count) % m_clear_freq.load() == 0)
            this->clear();
    }
    //------------------------------------------------------------------------//
    // clear the task result history
    void clear() { m_task_list.clear(); vtask_group::clear(); }

protected:
    //------------------------------------------------------------------------//
    // get specific task
    void get(data_type& _data)
    {
        if(!std::get<0>(_data))
        {
            std::get<1>(_data).get();
            std::get<0>(_data) = true;
        }
    }

protected:
    // Private variables
    task_list_t         m_task_list;
};

//----------------------------------------------------------------------------//

} // namespace mad

#include "madthreading/threading/task/task_group.tcc"

#endif
