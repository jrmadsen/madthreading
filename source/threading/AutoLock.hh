//
//
//
//
//
// created by jrmadsen on Sun Jun 21 17:28:55 2015
//
//
//
//


#ifndef autolock_hh_
#define autolock_hh_

#ifdef SWIG

%module AutoLock
%{
    #define SWIG_FILE_WITH_INIT
    #include "threading.hh"
    #include "TemplateAutoLock.hh"
    #include "AutoLock.hh"
%}

%import "threading.hh"
%import "TemplateAutoLock.hh"

#include "threading.hh"

%template(PyAutoLock) mad::TemplateAutoLock<mad::CoreMutex, mad::thread_lock, mad::thread_unlock>;
typedef mad::TemplateAutoLock<mad::CoreMutex,
                              mad::thread_lock,
                              mad::thread_unlock> PyAutoLock;
typedef mad::CoreMutex  CoreMutex;
%import "mutex.hh"
%include "AutoLock.hh"
#endif

#include "threading.hh"
#include "TemplateAutoLock.hh"
#include "mutex.hh"

//----------------------------------------------------------------------------//

namespace mad
{

//----------------------------------------------------------------------------//

struct ImplAutoLock : public TemplateAutoLock<mad::CoreMutex,
                                              mad::thread_lock,
                                              mad::thread_unlock>
{
    //------------------------------------------------------------------------//

    ImplAutoLock(mad::CoreMutex* mtx)
    : TemplateAutoLock<mad::CoreMutex,
                       mad::thread_lock,
                       mad::thread_unlock>(mtx,
                                           &COREMUTEXLOCK,
                                           &COREMUTEXUNLOCK)
    { }

    //------------------------------------------------------------------------//

    ImplAutoLock(mad::mutex* mtx)
    : TemplateAutoLock<mad::CoreMutex,
                       mad::thread_lock,
                       mad::thread_unlock>(mtx->base_mutex_ptr(),
                                           &COREMUTEXLOCK,
                                           &COREMUTEXUNLOCK)
    { }

    //------------------------------------------------------------------------//
#ifndef SWIG
    ImplAutoLock(mad::CoreMutex& mtx)
    : TemplateAutoLock<mad::CoreMutex,
                       mad::thread_lock,
                       mad::thread_unlock>(&mtx,
                                           &COREMUTEXLOCK,
                                           &COREMUTEXUNLOCK)
    { }

    //------------------------------------------------------------------------//

    ImplAutoLock(mad::mutex& mtx)
    : TemplateAutoLock<mad::CoreMutex,
                       mad::thread_lock,
                       mad::thread_unlock>(mtx.base_mutex_ptr(),
                                           &COREMUTEXLOCK,
                                           &COREMUTEXUNLOCK)
    { }
#endif
    //------------------------------------------------------------------------//
};


//----------------------------------------------------------------------------//

typedef ImplAutoLock AutoLock;

//----------------------------------------------------------------------------//

} // namespace mad

//----------------------------------------------------------------------------//

#endif // autolock_hh_
