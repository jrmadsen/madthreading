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
// created by jmadsen on Wed Aug 27 15:30:21 2014
//
//
//
//


#ifndef atomic_hh_
#define atomic_hh_

#ifdef SWIG
#   ifdef USE_BOOST_SERIALIZATION
#       undef USE_BOOST_SERIALIZATION
#   endif
#endif

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic
%{
    #define SWIG_FILE_WITH_INIT
    #include "atomic_typedefs.hh"
    #if defined(_MUTEXED_POD_ATOMICS_)
    #   include "mutex.hh"
    #endif
    #include "atomic.hh"
%}

#include "atomic_typedefs.hh"

%include "atomic.hh"
#endif
//----------------------------------------------------------------------------//

#include "atomic_typedefs.hh"

#if defined(_MUTEXED_POD_ATOMICS_)
#   include "threading/mutex.hh"
#endif

#if defined(USE_BOOST_SERIALIZATION)
#   include <boost/serialization/split_member.hpp>
#   include <boost/serialization/version.hpp>
#   include <boost/serialization/access.hpp>
#   include <boost/serialization/serialization.hpp>
#endif

#if defined(_HAS_ATOMICS_)

//----------------------------------------------------------------------------//

namespace mad
{

template<typename _Tp>
class atomic
{
public:
    typedef typename base_atomic::atomic<_Tp>   Base_t;
    typedef _Tp                                 value_type;
    typedef mad::atomic<_Tp>                    this_type;

public:
    atomic() { atomics::set(&_value, value_type()); }
    atomic(const value_type& _init) { atomics::set(&_value, _init); }
    atomic(const this_type& rhs) { atomics::set(&_value, rhs.base()); }
    atomic(const Base_t& rhs) { atomics::set(&_value, rhs); }

    this_type& operator=(const this_type& rhs)
    {
        if(this != &rhs)
            atomics::set(&_value, rhs._value);
        return *this;
    }

    this_type& operator=(const value_type& rhs)
    {
#if defined(_MUTEXED_POD_ATOMICS_)
        static mad::mutex mutex;
        auto_lock lock(&mutex);
        _value = rhs;
#else
        atomics::set(&_value, rhs);
#endif
        return *this;
    }

    this_type& operator=(const Base_t& rhs)
    {
        atomics::set(&_value, rhs);
        return *this;
    }

    ~atomic() { _value.~Base_t(); }

    Base_t& base() { return _value; }
    const Base_t& base() const { return _value; }
    Base_t& base() volatile { return _value; }
    const Base_t& base() const volatile { return _value; }

    bool is_lock_free() const { return _value.is_lock_free(); }
    bool is_lock_free() const volatile { return _value.is_lock_free(); }

    void store(_Tp _desired, base_atomic::memory_order mem_odr
               = base_atomic::memory_order_seq_cst)
    { atomics::set(_value, _desired, mem_odr); }
    void store(_Tp _desired, base_atomic::memory_order mem_odr
               = base_atomic::memory_order_seq_cst) volatile
    { atomics::set(_value, _desired, mem_odr); }

    _Tp load(base_atomic::memory_order mem_odr
             = base_atomic::memory_order_seq_cst) const
    { return atomics::get(_value, mem_odr); }
    _Tp load(base_atomic::memory_order mem_odr
             = base_atomic::memory_order_seq_cst) const volatile
    { return atomics::get(_value, mem_odr); }

