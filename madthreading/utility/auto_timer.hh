//
// created by jrmadsen on Thu Nov 16 00:55:30 2017
//

#ifndef auto_timer_hh_
#define auto_timer_hh_

#include "madthreading/utility/timing_manager.hh"
#include <string>
#include <cstdint>

namespace mad
{
namespace util
{

class auto_timer
{
public:
    typedef mad::util::timing_manager::mad_timer_t  mad_timer_t;

public:
    // Constructor and Destructors
    auto_timer(const std::string&);
    virtual ~auto_timer();

private:
    static uint64_t& ncount()
    { return details::base_timer::get_instance_count(); }

private:
    mad_timer_t& m_timer;
};

//----------------------------------------------------------------------------//
inline auto_timer::auto_timer(const std::string& timer_tag)
: m_timer(timing_manager::instance()->timer(timer_tag, "cxx",
                                            ++auto_timer::ncount()))
{
    m_timer.start();
}
//----------------------------------------------------------------------------//
inline auto_timer::~auto_timer()
{
    m_timer.stop();
    if(auto_timer::ncount() > 0)
        auto_timer::ncount()--;
}
//----------------------------------------------------------------------------//

} // namespace util

} // namespace mad

#endif

