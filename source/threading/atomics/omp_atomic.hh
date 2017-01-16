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


#ifndef omp_atomic_hh_
#define omp_atomic_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module omp_atomic
%{
    #include "omp_atomic.hh"
%}
#endif
//----------------------------------------------------------------------------//

#ifdef ENABLE_OPENMP

#include <omp.h>
#include <omp_atomic.hh>
#include <boost/serialization/split_member.hpp>

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

public:
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

};

//----------------------------------------------------------------------------//

} // namespace mad

//----------------------------------------------------------------------------//

#endif // ENABLE_OPENMP

#endif // omp_atomic_hh_
