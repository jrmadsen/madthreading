//
//
//
//
//
// created by jrmadsen on Wed Jul  2 11:40:42 2014
//
//
//
//
// Template implementation of some protected/private
// functions utilized by atomic_deque
//


#ifndef atomic_deque_tcc_
#define atomic_deque_tcc_

#include <iterator>

#ifdef CXX11
    #include <memory>
#elif defined(USE_BOOST)
    #include <boost/utility.hpp>
#elif defined(CXX0X)
    #include <memory>
    #include <tr1/memory>
#endif


namespace mad
{

//============================================================================//

// Called only if _impl._finish._cur == _impl._finish._last - 1.
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || defined(CXX11)
  template <typename _Tp, typename _Alloc>
  template<typename... _Args>
  void
  atomic_deque<_Tp, _Alloc>::_push_back_aux(_Args&&... __args)
#else
  template <typename _Tp, typename _Alloc>
  void
  atomic_deque<_Tp, _Alloc>::_push_back_aux(const basic_type& __t)
#endif
  {
    //std::cout << "push_back_aux 0 " << std::endl;
    _reserve_map_at_back();
    *(this->_impl._finish._node + 1) = this->_allocate_node();
    try
    {
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || defined(CXX11)
      this->_impl.construct(this->_impl._finish._curr,
                  std::forward<_Args>(__args)...);
#else
      this->_impl.construct(this->_impl._finish._curr, __t);
#endif
      this->_impl._finish._set_node(this->_impl._finish._node + 1);
      this->_impl._finish._curr = this->_impl._finish._first;
    } catch(...) {
        _deallocate_node(*(this->_impl._finish._node + 1));
        //__throw_exception_again;
        throw std::bad_alloc();
    }
  }

//============================================================================//

// Called only if _impl._start._cur == _impl._start._first.
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || defined(CXX11)
  template <typename _Tp, typename _Alloc>
  template<typename... _Args>
  void
  atomic_deque<_Tp,_Alloc>::_push_front_aux(_Args&&... __args)
#else
  template <typename _Tp, typename _Alloc>
  void
  atomic_deque<_Tp, _Alloc>::_push_front_aux(const basic_type& __t)
#endif
  {
    _reserve_map_at_front();
    *(this->_impl._start._node - 1) = this->_allocate_node();
    try
    {
      this->_impl._start._set_node(this->_impl._start._node - 1);
        this->_impl._start._curr = this->_impl._start._last - 1;
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || defined(CXX11)
        this->_impl.construct(this->_impl._start._curr,
              std::forward<_Args>(__args)...);
#else
        this->_impl.construct(this->_impl._start._curr, __t);
#endif
    } catch(...) {
        ++this->_impl._start;
        _deallocate_node(*(this->_impl._start._node - 1));
        //__throw_exception_again;
        throw std::bad_alloc();
      }
  }

//============================================================================//

// Called only if _impl._finish._cur == _impl._finish._first.
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_pop_back_aux()
{
  _deallocate_node(this->_impl._finish._first);
  this->_impl._finish._set_node(this->_impl._finish._node - 1);
  this->_impl._finish._curr = this->_impl._finish._last - 1;
  this->_impl.destroy(this->_impl._finish._curr);
}

//============================================================================//

// Called only if _impl._start._cur == _impl._start._last - 1.
// Note that if the deque has at least one element (a precondition for this
// member function), and if
//   _impl._start._cur == _impl._start._last,
// then the deque must have at least two nodes.
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_pop_front_aux()
{
  this->_impl.destroy(this->_impl._start._curr);
  _deallocate_node(this->_impl._start._first);
  this->_impl._start._set_node(this->_impl._start._node + 1);
  this->_impl._start._curr = this->_impl._start._first;
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_fill_assign(size_type _n, const basic_type& _val)
{
  size_type _stop = (_n > size()) ? size() : _n;
  for(size_type i = 0; i < _stop; ++i)
  {
    atomics::set(&(*this)[i], _val);
  }
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_erase_at_begin(iterator __pos)
{
  _destroy_data(begin(), __pos/*,_get_Tp_allocator()*/);
  _destroy_nodes(_impl._start._node, __pos._node);
  _impl._start = __pos;
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_erase_at_end(iterator __pos)
{
  _destroy_data(__pos, end()/*, _get_Tp_allocator()*/);
  _destroy_nodes(__pos._node + 1, _impl._finish._node + 1);
  _impl._finish = __pos;
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_default_append(size_type __n)
{
    if (__n)
  {
      iterator __new_finish = _reserve_elements_at_back(__n);
      try {
        //std::uninitialized_fill(this->_impl._finish, __new_finish, _Tp());
        for(iterator ite = this->_impl._finish; ite != __new_finish; ++ite)
        {
          *ite = _Tp();
        }
        this->_impl._finish = __new_finish;
      } catch(...) {
            _destroy_nodes(this->_impl._finish._node + 1,
                       __new_finish._node + 1);
            //__throw_exception_again;
            throw std::bad_alloc();
      }
  }
}
//============================================================================//
//@{
/**
 *  @brief Memory-handling helpers for the major %map.
 *
 *  Makes sure the _map has space for new nodes.  Does not
 *  actually add the nodes.  Can invalidate _map pointers.
 *  (And consequently, %deque iterators.)
 */
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_reserve_map_at_back(size_type __nodes_to_add)
{
  if (__nodes_to_add + 1
      > this->_impl._map_size - (this->_impl._finish._node
                                       - this->_impl._map))
    _reallocate_map(__nodes_to_add, false);
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_reserve_map_at_front(size_type __nodes_to_add)
{
  if (__nodes_to_add > size_type(this->_impl._start._node - this->_impl._map))
    _reallocate_map(__nodes_to_add, true);
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_new_elements_at_front(size_type __new_elems)
{
  if (this->max_size() - this->size() < __new_elems)
    //__throw_length_error(__N("deque::_new_elements_at_front"));
    throw std::length_error("atomic_deque::_new_elements_at_front");

    const size_type __new_nodes = ((__new_elems + _S_buffer_size() - 1)
             / _S_buffer_size());
    _reserve_map_at_front(__new_nodes);
    size_type __i;

    try {
      for (__i = 1; __i <= __new_nodes; ++__i)
        *(this->_impl._start._node - __i) = this->_allocate_node();
    } catch(...) {
        for (size_type __j = 1; __j < __i; ++__j)
          _deallocate_node(*(this->_impl._start._node - __j));
          //__throw_exception_again;
          throw std::bad_alloc();
    }
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_new_elements_at_back(size_type __new_elems)
{
  if (this->max_size() - this->size() < __new_elems)
    //__throw_length_error(__N("deque::_new_elements_at_back"));
    throw std::length_error("atomic_deque::_new_elements_at_back");

  const size_type __new_nodes = ((__new_elems + _S_buffer_size() - 1)
                  / _S_buffer_size());
  _reserve_map_at_back(__new_nodes);
  size_type __i;

  try {
    for (__i = 1; __i <= __new_nodes; ++__i)
      *(this->_impl._finish._node + __i) = this->_allocate_node();
  } catch(...) {
    for (size_type __j = 1; __j < __i; ++__j)
      _deallocate_node(*(this->_impl._finish._node + __j));
    //__throw_exception_again;
    throw std::bad_alloc();
  }
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_reallocate_map(size_type __nodes_to_add,
                                           bool __add_at_front)
{
  const size_type __old_num_nodes
    = this->_impl._finish._node - this->_impl._start._node + 1;
  const size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;

  _map_pointer __new_nstart;
  if (this->_impl._map_size > 2 * __new_num_nodes)
  {
    __new_nstart = this->_impl._map + (this->_impl._map_size
                       - __new_num_nodes) / 2
                    + (__add_at_front
                       ? __nodes_to_add
                       : 0);

    if (__new_nstart < this->_impl._start._node)
      std::copy(this->_impl._start._node,
            this->_impl._finish._node + 1,
            __new_nstart);
    else
      std::copy_backward(this->_impl._start._node,
                 this->_impl._finish._node + 1,
                 __new_nstart + __old_num_nodes);
  }
  else
  {
    size_type __new_map_size = this->_impl._map_size
                 + std::max(this->_impl._map_size,
                        __nodes_to_add) + 2;

    _map_pointer __new_map = this->_allocate_map(__new_map_size);
    __new_nstart = __new_map + (__new_map_size - __new_num_nodes) / 2
            + (__add_at_front ? __nodes_to_add : 0);
    std::copy(this->_impl._start._node,
          this->_impl._finish._node + 1,
          __new_nstart);
    _deallocate_map(this->_impl._map, this->_impl._map_size);

    this->_impl._map = __new_map;
    this->_impl._map_size = __new_map_size;
  }

  this->_impl._start._set_node(__new_nstart);
  this->_impl._finish._set_node(__new_nstart + __old_num_nodes - 1);

}

//============================================================================//
//@{
/// Memory-handling helpers for the previous internal insert functions.
template <typename _Tp, typename _Alloc>
typename atomic_deque<_Tp, _Alloc>::iterator
atomic_deque<_Tp, _Alloc>::_reserve_elements_at_front(size_type __n)
{
  const size_type __vacancies = this->_impl._start._curr
                  - this->_impl._start._first;
  if (__n > __vacancies)
    _new_elements_at_front(__n - __vacancies);
  return this->_impl._start - difference_type(__n);
}

//============================================================================//
template <typename _Tp, typename _Alloc>
typename atomic_deque<_Tp, _Alloc>::iterator
atomic_deque<_Tp, _Alloc>::_reserve_elements_at_back(size_type __n)
{
  const size_type __vacancies = (this->_impl._finish._last
                   - this->_impl._finish._curr) - 1;
  if (__n > __vacancies)
    _new_elements_at_back(__n - __vacancies);
  return this->_impl._finish + difference_type(__n);
}

//============================================================================//
template<typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_destroy_data_aux(iterator __first, iterator __last)
{
  for (_map_pointer __node = __first._node + 1;
      __node < __last._node; ++__node)
    this->_impl.destroy(*__node, *__node + _S_buffer_size());//,
              //_get_Tp_allocator());

    if (__first._node != __last._node)
  {
      this->_impl.destroy(__first._curr, __first._last);//,
                //_get_Tp_allocator());
      this->_impl.destroy(__last._first, __last._curr);//,
                //_get_Tp_allocator());
   }
    else
      this->_impl.destroy(__first._curr, __last._curr);//,
                //_get_Tp_allocator());
}

//============================================================================//
template<typename _Tp, typename _Alloc>
typename atomic_deque<_Tp, _Alloc>::iterator
atomic_deque<_Tp, _Alloc>::_insert_aux(iterator __pos, const basic_type& __x)
{
  basic_type __x_copy = __x;

  difference_type __index = __pos - this->_impl._start;
  if (static_cast<size_type>(__index) < size() / 2)
  {
    push_front(_GLIBCXX_MOVE(front()));
      iterator __front1 = this->_impl._start;
      ++__front1;
      iterator __front2 = __front1;
      ++__front2;
      __pos = this->_impl._start + __index;
      iterator __pos1 = __pos;
      ++__pos1;
      _GLIBCXX_MOVE3(__front2, __pos1, __front1);
  }
  else
  {
      push_back(_GLIBCXX_MOVE(back()));
      iterator __back1 = this->_impl._finish;
      --__back1;
      iterator __back2 = __back1;
      --__back2;
      __pos = this->_impl._start + __index;
      _GLIBCXX_MOVE_BACKWARD3(__pos, __back2, __back1);
  }
  *__pos = _GLIBCXX_MOVE(__x_copy);
  //*__pos = __x_copy.load();
  return __pos;
}

//============================================================================//

namespace atomics
{
namespace details
{
  namespace thisnp
  {
#ifdef CXX11
  using std::addressof;
#elif defined(USE_BOOST)
  using boost::addressof;
#endif
    /*
    static const bool true_type = true;
    static const bool false_type = false;

    // Primary template.
    /// Define a member typedef @c type only if a boolean constant is true.
      template<bool, typename _Tp = void>
      struct enable_if
      { };

      // Partial specialization for true.
      template<typename _Tp>
      struct enable_if<true, _Tp>
      { typedef _Tp type; };

      template<typename _Tp>
    struct __destroy_helper
    {
        template<typename _Alloc2,
        typename =
         decltype(std::declval<_Alloc2*>()->destroy(std::declval<_Tp*>()))>
        static true_type __test(int);

        template<typename> static false_type __test(...);

        typedef decltype(__test<_Alloc>(0)) type;
        static const bool value = type::value;
    };

      template<typename _Tp>
    static typename enable_if<__destroy_helper<_Tp>::value, void>::type
      _S_destroy(_Alloc& __a, _Tp* __p)
    { __a.destroy(__p); }

      template<typename _Tp>
    static typename enable_if<!__destroy_helper<_Tp>::value, void>::type
      _S_destroy(_Alloc&, _Tp* __p)
    { __p->~_Tp(); }*/
  }


  //------------------------------------------------------------------------//
  /**
   * Destroy the object pointed to by a pointer type.
   */
  template<typename _Tp>
  inline void
  _destroy(_Tp* __pointer) { __pointer->~_Tp(); }
  //------------------------------------------------------------------------//
  template <typename _Alloc, typename _Tp>
  static void destroy( _Alloc& _a, _Tp* _ptr)
  {
#ifdef CXX11
    typedef std::allocator_traits<_Alloc> __traits;
      __traits::destroy(_a, _ptr);
#else
    _ptr->~_Tp();
#endif
  }
  //------------------------------------------------------------------------//
  template<bool>
  struct _destroy_aux
  {
    template<typename _ForwardIterator>
    static void
    __destroy(_ForwardIterator __first, _ForwardIterator __last)
    {
      for (; __first != __last; ++__first)
#if defined(CXX11) || defined(USE_BOOST)
          _destroy(thisnp::addressof(*__first));
#else
          _destroy(&(*__first));
#endif
    }
    };
  //------------------------------------------------------------------------//
  template<>
  struct _destroy_aux<true>
  {
    template<typename _ForwardIterator>
    static void
    __destroy(_ForwardIterator, _ForwardIterator) { }
    };
  //------------------------------------------------------------------------//
  template<typename _ForwardIterator>
  inline void
  _destroy(_ForwardIterator __first, _ForwardIterator __last)
  {
    typedef typename
        std::iterator_traits<_ForwardIterator>::value_type _Value_type;
    _destroy_aux<__has_trivial_destructor(_Value_type)>::__destroy(__first,
                                                                       __last);
  }
  //------------------------------------------------------------------------//
  /**
   * Destroy a range of objects using the supplied allocator.  For
   * nondefault allocators we do not optimize away invocation of
   * destroy() even if _Tp has a trivial destructor.
   */
  template <typename _Tp> class allocator;

  //------------------------------------------------------------------------//
  template<typename _ForwardIterator, typename _Allocator>
  void
  _destroy(_ForwardIterator __first, _ForwardIterator __last,
         _Allocator& __alloc)
  {
      for (; __first != __last; ++__first)
#if defined(CXX11) || defined(USE_BOOST)
          destroy(__alloc, thisnp::addressof(*__first));
#else
          destroy(__alloc, &(*__first));
#endif
  }

  //------------------------------------------------------------------------//
  template<typename _ForwardIterator, typename _Tp>
  inline void
  _destroy(_ForwardIterator __first, _ForwardIterator __last,
         allocator<_Tp>&)
  {
    _destroy(__first, __last);
  }
  //------------------------------------------------------------------------//
}
}

//============================================================================//
template <typename _Tp, typename _Alloc>
void
atomic_deque<_Tp, _Alloc>::_fill_initialize(const basic_type& __value)
{
    _map_pointer __cur;
    try
    {
      for (__cur = this->_impl._start._node;
             __cur < this->_impl._finish._node; ++__cur)
        {
            for(value_type* ite = *__cur;
                ite != (*__cur + _S_buffer_size()); ++ite)
            {
                *ite = __value;
            }
        }

        for(value_type* ite = this->_impl._finish._first;
            ite != this->_impl._finish._curr; ++ite)
        {
            *ite = __value;
        }
    }
    catch(...)
    {
      atomics::details::_destroy(this->_impl._start,
                                   iterator(*__cur, __cur),
                                   _get_Tp_allocator());
      throw
        std::runtime_error("atomic_deque<_Tp>::_fill_initialize - bad_alloc()");
    }
}


//============================================================================//

} // namespace mad

//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//


#endif
