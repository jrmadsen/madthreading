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
// created by jrmadsen on Tue Jun 10 14:43:08 2014
//
//
//
//

#ifndef atomic_typedefs_hh_
#define atomic_typedefs_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic_typedefs
%{
    #include "atomic_typedefs.hh"
%}
#endif
//----------------------------------------------------------------------------//

//============================================================================//

// easier definitions for C++11
#if __cplusplus > 199711L || __cplusplus >= 201103L // C++11
  #ifndef CXX11
  #define CXX11
  #endif

  #ifndef cpp11
  #define cpp11
  #endif

  #ifndef _CXX11_
  #define _CXX11_
  #endif

  #ifndef _cpp11_
  #define _cpp11_
  #endif
#endif


// easier definitions for C++0x
#if __cplusplus >= 199711L // C++0x
  #ifndef CXX0X
  #define CXX0X
  #endif

  #ifndef cpp0x
  #define cpp0x
  #endif

  #ifndef _CXX0X_
  #define _CXX0X_
  #endif

  #ifndef _cpp0x_
  #define _cpp0x_
  #endif
#endif

//============================================================================//

#include <functional>
#include "../threading/threading.hh"
#define _ATTRIB_(attrib) __attribute__((attrib))

#ifndef __inline__
#   ifdef SWIG
#       define __inline__ inline
#   else
#       define __inline__ inline __attribute__ ((always_inline))
#   endif
#endif

//============================================================================//
// This defines what kind of atomic library being used. Note: the definitions
// USE_BOOST_ATOMICS, FORCE_BOOST_ATOMICS, and/or FORCE_MUTEX_POD are
// custom definitions that must be passed to the compiler. CXX11 is
// automatically determined
#if defined(_CXX11_) && !defined(FORCE_BOOST_ATOMICS) && !defined(FORCE_MUTEX_POD)
  #ifndef _CXX_ATOMICS_
  #define _CXX_ATOMICS_
  #endif

  #ifndef _CXX11_ATOMICS_
  #define _CXX11_ATOMICS_
  #endif
#elif (defined(USE_BOOST_ATOMICS) || defined(FORCE_BOOST_ATOMICS)) && \
      !defined(FORCE_MUTEX_POD)
  #ifdef USE_BOOST_ATOMICS
    #include <boost/version.hpp>
  #endif
  // Boost 1.53 first version with atomics
  #if (BOOST_VERSION / 100000 > 0) && (BOOST_VERSION / 100 % 1000 > 52)
    #ifndef _BOOST_ATOMICS_
    #define _BOOST_ATOMICS_
    #endif
  #else
    #warning Boost Atomics are only support in Boost v1.53 or higher
    #warning Enable C++11 or define FORCE_MUTEX_POD
    #warning Current Boost version is: BOOST_VERSION
  #endif
#else
    #ifndef _MUTEXED_POD_ATOMICS_
    #define _MUTEXED_POD_ATOMICS_
    #endif
//    #warning ATOMICS NOT SUPPORTED! ONLY SUPPORT FOR C++11, Boost, TBB atomics
//    #warning To enable, compile with C++11 or definitions:
//    #warning     USE_BOOST
//    #warning Order of atomic selection is C++11 > Boost
#endif

//============================================================================//

#if defined(_CXX11_ATOMICS_)
    #include <atomic>
#elif defined(_BOOST_ATOMICS_)
    #include <boost/atomic.hpp>
#endif

//============================================================================//

//============================================================================//
// We want to use the STL atomics if first and foremost (portability)
// If not C++11, use Boost atomics since C++11 atomics are essentially
// equivalent
// If not C++11 or Boost, mutexed PODs are used
#if defined(_CXX11_ATOMICS_)
    namespace base_atomic = std;
#elif defined(_BOOST_ATOMICS_)
    namespace base_atomic = boost;
