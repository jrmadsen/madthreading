//
//
//
//
//
// created by jrmadsen on Sun Jun 21 16:42:04 2015
//
//
//
//


#ifndef mutex_hh_
#define mutex_hh_

#ifdef SWIG
%module mutex
%{
    #include "mutex.hh"
%}
#endif

#include "threading.hh"

// utility mutex class. Cannot be used with AutoLock currently,
// but nice in some situations since it doesn't have to initialized
// like CoreMutex
// eventually need to make AutoLock accept it
namespace mad
{

//----------------------------------------------------------------------------//

class mutex
{
public:
    typedef mad::CoreMutex      base_mutex_type;

public:
    //------------------------------------------------------------------------//
    mutex(bool recursive = false)
    : m_is_locked(false)
    {
        if(recursive)
        {
            m_mutex = CORE_MUTEX_INITIALIZER;
            CORERECURSIVEMUTEXINIT(m_mutex);
        } else
        {
            COREMUTEXINIT(m_mutex);
        }
    }
    //------------------------------------------------------------------------//
    virtual ~mutex()
    {
        while(m_is_locked);
        unlock(); // Unlock mutex after shared resource is safe
        COREMUTEXDESTROY(m_mutex);
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    bool is_locked() const
    {
        return m_is_locked;
    }
    //------------------------------------------------------------------------//
    void lock()
    {
        COREMUTEXLOCK(&m_mutex);
        m_is_locked = true;
    }
    //------------------------------------------------------------------------//
    void unlock()
    {
        m_is_locked = false;
        COREMUTEXUNLOCK(&m_mutex);
    }
    //------------------------------------------------------------------------//

    base_mutex_type& base_mutex() { return m_mutex; }
    base_mutex_type* base_mutex_ptr() { return &m_mutex; }

    //------------------------------------------------------------------------//

private:
    base_mutex_type m_mutex;
    volatile bool m_is_locked;
};

//----------------------------------------------------------------------------//

} // namespace mad

#endif
