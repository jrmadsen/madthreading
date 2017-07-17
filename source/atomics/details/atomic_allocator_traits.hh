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
// created by jrmadsen on Wed Jul  2 12:59:07 2014
//
//
//
//
//  This is a custom allocator trait class for handling containers containing
//  atomics
//


#ifndef atomic_allocator_traits_hh_
#define atomic_allocator_traits_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic_allocator_traits
%{
    #define SWIG_FILE_WITH_INIT
    #include "atomic.hh"
    #include "atomic_allocator_traits.hh"
%}

%import "atomic.hh"
%include "atomic_allocator_traits.hh"
#endif
//----------------------------------------------------------------------------//

#include "atomic.hh"

namespace mad
{

template <typename _Tp_base, typename _Tp_atom>
class atomic_allocator_traits
{
public:
  typedef _Tp_base        basic_type;
  typedef _Tp_atom         value_type;

public:
  // convert to traits<_Tp> to traits<_Up>
  template <typename _Up_base, typename _Up_atom>
  struct rebind
  {
    typedef atomic_allocator_traits<_Up_base, _Up_atom> other;
  };

public:
  // Constructor and Destructors
  inline explicit atomic_allocator_traits() { }
  inline ~atomic_allocator_traits() { }

  template <typename _Up_base, typename _Up_atom>
  inline explicit
    atomic_allocator_traits(atomic_allocator_traits<_Up_base, _Up_atom> const&)
    { }

public:
  // address
  inline value_type* address(value_type& __r) { return &__r; }
  inline value_type const* address(value_type const& __r) { return &__r; }

  inline void construct(value_type* __p, const basic_type& t)
  {
        new(__p) value_type;
    //*__p = t;
        atomics::set(__p, t);
  }

  inline void construct(value_type* __p, const value_type& t)
  {
    new(__p) value_type;
    //*__p = t;
    atomics::set(__p, t);
  }

  inline void destroy(value_type* __p) { __p->~value_type(); }

  inline void destroy(value_type* __first, value_type* __last)
  {
    for(value_type* __p = __first; __p != __last; ++__p) { destroy(__p); }
  }
};



//----------------------------------------------------------------------------//

} // namespace mad

//----------------------------------------------------------------------------//


#endif