    operator _Tp() const { return this->load(); }
    operator _Tp() const volatile { return this->load(); }

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               base_atomic::memory_order _success,
                               base_atomic::memory_order _failure)
    { return _value.compare_exchange_weak(_expected, _desired,
                                          _success, _failure); }
    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               base_atomic::memory_order _success,
                               base_atomic::memory_order _failure) volatile
    { return _value.compare_exchange_weak(_expected, _desired,
                                          _success, _failure); }

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               base_atomic::memory_order _order)
    { return _value.compare_exchange_weak(_expected, _desired, _order); }
    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               base_atomic::memory_order _order) volatile
    { return _value.compare_exchange_weak(_expected, _desired, _order); }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 base_atomic::memory_order _success,
                                 base_atomic::memory_order _failure)
    { return _value.compare_exchange_weak(_expected, _desired,
                                          _success, _failure); }
    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 base_atomic::memory_order _success,
                                 base_atomic::memory_order _failure) volatile
    { return _value.compare_exchange_weak(_expected, _desired,
                                          _success, _failure); }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 base_atomic::memory_order _order)
    { return _value.compare_exchange_weak(_expected, _desired, _order); }
    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 base_atomic::memory_order _order) volatile
    { return _value.compare_exchange_weak(_expected, _desired, _order); }

    // value_type operators
    this_type& operator+=(const value_type& rhs)
    { atomics::increment(&_value, rhs); return *this; }
    this_type& operator-=(const value_type& rhs)
    { atomics::decrement(&_value, rhs); return *this; }
    this_type& operator*=(const value_type& rhs)
    { atomics::multiply(&_value, rhs); return *this; }
    this_type& operator/=(const value_type& rhs)
    { atomics::divide(&_value, rhs); return *this; }

    // atomic operators
    this_type& operator+=(const this_type& rhs)
    { atomics::increment(&_value, rhs._value); return *this; }
    this_type& operator-=(const this_type& rhs)
    { atomics::decrement(&_value, rhs._value); return *this; }
    this_type& operator*=(const this_type& rhs)
    { atomics::multiply(&_value, rhs._value); return *this; }
    this_type& operator/=(const this_type& rhs)
    { atomics::divide(&_value, rhs._value); return *this; }

    this_type& operator+=(const atomic& rhs) volatile
    { atomics::increment(&_value, rhs._value); return *this; }
    this_type& operator-=(const atomic& rhs) volatile
    { atomics::decrement(&_value, rhs._value); return *this; }
    this_type& operator*=(const atomic& rhs) volatile
    { atomics::multiply(&_value, rhs._value); return *this; }
    this_type& operator/=(const atomic& rhs) volatile
    { atomics::divide(&_value, rhs._value); return *this; }

    // base atomic operators
    this_type& operator+=(const base_atomic::atomic<_Tp>& rhs)
    { atomics::increment(&_value, rhs); return *this; }
    this_type& operator-=(const base_atomic::atomic<_Tp>& rhs)
    { atomics::decrement(&_value, rhs); return *this; }
    this_type& operator*=(const base_atomic::atomic<_Tp>& rhs)
    { atomics::multiply(&_value, rhs); return *this; }
    this_type& operator/=(const base_atomic::atomic<_Tp>& rhs)
    { atomics::divide(&_value, rhs); return *this; }

    this_type& operator+=(const base_atomic::atomic<_Tp>& rhs) volatile
    { atomics::increment(&_value, rhs); return *this; }
    this_type& operator-=(const base_atomic::atomic<_Tp>& rhs) volatile
    { atomics::decrement(&_value, rhs); return *this; }
    this_type& operator*=(const base_atomic::atomic<_Tp>& rhs) volatile
    { atomics::multiply(&_value, rhs); return *this; }
    this_type& operator/=(const base_atomic::atomic<_Tp>& rhs) volatile
    { atomics::divide(&_value, rhs); return *this; }

    // increment operators
    value_type operator++() { value_type _tmp = ++_value; return _tmp; }
    value_type operator++(int)
    { value_type _tmp = _value++; return _tmp; }

    value_type operator--() { value_type _tmp = --_value; return _tmp; }
    value_type operator--(int)
    { value_type _tmp = _value--; return _tmp; }

protected:
    Base_t _value;

#if defined(USE_BOOST_SERIALIZATION)
private:
    //------------------------------------------------------------------------//
    friend class boost::serialization::access;
    //------------------------------------------------------------------------//
    template <typename Archive>
    void save(Archive& ar, const unsigned int /*version*/) const
    {
        value_type _val = this->load();
        ar << _val;
    }
    //------------------------------------------------------------------------//
    template <typename Archive>
    void load(Archive& ar, const unsigned int /*version*/)
    {
        value_type _val = 0;
        ar >> _val;
        store(_val);
    }
    //------------------------------------------------------------------------//
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int file_version)
    {
        ar.template register_type<_Tp>();
        boost::serialization::split_member(ar, *this, file_version);
    }
    //------------------------------------------------------------------------//
#endif
};

#ifdef SWIG

%template(atomic_s)     mad::atomic<short>;
%template(atomic_i)     mad::atomic<int>;
%template(atomic_l)     mad::atomic<long>;
%template(atomic_u)     mad::atomic<unsigned>;
%template(atomic_us)    mad::atomic<unsigned short>;
%template(atomic_ui)    mad::atomic<unsigned int>;
%template(atomic_ul)    mad::atomic<unsigned long>;
%template(atomic_f)     mad::atomic<float>;
%template(atomic_d)     mad::atomic<double>;
%template(atomic_ld)    mad::atomic<long double>;

#endif

