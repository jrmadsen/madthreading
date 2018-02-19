//
//
//
// MIT License
// Copyright (c) 2017 Jonathan R. Madsen
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// created by jmadsen on Tue Jul 18 00:30:59 2017
//
//
//
//

#ifndef aligned_allocator_hh_
#define aligned_allocator_hh_

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <limits>
#include <climits>
#include <new>

#include "madthreading/macros.hh"

#ifndef do_pragma
#   define do_pragma(x) _Pragma(#x)
#endif

#if defined(__GNUC__) && defined(__x86_64__)
#define attrib_assume_aligned __attribute__((assume_aligned(64)))
#define attrib_aligned __attribute__((aligned (64)))
#else
#define attrib_assume_aligned
#define attrib_aligned
#endif

namespace mad
{
//----------------------------------------------------------------------------//

// Byte alignment for SIMD.  This should work for all modern systems,
// including MIC
static size_t const SIMD_WIDTH = 64;

void* aligned_alloc(size_t size, size_t align);
void  aligned_free(void* ptr);

template <typename T>
class simd_allocator
{
public:
    // type definitions
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;

    // rebind allocator to type U
    template <typename U>
    struct rebind
    {
        typedef simd_allocator<U> other;
    };

    // return address of values
    pointer address(reference value) const
    {
        return &value;
    }

    const_pointer address(const_reference value) const
    {
        return &value;
    }

    simd_allocator() throw() { }

    simd_allocator(const simd_allocator&) throw() { }

    template <typename U>
    simd_allocator(const simd_allocator<U>&) throw() { }

    ~simd_allocator() throw() { }

    // return maximum number of elements that can be allocated
    size_type max_size() const throw()
    {
#if defined(CXX11)
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
#else
        return SIZE_MAX / sizeof(T);
#endif
    }

    // allocate but don't initialize num elements of type T
    pointer allocate(const size_type num, const void* hint = 0)
    {
        return static_cast<pointer>(aligned_alloc(num*sizeof(T), SIMD_WIDTH));
    }

    // initialize elements of allocated storage p with value value
    void construct(pointer p, const T& value)
    {
        // initialize memory with placement new
        new (static_cast<void*>(p)) T(value);
    }

    // destroy elements of initialized storage p
    void destroy(pointer p)
    {
        // destroy objects by calling their destructor
        p->~T();
    }

    // deallocate storage p of deleted elements
    void deallocate (pointer p, size_type num)
    {
        aligned_free(static_cast<void*>(p));
    }
};

//----------------------------------------------------------------------------//
// return that all specializations of this allocator are interchangeable
template <typename T1, typename T2>
bool operator ==(const simd_allocator<T1>&,
                 const simd_allocator<T2>&) throw()
{
    return true;
}
//----------------------------------------------------------------------------//
//
template <typename T1, typename T2>
bool operator !=(const simd_allocator<T1>&,
                 const simd_allocator<T2>&) throw()
{
    return false;
}
//----------------------------------------------------------------------------//

//========================================================================//
// cleaner version of:
//     double* var = static_cast<double*>(mad::aligned_alloc(
//              n * sizeof(double), mad::SIMD_WIDTH ) );

template <typename _Tp>
_Tp* simd_alloc(size_t n)
{
    return static_cast<_Tp*>(aligned_alloc(n * sizeof(_Tp),
                                           mad::SIMD_WIDTH));
}

//========================================================================//
// template class for memory-aligned c-style array with internal
// allocation and deallocation
template <typename _Tp>
class simd_array
{
public:
    typedef std::size_t size_type;

public:
    simd_array()
    : m_data(nullptr)
    { }

    simd_array(size_type _n)
    : m_data(simd_alloc<_Tp>(_n))
    { }

    simd_array(size_type _n, const _Tp& _init)
    : m_data(simd_alloc<_Tp>(_n))
    {
        for(size_type i = 0; i < _n; ++i)
            m_data[i] = _init;
    }

    ~simd_array()
    {
        aligned_free(m_data);
    }

    // conversion function to const _Tp*
    operator const _Tp*() const attrib_assume_aligned { return m_data; }
    // conversion function to _Tp*
    operator _Tp*() attrib_assume_aligned { return m_data; }

    //_Tp&        operator [](size_type i)        { return m_data[i]; }
    //const _Tp&  operator [](size_type i) const  { return m_data[i]; }

    simd_array& operator=(const simd_array& rhs)
    {
        if(this != &rhs)
        {
            if(m_data)
                aligned_free(m_data);
            m_data = static_cast<_Tp*>(rhs.m_data);
            // otherwise will be deleted
            const_cast<simd_array&>(rhs).m_data = nullptr;
        }
        return *this;
    }

private:
    _Tp* m_data;

} attrib_aligned;

//========================================================================//

//----------------------------------------------------------------------------//
}

//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//

#endif
