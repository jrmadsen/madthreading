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
    #include "condition.hh"
%}

%include "condition.hh"
#endif

#include "threading.hh"

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
    // Constructor and Destructors
    condition()
    {
        CORECONDITIONINIT(&m_cond_var);
    }
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
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
    void wait(mad::mutex&);
#ifndef SWIG
    // indirectly interface with mad::mutex
    void wait(mad::mutex*);
#endif

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

} // namespace mad

#endif
