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
// created by jmadsen on Wed Aug 27 15:30:21 2014
//
//
//
//


#ifndef omp_atomic_hh_
#define omp_atomic_hh_

#ifdef SWIG
#   ifdef USE_BOOST_SERIALIZATION
#       undef USE_BOOST_SERIALIZATION
#   endif
#endif

//----------------------------------------------------------------------------//
#ifdef SWIG
%module omp_atomic
%{
    #define SWIG_FILE_WITH_INIT
    #include <omp.h>
    #include "omp_atomic.hh"
%}

%include "omp_atomic.hh"
#endif
//----------------------------------------------------------------------------//

#ifdef USE_OPENMP

#include <omp.h>

#if defined(USE_BOOST_SERIALIZATION)
#   include <boost/serialization/split_member.hpp>
#   include <boost/serialization/version.hpp>
#   include <boost/serialization/serialization.hpp>
#   include <boost/serialization/access.hpp>
#endif

//----------------------------------------------------------------------------//

namespace mad
{


template<typename _Tp>
class omp_atomic
{
public:
    typedef _Tp                         Base_t;
    typedef _Tp                         value_type;

public:
    omp_atomic()
    : _value(_Tp())
    { }

    omp_atomic(const _Tp& _init)
    : _value(_init)
    { }

    omp_atomic(const omp_atomic& rhs)
    : _value(rhs._value)
    { }

    omp_atomic& operator=(const omp_atomic& rhs)
    {
        if(this != &rhs)
        {
            _value = rhs._value;
        }
        return *this;
    }
    omp_atomic& operator=(const _Tp& rhs)
    {
        _value = rhs;
        return *this;
    }

    ~omp_atomic()
    {
        _value.~Base_t();
    }

    Base_t& base() { return _value; }
    const Base_t& base() const { return _value; }
    Base_t& base() volatile { return _value; }
    const Base_t& base() const volatile { return _value; }

    bool is_lock_free() const { return true; }
    bool is_lock_free() const volatile { return true; }

    void store(_Tp _desired, const int& = 0)
    {
        #pragma omp atomic
        _value += -_value + _desired;
    }
    void store(_Tp _desired, const int& = 0) volatile
    {
        #pragma omp atomic
        _value += -_value + _desired;
    }

    _Tp load(const int& = 0) const
    { return _value; }
    _Tp load(const int& = 0) const volatile
    { return _value; }

    operator _Tp() const { return this->load(); }
    operator _Tp() const volatile { return this->load(); }

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0,
                               const int& = 0)
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }
    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0,
                               const int& = 0)
    volatile
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0)
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0)
    volatile
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0,
                                 const int& = 0)
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0,
                                 const int& = 0)
    volatile
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0)
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0)
    volatile
    {
        #pragma omp atomic
        _value += -_value + _desired;
        return true;
    }

    // value_type operators
    omp_atomic& operator+=(const _Tp& rhs)
    {
        #pragma omp atomic
        _value += rhs;
        return *this;
    }
    omp_atomic& operator-=(const _Tp& rhs)
    {
        #pragma omp atomic
        _value -= rhs;
        return *this;
    }
    omp_atomic& operator*=(const _Tp& rhs)
    {
        #pragma omp atomic
        _value *= rhs;
        return *this;
    }
    omp_atomic& operator/=(const _Tp& rhs)
    {
        #pragma omp atomic
        _value /= rhs;
        return *this;
    }

    // atomic operators
    omp_atomic& operator+=(const omp_atomic& rhs)
    {
        #pragma omp atomic
        _value += rhs._value;
        return *this;
    }
    omp_atomic& operator-=(const omp_atomic& rhs)
    {
        #pragma omp atomic
        _value -= rhs._value;
        return *this;
    }
    omp_atomic& operator*=(const omp_atomic& rhs)
    {
        #pragma omp atomic
        _value *= rhs._value;
        return *this;
    }
    omp_atomic& operator/=(const omp_atomic& rhs)
    {
        #pragma omp atomic
        _value /= rhs._value;
        return *this;
    }

    omp_atomic& operator+=(const omp_atomic& rhs) volatile
    {
        #pragma omp atomic
        _value += rhs._value;
        return *this;
    }
    omp_atomic& operator-=(const omp_atomic& rhs) volatile
    {
        #pragma omp atomic
        _value -= rhs._value;
        return *this;
    }
    omp_atomic& operator*=(const omp_atomic& rhs) volatile
    {
        #pragma omp atomic
        _value *= rhs._value;
        return *this;
    }
    omp_atomic& operator/=(const omp_atomic& rhs) volatile
    {
        #pragma omp atomic
        _value /= rhs._value;
        return *this;
    }


    // increment operators
    omp_atomic& operator++()
    {
        #pragma omp atomic
        ++_value;
        return *this;
    }
    omp_atomic operator++(int)
    {
        omp_atomic _tmp = value_type();
        #pragma omp atomic
        _tmp += _value++;
        return _tmp;
    }

    omp_atomic& operator--()
    {
        #pragma omp atomic
        --_value;
        return *this;
    }
    omp_atomic operator--(int)
    {
        omp_atomic _tmp = value_type();
        #pragma omp atomic
        _tmp += _value--;
        return _tmp;
    }

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
        boost::serialization::split_member(ar, *this, file_version);
    }
    //------------------------------------------------------------------------//
#endif
};

//----------------------------------------------------------------------------//

} // namespace mad

//----------------------------------------------------------------------------//

#endif // USE_OPENMP

#endif // omp_atomic_hh_
