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
// created by jrmadsen on Thu Aug 20 15:00:54 2015
//
//
//
//


#include "timer.hh"
#include <algorithm>
#include <cassert>

#if !defined(WIN32)

namespace timer
{

namespace details
{

//============================================================================//

base_timer::base_timer(short prec, std::string fmt, std::ostream& os)
: m_places(prec), m_format_string(fmt), m_output_format(""),
  m_os(os), m_valid_times(false)
{
    this->start();
    this->parse_format();
}

//============================================================================//

base_timer::~base_timer()
{
    if(!m_valid_times)
    {
        this->stop();
        this->report();
    }
}

//============================================================================//

void base_timer::parse_format()
{
    std::stringstream ss;
    std::string str = m_format_string;
    size_type npos = std::string::npos;

    vstr_t fmts;
    fmts.push_back(clockstr_t("%w", WALL   ));
    fmts.push_back(clockstr_t("%u", USER   ));
    fmts.push_back(clockstr_t("%s", SYSTEM ));
    fmts.push_back(clockstr_t("%t", CPU    ));
    fmts.push_back(clockstr_t("%p", PERCENT));

    for(vstr_t::iterator itr = fmts.begin(); itr != fmts.end(); ++itr)
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
              clock_position_sorter());
}

//============================================================================//

void base_timer::report(std::ostream& os) const
{
    // stop, if not already stopped
    if(!m_valid_times)
        const_cast<base_timer*>(this)->stop();

    // use stringstream so precision and fixed don't directly affect
    // ostream
    std::stringstream ss;
    // set precision
    ss.precision(m_places);
    // output fixed
    ss << std::fixed;
    size_type pos = 0;
    for(unsigned i = 0; i < m_format_positions.size(); ++i)
    {
        // where to terminate the sub-string
        size_type ter = m_format_positions.at(i).first;
        assert(!(ter < pos));
        // length of substring
        size_type len = ter - pos;
        // create substring
        std::string substr = m_format_string.substr(pos, len);
        // add sub-string
        ss << substr;
        // print the appropriate timing mechanism
        switch (m_format_positions.at(i).second)
        {
            case WALL:
                ss << real_elapsed();
                break;
            case USER:
                ss << user_elapsed();
                break;
            case SYSTEM:
                ss << system_elapsed();
                break;
            case CPU:
                ss << (user_elapsed() + system_elapsed());
                break;
            case PERCENT:
                ss.precision(1);
                ss << std::setw(5)
                   << ((user_elapsed()+system_elapsed())/(real_elapsed())*100.0);
                break;
        }
        // skip over %{w,u,s,t,p} field
        pos = m_format_positions.at(i).first+2;
    }
    // write the end of the string
    size_type ter = m_format_string.length();
    size_type len = ter - pos;
    std::string substr = m_format_string.substr(pos, len);
    ss << substr;
    // output to ostream
    os << ss.str();
}

} // namespace details

//============================================================================//

std::string timer::default_format
    =  "%w wall, %u user + %s system = %t CPU [seconds] (%p%)";

//============================================================================//

unsigned long timer::default_precision = 6;

//============================================================================//

timer::timer(unsigned long prec, std::string _begin, std::string _close)
: base_type(prec, _begin + default_format + _close)
{ }

//============================================================================//

timer::timer(std::string _begin, std::string _close)
: base_type(default_precision, _begin + default_format + _close)
{ }

//============================================================================//

timer::~timer()
{ }

//============================================================================//

} // namespace timer

#endif // !defined(WIN32)