#else
    #include "mutexed_pod.hh"

    namespace mad
    {
    namespace base_atomic
    {
        template <typename _Tp>
        class atomic : public mutexed_pod<_Tp>
        {
        public:
            typedef typename mutexed_pod<_Tp>::Mutex_t Mutex_t;
            typedef typename mutexed_pod<_Tp>::Lock_t Lock_t;
            typedef mutexed_pod<_Tp> Base_t;
            typedef _Tp value_type;

        public:
            atomic() : Base_t() { }
            atomic(const _Tp& val) : Base_t(val) { }
            atomic(const Base_t& val) : Base_t(val) { }
        };
        // memory order defined in mutexed_pod.hh
    }
    namespace base_atomic
    {
        enum memory_order
        {
            memory_order_relaxed,
            memory_order_consume,
            memory_order_acquire,
            memory_order_release,
            memory_order_acq_rel,
            memory_order_seq_cst
        };

    }
    } // namespace mad
#endif

//============================================================================//

#if defined(_CXX_ATOMICS_) || defined(_BOOST_ATOMICS_) ||\
    defined(_MUTEXED_POD_ATOMICS_)
  #ifndef _HAS_ATOMICS_
  #define _HAS_ATOMICS_
  #endif
#endif

//============================================================================//

namespace mad
{

#if defined(_HAS_ATOMICS_)

namespace atomics
{
    //------------------------------------------------------------------------//
    namespace details
    {
        //--------------------------------------------------------------------//
        // The following structs are used for the operator's in
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct base_binary : public std::binary_function<_Tp, _Tp, _Tp>
        {
            virtual ~base_binary() { }

            typedef typename std::binary_function<_Tp, _Tp, _Tp>
            ::result_type result_type;
            typedef typename std::binary_function<_Tp, _Tp, _Tp>
            ::first_argument_type first_argument_type;
            typedef typename std::binary_function<_Tp, _Tp, _Tp>
            ::second_argument_type second_argument_type;

            virtual
            result_type operator() (first_argument_type,
                                    second_argument_type) const = 0;
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct set1st : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            __inline__
            result_type operator()(first_argument_type _val1,
                                   second_argument_type) const
            { return _val1; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct set2nd : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            __inline__
            result_type operator()(first_argument_type,
                                   second_argument_type _val2) const
            { return _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct plus : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            __inline__
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 + _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct minus : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            __inline__
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 - _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct multiplies : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            __inline__
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 * _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct divides : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            __inline__
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 / _val2; }
        };
        //--------------------------------------------------------------------//
    }
    //------------------------------------------------------------------------//

    namespace mem_order
    {
        using base_atomic::memory_order;
    }

    // MO = MemoryOrder
    template <typename _Tp>
    class MO
    {
    public:
        static base_atomic::memory_order default_mem_order;
        static base_atomic::memory_order secondary_mem_order;
        static void SetDefault(base_atomic::memory_order);
        static void SetSecondary(base_atomic::memory_order);

    };

    template <typename _Tp>
    base_atomic::memory_order MO<_Tp>::default_mem_order
    = base_atomic::memory_order_acq_rel;

    template <typename _Tp>
    base_atomic::memory_order MO<_Tp>::secondary_mem_order
    = base_atomic::memory_order_relaxed;

    template <typename _Tp>
    void MO<_Tp>::SetDefault(base_atomic::memory_order _odr)
    { default_mem_order = _odr; }

    template <typename _Tp>
    void MO<_Tp>::SetSecondary(base_atomic::memory_order _odr)
    { secondary_mem_order = _odr; }

#ifdef CXX11
    template <typename _Tp> using MemoryOrder = MO<_Tp>;
#endif

