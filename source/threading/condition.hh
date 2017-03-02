//
//
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
    // Public functions
    void wait(mad::CoreMutex* mutex) { CORECONDITIONWAIT(&m_cond_var, mutex); }
    void signal() { CORECONDITIONSIGNAL(&m_cond_var); }
    void broadcast() { CORECONDITIONBROADCAST(&m_cond_var); }

protected:
    // Protected variables
    CoreCondition m_cond_var;

};

//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//

} // namespace mad

#endif
