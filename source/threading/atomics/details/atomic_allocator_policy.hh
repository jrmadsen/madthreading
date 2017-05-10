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
//  This is a custom allocator policy class for handling containers containing
//  atomics
//



#ifndef atomic_allocator_policy_hh_
#define atomic_allocator_policy_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic_allocator_policy
%{
    #include "atomic_allocator_policy.hh"
%}

%import "atomic.hh"
%include "atomic_allocator_policy.hh"
#endif
//----------------------------------------------------------------------------//

#include "atomic.hh"

#include <limits>

namespace mad
{

template <typename _Tp_base, typename _Tp_atom>
class atomic_allocator_policy
{
public:
  typedef _Tp_base        basic_type;
  typedef _Tp_atom         value_type;
  typedef value_type*       pointer;
  typedef const value_type*     const_pointer;
  typedef value_type&        reference;
  typedef const value_type&    const_reference;
  typedef size_t          size_type;
  typedef ptrdiff_t        difference_type;

public:
  // convert policy<T> to policy<U>
  template <typename _Up_base, typename _Up_atom>
  struct rebind
  {
    typedef atomic_allocator_policy<_Up_base, _Up_atom> other;
  };

public:
  // Constructor and Destructors
  inline explicit atomic_allocator_policy() { }
  inline ~atomic_allocator_policy() { }
  inline explicit atomic_allocator_policy(atomic_allocator_policy const&) { }

  template <typename _Up_base, typename _Up_atom>
  inline explicit atomic_allocator_policy(atomic_allocator_policy<_Up_base,
                                            _Up_atom> const&) { }

public:
  // Public functions
  inline pointer allocate(size_type __n,
                            typename std::allocator<void>::const_pointer = 0)
  {
        return
        reinterpret_cast<pointer>(::operator new(__n * sizeof(value_type)));
  }

  inline void deallocate(pointer p, size_type)
  {
    ::operator delete(p);
  }

  inline size_type max_size() const
  {
    return std::numeric_limits<size_type>::max();
  }



};



//----------------------------------------------------------------------------//
template <typename _Tp1_base, typename _Tp2_base,
          typename _Tp1_atom, typename _Tp2_atom>
inline bool operator==(atomic_allocator_policy<_Tp1_base, _Tp1_atom> const&,
             atomic_allocator_policy<_Tp2_base, _Tp2_atom> const&)
{
  return true;
}
//----------------------------------------------------------------------------//
template <typename _Tp_base, typename _Tp_atom, typename _OtherAllocator>
inline bool operator==(atomic_allocator_policy<_Tp_base, _Tp_atom> const&,
             _OtherAllocator const&)
{
  return false;
}
//----------------------------------------------------------------------------//

} // namespace mad

#endif
