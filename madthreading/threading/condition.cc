//
//
//
// MIT License
// Copyright (c) 2017 Jonathan R. Madsen
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// created by jmadsen on Sat Jul 15 20:13:16 2017
//
//
//
//


#include "madthreading/threading/condition.hh"
#include "madthreading/threading/mutex.hh"

//----------------------------------------------------------------------------//

namespace mad
{

//============================================================================//

void condition::wait(mad::CoreMutex& cmutex)
{
    CORECONDITIONWAIT(&m_cond_var, &cmutex);
}

//============================================================================//

void condition::wait(mad::mutex& mutex)
{
    wait(mutex.base_mutex_ptr());
}

//============================================================================//

void condition::wait(mad::mutex* mutex)
{
    wait(mutex->base_mutex_ptr());
}

//============================================================================//

void condition::timed_wait(mad::CoreMutex& cmutex, const float& tseconds)
{
    timespec reltime = get_time(tseconds);
    CORECONDITIONTIMEWAIT(&m_cond_var, &cmutex, &reltime);
}

//============================================================================//

void condition::timed_wait(mad::mutex& mutex, const float& tsec)
{
    timed_wait(mutex.base_mutex_ptr(), tsec);
}

//============================================================================//

void condition::timed_wait(mad::mutex* mutex, const float& tsec)
{
    timed_wait(mutex->base_mutex_ptr(), tsec);
}

//============================================================================//

} // namespace mad
