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
