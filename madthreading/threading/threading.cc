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

#include "madthreading/threading/threading.hh"
#include "madthreading/threading/auto_lock.hh"

#if defined (WIN32)
   #include <Windows.h>
#else
   #include <unistd.h>
   #include <sys/types.h>
   #include <sys/syscall.h>
#endif

namespace mad
{

namespace
{
    ThreadLocal int ThreadID = Threading::MASTER_ID;
    bool isMTAppType = false;
}

Pid_t Threading::GetPidId()
{
    // In multithreaded mode return Thread ID
    return std::this_thread::get_id();
}

int Threading::GetNumberOfCores()
{
    return std::thread::hardware_concurrency();
}

void Threading::SetThreadId(int value ) { ThreadID = value; }
int Threading::GetThreadId() { return ThreadID; }
bool Threading::IsWorkerThread() { return (ThreadID>=0); }
bool Threading::IsMasterThread() { return (ThreadID==MASTER_ID); }

} // namespace mad
