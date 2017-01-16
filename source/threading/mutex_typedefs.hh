//
//
//
//
//
// created by jrmadsen on Sun Jun 21 16:42:04 2015
//
//
//
//

//
// The primary purpose of this file is to define which library the mutexes
// are coming from. Unlike with how atomics are handled, for the most part
// the user must implement most of the locking using #ifdef _CXX11_MUTEX_ ...
// #elif _BOOST_MUTEX_ ... etc.
//
//
// This typedef is similar to atomic_typedefs.hh except it is much simpler
// There are many different flavors of mutexes (basic, shared, recursive, timed,
// etc.) and trying to implement a generic interface for all of them would be
// very difficult. Instead, the user is primarily responsible for the different
// implementations. Although some template functions are provided, e.g.:
//
// template <typename MutexType>
// void do_something(MutexType& a_mutex)
// {
//    #ifdef _CXX11_MUTEX_
//       ... do something in the way C++11 wants it ...
//	  #elif _BOOST_MUTEX_
//       ... do something in the way Boost wants it ...
//	  #elif _TBB_MUTEX_
//       ... do something in the way TBB wants it ...
//	  #endif
// }
//

#ifndef mutex_typedefs_hh_
#define mutex_typedefs_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module mutex_typedefs
%{
    #include "mutex_typedefs.hh"
%}
#endif
//----------------------------------------------------------------------------//

// mainly for the C++11/C++0x definitions
#include "atomic_typedefs.hh"

//============================================================================//
// This defines what kind of mutex library being used. Note: the definitions
// ENABLE_BOOST_MUTEX, ENABLE_TBB_MUTEX, and/or FORCE_TBB_MUTEX are custom
// definitions that must be passed to the compiler. CXX11 is automatically
// determined
#if defined(_CXX11_) && !defined(FORCE_TBB_MUTEX)
  #ifndef _CXX_MUTEX_
  #define _CXX_MUTEX_
  #endif

  #ifndef _CXX11_MUTEX_
  #define _CXX11_MUTEX_
  #endif
#elif defined(ENABLE_BOOST_MUTEX) && !defined(FORCE_TBB_MUTEX)
  #if defined(ENABLE_BOOST) || defined(ENABLE_BOOST_MUTEX)
    #include <boost/version.hpp>
  #endif
  // Boost 1.31 first version with thread library
  #if (BOOST_VERSION / 100000 > 0) && (BOOST_VERSION / 100 % 1000 > 30)
    #ifndef _BOOST_MUTEX_
    #define _BOOST_MUTEX_
    #endif
    #else
        #warning Boost mutexes are only support in Boost v1.31 or higher
        #warning Enable C++11 or define either ENABLE_TBB_MUTEX or
        #warning FORCE_TBB_MUTEX
    #endif
#elif defined(ENABLE_TBB_MUTEX) || defined(FORCE_TBB_MUTEX)
  #ifndef _TBB_MUTEX_
  #define _TBB_MUTEX_
  #endif
  #error Mutexes are not supported! Please enable C++11, on include the \
    Boost thread library (and define ENABLE_BOOST_MUTEX) or include the \
    TBB library (and define ENABLE_TBB_MUTEX)
#endif

//----------------------------------------------------------------------------//

#ifdef _CXX11_MUTEX_
    #include <mutex>

    namespace base_mutex
    {
        class mutex : public std::mutex
        {
            friend class std::mutex;
        };
        class recursive_mutex : public std::recursive_mutex
        {
            friend class std::recursive_mutex;
        };
    }
#elif defined(_BOOST_MUTEX_)
    #include <boost/thread/mutex.hpp>
    #include <boost/thread/recursive_mutex.hpp>

    namespace base_mutex
    {
        class mutex : public boost::sync::mutex
        {
            friend class boost::sync::mutex;
        };
        class recursive_mutex : public boost::sync::recursive_mutex
        {
            friend class boost::sync::recursive_mutex;
        };
    }
#elif defined(_TBB_MUTEX_)
    #include <tbb/mutex.h>
    #include <tbb/recursive_mutex.h>

    namespace base_mutex
    {
        class mutex : public tbb::mutex
        {
            friend class tbb::mutex;
        };
        class recursive_mutex : public tbb::recursive_mutex
        {
            friend class tbb::recursive_mutex;
        };
    }
#endif

//----------------------------------------------------------------------------//


#endif
