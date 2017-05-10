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
//
//
//
// created by jrmadsen on Wed Jul  2 11:33:38 2014
//
//
//
//


#ifndef atomic_deque_iterator_hh_
#define atomic_deque_iterator_hh_

#include <cstddef>
#include "atomic.hh"

namespace mad
{
namespace atomics
{

namespace details
{
#ifndef _DEF_ATOMIC_DEQUE_BUF_SIZE
#define _DEF_ATOMIC_DEQUE_BUF_SIZE 512
#endif

  inline size_t __atomic_deque_buf_size(size_t __size)
  {
    return (__size < _DEF_ATOMIC_DEQUE_BUF_SIZE
                ? size_t(_DEF_ATOMIC_DEQUE_BUF_SIZE/__size)
                : size_t(1));
  }

  //========================================================================//
  //    ATOMIC DEQUE ITERATOR
  //========================================================================//
  /**
   *  @brief A atomic_deque::iterator.
   *
   *  Quite a bit of intelligence here.  Much of the functionality of
   *  atomic_deque is actually passed off to this class.  An atomic_deque
   *  holds two of these internally, marking its valid range.  Access to
   *  elements is done as offsets of either of those two, relying on
   *  operator overloading in this class.
   *
   *  All the functions are op overloads except for _set_node.
  */
  template <typename _Tp_base, typename _Tp_atom,
              typename _Ref, typename _Ptr>
  struct _atomic_deque_iterator
  {
    typedef _Tp_base                      basic_type;
    typedef _Tp_atom                      value_type;

    typedef _atomic_deque_iterator<_Tp_base,     _Tp_atom,
                                        value_type&, value_type*>  iterator;
    typedef _atomic_deque_iterator<_Tp_base,     _Tp_atom,
                                        const value_type&, const value_type*>
                                                                const_iterator;

    static size_t _S_buffer_size()
        { return __atomic_deque_buf_size(sizeof(_Tp_atom)); }

    typedef std::random_access_iterator_tag iterator_category;

    typedef _Ptr          pointer;
    typedef _Ref          reference;
    typedef size_t          size_type;
    typedef ptrdiff_t        difference_type;
    typedef value_type**      _map_pointer;
    typedef _atomic_deque_iterator  _self;

    value_type* _curr;
    value_type* _first;
    value_type* _last;
    _map_pointer _node;

    _atomic_deque_iterator(value_type* __x, _map_pointer __y)
    : _curr(__x), _first(*__y),
      _last(*__y + _S_buffer_size()),
      _node(__y)
    { }

    _atomic_deque_iterator()
    : _curr(0), _first(0), _last(0), _node(0)
    { }

    _atomic_deque_iterator(const iterator& __x)
    : _curr(__x._curr), _first(__x._first),
      _last(__x._last), _node(__x._node)
    { }

    void _set_node(_map_pointer __new_node)
    {
      _node = __new_node;
      _first = *__new_node;
      _last = _first + difference_type(_S_buffer_size());
    }

    reference operator*() const { return *_curr; }

    pointer operator->() const { return _curr; }

    _self& operator++()
    {
      ++_curr;
      if(_curr == _last) { _set_node(_node + 1); _curr = _first; }
      return *this;
    }

    _self& operator--()
    {
      if(_curr == _first) { _set_node(_node -1); _curr = _last; }
      --_curr;
      return *this;
    }

    _self operator++(int)
    {
      _self _tmp = *this;
      ++*this;
      return _tmp;
    }

    _self operator--(int)
    {
      _self _tmp = *this;
      --*this;
      return _tmp;
    }

    _self& operator+=(difference_type __n)
    {
      const difference_type __offset = __n + (_curr - _first);
      if(__offset >= 0 && __offset < difference_type(_S_buffer_size()))
                _curr += __n;
      else {
        const difference_type __node_offset = __offset > 0
            ? __offset / difference_type(_S_buffer_size())
            : -difference_type((-__offset-1)/_S_buffer_size()) - 1;
        _set_node(_node + __node_offset);
        _curr = _first + (__offset - __node_offset
                                  * difference_type(_S_buffer_size()));
      }
      return *this;
    }

