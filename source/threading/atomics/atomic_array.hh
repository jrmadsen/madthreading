//
//
//
//
//
// created by jmadsen on Wed Apr  9 02:08:19 2014
//
//
//
//


#ifndef atomic_array_hh_
#define atomic_array_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic_array
%{
    #include "atomic_array.hh"
%}
#endif
//----------------------------------------------------------------------------//

#include "atomic.hh"
#include <sstream>

#ifdef _HAS_ATOMICS_

namespace mad
{

template <typename _Tp, std::size_t N>
class atomic_array
{
public:
    typedef _Tp                                         basic_type;
    typedef atomic<_Tp>                                 value_type;
    typedef value_type*                                 pointer;
    typedef const value_type*                           const_pointer;
    typedef value_type&                                 reference;
    typedef const value_type&                           const_reference;
    typedef value_type*                                 iterator;
    typedef const value_type*                           const_iterator;
    typedef std::reverse_iterator<value_type*>          reverse_iterator;
    typedef std::reverse_iterator<const value_type*>    const_reverse_iterator;
    typedef std::size_t                                 size_type;
    typedef std::ptrdiff_t                              difference_type;

public:
    // Constructor and Destructors
    atomic_array(const _Tp& _default_val = _Tp());
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    virtual ~atomic_array();

public:
    // Public functions

    static size_type size() { return N; }
    static bool empty() { return false; }
    static size_type max_size() { return N; }
    enum { static_size = N };

    void swap(atomic_array<_Tp, N>& other)
    {
        for(size_type i = 0; i < N; ++i)
            std::swap(_array[i], other._array[i]);
    }

    // direct access to data (read-only)
    const_pointer data() const { return _array; }
    pointer data() { return _array; }

    // use array as C array (direct read/write access to data)
    pointer c_array() { return _array; }

    inline reference operator[](const size_type&);
    inline const_reference operator[](const size_type&) const;

    inline reference at(const size_type&);
    inline const_reference at(const size_type&) const;

    iterator begin() { return &_array[0]; }
    iterator end() { return &_array[N]; }
    const_iterator begin() const { return &_array[0]; }
    const_iterator end() const { return &_array[N]; }

    reverse_iterator rbegin() { return reverse_iterator(&_array[N]); }
    reverse_iterator rend() { return reverse_iterator(&_array[0]); }
    const_reverse_iterator rbegin() const
    { return const_reverse_iterator(&_array[N]); }
    const_reverse_iterator rend() const
    { return const_reverse_iterator(&_array[0]); }

    const_iterator cbegin() const { return &_array[0]; }
    const_iterator cend() const { return &_array[N]; }

    inline reference front() { return _array[0]; }
    inline const_reference front() const { return _array[0]; }

    inline reference back() { return _array[N-1]; }
    inline const_reference back() const { return _array[N-1]; }

    //inline atomic_array& operator=(const std::array<_Tp>&);

    inline void assign(const _Tp&);

protected:
    // Protected functions

protected:
    // Protected variables
    atomic<_Tp> _array[N];

private:
    // Private functions
    void _Check_Size(const size_type& _cmp_size)
    {
      if(_cmp_size >= N)
      {
            std::stringstream ss;
            ss << "atomic_array<_Tp, N> index is greater than size ("
            << _cmp_size << " vs. " << N << ")";
            throw std::runtime_error(ss.str().c_str());
      }
    }
private:
    // Private variables

public:
    //------------------------------------------------------------------------//
    atomic_array(const atomic_array& rhs)
    {
        for(size_type i = 0; i < N; ++i)
        {
            _array[i] = rhs._array[i];
        }
    }
    //------------------------------------------------------------------------//
    template <typename _Up>
    inline atomic_array& operator=(const atomic_array<_Up, N>& rhs)
    {
        if(this != &rhs)
        {
            for(size_type i = 0; i < N; ++i)
            {
                _array[i] = static_cast<_Tp>(rhs._array[i]);
            }
        }
        return *this;
    }
    //------------------------------------------------------------------------//

public:
#ifdef ENABLE_BOOST
    //------------------------------------------------------------------------//
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        for(iterator itr = begin(); itr != end(); ++itr)
        {
            basic_type val = *itr;
            ar << val;
        }
    }
    //------------------------------------------------------------------------//
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        for(size_type i = 0; i < size(); ++i)
        {
            value_type _val = 0;
            ar >> _val;
            this->at(i) = _val;
        }
    }
    //------------------------------------------------------------------------//
    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version)
    {
        boost::serialization::split_member(ar, *this, file_version);
    }
    //------------------------------------------------------------------------//
#endif
};



//----------------------------------------------------------------------------//
template <typename _Tp, std::size_t N>
atomic_array<_Tp, N>::atomic_array(const _Tp& _default_val)
{
    for(size_type i = 0; i < N; ++i)
    {
        _array[i] = _default_val;
    }
}
//----------------------------------------------------------------------------//
template <typename _Tp, std::size_t N>
atomic_array<_Tp, N>::~atomic_array()
{
    //delete [] _array;
}
//----------------------------------------------------------------------------//
template <typename _Tp, std::size_t N>
inline const
atomic<_Tp>& atomic_array<_Tp, N>::operator[](const size_type& _index) const
{
  return _array[_index];
}
//----------------------------------------------------------------------------//
template <typename _Tp, std::size_t N>
inline atomic<_Tp>& atomic_array<_Tp, N>::operator[](const size_type& _index)
{
    return _array[_index];
}
//----------------------------------------------------------------------------//
template <typename _Tp, std::size_t N>
inline const
atomic<_Tp>& atomic_array<_Tp, N>::at(const size_type& _index) const
{
  _Check_Size(_index);
  return _array[_index];
}
//----------------------------------------------------------------------------//
template <typename _Tp, std::size_t N>
inline atomic<_Tp>& atomic_array<_Tp, N>::at(const size_type& _index)
{
  _Check_Size(_index);
  return _array[_index];
}
//----------------------------------------------------------------------------//
template <typename _Tp, std::size_t N>
inline void atomic_array<_Tp, N>::assign(const _Tp& _val)
{
  for(size_type i = 0; i < N; ++i) { _array[i] = _val; }
}
//----------------------------------------------------------------------------//

} // namespace mad

#else // _HAS_ATOMICS_

#include <boost/array.hpp>

namespace mad {
template <typename _Tp, std::size_t N>
class atomic_array : public boost::array<_Tp, N>
{ };
}

#endif // _HAS_ATOMICS_

#endif // atomic_array_hh_
