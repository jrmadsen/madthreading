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
// created by jrmadsen on Tue Jul  1 17:04:19 2014
//
//
//
//
// The atomic_deque class is an implementation of the STL deque class with
// the ability to use atomics as the value type. In general, STL containers
// cannot use atomics as the value type because of the use of the copy constructor
// in initializing/adding values to the container
//
// The atomic_deque (as of July 8, 2014) has almost all the same functionality
// as the STL deque. In general, atomic_deque takes only POD type as template
// parameters but can support pairs of atomics as the basic type (verified)
// and, likely, pairs of pairs, (untested) in a fashion similar to the following:
//
//  // typedef-ing the the basic and atomic types
//  typedef uint32 first_t;
//  typedef double second_t;
//  typedef atomic<first_t> first_atom_t;
//  typedef atomic<second_t> second_atom_t;
//  typedef std::pair<first_t, second_t> basic_pair;
//  typedef std::pair<first_atom_t, second_atom_t> atomic_pair;
//
//  // typedef-ing the allocator, allocator policy, and allocator traits
//  typedef atomic_allocator_policy<basic_pair, atomic_pair > pair_alloc_policy;
//  typedef atomic_allocator_traits<basic_pair, atomic_pair > pair_alloc_traits;
//  typedef atomic_allocator<basic_pair,
//                           pair_alloc_policy, pair_alloc_traits> pair_alloc;
//
//  // typedef-ing the atomic deque containing pairs of atomics
//  typedef atomic_deque<basic_pair, pair_alloc> pair_atomic_deque;
//

#ifndef atomic_deque_hh_
#define atomic_deque_hh_

#ifdef SWIG
#   ifdef USE_BOOST_SERIALIZATION
#       undef USE_BOOST_SERIALIZATION
#   endif
#endif

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic_deque
%{
    #define SWIG_FILE_WITH_INIT
    #include "madthreading/atomics/atomic.hh"
    #include "madthreading/atomics/atomic_deque.hh"
%}

%import "madthreading/atomics/atomic.hh"
%include "atomic_deque.hh"
#endif
//----------------------------------------------------------------------------//

#include "atomic.hh"

#ifdef _HAS_ATOMICS_

#include "details/atomic_deque_base.hh"
#include "details/atomic_allocator.hh"

namespace mad
{

template <typename _Tp, typename _Alloc = atomic_allocator<_Tp> >
class atomic_deque : protected atomics::details::_atomic_deque_base<_Tp, _Alloc>
{
  typedef typename _Alloc::value_type     _Alloc_value_type;
  typedef atomics::details::_atomic_deque_base<_Tp, _Alloc>   _base;
  typedef typename _base::_Tp_alloc_type     _Tp_alloc_type;

public:
  typedef _Tp                      basic_type;
  typedef typename _Tp_alloc_type::value_type      value_type;
  typedef typename _Tp_alloc_type::pointer      pointer;
  typedef typename _Tp_alloc_type::const_pointer     const_pointer;
  typedef typename _Tp_alloc_type::reference      reference;
  typedef typename _Tp_alloc_type::const_reference   const_reference;
  typedef typename _base::iterator          iterator;
  typedef typename _base::const_iterator        const_iterator;
  typedef std::reverse_iterator<iterator>        reverse_iterator;
  typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;
  typedef size_t                    size_type;
  typedef ptrdiff_t                  difference_type;
  typedef _Alloc                    allocator_type;

protected:
  typedef pointer*                  _map_pointer;

  static size_t _S_buffer_size()
    { return atomics::details::__atomic_deque_buf_size(sizeof(value_type)); }

  using _base::_initialize_map;
  using _base::_create_nodes;
  using _base::_destroy_nodes;
  using _base::_allocate_node;
  using _base::_deallocate_node;
  using _base::_allocate_map;
  using _base::_deallocate_map;
  using _base::_get_Tp_allocator;

  using _base::_impl;

public:
  atomic_deque()
  : _base()
  { }

  explicit
  atomic_deque(const allocator_type& _a)
  : _base(_a, 0)
  { }

  explicit
  atomic_deque(size_type _n,
         const basic_type& _value = basic_type(),
         const allocator_type& _a = allocator_type())
  : _base(_a, _n)
  {
    _fill_initialize(_value);
  }

  explicit
  atomic_deque(size_type _n,
         const value_type& _value = value_type(),
         const allocator_type& _a = allocator_type())
  : _base(_a, _n)
  {
    _fill_initialize(_value.load());
  }

  atomic_deque(const atomic_deque& _rhs)
  : _base(_rhs._get_Tp_allocator(), _rhs.size())
  {
    atomics::details::copy(_rhs.begin(), _rhs.end(), this->_impl._start);
  }

public:
  // Public functions

