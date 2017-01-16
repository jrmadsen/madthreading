//
//
//
//
//
// created by jmadsen on Fri Jan 23 02:53:38 2015
//
//
//
//

#ifndef atomic_map_hh_
#define atomic_map_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module atomic_map
%{
    #include "atomic_map.hh"
%}
#endif
//----------------------------------------------------------------------------//

#include <map>
#include <threading/atomics/atomic_typedefs.hh>

#ifdef _HAS_ATOMICS_

#include "AutoLock.hh"

namespace mad
{

template <typename _Key, typename _Tp, typename _Compare = std::less<_Key> >
class atomic_map
{
public:
    typedef std::pair<_Key, _Tp> Insert_t;
    typedef std::pair<_Key, atomic<_Tp>*> InsertPtr_t;
    typedef std::map<_Key, atomic<_Tp>*, _Compare> Base_t;
    typedef Base_t value_type;
    typedef _Key key_type;
    typedef _Tp basic_type;
    typedef atomic<_Tp> mapped_type;
    typedef typename Base_t::size_type size_type;
    typedef typename Base_t::key_compare key_compare;

    typedef typename Base_t::allocator_type allocator_type;

    typedef atomic_map<_Key, _Tp, _Compare> this_type;

    typedef CoreMutex  	Mutex_t;
    typedef AutoLock 	Lock_t;

public:
    typedef typename Base_t::iterator               iterator;
    typedef typename Base_t::const_iterator         const_iterator;
    typedef typename Base_t::reverse_iterator       reverse_iterator;
    typedef typename Base_t::const_reverse_iterator const_reverse_iterator;

public:
  // Constructor and Destructors
    atomic_map()
    : mutex(CORE_MUTEX_INITIALIZER)
    { CORERECURSIVEMUTEXINIT(mutex); }
  // Virtual destructors are required by abstract classes
  // so add it by default, just in case
    virtual ~atomic_map() { COREMUTEXDESTROY(mutex); }

public:
    mapped_type& operator[](const key_type& _key)
    {
        if(0 == key_map.count(_key)) {
            Lock_t lock(&mutex);
            if(0 == key_map.count(_key)) {
                key_map[_key] = new mapped_type(basic_type(0.));
            }
        }
        return *(key_map[_key]);
    }

    const mapped_type& operator[](const key_type& _key) const
    {
        if(0 == key_map.count(_key)) {
            Lock_t lock(&mutex);
            if(0 == key_map.count(_key)) {
                const_cast<Base_t&>(key_map)[_key] = new mapped_type(basic_type(0.));
            }
        }
        return *(const_cast<Base_t&>(key_map)[_key]);
    }

    // similar access as 'operator[]' except no locking
    mapped_type& operator()(const key_type& _key)
    {
        if(0 == key_map.count(_key)) {
            key_map[_key] = new mapped_type(basic_type(0.0));
        }
        return *(key_map[_key]);
    }

    // similar access as 'operator[] const' except no locking
    const mapped_type& operator()(const key_type& _key) const
    {
        if(0 == key_map.count(_key)) {
            const_cast<Base_t&>(key_map)[_key] = new mapped_type(basic_type());
        }
        return *(key_map[_key]);
    }

    std::pair<iterator, bool> insert(const Insert_t& _insert)
    {
        std::pair<iterator, bool> pair_itr_bool =
        key_map.insert(InsertPtr_t(_insert.first, 0));
        if(pair_itr_bool.second)
        {
            Lock_t lock(&mutex);
            if(0 == key_map.count(_insert.first)) {
                key_map.insert(InsertPtr_t(_insert.first, new mapped_type(_insert.second)));
            } else {
                pair_itr_bool =
                key_map.insert(InsertPtr_t(_insert.first,
                                        key_map.find(_insert.first)->second));
            }
        }
        return pair_itr_bool;
    }

    iterator insert (iterator position, const Insert_t& val)
    {
        mapped_type* _mapped_val = 0;
        if(0 == key_map.count(val.first)) {
            Lock_t lock(&mutex);
            if(0 == key_map.count(val.first)) {
                _mapped_val = new mapped_type(val.second);
            }
        }
        return key_map.insert(position, InsertPtr_t(val.first, _mapped_val));
    }

