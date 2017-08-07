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
//
//  This is a custom allocator class for handling containers containing atomics
//    This class inherits, by default, from atomic_allocator_policy and
//    atomic_allocator_traits
//  The policy and traits class can be over-ridden from the default
//  via atomic_allocator<_Tp, _other_policy, _other_traits>
//

#ifndef atomic_allocator_hh_
#define atomic_allocator_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic_allocator
%{
    #include "atomic_allocator.hh"
%}
#endif
//----------------------------------------------------------------------------//

#include "atomic_allocator_traits.hh"
#include "atomic_allocator_policy.hh"

namespace mad
{

template <typename _Tp,
      typename _Policy = atomic_allocator_policy<_Tp, atomic<_Tp> >,
      typename _Traits = atomic_allocator_traits<_Tp, atomic<_Tp> > >
class atomic_allocator : public _Policy, public _Traits
{
private:
  typedef _Policy AllocationPolicy;
  typedef _Traits AllocationTraits;

public:
  typedef typename AllocationPolicy::basic_type    basic_type;
  typedef typename AllocationPolicy::value_type    value_type;
  typedef typename AllocationPolicy::pointer      pointer;
  typedef typename AllocationPolicy::const_pointer  const_pointer;
  typedef typename AllocationPolicy::reference    reference;
  typedef typename AllocationPolicy::const_reference  const_reference;
  typedef typename AllocationPolicy::size_type    size_type;
  typedef typename AllocationPolicy::difference_type  difference_type;

public:
  template <typename _Up_base, typename _Up_atom = atomic<_Up_base> >
  struct rebind
  {
    typedef atomic_allocator<_Up_base,
            typename AllocationPolicy::template rebind<_Up_base,
                                                       _Up_atom>::other,
      typename AllocationTraits::template rebind<_Up_base,
                                                       _Up_atom>::other > other;

  };

public:
  // Constructor and Destructors
  inline explicit atomic_allocator() { }
  inline ~atomic_allocator() { }
  inline atomic_allocator(atomic_allocator const& rhs)
  : _Policy(rhs), _Traits(rhs)
  { }

  template <typename _Up>
  inline atomic_allocator(atomic_allocator<_Up> const&) { }

  template <typename _Up, typename _Policy2, typename _Traits2>
  inline atomic_allocator(atomic_allocator<_Up,_Policy2,_Traits2> const& rhs)
  : _Policy(rhs), _Traits(rhs)
  { }

};



//----------------------------------------------------------------------------//
// == and != for identical templates of atomic_allocator
//----------------------------------------------------------------------------//
template <typename _T, typename _Pol, typename _Tr>
inline bool operator==(atomic_allocator<_T, _Pol, _Tr> const& lhs,
             atomic_allocator<_T, _Pol, _Tr> const& rhs)
{
  return operator==(static_cast<_Pol&>(lhs), static_cast<_Pol&>(rhs));
}
//----------------------------------------------------------------------------//
template <typename _T, typename _Pol, typename _Tr>
inline bool operator!=(atomic_allocator<_T, _Pol, _Tr> const& lhs,
             atomic_allocator<_T, _Pol, _Tr> const& rhs)
{
  return !operator==(lhs, rhs);
}
//----------------------------------------------------------------------------//
// == and != for differing templates of atomic_allocator
//----------------------------------------------------------------------------//
template <typename _T1, typename _Pol1, typename _Tr1,
      typename _T2, typename _Pol2, typename _Tr2>
inline bool operator==(atomic_allocator<_T1, _Pol1, _Tr1> const& lhs,
             atomic_allocator<_T2, _Pol2, _Tr2> const& rhs)
{
  return operator==(static_cast<_Pol1&>(lhs), static_cast<_Pol2&>(rhs));
}
//----------------------------------------------------------------------------//
template <typename _T1, typename _Pol1, typename _Tr1,
      typename _T2, typename _Pol2, typename _Tr2>
inline bool operator!=(atomic_allocator<_T1, _Pol1, _Tr1> const& lhs,
             atomic_allocator<_T2, _Pol2, _Tr2> const& rhs)
{
  return !operator==(lhs, rhs);
}
//----------------------------------------------------------------------------//
// == and != for comparing atomic_allocator vs. other allocator
//----------------------------------------------------------------------------//
template <typename _T, typename _Pol, typename _Tr,typename _OtherAllocator>
inline bool operator==(atomic_allocator<_T, _Pol, _Tr> const& lhs,
             _OtherAllocator const& rhs)
{
  return operator==(static_cast<_Pol&>(lhs), rhs);
}
//----------------------------------------------------------------------------//
template <typename _T, typename _Pol, typename _Tr,typename _OtherAllocator>
inline bool operator!=(atomic_allocator<_T, _Pol, _Tr> const& lhs,
             _OtherAllocator const& rhs)
{
  return !operator==(lhs, rhs);
}
//----------------------------------------------------------------------------//

} // namespace mad

#endif
