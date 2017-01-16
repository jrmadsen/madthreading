//
//
//
//
//
// created by jrmadsen on Wed Jul  8 12:46:43 2015
//
//
//
//


#ifndef allocator_hh_
#define allocator_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module allocator
%{
    #include "tls.hh"
    #include "allocator.hh"
%}

#ifdef ThreadLocal
#   undef ThreadLocal
#endif
#ifdef ThreadLocalStatic
#   undef ThreadLocalStatic
#endif
#define ThreadLocal thread_local
#define ThreadLocalStatic static thread_local

%include "allocator.hh"
#endif
//----------------------------------------------------------------------------//

#include <new>
#include <memory>
#include <cstddef>
#include <typeinfo>

#include "cache_line_size.hh"
#include "allocator_pool.hh"
#include "../threading/threading.hh"
#include "../threading/AutoLock.hh"

#ifdef ENABLE_TBB
#include <tbb/tbb.h>
#include <tbb/scalable_allocator.h>
#include <tbb/cache_aligned_allocator.h>
#include <tbb/tbb_allocator.h>
#endif

namespace mad
{
namespace details
{

#ifdef ENABLE_TBB
static size_t cache_size = cache::cache_line_size();
#endif

//============================================================================//

class allocator_base
{
public:
    typedef std::size_t size_type;

public:
    allocator_base(bool);
    virtual ~allocator_base();

public:
    virtual void reset_storage() = 0;
    virtual size_type get_allocated_size() const = 0;
    virtual long get_num_pages() const = 0;
    virtual size_type get_page_size() const = 0;
    virtual void increase_page_size(size_type) = 0;
    virtual const char* get_pool_type() const = 0;

};

//============================================================================//

template <typename T, bool USE_TLP = true> // USE_TLP = Use Thread-Local Pool
class allocator : public allocator_base
{
public:
    allocator() throw();
    virtual ~allocator() throw();

public:
    inline T* malloc_single();
    inline void free_single(T* an_element);

