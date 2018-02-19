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
// created by jrmadsen on Mon Jul 27 16:42:05 2015
//
//
//
//


#ifndef types_hh_
#define types_hh_

#include <cstdint>

#include "madthreading/threading/threading.hh"
#include "madthreading/allocator/allocator.hh"
#include "madthreading/threading/auto_lock.hh"
#include "madthreading/atomics/atomic.hh"

#ifdef USE_TBB
    #include <tbb/tbb_stddef.h>
#endif

//============================================================================//

namespace mad
{

#ifdef USE_TBB
    typedef tbb::split splitter;
#else
    class splitter { };
#endif

    typedef atomic<int16_t>         atomic_short_t;
    typedef atomic<uint16_t>        atomic_ushort_t;
    typedef atomic<int32_t>         atomic_int_t;
    typedef atomic<uint32_t>        atomic_uint_t;
    typedef atomic<int64_t>         atomic_long_t;
    typedef atomic<uint64_t>        atomic_ulong_t;

    typedef atomic<float>           atomic_float_t;
    typedef atomic<double>          atomic_double_t;

    typedef int16_t                 short_t;
    typedef uint16_t                ushort_t;
    typedef int32_t                 int_t;
    typedef uint32_t                uint_t;
    typedef int64_t                 long_t;
    typedef uint64_t                ulong_t;

    typedef float                   float_t;
    typedef double                  double_t;

    #define type_conversion(type)   atomic<type>
    #define base_conversion(type)   typename type::value_type

    typedef auto_lock                lock_t;

} // namespace mad

//============================================================================//



#endif