    template <class InputIterator>
    void insert (InputIterator first, InputIterator last)
    {
        Lock_t lock(&mutex);
        for(InputIterator itr = first; itr != last; ++itr)
            key_map.insert(*itr);
    }

    //  Returns a collection map.
    inline size_type add(const _Key& key, const _Tp& aHit)
    {
        (*this)[key] += aHit;
        return key_map.size();
    }

    void erase(iterator position)
    {
        delete *position;
        key_map.erase((*position)->first);
    }

    size_type erase(const key_type& key)
    {
        iterator position = key_map.find(key);
        delete *position;
        key_map.erase(key);
        return key_map.size();
    }

    void erase(iterator first, iterator last)
    {
        for(iterator itr = first; itr != last; ++itr)
            erase(itr);
    }

public:
  // Public functions
    iterator begin() { return key_map.begin(); }
    const_iterator begin() const { return key_map.begin(); }
    iterator end() { return key_map.end(); }
    const_iterator end() const { return key_map.end(); }

    reverse_iterator rbegin() { return key_map.rbegin(); }
    const_reverse_iterator rbegin() const { return key_map.rbegin(); }
    reverse_iterator rend() { return key_map.rend(); }
    const_reverse_iterator rend() const { return key_map.rend(); }

#ifdef CXX11
    const_iterator cbegin() const { return key_map.cbegin(); }
    const_iterator cend() const { return key_map.cend(); }
    const_reverse_iterator crbegin() const { return key_map.crbegin(); }
    const_reverse_iterator crend() const { return key_map.crend(); }

    mapped_type& at(const key_type& _key) { return *key_map.at(_key); }
    const mapped_type& at(const key_type& _key) const
    { return *key_map.at(_key); }

#endif

    bool empty() const { return key_map.empty(); }
    size_type size() const { return key_map.size(); }
    size_type max_size() const { return key_map.max_size(); }

    key_compare key_comp() const { return key_map.key_comp(); }
    iterator find (const key_type& k) { return key_map.find(k); }
    const_iterator find (const key_type& k) const { return key_map.find(k); }
    size_type count (const key_type& k) const { return key_map.count(k); }
    iterator lower_bound (const key_type& k) { return key_map.lower_bound(k); }
    const_iterator lower_bound (const key_type& k) const
    { return key_map.lower_bound(k); }
    iterator upper_bound (const key_type& k) { return key_map.upper_bound(k); }
    const_iterator upper_bound (const key_type& k) const
    { return key_map.upper_bound(k); }

    std::pair<const_iterator,const_iterator>
    equal_range(const key_type& k) const
    { return key_map.equal_range(k); }

    std::pair<iterator,iterator>
    equal_range (const key_type& k)
    { return key_map.equal_range(k); }

    allocator_type get_allocator() const { return key_map.get_allocator(); }

protected:
  // Protected functions

protected:
  // Protected variables
    Base_t key_map;
    mutable Mutex_t mutex;

private:
  // Private functions

private:
  // Private variables


public:
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar << size();
        for(iterator itr = begin(); itr != end(); ++itr)
        {
            key_type _key = itr->first;
            basic_type _val = itr->second;
            ar << _key;
            ar << _val;
        }
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        size_type _this_size;
        ar >> _this_size;
        for(size_type i = 0; i < _this_size; ++i)
        {
            key_type _key;
            value_type _val = 0;
            ar >> _key;
            ar >> _val;
            this->insert(Insert_t(_key, _val));
        }
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version)
    {
        boost::serialization::split_member(ar, *this, file_version);
    }


};



//----------------------------------------------------------------------------//

} //namespace mad

//----------------------------------------------------------------------------//

#else // _HAS_ATOMICS_

namespace mad {
template <typename _Key, typename _Tp, typename _Compare = std::less<_Key> >
class atomic_map : public std::map<_Key, _Tp, _Compare>
{ };
} // namespace mad

#endif // _HAS_ATOMICS_

#endif // atomic_map_hh_