  atomic_deque& operator=(const atomic_deque&);

  void assign(size_type _n, const value_type& _val);

  template <typename InputIterator>
  void assign(InputIterator _first, InputIterator _last);

  allocator_type get_allocator() const { return _base::get_allocator(); }

  iterator begin() { return this->_impl._start; }
  const_iterator begin() const { return this->_impl._start; }

  iterator end() { return this->_impl._finish; }
  const_iterator end() const { return this->_impl._finish; }

  reverse_iterator rbegin()
    { return reverse_iterator(this->_impl._finish); }
  const_reverse_iterator rbegin() const
    { return const_reverse_iterator(this->_impl._finish); }

  reverse_iterator rend()
    { return reverse_iterator(this->_impl._start); }
  const_reverse_iterator rend() const
    { return const_reverse_iterator(this->_impl._start); }

  size_type size() const { return this->_impl._finish - this->_impl._start; }

  size_type max_size() const { return _get_Tp_allocator().max_size(); }

  void resize(size_type __new_size, const basic_type& _val = basic_type())
  {
    const size_type _len = size();
    if(__new_size > _len) {
      _default_append(__new_size - _len);
      iterator ite = begin();
      std::advance(ite, _len);
      for(; ite != end(); ++ite) { *ite = _val; }
    } else if(__new_size < _len)
      _erase_at_end(this->_impl._start + difference_type(__new_size));
  }

  bool empty() const { return this->_impl._finish == this->_impl._start; }

  reference operator[](size_type _n)
    { return this->_impl._start[difference_type(_n)]; }
  const_reference operator[](size_type _n) const
    { return this->_impl._start[difference_type(_n)]; }

  iterator insert(iterator __position, const value_type& __x);

  basic_type operator()(size_type _n)
    { return this->_impl._start[difference_type(_n)].load(); }

protected:
  void _range_check(size_type _n) const
  {
    if(_n >= this->size())
    {
      throw std::out_of_range("atomic_deque::_range_check");
    }
  }

  //------------------------------------------------------------------------//
  // Defined in atomic_deque.tcc
  //------------------------------------------------------------------------//
#ifdef __GXX_EXPERIMENTAL_CXX0X__
  template<typename... _Args> void _push_back_aux(_Args&&...);
  template<typename... _Args> void _push_front_aux(_Args&&...);
#else
  void _push_back_aux(const basic_type&);
  void _push_front_aux(const basic_type&);

  void _push_back_aux(const value_type& _val)
    { _push_back_aux(_val.load()); }
  void _push_front_aux(const value_type& _val)
    { _push_front_aux(_val.load()); }
#endif

  void _pop_back_aux();
  void _pop_front_aux();
  //------------------------------------------------------------------------//


public:
  //------------------------------------------------------------------------//

  reference at(size_type _n)
  {
    _range_check(_n);
    return (*this)[_n];
  }

  //------------------------------------------------------------------------//

  const_reference at(size_type _n) const
  {
    _range_check(_n);
    return (*this)[_n];
  }
  //------------------------------------------------------------------------//

  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }

  //------------------------------------------------------------------------//

  reference back() { iterator ite = end(); return *(--ite); }
  const_reference back() const
    { const_iterator ite = end(); return *(--ite); }

  //------------------------------------------------------------------------//

  void push_front(const value_type& _x) { this->push_front(_x.load()); }
  void push_front(const basic_type& _x)
  {
    if(this->_impl._start._curr != this->_impl._start._first) {
      this->_impl.construct(this->_impl._start._curr -1, _x);
      --this->_impl._start._curr;
    } else {
      _push_front_aux(_x);
    }
  }

  //------------------------------------------------------------------------//

  void push_back(const value_type& _x) { this->push_back(_x.load()); }
  void push_back(const basic_type& _x)
  {
    if(this->_impl._finish._curr != this->_impl._finish._last - 1) {
      this->_impl.construct(this->_impl._finish._curr, _x);
      ++this->_impl._finish._curr;
    } else {
      _push_back_aux(_x);
    }
  }

  //------------------------------------------------------------------------//

  void pop_front()
  {
    if(this->_impl._start._curr != this->_impl._start._last - 1) {
      this->_impl.destroy(this->_impl._start._curr);
      ++this->_impl._start._curr;
    } else {
      _pop_front_aux();
    }
  }

  //------------------------------------------------------------------------//

  void pop_back()
  {
    if(this->_impl._finish._curr != this->_impl._finish._first)
    {
      --this->_impl._finish._curr;
      this->_impl.destroy(this->_impl._finish._curr);
    } else {
      _pop_back_aux();
    }
  }