    //------------------------------------------------------------------------//
    namespace details
    {
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_fetch_and_store(base_atomic::atomic<_Tp>* _atomic,
                                       const _Tp& _value,
                                       base_atomic::memory_order mem_odr
                                       = base_atomic::memory_order_seq_cst)
        {
            _atomic->store(_value, mem_odr);
        }
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_fetch_and_store(base_atomic::atomic<_Tp>* _atomic,
                                       const base_atomic::atomic<_Tp>& _value,
                                       base_atomic::memory_order mem_odr
                                       = base_atomic::memory_order_seq_cst)
        {
            _atomic->store(_value.load(), mem_odr);
        }
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_compare_and_swap(base_atomic::atomic<_Tp>* _atomic,
                          const _Tp& _value,
                          const base_binary<_Tp>& _operator,
                          base_atomic::memory_order mem_odr_1,
                          base_atomic::memory_order mem_odr_2)
        {
              /*bool performed = false;
              _Tp _expected = _Tp();
              do {
                  _expected = _atomic->load();
                  performed
                  = _atomic->compare_exchange_weak(_expected,
                                                   _operator(_expected, _value),
                                                   mem_odr_1,
                                                   mem_odr_2);

              } while (!performed);*/
            _Tp _expected = _Tp();
            do {
                _expected = _atomic->load();
            } while (!(_atomic->compare_exchange_weak(_expected,
                                                      _operator(_expected,
                                                                _value),
                                                      mem_odr_1,
                                                      mem_odr_2)));
        }
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_compare_and_swap(base_atomic::atomic<_Tp>* _atomic,
                        const base_atomic::atomic<_Tp>&
                                            _atomic_value,
                        const base_binary<_Tp>& _operator,
                        base_atomic::memory_order mem_odr_1,
                        base_atomic::memory_order mem_odr_2)
        {
            _Tp _expected = _Tp();
            do {
                _expected = _atomic->load();
            } while (!(_atomic->compare_exchange_weak(_expected,
                                                      _operator(_expected,
                                                                _atomic_value.load()),
                                                      mem_odr_1,
                                                      mem_odr_2)));
        }
        //--------------------------------------------------------------------//
    }
    //------------------------------------------------------------------------//
    //  WITH ATOMIC TEMPLATE BASE TYPE AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>* _atomic, const T& _desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _desired, details::set2nd<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>& _atomic, const T& _desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        set(&_atomic, _desired, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void increment(base_atomic::atomic<T>* _atomic, const T& _increment,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _increment, details::plus<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void decrement(base_atomic::atomic<T>* _atomic, const T& _decrement,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _decrement, details::minus<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void multiply(base_atomic::atomic<T>* _atomic, const T& _factor,
                         base_atomic::memory_order mem_odr_1
                         = MO<T>::default_mem_order,
                         base_atomic::memory_order mem_odr_2
                         = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _factor, details::multiplies<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void divide(base_atomic::atomic<T>* _atomic, const T& _factor,
                       base_atomic::memory_order mem_odr_1
                       = MO<T>::default_mem_order,
                       base_atomic::memory_order mem_odr_2
                       = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _factor, details::divides<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    //  WITH ATOMICS AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>* _atomic,
                    const base_atomic::atomic<T>& _atomic_desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        //details::do_fetch_and_store(_atomic, _desired);
        details::do_compare_and_swap(_atomic, _atomic_desired,
                                     details::set2nd<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>& _atomic,
                    const base_atomic::atomic<T>& _atomic_desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        set(&_atomic, _atomic_desired, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void increment(base_atomic::atomic<T>* _atomic,
                          const base_atomic::atomic<T>& _atomic_increment,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_increment,
                                     details::plus<T>(), mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void decrement(base_atomic::atomic<T>* _atomic,
                          const base_atomic::atomic<T>& _atomic_decrement,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_decrement,
                                     details::minus<T>(), mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void multiply(base_atomic::atomic<T>* _atomic,
                         const base_atomic::atomic<T>& _atomic_factor,
                         base_atomic::memory_order mem_odr_1
                         = MO<T>::default_mem_order,
                         base_atomic::memory_order mem_odr_2
                         = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_factor,
                                     details::multiplies<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void divide(base_atomic::atomic<T>* _atomic,
                       const base_atomic::atomic<T>& _atomic_factor,
                       base_atomic::memory_order mem_odr_1
                       = MO<T>::default_mem_order,
                       base_atomic::memory_order mem_odr_2
                       = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_factor,
                                     details::divides<T>(),
                                     mem_odr_1, mem_odr_2);
    }

    //------------------------------------------------------------------------//
    //               STANDARD OVERLOAD                //
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(T* _non_atomic, const T& _desired)
    {
      *_non_atomic = _desired;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(T& _non_atomic, const T& _desired)
    {
      set(&_non_atomic, _desired);
    }
    //------------------------------------------------------------------------//
    //               STL PAIR OVERLOAD                                        //
    //------------------------------------------------------------------------//
    //
    //------------------------------------------------------------------------//
    //  WITH ATOMIC TEMPLATE TYPE AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >* _atomic,
                    const std::pair<T, U>& _desired)
    {
        set(&_atomic->first, _desired.first);
        set(&_atomic->second, _desired.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >& _atomic,
                    const std::pair<T, U>& _desired)
    {
        set(&_atomic, _desired);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void increment(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<T, U>& _increment)
    {
        increment(&_atomic->first, _increment.first);
        increment(&_atomic->second, _increment.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void decrement(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<T, U>& _decrement)
    {
        decrement(&_atomic->first, _decrement.first);
        decrement(&_atomic->second, _decrement.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void multiply(std::pair<base_atomic::atomic<T>,
                                   base_atomic::atomic<U> >* _atomic,
                         const std::pair<T, U>& _factor)
    {
        multiply(&_atomic->first, _factor.first);
        multiply(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void divide(std::pair<base_atomic::atomic<T>,
                                 base_atomic::atomic<U> >* _atomic,
                       const std::pair<T, U>& _factor)
    {
        divide(&_atomic->first, _factor.first);
        divide(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//
    //  WITH ATOMICS AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >* _atomic,
                    const std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >& _desired)
    {
        set(&_atomic->first, _desired.first);
        set(&_atomic->second, _desired.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >& _atomic,
                    const std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >& _desired)
    {
        set(&_atomic, _desired);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void increment(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<base_atomic::atomic<T>,
                                          base_atomic::atomic<U> >& _increment)
    {
        increment(&_atomic->first, _increment.first);
        increment(&_atomic->second, _increment.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void decrement(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<base_atomic::atomic<T>,
                                          base_atomic::atomic<U> >& _decrement)
    {
        decrement(&_atomic->first, _decrement.first);
        decrement(&_atomic->second, _decrement.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void multiply(std::pair<base_atomic::atomic<T>,
                                   base_atomic::atomic<U> >* _atomic,
                         const std::pair<base_atomic::atomic<T>,
                                         base_atomic::atomic<U> >& _factor)
    {
        multiply(&_atomic->first, _factor.first);
        multiply(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void divide(std::pair<base_atomic::atomic<T>,
                                 base_atomic::atomic<U> >* _atomic,
                       const std::pair<base_atomic::atomic<T>,
                                       base_atomic::atomic<U> >& _factor)
    {
        divide(&_atomic->first, _factor.first);
        divide(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//


    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const T& _non_atomic)
    {
        return _non_atomic;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const T& _non_atomic, base_atomic::memory_order)
    {
        return _non_atomic;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const base_atomic::atomic<T>& _atomic)
    {
        return _atomic.load();
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const base_atomic::atomic<T>& _atomic,
                 base_atomic::memory_order mem_odr)
    {
        return _atomic.load(mem_odr);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline std::pair<T, U> get(const std::pair<base_atomic::atomic<T>,
                               base_atomic::atomic<U> >& _atomic)
    {
        return std::pair<T, U>(get(_atomic.first), get(_atomic.second));
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline std::pair<T, U> get(const std::pair<base_atomic::atomic<T>,
                               base_atomic::atomic<U> >& _atomic,
                               base_atomic::memory_order mem_odr)
    {
        return std::pair<T, U>(get(_atomic.first, mem_odr),
                               get(_atomic.second, mem_odr));
    }
    //------------------------------------------------------------------------//



    //------------------------------------------------------------------------//
    // for plain old data (POD) and pairs (e.g. std::pair<atomic<T>, atomic<U> >)
    template <typename _Tp_base, typename _Tp_atom>
    inline _Tp_base base(const _Tp_atom& _atomic)
    {
        return get(_atomic);
    }
    //------------------------------------------------------------------------//

}

#else
// so that codes using atomics::get and atomics::set work without
// atomics being available. Other operators +=, -=, *=, /= should be fine
namespace atomics
{
    //------------------------------------------------------------------------//
    //        SET OVERLOADS
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(T* _non_atomic, const T& _desired)
    {
        *_non_atomic = _desired;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(T& _non_atomic, const T& _desired)
    {
        set(&_non_atomic, _desired);
    }
    //------------------------------------------------------------------------//
    //         GET OVERLOADS
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const T& _non_atomic)
    {
        return _non_atomic;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const T* _non_atomic)
    {
        return get(*_non_atomic);
    }
    //------------------------------------------------------------------------//
}
#endif // defined(_HAS_ATOMICS_)
//============================================================================//

} // namespace mad

//============================================================================//

#endif  // atomic_typedefs_hh_
