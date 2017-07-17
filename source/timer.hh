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
// created by jrmadsen on Thu Aug 20 14:56:38 2015
//
//
//
//


#ifndef timer_hh_
#define timer_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module timer
%{
    #define SWIG_FILE_WITH_INIT
    #include "timer.hh"
%}

%include "timer.hh"
#endif
//----------------------------------------------------------------------------//

#if !defined(WIN32)

#include <unistd.h>
#include <sys/times.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

//----------------------------------------------------------------------------//
// essentially an implementation of boost::auto_cpu_timer
// because of some OS X issues with GNU compiler
//----------------------------------------------------------------------------//
namespace timer
{

//============================================================================//

namespace details
{

class base_timer
{
protected:
    enum CLOCK_TYPE { WALL, USER, SYSTEM, CPU, PERCENT };

    typedef std::string                         str_t;
    typedef std::string::size_type              size_type;
    typedef std::pair<size_type, CLOCK_TYPE>    clockpos_t;
    typedef std::pair<str_t,     CLOCK_TYPE>    clockstr_t;
    typedef std::vector<clockstr_t>             vstr_t;
    typedef std::vector<clockpos_t>             vpos_t;

protected:
#ifndef SWIG
    struct clock_position_sorter
    {
        bool operator()(clockpos_t lhs, clockpos_t rhs)
        {
            return lhs.first < rhs.first;
        }
    };
#endif

public:
    base_timer(short = 3, std::string
               = "%w wall, %u user + %s system = %t CPU [seconds] (%p%)\n",
               std::ostream& = std::cout);
    virtual ~base_timer();

public:
    inline void start();
    inline void stop();
    inline bool is_valid() const;
    double real_elapsed() const;
    double system_elapsed() const;
    double user_elapsed() const;
    inline const char* clock_time() const;
    inline void pause();
    inline void resume();

public:
    void report() const { this->report(m_os); }
    void report(std::ostream&) const;

protected:
    void parse_format();

protected:
    short m_places;
    std::string m_format_string;
    std::string m_output_format;
    std::ostream& m_os;
    vpos_t m_format_positions;

protected:
#ifndef SWIG
    struct timing
    {
        clock_t m_start_real_time;
        clock_t m_end_real_time;
        tms m_start_times;
        tms m_end_times;
    };
#endif

protected:
    mutable bool m_valid_times;
    timing tmain;
    timing twait;
    std::vector<timing> twait_history;

};

//----------------------------------------------------------------------------//
// Print timer status n std::ostream
static inline
std::ostream& operator << (std::ostream& os, const base_timer& t)
{
    bool restart = !t.is_valid();
    if(restart)
        const_cast<base_timer&>(t).pause();
    t.report(os);
    if(restart)
        const_cast<base_timer&>(t).resume();

    return os;
}
//----------------------------------------------------------------------------//
inline
double base_timer::real_elapsed() const
{
    if (!m_valid_times)
    {
        throw std::runtime_error("base_timer::real_elapsed() - InvalidCondition"
                                 " base_timer not stopped or times not recorded"
                                 "!");
    }
    double diff = tmain.m_end_real_time - tmain.m_start_real_time;
    return diff/sysconf(_SC_CLK_TCK);
}
//----------------------------------------------------------------------------//
inline
double base_timer::system_elapsed() const
{
    if (!m_valid_times)
    {
        throw std::runtime_error("base_timer::system_elapsed() - "
                                 "InvalidCondition: base_timer not stopped or "
                                 "times not recorded!");
    }
    double diff = tmain.m_end_times.tms_stime - tmain.m_start_times.tms_stime;
    for(unsigned i = 0; i < twait_history.size(); ++i)
        diff -= (twait_history[i].m_end_times.tms_stime
                 - twait_history[i].m_start_times.tms_stime);

    return diff/sysconf(_SC_CLK_TCK);
}
//----------------------------------------------------------------------------//
inline
double base_timer::user_elapsed() const
{
    if (!m_valid_times)
    {
        throw std::runtime_error("base_timer::user_elapsed() - InvalidCondition"
                                 ": base_timer not stopped or times not "
                                 "recorded!");
    }
    double diff = tmain.m_end_times.tms_utime - tmain.m_start_times.tms_utime;
    for(unsigned i = 0; i < twait_history.size(); ++i)
        diff -= (twait_history[i].m_end_times.tms_utime
                 - twait_history[i].m_start_times.tms_utime);

    return diff/sysconf(_SC_CLK_TCK);
}
//----------------------------------------------------------------------------//
inline
void base_timer::start()
{
    m_valid_times=false;
    tmain.m_start_real_time = times(&tmain.m_start_times);
}
//----------------------------------------------------------------------------//
inline
void base_timer::pause()
{
    m_valid_times=false;
    twait.m_start_real_time = times(&twait.m_start_times);
}
//----------------------------------------------------------------------------//
inline
void base_timer::stop()
{
    tmain.m_end_real_time = times(&tmain.m_end_times);
    m_valid_times=true;
}
//----------------------------------------------------------------------------//
inline
void base_timer::resume()
{
    twait.m_end_real_time = times(&twait.m_end_times);
    twait_history.push_back(twait);
    twait.m_end_real_time = twait.m_start_real_time;
    m_valid_times=true;
}
//----------------------------------------------------------------------------//
inline
bool base_timer::is_valid() const
{
    return m_valid_times;
}
//----------------------------------------------------------------------------//
inline const char* base_timer::clock_time() const
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    return asctime (timeinfo);
}

} // namespace details

//============================================================================//
// Main timer class
//============================================================================//

class timer : public details::base_timer
{
public:
    typedef base_timer                      base_type;
    typedef timer                           this_type;
    typedef std::string                     str_t;

public:
    timer(unsigned long prec = 3,
          std::string begin = "[ ",
          std::string close = " ]");
    timer(std::string, std::string close = "");
    virtual ~timer();

public:
    static std::string default_format;
    static unsigned long default_precision;

public:
    timer& stop_and_return() { this->stop(); return *this; }
    str_t begin() const { return m_begin; }
    str_t close() const { return m_close; }

protected:
    str_t m_begin;
    str_t m_close;
};

//----------------------------------------------------------------------------//

} // namespace timer

//----------------------------------------------------------------------------//

#endif // !defined(WIN32)

#endif // timer_hh_
