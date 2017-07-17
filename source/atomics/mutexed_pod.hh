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


#ifndef mutexed_pod_hh_
#define mutexed_pod_hh_

#ifdef SWIG
#   ifdef USE_BOOST_SERIALIZATION
#       undef USE_BOOST_SERIALIZATION
#   endif
#endif

#ifdef SWIG
%module mutexed_pod
%{
    #define SWIG_FILE_WITH_INIT
    #include "../threading.hh"
    #include "../AutoLock.hh"
    #include "mutexed_pod.hh"
%}

%import "AutoLock.hh"
%include "mutexed_pod.hh"
#endif

#include "../threading/threading.hh"
#include "../threading/AutoLock.hh"

#if defined(USE_BOOST_SERIALIZATION)
#   include <boost/serialization/split_member.hpp>
#   include <boost/serialization/version.hpp>
#   include <boost/serialization/serialization.hpp>
#   include <boost/serialization/access.hpp>
#endif

//----------------------------------------------------------------------------//

namespace mad
{


template<typename _Tp, typename _Mutex = CoreMutex, typename _Lock = AutoLock>
class mutexed_pod
{
public:
    typedef _Tp                         Base_t;
    typedef _Tp                         value_type;
    typedef _Mutex                      Mutex_t;
    typedef _Lock                       Lock_t;

public:
    mutexed_pod()
    : _value(_Tp())
    { CORERECURSIVEMUTEXINIT(m_mutex); }

    mutexed_pod(const _Tp& _init)
    : _value(_init)
    { CORERECURSIVEMUTEXINIT(m_mutex); }

    mutexed_pod(const mutexed_pod& rhs)
    : _value(rhs._value)
    { CORERECURSIVEMUTEXINIT(m_mutex); }

#ifndef SWIG
    mutexed_pod& operator=(const mutexed_pod& rhs)
    {
        if(this != &rhs)
        {
            CORERECURSIVEMUTEXINIT(m_mutex);
            _value = rhs._value;
        }
        return *this;
    }

    mutexed_pod& operator=(const _Tp& rhs)
    {
        CORERECURSIVEMUTEXINIT(m_mutex);
        _value = rhs;
        return *this;
    }
#endif

    ~mutexed_pod()
    {
        _value.~Base_t();
        COREMUTEXDESTROY(m_mutex);
    }

    Base_t& base() { return _value; }
    const Base_t& base() const { return _value; }
    Base_t& base() volatile { return _value; }
    const Base_t& base() const volatile { return _value; }

    bool is_lock_free() const { return false; }
    bool is_lock_free() const volatile { return false; }

    void store(_Tp _desired, const int& = 0)
    { Lock_t lock(&m_mutex); _value = _desired; }

    _Tp load(const int& = 0) const
    { return _value; }

#ifndef SWIG
    void store(_Tp _desired, const int& = 0) volatile
    { Lock_t lock(&m_mutex); _value = _desired; }

    _Tp load(const int& = 0) const volatile
    { return _value; }
#endif

    operator _Tp() const { return this->load(); }
    operator _Tp() const volatile { return this->load(); }

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0,
                               const int& = 0)
    {
        bool success = false;
        {
            Lock_t lock(&m_mutex);
            if(_value == _expected)
            {
                _value = _desired;
                success = true;
            }
        }
        return success;
    }

#ifndef SWIG
    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0,
                               const int& = 0)
    volatile
    {
        bool success = false;
        {
            Lock_t lock(&m_mutex);
            if(_value == _expected)
            {
                _value = _desired;
                success = true;
            }
        }
        return success;
    }
#endif

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0)
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

#ifndef SWIG
    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0)
    volatile
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }
#endif

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0,
                                 const int& = 0)
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

#ifndef SWIG
    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0,
                                 const int& = 0)
    volatile
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }
#endif

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0)
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

#ifndef SWIG
    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0)
    volatile
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }
#endif

    // value_type operators
    mutexed_pod& operator+=(const _Tp& rhs)
    { Lock_t lock(&m_mutex); _value += rhs; return *this; }
    mutexed_pod& operator-=(const _Tp& rhs)
    { Lock_t lock(&m_mutex); _value -= rhs; return *this; }
    mutexed_pod& operator*=(const _Tp& rhs)
    { Lock_t lock(&m_mutex); _value *= rhs; return *this; }
    mutexed_pod& operator/=(const _Tp& rhs)
    { Lock_t lock(&m_mutex); _value /= rhs; return *this; }

    // atomic operators
    mutexed_pod& operator+=(const mutexed_pod& rhs)
    { Lock_t lock(&m_mutex); _value += rhs._value; return *this; }
    mutexed_pod& operator-=(const mutexed_pod& rhs)
    { Lock_t lock(&m_mutex); _value -= rhs._value; return *this; }
    mutexed_pod& operator*=(const mutexed_pod& rhs)
    { Lock_t lock(&m_mutex); _value *= rhs._value; return *this; }
    mutexed_pod& operator/=(const mutexed_pod& rhs)
    { Lock_t lock(&m_mutex); _value /= rhs._value; return *this; }

    mutexed_pod& operator+=(const mutexed_pod& rhs) volatile
    { Lock_t lock(&m_mutex); _value += rhs._value; return *this; }
    mutexed_pod& operator-=(const mutexed_pod& rhs) volatile
    { Lock_t lock(&m_mutex); _value -= rhs._value; return *this; }
    mutexed_pod& operator*=(const mutexed_pod& rhs) volatile
    { Lock_t lock(&m_mutex); _value *= rhs._value; return *this; }
    mutexed_pod& operator/=(const mutexed_pod& rhs) volatile
    { Lock_t lock(&m_mutex); _value /= rhs._value; return *this; }

#ifndef SWIG
    // increment operators
    mutexed_pod& operator++() { { Lock_t lock(&m_mutex); ++_value; } return *this; }
    mutexed_pod operator++(int)
    { mutexed_pod _tmp(_value); { Lock_t lock(&m_mutex); ++_value; } return _tmp; }

    mutexed_pod& operator--() { Lock_t lock(&m_mutex); --_value; return *this; }
    mutexed_pod operator--(int)
    { Lock_t lock(&m_mutex); mutexed_pod _tmp(_value); --_value; return _tmp; }
#endif

public:
    mutable Mutex_t m_mutex;

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

#endif // mutexed_pod_hh_
