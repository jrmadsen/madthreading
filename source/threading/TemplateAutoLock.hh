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
