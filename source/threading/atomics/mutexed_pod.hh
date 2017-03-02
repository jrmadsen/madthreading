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
%module mutexed_pod
%{
    #include "mutexed_pod.hh"
%}
#endif

#include "../threading.hh"
#include "../AutoLock.hh"

#ifdef USE_BOOST
#   include <boost/serialization/split_member.hpp>
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

    mutexed_pod& operator=(const mutexed_pod& rhs)
    {
        if(this != &rhs)
        {
            m_mutex = CORE_MUTEX_INITIALIZER;
            CORERECURSIVEMUTEXINIT(m_mutex);
            _value = rhs._value;
        }
        return *this;
    }
    mutexed_pod& operator=(const _Tp& rhs)
    {
        m_mutex = CORE_MUTEX_INITIALIZER;
        CORERECURSIVEMUTEXINIT(m_mutex);
        _value = rhs;
        return *this;
    }

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
    void store(_Tp _desired, const int& = 0) volatile
    { Lock_t lock(&m_mutex); _value = _desired; }

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

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0)
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

    bool compare_exchange_weak(_Tp& _expected, _Tp _desired,
                               const int& = 0)
    volatile
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0,
                                 const int& = 0)
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

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

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0)
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

    bool compare_exchange_strong(_Tp& _expected, _Tp _desired,
                                 const int& = 0)
    volatile
    {
        Lock_t lock(&m_mutex);
        if(_value == _expected)
        { _value = _desired; return true; }
        return false;
    }

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


    // increment operators
    mutexed_pod& operator++() { { Lock_t lock(&m_mutex); ++_value; } return *this; }
    mutexed_pod operator++(int)
    { mutexed_pod _tmp(_value); { Lock_t lock(&m_mutex); ++_value; } return _tmp; }

    mutexed_pod& operator--() { Lock_t lock(&m_mutex); --_value; return *this; }
    mutexed_pod operator--(int)
    { Lock_t lock(&m_mutex); mutexed_pod _tmp(_value); --_value; return _tmp; }

public:
    mutable Mutex_t m_mutex;

protected:
    Base_t _value;

public:
#ifdef USE_BOOST
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        value_type _val = this->load();
        ar << _val;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        value_type _val = 0;
        ar >> _val;
        store(_val);
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version)
    {
        boost::serialization::split_member(ar, *this, file_version);
    }

    //BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif
};

//----------------------------------------------------------------------------//

} // namespace mad

//----------------------------------------------------------------------------//

#endif // mutexed_pod_hh_
