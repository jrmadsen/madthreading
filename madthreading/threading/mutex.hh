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
    #define SWIG_FILE_WITH_INIT
    #include "madthreading/threading/mutex.hh"
%}

%include "mutex.hh"
#endif

#include "madthreading/threading/threading.hh"
#include "madthreading/threading/condition.hh"

// utility mutex class. Cannot be used with auto_lock currently,
// but nice in some situations since it doesn't have to initialized
// like CoreMutex
// eventually need to make auto_lock accept it
namespace mad
{

//----------------------------------------------------------------------------//

class mutex
{
public:
    typedef mad::CoreMutex      base_mutex_type;
    typedef mad::condition      condition_type;

public:
    //------------------------------------------------------------------------//
    mutex(bool recursive = false)
    : m_is_locked(false)
    {
        if(recursive)
            { CORERECURSIVEMUTEXINIT(m_mutex); }
        else
            { COREMUTEXINIT(m_mutex); }
    }
    //------------------------------------------------------------------------//
    virtual ~mutex()
    {
        while(m_is_locked)
            m_condition.wait(&m_mutex);
        unlock(); // Unlock mutex after shared resource is safe
        COREMUTEXDESTROY(m_mutex);
    }
    //------------------------------------------------------------------------//

public:
    //------------------------------------------------------------------------//
    bool is_locked() const volatile
    {
        return m_is_locked;
    }
    //------------------------------------------------------------------------//
    void lock()
    {
        COREMUTEXLOCK(&m_mutex);
        m_is_locked = true;
        m_condition.broadcast();
    }
    //------------------------------------------------------------------------//
    void unlock()
    {
        m_is_locked = false;
        COREMUTEXUNLOCK(&m_mutex);
        m_condition.broadcast();
    }
    //------------------------------------------------------------------------//

    base_mutex_type& base_mutex() { return m_mutex; }
    base_mutex_type* base_mutex_ptr() { return &m_mutex; }

    //------------------------------------------------------------------------//

private:
    base_mutex_type m_mutex;
    volatile bool m_is_locked;
    condition_type m_condition;
};

//----------------------------------------------------------------------------//

} // namespace mad

#endif
