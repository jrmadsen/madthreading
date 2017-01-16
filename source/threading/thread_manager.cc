//
//
//
//
//
// created by jrmadsen on Wed Jul 15 14:45:53 2015
//
//
//
//


#include "thread_manager.hh"

namespace mad
{

//============================================================================//

thread_manager* thread_manager::fgInstance = 0;

//============================================================================//

thread_manager* thread_manager::Instance() { return fgInstance; }

//============================================================================//

void thread_manager::check_instance()
{
    static std::string msg = "Instance of singleton \"thread_manager\" already"
                             " exists!";
    if(fgInstance)
        throw std::runtime_error(msg);
}

//============================================================================//

thread_manager::size_type thread_manager::max_threads
    = Threading::GetNumberOfCores();

//============================================================================//

} // namespace mad