namespace atomics
{
    using namespace mem_order;
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(atomic<T>* _atomic,
                    const T& _desired,
                    memory_order mem_odr_1 = base_atomic::memory_order_seq_cst,
                    memory_order mem_odr_2 = base_atomic::memory_order_relaxed)
    {
        //_atomic->fetch_and_store(_desired);
        details::do_compare_and_swap(&_atomic->base(), _desired,
                                     details::set2nd<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(atomic<T>& _atomic,
                    const T& _desired,
                    memory_order mem_odr_1 = base_atomic::memory_order_seq_cst,
                    memory_order mem_odr_2 = base_atomic::memory_order_relaxed)
    {
        set(&_atomic, _desired, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<atomic<T>, atomic<U> >* _atomic,
                    const std::pair<T, U>& _desired,
                    memory_order mem_odr_1 = base_atomic::memory_order_seq_cst,
                    memory_order mem_odr_2 = base_atomic::memory_order_relaxed)
    {
        set(&(_atomic->first), _desired.first, mem_odr_1, mem_odr_2);
        set(&(_atomic->second), _desired.second, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void increment(atomic<T>* _atomic,
                          const T& _increment,
                          memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&_atomic->base(), _increment,
                                     details::plus<T>(), mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void decrement(atomic<T>* _atomic,
                          const T& _decrement,
                          memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&_atomic->base(), _decrement,
                                     details::minus<T>(), mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void multiply(atomic<T>* _atomic,
                         const T& _factor,
                         memory_order mem_odr_1
                         = MO<T>::default_mem_order,
                         memory_order mem_odr_2
                         = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&_atomic->base(), _factor,
                                     details::multiplies<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void divide(atomic<T>* _atomic,
                       const T& _factor,
                       memory_order mem_odr_1 = MO<T>::default_mem_order,
                       memory_order mem_odr_2
                       = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&_atomic->base(), _factor,
                                     details::divides<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(atomic<T>* _atomic,
                    const atomic<T>& _atomic_desired,
                    memory_order mem_odr_1 = base_atomic::memory_order_seq_cst,
                    memory_order mem_odr_2 = base_atomic::memory_order_relaxed)
    {
        details::do_compare_and_swap(&_atomic->base(), _atomic_desired.base(),
                                     details::set2nd<T>(),
                                     mem_odr_1, mem_odr_2);
      //_atomic->fetch_and_store(_desired);
    }
    //------------------------------------------------------------------------//

    template <typename T>
    inline void set(atomic<T>& _atomic,
                    const atomic<T>& _atomic_desired,
                    memory_order mem_odr_1 = base_atomic::memory_order_seq_cst,
                    memory_order mem_odr_2 = base_atomic::memory_order_relaxed)
    {
        set(&_atomic, _atomic_desired, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<atomic<T>, atomic<U> >* _atomic,
                    const std::pair<atomic<T>, atomic<U> >& _desired,
                    memory_order mem_odr_1 = base_atomic::memory_order_seq_cst,
                    memory_order mem_odr_2 = base_atomic::memory_order_relaxed)
    {
        set(&(_atomic->first), _desired.first, mem_odr_1, mem_odr_2);
        set(&(_atomic->second), _desired.second, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void increment(atomic<T>* _atomic,
                          const atomic<T>& _atomic_increment,
                          memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&(_atomic->base()),
                                     _atomic_increment.base(),
                                     details::plus<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void decrement(atomic<T>* _atomic,
                          const atomic<T>& _atomic_decrement,
                          memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&_atomic->base(),
                                     _atomic_decrement.base(),
                                     details::minus<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void multiply(atomic<T>* _atomic,
                         const atomic<T>& _atomic_factor,
                         memory_order mem_odr_1
                         = MO<T>::default_mem_order,
                         memory_order mem_odr_2
                         = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&_atomic->base(),
                                     _atomic_factor.base(),
                                     details::multiplies<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void divide(atomic<T>* _atomic,
                       const atomic<T>& _atomic_factor,
                       memory_order mem_odr_1 = MO<T>::default_mem_order,
                       memory_order mem_odr_2
                       = MO<T>::secondary_mem_order)
    {
      details::do_compare_and_swap(&_atomic->base(),
                                     _atomic_factor.base(),
                                     details::divides<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//

} // namespace atomics

} // namespace mad

#else // defined(_HAS_ATOMICS_)

namespace mad
{

    #include "mutexed_pod.hh"
    #ifdef ENABLE_THREADING

        template<typename _Tp>
        class atomic : public mutexed_pod<_Tp>
        { };

    #else // ENABLE_THREADING
        // <_Tp, int, int> : int, int template parameters make mutexes/locks
        // non-existant
        class atomic : public mutexed_pod<_Tp, int, int>
        { };

    #endif // ENABLE_THREADING

} // namespace mad

#endif // defined(_HAS_ATOMICS_)

//----------------------------------------------------------------------------//

#endif
