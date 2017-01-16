//
//
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

//----------------------------------------------------------------------------//
#ifdef SWIG
%module types
%{
    #include "types.hh"
%}
#endif
//----------------------------------------------------------------------------//

#include "allocator/allocator.hh"
#include "threading/threading.hh"
#include "threading/AutoLock.hh"

#ifdef ENABLE_THREADING
    #include "threading/atomics/atomic.hh"
#endif

#ifdef ENABLE_TBB
    #include <tbb/tbb_stddef.h>
#endif

//============================================================================//

namespace mad
{

#ifdef ENABLE_TBB
    typedef tbb::split splitter;
#else
    class splitter { };
#endif

#ifdef ENABLE_THREADING

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

    typedef AutoLock                Lock_t;

#else

    typedef short                   short_ts;
    typedef unsigned short          ushort_ts;
    typedef int                     int_ts;
    typedef unsigned                uint_ts;
    typedef unsigned                unsigned_ts;
    typedef long                    long_ts;
    typedef unsigned long           ulong_ts;

    typedef float                   float_ts;
    typedef double                  double_ts;

    typedef short                   short_type;
    typedef unsigned short          ushort_type;
    typedef int                     int_type;
    typedef unsigned                uint_type;
    typedef unsigned                unsigned_type;
    typedef long                    long_type;
    typedef unsigned long           ulong_type;

    typedef float                   float_type;
    typedef double                  double_type;

    #define type_conversion(type)   type
    #define base_conversion(type)   type

    typedef AutoLock                Lock_t;

#endif

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

    using mad::Lock_t;

#endif

//============================================================================//



#endif
