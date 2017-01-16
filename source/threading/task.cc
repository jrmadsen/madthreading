//
//
//
//
//
// created by jrmadsen on Wed Jul 22 09:15:04 2015
//
//
//
//


#include "task.hh"

namespace mad
{

//============================================================================//

vtask::vtask(void* result, void* arg1, void* arg2, void* arg3)
: m_force_delete(false),
  m_is_stored_elsewhere(false),
  m_result(result)
{
    set_arg_array(arg1, arg2, arg3);
}

//============================================================================//

} // namespace mad
