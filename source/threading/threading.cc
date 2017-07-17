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

#include "threading.hh"
#include "auto_lock.hh"

#if defined (WIN32)
   #include <Windows.h>
#else
   #include <unistd.h>
   #include <sys/types.h>
   #include <sys/syscall.h>
#endif

namespace mad
{

#if defined(ENABLE_THREADING)

namespace
{
    ThreadLocal int ThreadID = Threading::MASTER_ID;
    bool isMTAppType = false;
}

Pid_t Threading::GetPidId()
{ // In multithreaded mode return Thread ID
#if defined(__MACH__)
    return syscall(SYS_thread_selfid);
#elif defined(WIN32)
    return GetCurrentThreadId();
#elif defined(__ANDROID__)
    return gettid();
#else
    return syscall(SYS_gettid);
#endif
}

int Threading::GetNumberOfCores()
{
#if defined(WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    return static_cast<int>( sysinfo.dwNumberOfProcessors );
#else
    return static_cast<int>(sysconf( _SC_NPROCESSORS_ONLN ));
#endif
}

void Threading::SetThreadId(int value ) { ThreadID = value; }
int Threading::GetThreadId() { return ThreadID; }
bool Threading::IsWorkerThread() { return (ThreadID>=0); }
bool Threading::IsMasterThread() { return (ThreadID==MASTER_ID); }

#if defined(WIN32)  // WIN32 stuff needed for MT
DWORD /*WINAPI*/ WaitForSingleObjectInf( __in CoreMutex m )
 { return WaitForSingleObject( m , INFINITE); }
BOOL ReleaseMutex( __in CoreMutex m)
 { return ReleaseMutex(m); }
#endif

void Threading::SetMultithreadedApplication(bool value ) { isMTAppType = value; }
bool Threading::IsMultithreadedApplication() { return isMTAppType; }

#else  // Sequential mode

Pid_t Threading::GetPidId()
{                    // In sequential mode return Process ID and not Thread ID
    #if defined(WIN32)
    return GetCurrentProcessId();
    #else
    return getpid();
    #endif
}

int Threading::GetNumberOfCores() { return 1; }
int Threading::GetThreadId() { return Threading::SEQUENTIAL_ID; }
bool Threading::IsWorkerThread() { return false; }
bool Threading::IsMasterThread() { return true; }
void Threading::SetThreadId(int) {}

void Threading::SetMultithreadedApplication(bool) {}
bool Threading::IsMultithreadedApplication() { return false; }
#endif

} // namespace mad
