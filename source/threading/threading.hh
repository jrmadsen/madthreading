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


#ifndef threading_hh_
#define threading_hh_

#include <sstream>
#include <exception>
#include <stdexcept>

// Macro to put current thread to sleep
//
#if defined(WIN32)
#   define THREADSLEEP( tick ) { Sleep(tick); }
#else
#include <unistd.h>    // needed for sleep()
#   define THREADSLEEP( tick ) { sleep(tick); }
#endif

//----------------------------------------------------------------------------//

#if defined(ENABLE_THREADING)

#if ( defined(__MACH__) && defined(__clang__) && defined(__x86_64__) ) || \
    ( defined(__MACH__) && defined(__GNUC__) && __GNUC__>=4 && __GNUC_MINOR__>=7 ) || \
    defined(__linux__) || defined(_AIX) || defined(__bg__) || defined(__bgq__)
    //
    // Multi-threaded build: for POSIX systems
    //
#   include <pthread.h>
#   include <unistd.h>
#   include <semaphore.h>

#   include <csignal>
#   include <errno.h>
#   include <utility>

#   if defined(__MACH__)  // needed only for MacOSX for definition of pid_t
#       include <sys/types.h>
#   endif

namespace mad
{

    typedef pthread_mutex_t CoreMutex;
    typedef pthread_t CoreThread;
    typedef pthread_mutexattr_t CoreMutexAttr;

    typedef sem_t CoreSemaphore;

#   define CORE_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

#   define COREMUTEXLOCK pthread_mutex_lock
#   define COREMUTEXTRYLOCK pthread_mutex_trylock
#   define COREMUTEXUNLOCK pthread_mutex_unlock

#   define COREMUTEXINIT(mutex) pthread_mutex_init(&mutex, NULL)
#   define COREMUTEXDESTROY(mutex) pthread_mutex_destroy(&mutex)

#   define CORERECURSIVEMUTEXINIT(mutex) { \
                pthread_mutexattr_t _attr; \
                pthread_mutexattr_init(&_attr); \
                pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE); \
                pthread_mutex_init(&mutex, &_attr); }


#   define CORETHREADCREATE( worker , func , arg )  { \
                pthread_attr_t attr; \
                pthread_attr_init(&attr); \
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); \
                int ret = pthread_create( worker, &attr, func , arg ); \
                if (ret != 0) \
                { \
                    std::stringstream msg; \
                    msg << "pthread_create() failed: " << ret << std::endl; \
                    throw std::runtime_error(msg.str()); \
                } \
            }

    #if defined(__APPLE__) || defined(__ANDROID__)
#       define CORETHREADCREATEID( worker , func , arg, id)  { \
            pthread_attr_t attr; \
            pthread_attr_init(&attr); \
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); \
            int ret = pthread_create( worker, &attr, func , arg ); \
            if (ret != 0) \
            { \
                std::stringstream msg; \
                msg << "pthread_create() failed: " << ret << std::endl; \
                throw std::runtime_error(msg.str()); \
            } \
    }
#   else
#       define CORETHREADCREATEID( worker , func , arg, id )  { \
                pthread_attr_t attr; \
                static cpu_set_t cpuset; \
                pthread_attr_init(&attr); \
                CPU_ZERO(&cpuset); \
                CPU_SET(id, &cpuset); \
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); \
                pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset); \
                int ret = pthread_create( worker, &attr, func , arg ); \
                if (ret != 0) \
                { \
                    std::stringstream msg; \
                    msg << "pthread_create() failed: " << ret << std::endl; \
                    throw std::runtime_error(msg.str()); \
                } \
        }
#   endif

#   define CORETHREADJOIN(worker) pthread_join(worker, NULL)
#   define CORETHREADCANCEL(worker) pthread_cancel(worker)
#   define CORETHREADJOINVALUE(worker, value) pthread_join(worker, value)
#   define CORETHREADEXIT pthread_exit
#   define CORETHREADEQUAL pthread_equal
#   define CORETHREADSELF pthread_self
#ifndef __APPLE__
#   define CORETHREADSELFINT pthread_self
#else
#   define CORETHREADSELFINT (unsigned long) pthread_self
#endif

    typedef void* ThreadFuncReturnType;
    typedef void* ThreadFuncArgType;
    typedef int (*thread_lock)(CoreMutex*);
    typedef int (*thread_unlock)(CoreMutex*);

    typedef pid_t Pid_t;

    typedef pthread_cond_t CoreCondition;
#   define CORE_CONDITION_INITIALIZER PTHREAD_COND_INITIALIZER
#   define CORECONDITIONINIT(cond) pthread_cond_init(cond, NULL)
#   define CORECONDITIONDESTROY(cond) pthread_cond_destroy(cond)
#   define CORECONDITIONWAIT(cond, mutex) pthread_cond_wait(cond, mutex)
#   define CORECONDITIONTIMEWAIT(cond, mutex, atime) pthread_cond_timedwait(cond, mutex, atime)
#   define CORECONDITIONSIGNAL(cond) pthread_cond_signal(cond)
#   define CORECONDITIONBROADCAST(cond) pthread_cond_broadcast(cond)

#   define CORESEMAPHOREINIT(sema, initial) sem_init(&sema, 0, initial)
#   define CORESEMAPHOREWAIT(sema) sem_wait(&sema)
#   define CORESEMAPHOREPOST(sema) sem_post(&sema)
#   define CORESEMAPHOREGET(sema, holder) sem_getvalue(&sema, &holder)

} // namespace mad