  //------------------------------------------------------------------------//

  void swap(atomic_deque& _rhs)
  {
    std::swap(_impl._start, _rhs._impl._start);
    std::swap(_impl._finish, _rhs._impl._finish);
    std::swap(_impl._map, _rhs._impl._map);
    std::swap(_impl._map_size, _rhs._impl._map_size);
  }

  //------------------------------------------------------------------------//

  void clear() { _erase_at_end(begin()); }

  //------------------------------------------------------------------------//

  iterator erase(iterator __position);
  iterator erase(iterator __first, iterator __last);

  //------------------------------------------------------------------------//

protected:
  //------------------------------------------------------------------------//
  // Defined in atomic_deque.tcc
  //------------------------------------------------------------------------//
  void _fill_initialize(const basic_type&);

  void _fill_assign(size_type _n, const basic_type& _val);
  void _fill_assign(size_type _n, const value_type& _val)
    { this->_fill_assign(_n, _val.load()); }

  void _erase_at_begin(iterator __pos);

  void _erase_at_end(iterator __pos);

  void _default_append(size_type __n);

    void _reserve_map_at_back(size_type __nodes_to_add = 1);

    void _reserve_map_at_front(size_type __nodes_to_add = 1);

    void _new_elements_at_front(size_type __new_elems);

    void _new_elements_at_back(size_type __new_elems);

    void _reallocate_map(size_type __nodes_to_add, bool __add_at_front);

    iterator _reserve_elements_at_front(size_type __n);

    iterator _reserve_elements_at_back(size_type __n);

    void _destroy_data_aux(iterator __first, iterator __last);

    iterator _insert_aux(iterator __position, const basic_type& __x);
    iterator _insert_aux(iterator __position, const value_type& __x)
    { return _insert_aux(__position, __x.load()); }


  //------------------------------------------------------------------------//

protected:
    void _destroy_data(iterator __first, iterator __last)
    {
      _destroy_data_aux(__first, __last);
    }

  //------------------------------------------------------------------------//

#if defined(USE_BOOST_SERIALIZATION)
private:
    //------------------------------------------------------------------------//
    friend class boost::serialization::access;
    //------------------------------------------------------------------------//
    template <typename Archive>
    void save(Archive& ar, const unsigned int /*version*/) const
    {
        size_type _this_size = size();;
        ar << _this_size;
        for(const_iterator itr = begin(); itr != end(); ++itr)
        {
            basic_type val = *itr;
            ar << val;
        }
    }
    //------------------------------------------------------------------------//
    template <typename Archive>
    void load(Archive& ar, const unsigned int /*version*/)
    {
        size_type _this_size;
        ar >> _this_size;
        for(size_type i = 0; i < _this_size; ++i)
        {
            value_type _val = 0;
            ar >> _val;
            this->push_back(_val);
        }
    }
    //------------------------------------------------------------------------//
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int file_version)
    {
        boost::serialization::split_member(ar, *this, file_version);
    }
    //------------------------------------------------------------------------//
#endif
};

} // namespace mad

//----------------------------------------------------------------------------//

// Out-of-line definitions
#include "details/atomic_deque.tcc"

//----------------------------------------------------------------------------//

namespace mad
{

//  atomic_deque& operator=(const atomic_deque&);
template <typename _Tp, typename _Alloc>
atomic_deque<_Tp, _Alloc>& atomic_deque<_Tp, _Alloc>
::operator=(const atomic_deque<_Tp, _Alloc>& rhs)
{
    if(this != &rhs)
    {
        const size_type __len = size();

        if(__len >= rhs.size())
        {
            iterator ite = begin();
            std::advance(ite, rhs.size());
            _erase_at_end(ite);
        } else
        {
            this->resize(rhs.size(), _Tp());
        }
        atomics::details::copy(rhs.begin(), rhs.end(), this->begin());
    }

    return *this;
}
//----------------------------------------------------------------------------//
template <typename _Tp, typename _Alloc>
typename atomic_deque<_Tp, _Alloc>::iterator
atomic_deque<_Tp, _Alloc>::insert(iterator __position, const value_type& __x)
{
    if (__position._curr == this->_impl._start._curr)
    {
        push_front(__x);
        return this->_impl._start;
    }
    else if (__position._curr == this->_impl._finish._curr)
    {
        push_back(__x);
        iterator __tmp = this->_impl._finish;
        --__tmp;
        return __tmp;
    } else
    {
      return _insert_aux(__position, __x);
    }
}
//----------------------------------------------------------------------------//

#endif // _HAS_ATOMICS_
#ifndef SWIG
} // namespace mad
#endif
#endif // _atomic_deque_hh_