    _self operator+(difference_type __n) const
    {
      _self _tmp = *this;
      return _tmp += __n;
    }

    _self& operator-=(difference_type __n)
    {
      return *this += -__n;
    }

    _self operator-(difference_type __n) const
    {
      _self _tmp = *this;
      return _tmp -= __n;
    }

    reference operator[](difference_type __n) const
    {
      return *(*this + __n);
    }

  };

  //------------------------------------------------------------------------//
  // OPERATOR ==
  //------------------------------------------------------------------------//
  template <typename _Tp_base, typename _Tp_atom,
              typename _Ref, typename _Ptr>
  inline bool
  operator==(const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _x,
         const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _y)
  {
    return _x._curr == _y._curr;
  }

  template <typename _Tp_base, typename _Tp_atom,
              typename _RefL, typename _PtrL,
              typename _RefR, typename _PtrR>
  inline bool
  operator==(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefL,_PtrL>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefR,_PtrR>& _y)
  {
    return _x._curr == _y._curr;
  }

  //------------------------------------------------------------------------//
  // OPERATOR !=
  //------------------------------------------------------------------------//
  template <typename _Tp_base, typename _Tp_atom,
              typename _Ref, typename _Ptr>
  inline bool
  operator!=(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_Ref,_Ptr>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_Ref,_Ptr>& _y)
  {
    return !(_x == _y);
  }

  template <typename _Tp_base, typename _Tp_atom,
              typename _RefL, typename _PtrL,
              typename _RefR, typename _PtrR>
  inline bool
  operator!=(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefL,_PtrL>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefR,_PtrR>& _y)
  {
    return !(_x == _y);
  }

  //------------------------------------------------------------------------//
  // OPERATOR <
  //------------------------------------------------------------------------//
  template <typename _Tp_base, typename _Tp_atom,
              typename _Ref, typename _Ptr>
  inline bool
  operator<(const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _x,
         const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _y)
  {
    return (_x._node == _y._node)
        ? (_x._curr < _y._curr)
        : (_x._node < _y._node);
  }

  template <typename _Tp_base, typename _Tp_atom,
              typename _RefL, typename _PtrL,
              typename _RefR, typename _PtrR>
  inline bool
  operator<(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefL,_PtrL>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefR,_PtrR>& _y)
  {
    return (_x._node == _y._node)
        ? (_x._curr < _y._curr)
        : (_x._node < _y._node);
  }

  //------------------------------------------------------------------------//
  // OPERATOR >
  //------------------------------------------------------------------------//
  template <typename _Tp_base, typename _Tp_atom,
              typename _Ref, typename _Ptr>
  inline bool
  operator>(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_Ref,_Ptr>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_Ref,_Ptr>& _y)
  {
    return _y < _x;
  }

  template <typename _Tp_base, typename _Tp_atom,
              typename _RefL, typename _PtrL,
              typename _RefR, typename _PtrR>
  inline bool
  operator>(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefL,_PtrL>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefR,_PtrR>& _y)
  {
    return _y < _x;
  }

  //------------------------------------------------------------------------//
  // OPERATOR <=
  //------------------------------------------------------------------------//
  template <typename _Tp_base, typename _Tp_atom,
              typename _Ref, typename _Ptr>
  inline bool
  operator<=(const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _x,
         const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _y)
  {
    return !(_y < _x);
  }

  template <typename _Tp_base, typename _Tp_atom,
              typename _RefL, typename _PtrL,
              typename _RefR, typename _PtrR>
  inline bool
  operator<=(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefL, _PtrL>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefR, _PtrR>& _y)
  {
    return !(_y < _x);
  }

  //------------------------------------------------------------------------//
  // OPERATOR >=
  //------------------------------------------------------------------------//
  template <typename _Tp_base, typename _Tp_atom,
              typename _Ref, typename _Ptr>
  inline bool
  operator>=(const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _x,
         const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _y)
  {
    return !(_x < _y);
  }

  template <typename _Tp_base, typename _Tp_atom,
              typename _RefL, typename _PtrL,
              typename _RefR, typename _PtrR>
  inline bool
  operator>=(const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefL,_PtrL>& _x,
         const _atomic_deque_iterator<_Tp_base,_Tp_atom,_RefR,_PtrR>& _y)
  {
    return !(_x < _y);
  }