    inline void reset_storage();
    inline size_type get_allocated_size() const;
    inline long get_num_pages() const;
    inline size_type get_page_size() const;
    inline void increase_page_size(size_type);
    inline const char* get_pool_type() const;

public:
    typedef T                 value_type;
    typedef std::size_t       size_type;
    typedef std::ptrdiff_t    difference_type;
    typedef T*                pointer;
    typedef const T*          const_pointer;
    typedef T&                reference;
    typedef const T&          const_reference;

public:
    //------------------------------------------------------------------------//
    template <typename U>
    allocator(const allocator<U>& rhs) throw()
    : allocator_base(USE_TLP), m_mem(rhs.m_mem)
    { }
    //------------------------------------------------------------------------//
    pointer address(reference r) const { return &r; }
    const_pointer address(const_reference r) const { return &r; }
    //------------------------------------------------------------------------//
    pointer allocate(size_type n, void* = 0)
    {
        // Allocates space for n elements of type T, but does not initialise
        pointer mem_alloc = 0;
        if (n == 1)
          mem_alloc = malloc_single();
        else
          mem_alloc = static_cast<pointer>(::operator new(n*sizeof(T)));
        return mem_alloc;
    }
    //------------------------------------------------------------------------//
    void deallocate(pointer p, size_type n)
    {
        // Deallocates n elements of type Type, but doesn't destroy
        if (n == 1)
            free_single(p);
        else
            ::operator delete((void*)p);
        return;
    }
    //------------------------------------------------------------------------//
    // Initialises *p by val
    void construct(pointer p, const value_type& val)
    {
        new((void*)p) value_type(val);
    }
    //------------------------------------------------------------------------//
    // Destroy *p but doesn't deallocate
    void destroy(pointer p)
    {
        p->~value_type();
    }
    //------------------------------------------------------------------------//
    size_type max_size() const throw()
    {
        // Returns the maximum number of elements that can be allocated
        return 2147483647/sizeof(T);
        //return 9223372036854775807/sizeof(value_type);
    }
    //------------------------------------------------------------------------//
    // Rebind allocator to type U
    template <class U>
    struct rebind { typedef allocator<U> other; };
    //------------------------------------------------------------------------//
    // Pool of elements of sizeof(T)
    allocator_pool m_mem;
    //------------------------------------------------------------------------//

private:
    const char* m_tname;
};

//============================================================================//

} // namespace details

//============================================================================//
// Thread-global pool allocator to be inherited from
//============================================================================//
template <typename T>
class PoolAllocator_t
{
private:
    typedef details::allocator<T, false>  alloc_type;
    static alloc_type* this_alloc;

public:
    //------------------------------------------------------------------------//
    void* operator new(size_t /*size*/) throw (std::bad_alloc)
    {
        if(!this_alloc)
            this_alloc = new alloc_type;
        return this_alloc->malloc_single();
    }
    //------------------------------------------------------------------------//
    void* operator new (size_t /*size*/, const std::nothrow_t&) throw ()
    {
        if(!this_alloc)
            this_alloc = new alloc_type;
        return this_alloc->malloc_single();
    }
    //------------------------------------------------------------------------//
    void operator delete(void* ptr) throw ()
    {
        if(ptr != 0)
            this_alloc->free_single((T*)(ptr));
    }
    //------------------------------------------------------------------------//
    void operator delete (void* ptr, const std::nothrow_t&) throw()
    {
        if(ptr != 0)
            this_alloc->free_single((T*)(ptr));
    }
    //------------------------------------------------------------------------//
};

//============================================================================//

template <typename T> typename
PoolAllocator_t<T>::alloc_type* PoolAllocator_t<T>::this_alloc = 0;

//============================================================================//
// Thread-local pool allocator to be inherited from
//============================================================================//
template <typename T>
class PoolAllocator_tl
{
private:
    typedef details::allocator<T, true>  alloc_type;
    ThreadLocalStatic alloc_type* this_alloc;

public:
    //------------------------------------------------------------------------//
    void* operator new(size_t /*size*/) throw (std::bad_alloc)
    {
        if(!this_alloc)
            this_alloc = new alloc_type;
        return this_alloc->malloc_single();
    }
    //------------------------------------------------------------------------//
    void* operator new (size_t /*size*/, const std::nothrow_t&) throw ()
    {
        if(!this_alloc)
            this_alloc = new alloc_type;
        return this_alloc->malloc_single();
    }
    //------------------------------------------------------------------------//
    void operator delete(void* ptr) throw ()
    {
        if(ptr != 0)
            this_alloc->free_single((T*)(ptr));
    }
    //------------------------------------------------------------------------//
    void operator delete (void* ptr, const std::nothrow_t&) throw()
    {
        if(ptr != 0)
            this_alloc->free_single((T*)(ptr));
    }
    //------------------------------------------------------------------------//
};

//============================================================================//

template <typename T> ThreadLocal
typename PoolAllocator_tl<T>::alloc_type* PoolAllocator_tl<T>::this_alloc = 0;

//============================================================================//

#ifdef ENABLE_TBB

#ifdef TBB_CACHE_ALIGNED_ALLOCATOR

#define Allocator_t(type) tbb::cache_aligned_allocator<type>
#define PairAllocator_t(key, type) std::allocator<std::pair<key, type> >

//============================================================================//

class Allocator
{
public:
    //------------------------------------------------------------------------//
    void* operator new(size_t size) throw (std::bad_alloc)
    {
        //static size_t cache_size = cache::cache_line_size();
        if(size == 0) size = 1;
        if(void* ptr = scalable_aligned_malloc(size, cache_size))
            return ptr;
        throw std::bad_alloc();
    }

