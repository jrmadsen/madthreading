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


#include "madthreading/threading/task/vtask.hh"
#include "madthreading/utility/fpe_detection.hh"
#include <functional>


//============================================================================//

template <typename _Tp, typename _Arg>
template <typename _Func>
mad::task_group<_Tp, _Arg>::task_group(_Func _join, mad::thread_pool* tp)
: mad::details::vtask_group(tp)
{
    this->set_join_function<_Func>(_join);
}

//============================================================================//

template <typename _Tp, typename _Arg>
mad::task_group<_Tp, _Arg>::~task_group()
{ }

//============================================================================//

template <typename _Tp, typename _Arg>
template <typename _Func>
void mad::task_group<_Tp, _Arg>::set_join_function(_Func _join)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    m_join_function = std::bind<_Tp>(_join, _1, _2);
}

//============================================================================//

template <typename _Tp, typename _Arg>
inline void
mad::task_group<_Tp, _Arg>::add(future_type&& _f)
{
    m_task_list.push_back(data_type(false, std::move(_f), _Tp()));
}

//============================================================================//

template <typename _Tp, typename _Arg>
inline _Tp mad::task_group<_Tp, _Arg>::join(_Tp accum)
{
    this->wait();
    for(auto& itr : *this)
    {
        _Arg _val = this->get(itr);
        accum = m_join_function(accum, _val);
    }

    if(m_clear_freq.load() > 0 && (++m_clear_count) % m_clear_freq.load() == 0)
        this->clear();

    return accum;
}

//============================================================================//

template <typename _Tp, typename _Arg>
_Arg mad::task_group<_Tp, _Arg>::get(data_type& _data)
{
    if(!std::get<0>(_data))
    {
        std::get<2>(_data) = std::get<1>(_data).get();
        std::get<0>(_data) = true;
    }
    return std::get<2>(_data);
}

//============================================================================//