  // According to the resolution of DR179 not only the various comparison
  // operators but also operator- must accept mixed iterator/const_iterator
  // parameters.
  template<typename _Tp_base, typename _Tp_atom,
             typename _Ref, typename _Ptr>
  inline typename _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                           _Ref, _Ptr>::difference_type
  operator-(const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _x,
          const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _y)
  {
    return typename _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                               _Ref, _Ptr>::difference_type
    (_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _Ref, _Ptr>::_S_buffer_size())
    * (_x._node - _y._node - 1) + (_x._curr - _x._first)
    + (_y._last - _y._curr);
  }

  template<typename _Tp_base, typename _Tp_atom,
             typename _RefL, typename _PtrL, typename _RefR, typename _PtrR>
  inline typename _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                           _RefL, _PtrL>::difference_type
  operator-(const _atomic_deque_iterator<_Tp_base,_Tp_atom, _RefL, _PtrL>& _x,
          const _atomic_deque_iterator<_Tp_base,_Tp_atom, _RefR, _PtrR>& _y)
  {
    return typename _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                               _RefL, _PtrL>::difference_type
    (_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _RefL, _PtrL>::_S_buffer_size())
    * (_x._node - _y._node - 1) + (_x._curr - _x._first)
    + (_y._last - _y._curr);
  }

  template<typename _Tp_base, typename _Tp_atom, typename _Ref, typename _Ptr>
  inline _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>
  operator+(ptrdiff_t __n,
              const _atomic_deque_iterator<_Tp_base, _Tp_atom, _Ref, _Ptr>& _x)
  {
    return _x + __n;
  }

  template <typename _Tp_base, typename _Tp_atom>
  void
  fill(const _atomic_deque_iterator<_Tp_base,_Tp_atom, _Tp_atom&, _Tp_atom*>&,
     const _atomic_deque_iterator<_Tp_base,_Tp_atom, _Tp_atom&, _Tp_atom*>&,
     const _Tp_base&);

  /*
  template <typename _Tp_base>
  _atomic_deque_iterator<_Tp_base, atomic<_Tp_base>,
     atomic<_Tp_base>&, atomic<_Tp_base>*>
  copy(_atomic_deque_iterator<_Tp_base, atomic<_Tp_base>,
     const atomic<_Tp_base>&, const atomic<_Tp_base>*> _first,
     _atomic_deque_iterator<_Tp_base, atomic<_Tp_base>,
     const atomic<_Tp_base>&, const atomic<_Tp_base>*> _last,
     _atomic_deque_iterator<_Tp_base, atomic<_Tp_base>,
     atomic<_Tp_base>&, atomic<_Tp_base>*> _result)
  {
    while (_first != _last) {
        *_result = (*_first).load();
        ++_result;
        ++_first;
    }
    return _result;
  }*/

  template <typename _Tp_base, typename _Tp_atom>
  _atomic_deque_iterator<_Tp_base, _Tp_atom, _Tp_atom&, _Tp_atom*>
  copy(_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                const _Tp_atom&, const _Tp_atom*> _first,
     _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                const _Tp_atom&, const _Tp_atom*> _last,
     _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _Tp_atom&, _Tp_atom*> _result)
  {
    while (_first != _last) {
        //_Tp_base _val = (*_first);
        *_result = atomics::get(*_first);
        ++_result;
        ++_first;
    }
    return _result;
  }

  template <typename _Tp_base, typename _Tp_atom>
  inline _atomic_deque_iterator<_Tp_base, _Tp_atom, _Tp_atom&, _Tp_atom*>
  copy(_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _Tp_atom&, _Tp_atom*> _first,
     _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _Tp_atom&, _Tp_atom*> _last,
     _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _Tp_atom&, _Tp_atom*> _result)
  {
    return copy(_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                           const _Tp_atom&,
                                           const _Tp_atom*>(_first),
                    _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                           const _Tp_atom&,
                                           const _Tp_atom*>(_last),
          _result);
  }