#elif defined(WIN32)
    //
    // Multi-threaded build: for Windows systems
    //
    #include "windefs.hh"  // Include 'safe...' <windows.h>

    // NOT ALL TYPES HAVE BEEN ADDED
    // I need to do this, but it is low in priority since we don't run on
    // Windows and Windows sucks

namespace mad
{

    typedef HANDLE CoreMutex;
    typedef HANDLE CoreThread;

#   define CORE_MUTEX_INITIALIZER CreateMutex(NULL,FALSE,NULL)
    DWORD /*WINAPI*/ WaitForSingleObjectInf( __in CoreMutex m );
#   define COREMUTEXLOCK WaitForSingleObjectInf

    // #   define COREMUTEXINIT(mutex) InitializeCriticalSection( &mutex );
#   define COREMUTEXINIT(mutex);
#   define COREMUTEXDESTROY(mutex);

    //
    BOOL ReleaseMutex( __in CoreMutex m);
#   define COREMUTEXUNLOCK ReleaseMutex

#   define CORETHREADCREATE( worker, func, arg ) \
        { *worker = CreateThread( NULL, 16*1024*1024 , func , arg , 0 , NULL ); }
#   define CORETHREADCREATEID( worker, func, arg, id ) \
        CORETHREADCREATE( worker, func, arg )
#   define CORETHREADJOIN( worker ) WaitForSingleObject( worker , INFINITE);
#   define CORETHREADSELF GetCurrentThreadId
#   define ThreadFunReturnType DWORD WINAPI
    typedef LPVOID ThreadFunArgType;
    typedef DWORD (*thread_lock)(CoreMutex);
    typedef BOOL (*thread_unlock)(CoreMutex);
    typedef DWORD Pid_t;

    // Conditions
    //
    typedef CONDITION_VARIABLE CoreCondition;
#   define CORE_CONDITION_INITIALIZER CONDITION_VARIABLE_INIT
#   define CORECONDITIONWAIT( cond , criticalsectionmutex ) \
        SleepConditionVariableCS( cond, criticalsectionmutex , INFINITE );
#   define CORECONDTIONSIGNAL( cond ) WakeConditionVariable( cond );
#   define CORECONDTIONBROADCAST( cond ) WakeAllConditionVariable( cond );

} // namespace mad


#else

    #error "No Threading model technology supported for this platform. Use sequential build !"

#endif

#else // defined(ENABLE_THREADING)

namespace mad
{

    //==========================================
    // MULTITHREADED is OFF - Sequential build
    //==========================================
    typedef int CoreMutex;
    typedef int CoreThread;
    typedef int CoreSemaphore;
#   define CORE_MUTEX_INITIALIZER 1
    inline int fake_mutex_lock_unlock( CoreMutex* ) { return 0; }
#   define COREMUTEXINIT(mutex) ;;
#   define COREMUTEXDESTROY(mutex) ;;
#   define CORERECURSIVEMUTEXINIT(mutex) ;;
#   define COREMUTEXLOCK fake_mutex_lock_unlock
#   define COREMUTEXTRYLOCK fake_mutex_lock_unlock
#   define COREMUTEXUNLOCK fake_mutex_lock_unlock
#   define CORETHREADCREATE( worker , func , arg ) ;;
#   define CORETHREADCREATEID( worker , func , arg, id ) ;;
#   define CORETHREADJOIN( worker ) {;};
#   define CORETHREADCANCEL ( worker ) {;} ;
#   define CORETHREADSELF() 0
#   define CORETHREADSELFINT() 0UL
#   define CORETHREADEXIT(nothing) ;;
    typedef void* ThreadFunReturnType;
    typedef void* ThreadFunArgType;
    typedef int (*thread_lock)(CoreMutex*);
    typedef int (*thread_unlock)(CoreMutex*);
    typedef int Pid_t;
    typedef int CoreCondition;
#   define CORE_CONDITION_INITIALIZER 1
#   define CORECONDITIONINIT(cond) ;;
#   define CORECONDITIONDESTROY(cond) ;;
#   define CORECONDITIONWAIT( cond, mutex ) ;;
#   define CORECONDITIONTIMEWAIT( cond, mutex, atime ) ;;
#   define CORECONDITIONSIGNAL(cond) ;;
#   define CORECONDITIONBROADCAST(cond) ;;
#   define CORESEMAPHOREINIT(sema, inital) ;;
#   define CORESEMAPHOREWAIT(sema) ;;
#   define CORESEMAPHOREPOST(sema) ;;
#   define CORESEMAPHOREGET(sema, holder) ;;
#   define CORETHREADEQUAL(n1, n2) true

} // namespace mad

#endif // defined(ENABLE_THREADING)
//----------------------------------------------------------------------------//

#include "tls.hh"

//============================================================================//
// Some functions that help with threading
namespace mad
{

namespace Threading
{

//----------------------------------------------------------------------------//

enum
{
    SEQUENTIAL_ID = -2,
    MASTER_ID = -1,
    WORKER_ID = 0,
    GENERICTHREAD_ID = -1000
};

mad::Pid_t GetPidId();
int GetNumberOfCores();
int GetThreadId();
bool IsWorkerThread();
bool IsMasterThread();
void SetThreadId( int aNewValue );
void SetMultithreadedApplication(bool value);
bool IsMultithreadedApplication();

//----------------------------------------------------------------------------//

} // namespace Threading

} // namespace mad

//============================================================================//

#endif
