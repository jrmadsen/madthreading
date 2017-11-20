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

    typedef atomic<short>           short_ts;
    typedef atomic<unsigned short>  ushort_ts;
    typedef atomic<int>             int_ts;
    typedef atomic<unsigned>        uint_ts;
    typedef atomic<unsigned>        unsigned_ts;
    typedef atomic<long>            long_ts;
    typedef atomic<unsigned long>   ulong_ts;

    typedef atomic<float>           float_ts;
    typedef atomic<double>          double_ts;

    typedef short                   short_type;
    typedef unsigned short          ushort_type;
    typedef int                     int_type;
    typedef unsigned                uint_type;
    typedef unsigned                unsigned_type;
    typedef long                    long_type;
    typedef unsigned long           ulong_type;

    typedef float                   float_type;
    typedef double                  double_type;

    #define type_conversion(type)   atomic<type>
    #define base_conversion(type)   typename type::value_type

    typedef auto_lock                lock_t;

} // namespace mad

//============================================================================//

#ifdef USE_TYPES_IN_GLOBAL_NAMESPACE

    using mad::short_ts;
    using mad::ushort_ts;
    using mad::int_ts;
    using mad::uint_ts;
    using mad::unsigned_ts;
    using mad::long_ts;
    using mad::ulong_ts;

    using mad::float_ts;
    using mad::double_ts;

    using mad::short_type;
    using mad::ushort_type;
    using mad::int_type;
    using mad::uint_type;
    using mad::unsigned_type;
    using mad::long_type;
    using mad::ulong_type;

    using mad::float_type;
    using mad::double_type;

    using mad::lock_t;

#endif

//============================================================================//



#endif
