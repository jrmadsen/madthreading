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
// created by jrmadsen on Wed Jul 22 09:17:12 2015
//
//
//
//


#ifndef condition_hh_
#define condition_hh_

#ifdef SWIG
%module condition
%{
    #define SWIG_FILE_WITH_INIT
    #include <time.h>
    #include "madthreading/threading/threading.hh"
    #include "madthreading/threading/condition.hh"
%}

%import "madthreading/threading/threading.hh"
%include "condition.hh"
#endif

#include "madthreading/threading/threading.hh"
#include <time.h>

namespace mad
{

// pre-declaration of mad::mutex
class mutex;

//----------------------------------------------------------------------------//
// conditions are used to put idle threads to sleep instead of placing them in
// a while loop (which eats up unnecessary compute cycles)
// A signal notifies one thread to wake up and acquire the mutex
// Broadcast signals all threads to wake up and wait to acquire the mutex
//----------------------------------------------------------------------------//

class condition
{
public:
    /// If shared == true then the condition is initialized with
    /// PTHREAD_PROCESS_SHARED set, meaning  it does not have
    /// to be called with "mutex" locked by the calling thread
    /// If shared == false then when a wait/timed_wait function
    /// is called with "mutex" NOT locked by the calling thread,
    /// an undefined behavior results
    condition(bool shared = true)
    {
        if(shared)
        { CORECONDITIONSHAREDINIT(&m_cond_var); }
        else
        { CORECONDITIONINIT(&m_cond_var); }
    }
    // destructor
    virtual ~condition()
    {
        CORECONDITIONDESTROY(&m_cond_var);
    }

public:
    // send a signal to be consumed by one thread
    void signal();
    // send a signal to be consumed by all threads
    void broadcast();

    // interface directly with pthread_mutex_t
    void wait(mad::CoreMutex* mutex);
    // indirectly interface with mad::mutex
    void wait(mad::mutex*);
#ifndef SWIG
    // interface directly with pthread_mutex_t
    void wait(mad::CoreMutex& mutex);
    // indirectly interface with mad::mutex
    void wait(mad::mutex&);
#endif

    // interface directly with pthread_mutex_t
    void timed_wait(mad::CoreMutex* mutex, const float& tseconds);
    // indirectly interface with mad::mutex
    void timed_wait(mad::mutex*, const float& tseconds);
#ifndef SWIG
    // interface directly with pthread_mutex_t
    void timed_wait(mad::CoreMutex& mutex, const float& tseconds);
    // indirectly interface with mad::mutex
    void timed_wait(mad::mutex&, const float& tseconds);
#endif

protected:
    timespec get_time(const float& tseconds) const;

protected:
    // Protected variables
    CoreCondition m_cond_var;

};

//----------------------------------------------------------------------------//
inline
void condition::signal()
{
    CORECONDITIONSIGNAL(&m_cond_var);
}
//----------------------------------------------------------------------------//
inline
void condition::broadcast()
{
    CORECONDITIONBROADCAST(&m_cond_var);
}
//----------------------------------------------------------------------------//
inline
void condition::wait(mad::CoreMutex* cmutex)
{
    CORECONDITIONWAIT(&m_cond_var, cmutex);
}
//----------------------------------------------------------------------------//
inline
void condition::timed_wait(mad::CoreMutex* cmutex, const float& tseconds)
{
    timespec reltime = get_time(tseconds);
    CORECONDITIONTIMEWAIT(&m_cond_var, cmutex, &reltime);
}
//----------------------------------------------------------------------------//
inline
timespec condition::get_time(const float& tsec) const
{
    constexpr float nsec = 1.0e9;
    timespec now;
    timespec rel;
    clock_gettime(CLOCK_REALTIME, &now);
    rel = now;
    rel.tv_nsec += static_cast<int64_t>(nsec * tsec);
    return rel;
}
//----------------------------------------------------------------------------//

} // namespace mad

#endif