    //------------------------------------------------------------------------//
    void* operator new [] (size_t size) throw (std::bad_alloc)
    {
        return operator new (size);
    }
    //------------------------------------------------------------------------//
    void* operator new (size_t size, const std::nothrow_t&) throw ()
    {
        //static size_t cache_size = cache::cache_line_size();
        if(size == 0) size = 1;
        if(void* ptr = scalable_aligned_malloc(size, cache_size))
            return ptr;
        return NULL;
    }
    //------------------------------------------------------------------------//
    void* operator new [] (size_t size, const std::nothrow_t&) throw ()
    {
        return operator new(size, std::nothrow);
    }
    //------------------------------------------------------------------------//
    void operator delete(void* ptr) throw ()
    {
        if(ptr != 0) scalable_aligned_free(ptr);
    }
    //------------------------------------------------------------------------//
    void operator delete [] (void* ptr) throw ()
    {
        operator delete(ptr);
    }
    //------------------------------------------------------------------------//
    void operator delete (void* ptr, const std::nothrow_t&) throw()
    {
        if(ptr != 0) scalable_aligned_free(ptr);
    }
    //------------------------------------------------------------------------//
    void operator delete [] (void* ptr, const std::nothrow_t&) throw ()
    {
        operator delete(ptr, std::nothrow);
    }

};

//============================================================================//


//----------------------------------------------------------------------------//
/*
extern "C" void* scalable_malloc(size_t size);
extern "C" void  scalable_free(void* object);
extern "C" void* scalable_realloc(void* ptr, size_t size);
extern "C" void* scalable_calloc(size_t nobj, size_t size);

extern "C" int   scalable_posix_memalign(void**, size_t, size_t);
extern "C" void* scalable_aligned_malloc(size_t, size_t);
extern "C" void* scalable_aligned_realloc(void*, size_t, size_t);
extern "C" void  scalable_aligned_free(void*);

//----------------------------------------------------------------------------//

#define calloc          scalable_calloc
#define malloc(size)    scalable_aligned_malloc(size, 64)
#define realloc         scalable_aligned_realloc
//#define free    scalable_aligned_free
*/

//----------------------------------------------------------------------------//

#elif defined(TBB_SCALABLE_ALLOCATOR)

//============================================================================//

#define Allocator_t(type) tbb::scalable_allocator<type>
#define PairAllocator_t(key, type) std::allocator<std::pair<key, type> >

//============================================================================//

class Allocator
{
public:
    //------------------------------------------------------------------------//
    void* operator new(size_t size) throw (std::bad_alloc)
    {
        if(size == 0) size = 1;
        if(void* ptr = scalable_malloc(size))
            return ptr;
        throw std::bad_alloc();
    }
    //------------------------------------------------------------------------//
    void* operator new [] (size_t size) throw (std::bad_alloc)
    {
        return operator new (size);
    }
    //------------------------------------------------------------------------//
    void* operator new (size_t size, const std::nothrow_t&) throw ()
    {
        if(size == 0) size = 1;
        if(void* ptr = scalable_malloc(size))
            return ptr;
        return NULL;
    }
    //------------------------------------------------------------------------//
    void* operator new [] (size_t size, const std::nothrow_t&) throw ()
    {
        return operator new(size, std::nothrow);
    }
    //------------------------------------------------------------------------//
    void operator delete(void* ptr) throw ()
    {
        if(ptr != 0) scalable_free(ptr);
        //if(ptr != 0) free(ptr);
    }
    //------------------------------------------------------------------------//
    void operator delete [] (void* ptr) throw ()
    {
        operator delete(ptr);
    }
    //------------------------------------------------------------------------//
    void operator delete (void* ptr, const std::nothrow_t&) throw()
    {
        if(ptr != 0) scalable_free(ptr);
        //if(ptr != 0) free(ptr);
    }
    //------------------------------------------------------------------------//
    void operator delete [] (void* ptr, const std::nothrow_t&) throw ()
    {
        operator delete(ptr, std::nothrow);
    }
    //------------------------------------------------------------------------//

};

//============================================================================//

/*
//----------------------------------------------------------------------------//

extern "C" void* scalable_malloc(size_t size);
extern "C" void  scalable_free(void* object);
extern "C" void* scalable_realloc(void* ptr, size_t size);
extern "C" void* scalable_calloc(size_t nobj, size_t size);

//----------------------------------------------------------------------------//

//#define calloc  scalable_calloc
//#define malloc  scalable_malloc
//#define realloc scalable_realloc
//#define free    scalable_free

//----------------------------------------------------------------------------//
*/

#else // TBB_CACHE_ALIGNED_ALLOCATOR/TBB_SCALABLE_ALLOCATOR

#define Allocator_t(type) std::allocator<type>
#define PairAllocator_t(key, type) std::allocator<std::pair<key, type> >

//============================================================================//

class Allocator { };

//============================================================================//

#endif // TBB_CACHE_ALIGNED_ALLOCATOR/TBB_SCALABLE_ALLOCATOR

#else // ENABLE_TBB

#define Allocator_t(type) std::allocator<type>
#define PairAllocator_t(key, type) std::allocator<std::pair<key, type> >

//============================================================================//

class Allocator { };

//============================================================================//

#endif // ENABLE_TBB

namespace details
{
//----------------------------------------------------------------------------//
// Inline implementation of allocator
//----------------------------------------------------------------------------//
// Initialization of the static pool
//
//============================================================================//
// allocator constructor
//============================================================================//
//
template <typename T, bool USE_TLP>
allocator<T, USE_TLP>::allocator() throw()
: allocator_base(USE_TLP), m_mem(sizeof(T))
{
  m_tname = typeid(T).name();
}

//============================================================================//
// allocator destructor
//============================================================================//
//
template <typename T, bool USE_TLP>
allocator<T, USE_TLP>::~allocator() throw()
{
}

//============================================================================//
// MallocSingle
//============================================================================//
//
template <typename T, bool USE_TLP>
T* allocator<T, USE_TLP>::malloc_single()
{
    static mad::CoreMutex mutex = CORE_MUTEX_INITIALIZER;
    mad::AutoLock l(&mutex);
    return static_cast<T*>(m_mem.alloc());
}

//============================================================================//
// FreeSingle
//============================================================================//
//
template <typename T, bool USE_TLP>
void allocator<T, USE_TLP>::free_single(T* anElement)
{
    m_mem.free(anElement);
    return;
}

//============================================================================//
// ResetStorage
//============================================================================//
//
template <typename T, bool USE_TLP>
void allocator<T, USE_TLP>::reset_storage()
{
    // Clear all allocated storage and return it to the free store
    //
    m_mem.reset();
    return;
}

//============================================================================//
// GetAllocatedSize
//============================================================================//
//
template <typename T, bool USE_TLP>
size_t allocator<T, USE_TLP>::get_allocated_size() const
{
    return m_mem.size();
}

//============================================================================//
// GetNoPages
//============================================================================//
//
template <typename T, bool USE_TLP>
long allocator<T, USE_TLP>::get_num_pages() const
{
    return m_mem.get_num_pages();
}

//============================================================================//
// GetPageSize
//============================================================================//
//
template <typename T, bool USE_TLP>
size_t allocator<T, USE_TLP>::get_page_size() const
{
    return m_mem.get_page_size();
}

//============================================================================//
// IncreasePageSize
//============================================================================//
//
template <typename T, bool USE_TLP>
void allocator<T, USE_TLP>::increase_page_size(size_type sz)
{
    reset_storage();
    m_mem.grow_page_size(sz);
}

//============================================================================//
// GetPoolType
//============================================================================//
//
template <typename T, bool USE_TLP>
const char* allocator<T, USE_TLP>::get_pool_type() const
{
    return m_tname;
}

//============================================================================//
// operator==
//============================================================================//
//
template <typename T1, typename T2, bool USE_TLP>
bool operator==(const allocator<T1, USE_TLP>&,
                const allocator<T2, USE_TLP>&) throw()
{
    return true;
}

//============================================================================//
// operator!=
//============================================================================//
//
template <typename T1, typename T2, bool USE_TLP>
bool operator!=(const allocator<T1, USE_TLP>&,
                const allocator<T2, USE_TLP>&) throw()
{
    return false;
}

//============================================================================//
} // namespace details
} // namespace mad

#endif // allocator_hh_

