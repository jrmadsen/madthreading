//
//
//
//
//
// created by jmadsen on Fri Aug  7 09:44:32 2015
//
//
//
//

#include "allocator.hh"
#include "allocator_list.hh"

namespace mad
{
namespace details
{

//============================================================================//

allocator_base::allocator_base(bool is_thread_local)
{
    if(is_thread_local)
        allocator_list_tl::get_allocator_list()->Register(this);
    else
           allocator_list::get_allocator_list()->Register(this);
}

//============================================================================//

allocator_base::~allocator_base()
{

}

//============================================================================//

} // namespace details
} // namespace mad
