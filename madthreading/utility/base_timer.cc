/*
Copyright (c) 2015-2017 by the parties listed in the AUTHORS file.
All rights reserved.  Use of this source code is governed by
a BSD-style license that can be found in the LICENSE file.
*/


#include "madthreading/utility/base_timer.hh"
#include <cassert>
#include <algorithm>

namespace mad
{
namespace util
{
namespace details
{

//============================================================================//

thread_local uint64_t base_timer::f_instance_count = 0;

//============================================================================//

base_timer::mutex_map_t base_timer::w_mutex_map;

//============================================================================//

base_timer::base_timer(uint16_t prec, const string_t& fmt, std::ostream* os)
: m_valid_times(false),
  m_running(false),
  m_precision(prec),
  m_os(os),
  m_format_positions(pos_list_t()),
  m_format_string(fmt),
  m_output_format("")
{
    this->start();
}

//============================================================================//

base_timer::~base_timer()
{
    if(!m_valid_times)
    {
        this->stop();
        if(m_os != &std::cout && *m_os)
            this->report();
    }
}

//============================================================================//

void base_timer::parse_format()
{
    m_format_positions.clear();

    this->compose();

    size_type npos = std::string::npos;

    str_list_t fmts;
    fmts.push_back(clockstr_t("%w", clock_type::wall    ));
    fmts.push_back(clockstr_t("%u", clock_type::user    ));
    fmts.push_back(clockstr_t("%s", clock_type::system  ));
    fmts.push_back(clockstr_t("%t", clock_type::cpu     ));
    fmts.push_back(clockstr_t("%p", clock_type::percent ));

    for(str_list_t::iterator itr = fmts.begin(); itr != fmts.end(); ++itr)
    {
        size_type pos = 0;
        // start at zero and look for all instances of string
        while((pos = m_format_string.find(itr->first, pos)) != npos)
        {
            // post-increment pos so we don't find same instance next
            // time around
            m_format_positions.push_back(clockpos_t(pos++, itr->second));
        }
    }
    std::sort(m_format_positions.begin(), m_format_positions.end(),
              [] (const clockpos_t& lhs, const clockpos_t& rhs)
              { return lhs.first < rhs.first; });

}

//============================================================================//

void base_timer::report(std::ostream& os, bool endline, bool avg) const
{
    const_cast<base_timer*>(this)->parse_format();

    // stop, if not already stopped
    if(!m_valid_times)
        const_cast<base_timer*>(this)->stop();

    // for average reporting
    double div = 1.0;
    if(avg && this->laps() > 0)
        div = 1.0 / static_cast<double>(this->laps());

    double _real = real_elapsed();
    double _user = user_elapsed();
    double _system = system_elapsed();
    double _cpu = _user + _system;
    double _perc = (_cpu / _real) * 100.0;

    double tmin = 1.0 / (pow( (uint32_t) 10, (uint32_t) m_precision));
    // skip if it will be reported as all zeros
    // e.g. tmin = ( 1. / 10^3 ) = 0.001;
    if(_real < tmin || _cpu < tmin || _perc < 0.1)
        return;

    // timing spacing
    static uint16_t noff = 3;
    if(_cpu > 10.0)
        noff = std::max(noff, (uint16_t) ( log10(_cpu) + 2 ));

    // use stringstream so precision and fixed don't directly affect
    // ostream
    std::stringstream ss;
    // set precision
    ss.precision(m_precision);
    // output fixed
    ss << std::fixed;
    size_type pos = 0;
    for(size_type i = 0; i < m_format_positions.size(); ++i)
    {
        // where to terminate the sub-string
        size_type ter = m_format_positions.at(i).first;
        assert(!(ter < pos));
        // length of substring
        size_type len = ter - pos;
        // create substring
        string_t substr = m_format_string.substr(pos, len);
        // add sub-string
        ss << substr;
        // print the appropriate timing mechanism
        switch (m_format_positions.at(i).second)
        {
            case clock_type::wall:
                // the real elapsed time
                ss << std::setw(noff+m_precision)
                   << (_real * div);
                break;
            case clock_type::user:
                // CPU time of non-system calls
                ss << std::setw(noff+m_precision)
                   << (_user * div);
                break;
            case clock_type::system:
                // thread specific CPU time, e.g. thread creation overhead
                ss << std::setw(noff+m_precision)
                   << (_system * div);
                break;
            case clock_type::cpu:
                // total CPU time
                ss << std::setw(noff+m_precision)
                   << (_cpu * div);
                break;
            case clock_type::percent:
                // percent CPU utilization
                ss.precision(1);
                ss << std::setw(5) << (_perc);
                break;
        }
        // skip over %{w,u,s,t,p} field
        pos = m_format_positions.at(i).first+2;
    }
    // write the end of the string
    size_type ter = m_format_string.length();
    size_type len = ter - pos;
    string_t substr = m_format_string.substr(pos, len);
    ss << substr;
    if(avg)
        ss << " (average of " << this->laps() << " laps)";
    else if(this->laps() > 1)
        ss << " (total # of laps: " << this->laps() << ")";

    if(endline)
        ss << std::endl;

    // ensure thread-safety
    auto_lock_t lock(w_mutex_map[&os]);
    // output to ostream
    os << ss.str();
}

} // namespace details

} // namespace util

} // namespace mad

//============================================================================//



