//
//
//
//
//
// created by jrmadsen on Sun Jun 21 17:40:00 2015
//
//
//
//


#ifndef tls_hh_
#define tls_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module tls
%{
    #include "tls.hh"
%}

%include "tls.hh"
#endif
//----------------------------------------------------------------------------//

// Defines ThreadLocal and ThreadLocalStatic for thread local storage
// prefix the declaration with these, e.g.:
//      ThreadLocalStatic int this_threads_counter;

#if __cplusplus > 199711L || __cplusplus >= 201103L // C++11
#   ifndef USE_STD11
#       define USE_STD11
#   endif
#endif


#if defined (ENABLE_THREADING)
    #if ( defined(__MACH__) && defined(__clang__) && defined(__x86_64__) ) || \
        ( defined(__linux__) && defined(__clang__) )
        #if (defined (USE_STD11) && __has_feature(cxx_thread_local))
            #define ThreadLocalStatic static thread_local
            #define ThreadLocal thread_local
        #else
            #define ThreadLocalStatic static __thread
            #define ThreadLocal __thread
        #endif
    #elif ( (defined(__linux__) || defined(__MACH__)) && \
            defined(__GNUC__) && ((__GNUC__==4 && __GNUC_MINOR__<9) || (__GNUC__ > 4)) && \
            defined(__INTEL_COMPILER) )
        #if defined (USE_STD11)
            #define ThreadLocalStatic static __thread
            #if (__INTEL_COMPILER < 1500)
                #define ThreadLocal __thread
            #else
                #define ThreadLocal thread_local
            #endif
        #else
            #define ThreadLocalStatic static __thread
            #define ThreadLocal __thread
        #endif
    #elif ( (defined(__linux__) || defined(__MACH__)) && \
            defined(__GNUC__) && ((__GNUC__==4 && __GNUC_MINOR__<9) || (__GNUC__ > 4)) )
        #if defined (USE_STD11)
            #define ThreadLocalStatic static thread_local
            #define ThreadLocal thread_local
        #else
            #define ThreadLocalStatic static __thread
            #define ThreadLocal __thread
        #endif
    #elif ( (defined(__linux__) || defined(__MACH__)) && \
            defined(__INTEL_COMPILER) )
        #if (defined (USE_STD11) && __INTEL_COMPILER>=1500)
            #define ThreadLocalStatic static thread_local
            #define ThreadLocal thread_local
        #else
            #define ThreadLocalStatic static __thread
            #define ThreadLocal __thread
        #endif
    #elif defined(_AIX)
        #if defined (USE_STD11)
            #define ThreadLocalStatic static thread_local
            #define ThreadLocal thread_local
        #else
            #define ThreadLocalStatic static __thread
            #define ThreadLocal __thread
        #endif
    #elif defined(WIN32)
        #define ThreadLocalStatic static __declspec(thread)
        #define ThreadLocal __declspec(thread)
    #elif defined(__IBMC__) || defined(__IBMCPP__)
        #define ThreadLocalStatic static __thread
        #define ThreadLocal __thread
    #else
        #error "No Thread Local Storage (TLS) technology supported for this platform. Use sequential build !"
    #endif
#else
    #define ThreadLocalStatic static
    #define ThreadLocal
#endif // defined(ENABLE_THREADING)

#endif // defined(tls_hh_)
