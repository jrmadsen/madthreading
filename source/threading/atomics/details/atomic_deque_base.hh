//
//
//
//
//
// created by jrmadsen on Wed Jul  2 11:33:38 2014
//
//
//
//


#ifndef atomic_deque_base_hh_
#define atomic_deque_base_hh_

#include "atomic.hh"
#include "atomic_deque_iterator.hh"

namespace mad
{

namespace atomics
{

namespace details
{

  //========================================================================//
  //    ATOMIC DEQUE BASE
  //========================================================================//
  /**
   *  atomic_deque base class.  This class provides the unified face for
   *  %atomic_deque's allocation.  This class's constructor and destructor
   *  allocate and deallocate (but do not initialize) storage.  This makes
   *  %exception safety easier.
   *
   *  Nothing in this class ever constructs or destroys an actual Tp element.
   *  (atomic_deque handles that itself.)  Only/All memory management is
   *  performed here.
  */
  template <typename _Tp, typename _Alloc>
  class _atomic_deque_base
  {
  public:
    typedef _Alloc                      allocator_type;
    typedef _Tp                        basic_type;
    typedef typename allocator_type::value_type             value_type;
    typedef _atomic_deque_iterator<basic_type,
                                       value_type,
                                       value_type&,
                                       value_type*>             iterator;
    typedef _atomic_deque_iterator<basic_type,
                                       value_type,
                                       const value_type&,
                                       const value_type*>       const_iterator;

    allocator_type get_allocator() const
        { return allocator_type(_get_Tp_allocator()); }

    _atomic_deque_base()
    : _impl()
    {
      _initialize_map(0);
    }

    _atomic_deque_base(size_t __num_elements)
    : _impl()
    {
      _initialize_map(__num_elements);
    }

    _atomic_deque_base(const allocator_type& _a, size_t __num_elements)
    : _impl(_a)
    {
      _initialize_map(__num_elements);
    }

    _atomic_deque_base(const allocator_type& _a)
    : _impl(_a)
    { }

    virtual ~_atomic_deque_base();

  protected:
    typedef typename allocator_type::template
            rebind<basic_type, value_type*>::other  _map_alloc_type;
    typedef typename allocator_type::template
            rebind<basic_type, value_type >::other  _Tp_alloc_type;

    //--------------------------------------------------------------------//
    struct _atomic_deque_impl : public _Tp_alloc_type
    {
      value_type** _map;
      size_t _map_size;
      iterator _start;
      iterator _finish;

      _atomic_deque_impl()
      : _Tp_alloc_type(),
        _map(0), _map_size(0),
        _start(), _finish()
      { }

      _atomic_deque_impl(const _Tp_alloc_type& _a)
      : _Tp_alloc_type(_a),
        _map(0), _map_size(0),
        _start(), _finish()
      { }

    };
    //--------------------------------------------------------------------//

    _Tp_alloc_type& _get_Tp_allocator()
        { return *static_cast<_Tp_alloc_type*>(&this->_impl); }

        const _Tp_alloc_type& _get_Tp_allocator() const
        { return *static_cast<const _Tp_alloc_type*>(&this->_impl); }

        //_map_alloc_type _get_map_allocator() const
        //{ return *static_cast<const _map_alloc_type*>(&this->_impl); }

        _map_alloc_type _get_map_allocator() const
        { return _map_alloc_type(_get_Tp_allocator()); }

    value_type* _allocate_node()
        { return _impl._Tp_alloc_type
            ::allocate(__atomic_deque_buf_size(sizeof(value_type))); }

        void _deallocate_node(value_type* _p)
        { _impl._Tp_alloc_type
            ::deallocate(_p, __atomic_deque_buf_size(sizeof(value_type))); }

    value_type** _allocate_map(size_t __n)
    {
      return _get_map_allocator().allocate(__n);
    }

    void _deallocate_map(value_type** _p, size_t __n)
    {
      _get_map_allocator().deallocate(_p, __n);
    }


  protected:
    void _initialize_map(size_t);
    void _create_nodes(value_type** _nstart, value_type** _nfinish);
    void _destroy_nodes(value_type** _nstart, value_type** _nfinish);
    enum { _S_initial_map_size = 8 };

    _atomic_deque_impl _impl;

  };



//----------------------------------------------------------------------------//
template <typename _Tp, typename _Alloc>
_atomic_deque_base<_Tp, _Alloc>::~_atomic_deque_base()
{
  if(this->_impl._map)
  {
    _destroy_nodes(this->_impl._start._node,
             this->_impl._finish._node + 1);
    _deallocate_map(this->_impl._map, this->_impl._map_size);
  }
}
//----------------------------------------------------------------------------//
template <typename _Tp, typename _Alloc>
void
_atomic_deque_base<_Tp, _Alloc>::_initialize_map(size_t __num_elements)
{
  const size_t __num_nodes = (__num_elements/
                                __atomic_deque_buf_size(sizeof(value_type))
                                + 1);
  this->_impl._map_size = std::max((size_t) _S_initial_map_size,
                                     size_t(__num_nodes + 2));
  this->_impl._map = _allocate_map(this->_impl._map_size);

  value_type** __nstart = (this->_impl._map
                             + (this->_impl._map_size - __num_nodes) / 2);
  value_type** __nfinish = __nstart + __num_nodes;

  try {
    _create_nodes(__nstart, __nfinish);
  } catch(...) {
    _deallocate_map(this->_impl._map, this->_impl._map_size);
    this->_impl._map = 0;
    this->_impl._map_size = 0;
    throw std::runtime_error("atomic_deque::_initialize_map - bad alloc");
  //  throw_exception_again;
  }

  //_create_nodes(__nstart, __nfinish);

  this->_impl._start._set_node(__nstart);
  this->_impl._finish._set_node(__nfinish - 1);
  this->_impl._start._curr = _impl._start._first;
  this->_impl._finish._curr = (this->_impl._finish._first
                 + __num_elements
                 % __atomic_deque_buf_size(sizeof(value_type)));
}
//----------------------------------------------------------------------------//
template <typename _Tp, typename _Alloc>
void
_atomic_deque_base<_Tp, _Alloc>::_destroy_nodes(value_type** _nstart,
                                                value_type** _nfinish)
{
  for(value_type** __n = _nstart; __n < _nfinish; ++__n)
        _deallocate_node(*__n);
}
//----------------------------------------------------------------------------//
template <typename _Tp, typename _Alloc>
void
_atomic_deque_base<_Tp, _Alloc>::_create_nodes(value_type** __nstart,
                                               value_type** __nfinish)
{
  value_type** __curr;
  try {
    for(__curr = __nstart; __curr < __nfinish; ++__curr) {
      *__curr = this->_allocate_node();
    }
  } catch (...) {
    _destroy_nodes(__nstart, __curr);
    throw std::bad_alloc();//("atomic_deque::_create_nodes");
  }
}
//----------------------------------------------------------------------------//

} // namespace details

} // namespace atomics

} // namespace mad

#endif
