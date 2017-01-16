//
//
//
//
//
// created by jmadsen on Wed Oct  5 19:56:25 2016
//
//
//
//

#ifndef templateautolock_hh_
#define templateautolock_hh_

#include "threading.hh"

//----------------------------------------------------------------------------//

namespace mad
{

//----------------------------------------------------------------------------//

// Cannot be shared among threads
template <typename _Mutex, typename _Lock, typename _Unlock>
class TemplateAutoLock
{
public:
    //------------------------------------------------------------------------//
    TemplateAutoLock(_Mutex* _mtx, _Lock _lock, _Unlock _unlock)
    : is_locked(false),
      m_mtx(_mtx),
      m_lock(_lock),
      m_unlock(_unlock)
    {
        lock();
    }
    //------------------------------------------------------------------------//
    virtual ~TemplateAutoLock()
    {
        if(is_locked)
            unlock();
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    inline void lock()
    {
        m_lock(m_mtx);
        is_locked = true;
        /*if(is_locked)
            return;
        m_lock(m_mtx);
        is_locked = true;*/
    }
    //------------------------------------------------------------------------//
    inline void unlock()
    {
        is_locked = false;
        m_unlock(m_mtx);
        /*if(!is_locked)
            return;
        m_unlock(m_mtx);
        is_locked = false;*/
    }
    //------------------------------------------------------------------------//

    _Mutex& base_mutex() { return *m_mtx; }
    _Mutex* base_mutex_ptr() { return m_mtx; }

    //------------------------------------------------------------------------//

protected:
    volatile bool is_locked;
    _Mutex* m_mtx;
    _Lock m_lock;
    _Unlock m_unlock;

private:
    // Disable copy and assignment
    TemplateAutoLock(const TemplateAutoLock&) {}
    TemplateAutoLock& operator=(const TemplateAutoLock&) { }

};

//----------------------------------------------------------------------------//

} // namespace mad

//----------------------------------------------------------------------------//

#endif
