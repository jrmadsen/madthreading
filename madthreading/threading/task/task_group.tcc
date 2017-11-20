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

namespace mad
{

using namespace std::placeholders;

//============================================================================//

template <typename _Tp, typename _Arg>
template <typename _Func>
task_group<_Tp, _Arg>::task_group(_Func _join, thread_pool* tp)
: details::vtask_group(tp)
{
    m_join_function = std::bind<_Tp>(_join, _1, _2);
}

//============================================================================//

template <typename _Tp, typename _Arg>
task_group<_Tp, _Arg>::~task_group()
{ }

//============================================================================//

template <typename _Tp, typename _Arg>
inline typename task_group<_Tp, _Arg>::this_type&
task_group<_Tp, _Arg>::add(future_type&& _f)
{
    m_task_list.push_back(data_type(false, std::move(_f), _Tp()));
    return *this;
}

//============================================================================//

template <typename _Tp, typename _Arg>
inline _Tp task_group<_Tp, _Arg>::join(_Tp accum)
{
    this->wait();
    for(auto& itr : *this)
        accum = m_join_function(accum, this->get(itr));
    return accum;
}

//============================================================================//

template <typename _Tp, typename _Arg>
_Arg task_group<_Tp, _Arg>::get(data_type& _data)
{
    if(!std::get<0>(_data))
    {
        std::get<2>(_data) = std::get<1>(_data).get();
        std::get<0>(_data) = true;
    }
    return std::get<2>(_data);
}

//============================================================================//

} // namespace mad