  template <typename _Tp_base, typename _Tp_atom>
  _atomic_deque_iterator<_Tp_base, _Tp_atom, _Tp_atom&, _Tp_atom*>
  copy_backward(_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                         const _Tp_atom&,
                                         const _Tp_atom*> _first,
          _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                         const _Tp_atom&,
                                         const _Tp_atom*> _last,
          _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                         _Tp_atom&, _Tp_atom*> _result)
  {
    while (_first != _last) {
      //_Tp_base _val = (*_last);
        *_result = atomics::get(*_last);
        ++_result;
        ++_last;
    }
    return _result;
  }

  template <typename _Tp_base, typename _Tp_atom>
  inline _atomic_deque_iterator<_Tp_base, _Tp_atom, _Tp_atom&, _Tp_atom*>
  copy_backward(_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                         _Tp_atom&, _Tp_atom*> _first,
     _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _Tp_atom&, _Tp_atom*> _last,
     _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                _Tp_atom&, _Tp_atom*> _result)
  {
    return std::copy_backward(_atomic_deque_iterator<_Tp_base, _Tp_atom,
                                                         const _Tp_atom&,
                                                         const _Tp_atom*>
                                  (_first),
                    _atomic_deque_iterator<_Tp_base, _Tp_atom,
                                                         const _Tp_atom&,
                                                         const _Tp_atom*>
                                  (_last),
                    _result);
  }



} // namespace details

} // namespace atomics

} // namespace mad

//----------------------------------------------------------------------------//
/*
namespace std
{
  template <typename _Tp>
  inline
  atomics::details::_atomic_deque_iterator<_Tp, atomic<_Tp>&,
 atomic<_Tp>*>
  copy(atomics::details::_atomic_deque_iterator<_Tp, atomic<_Tp>&,
 atomic<_Tp>*> _first,
     atomics::details::_atomic_deque_iterator<_Tp, atomic<_Tp>&,
 atomic<_Tp>*> _last,
     atomics::details::_atomic_deque_iterator<_Tp, atomic<_Tp>&,
 atomic<_Tp>*> result)
  {
    while (_first != _last) {
        *result = (*_first)();
        ++result;
        ++_first;
    }
    return result;
  }

  template <typename _Tp>
  inline
  atomics::details::_atomic_deque_iterator<_Tp, atomic<_Tp>&,
 atomic<_Tp>*>
  copy_backward(atomics::details::_atomic_deque_iterator<_Tp,
 atomic<_Tp>&, atomic<_Tp>*> _first,
          atomics::details::_atomic_deque_iterator<_Tp,
 atomic<_Tp>&, atomic<_Tp>*> _last,
          atomics::details::_atomic_deque_iterator<_Tp,
 atomic<_Tp>&, atomic<_Tp>*> result)
  {
    while (_first != _last) {
        *result = (*_last)();
        ++result;
        ++_last;
    }
    return result;
  }

  template <typename _Tp>
  inline
  atomics::details::_atomic_deque_iterator<_Tp, atomic<_Tp>&,
 atomic<_Tp>*>
  copy(atomics::details::_atomic_deque_iterator<_Tp,
 const atomic<_Tp>&, const atomic<_Tp>*> _first,
     atomics::details::_atomic_deque_iterator<_Tp,
 const atomic<_Tp>&, const atomic<_Tp>*> _last,
     atomics::details::_atomic_deque_iterator<_Tp,
 atomic<_Tp>&, atomic<_Tp>*> result)
  {
    while (_first != _last) {
        *result = (*_first)();
        ++result;
        ++_first;
    }
    return result;
  }

  template <typename _Tp>
  inline
  atomics::details::_atomic_deque_iterator<_Tp, atomic<_Tp>&,
 atomic<_Tp>*>
  copy_backward(atomics::details::_atomic_deque_iterator<_Tp,
 const atomic<_Tp>&, const atomic<_Tp>*> _first,
          atomics::details::_atomic_deque_iterator<_Tp,
 const atomic<_Tp>&, const atomic<_Tp>*> _last,
          atomics::details::_atomic_deque_iterator<_Tp,
 atomic<_Tp>&, atomic<_Tp>*> result)
  {
    while (_first != _last) {
        *result = (*_last)();
        ++result;
        ++_last;
    }
    return result;
  }
}
*/
//----------------------------------------------------------------------------//


#endif
