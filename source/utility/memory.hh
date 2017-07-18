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
// created by jrmadsen on Thu Jun 26 14:52:42 2014
//
//
//
//


#ifndef memory_hh_
#define memory_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module memory
%{
    #include "memory.hh"
%}
#endif
//----------------------------------------------------------------------------//

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <limits>
#include <climits>
#include <new>

#include "macros.hh"

//============================================================================//

namespace mad
{

namespace memory
{

    ////////////////////////////////////////////////////////////////////////////
    //
    // process_mem_usage(double &, double &) - takes two doubles by reference,
    // attempts to read the system-dependent data for a process' virtual memory
    // size and resident set size, and return the results in KB.
    //
    // On failure, returns 0.0, 0.0

    static inline
    void process_mem_usage(std::size_t& vm_usage, std::size_t& resident_set)
    {
       using std::ios_base;
       using std::ifstream;
       using std::string;

       vm_usage     = 0;
       resident_set = 0;

       // 'file' stat seems to give the most reliable results
       //
       ifstream stat_stream("/proc/self/stat", ios_base::in);

       if(!stat_stream) { return; }

       // dummy vars for leading entries in stat that we don't care about
       //
       string pid, comm, state, ppid, pgrp, session, tty_nr;
       string tpgid, flags, minflt, cminflt, majflt, cmajflt;
       string utime, stime, cutime, cstime, priority, nice;
       string O, itrealvalue, starttime;

       // the two fields we want
       //
       unsigned long vsize;
       long rss;

       stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                   >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                   >> utime >> stime >> cutime >> cstime >> priority >> nice
                   >> O >> itrealvalue >> starttime >> vsize >> rss;
       // don't care about the rest

       stat_stream.close();

       // in case x86-64 is configured to use 2MB pages
       std::size_t page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
       vm_usage     = vsize / 1024;
       resident_set = rss * page_size_kb;
    }

    static inline
    std::size_t getHeapPss()
    {
        using std::ios_base;
        using std::ifstream;
        using std::string;
        using std::size_t;

        size_t pss = 0;

        ifstream stat_stream("/proc/self/smaps", ios_base::in);

        if(!stat_stream)
            return 0;

        bool include = false;
        bool got_heap = false;
        while(!stat_stream.eof())
        {
            string field;
            stat_stream >> field;

            if(field.length() == 4 && field[3] == 'p')
                include = false;

            if(field == "[heap]")
                include = true;

            if(include && field == "Pss:")
            {
                string unit;
                size_t amt;
                stat_stream >> amt >> unit;

                if(stat_stream.eof())
                    break;

                for(unsigned i = 0; i < unit.length(); ++i)
                    unit[i] = tolower(unit[i]);

                if(unit == "kb")
                    pss += amt;
                else if(unit == "mb")
                    pss += amt * 1024;
                else if(unit == "gb")
                    pss += amt * 1024 * 1024;

                got_heap = true;
            }

            if(got_heap && !include)
                break;
        }

        stat_stream.close();

        return pss;
    }
}

/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || \
    (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || \
    defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>

#elif defined(__linux__) || defined(__linux) || defined(linux) || \
    defined(__gnu_linux__)
#include <stdio.h>

#endif

#else
#error "Cannot define getPeakRSS( ) or getCurrentRSS( ) for an unknown OS."
#endif


// RSS - Resident set size (physical memory use)

namespace memory
{
    /**
     * Returns the peak (maximum so far) resident set size (physical
     * memory use) measured in bytes, or zero if the value cannot be
     * determined on this OS.
     */
    static inline
    std::size_t getPeakRSS( )
    {
        using std::size_t;

    #if defined(_WIN32)
        /* Windows -------------------------------------------------- */
        PROCESS_MEMORY_COUNTERS info;
        GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
        return (size_t)info.PeakWorkingSetSize;

    #elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || \
    defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
        /* AIX and Solaris ------------------------------------------ */
        struct psinfo psinfo;
        int fd = -1;
        if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
            return (size_t)0L;      /* Can't open? */
        if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
        {
            close( fd );
            return (size_t)0L;      /* Can't read? */
        }
        close( fd );
        return (size_t)(psinfo.pr_rssize * 1024L);

    #elif defined(__unix__) || defined(__unix) || defined(unix) || \
    (defined(__APPLE__) && defined(__MACH__))
        /* BSD, Linux, and OSX -------------------------------------- */
        struct rusage rusage;
        getrusage( RUSAGE_SELF, &rusage );
    #if defined(__APPLE__) && defined(__MACH__)
        return (size_t)rusage.ru_maxrss;
    #else
        return (size_t)(rusage.ru_maxrss);
    #endif

    #else
        /* Unknown OS ----------------------------------------------- */
        return (size_t)0L;          /* Unsupported. */
    #endif
    }


    /**
     * Returns the current resident set size (physical memory use) measured
     * in bytes, or zero if the value cannot be determined on this OS.
     */
    static inline
    std::size_t getCurrentRSS( )
    {
        using std::size_t;

    #if defined(_WIN32)
        /* Windows -------------------------------------------------- */
        PROCESS_MEMORY_COUNTERS info;
        GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
        return (size_t)info.WorkingSetSize;

    #elif defined(__APPLE__) && defined(__MACH__)
        /* OSX ------------------------------------------------------ */
        struct mach_task_basic_info info;
        mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
        if ( task_info( mach_task_self( ), MACH_TASK_BASIC_INFO,
            (task_info_t)&info, &infoCount ) != KERN_SUCCESS )
            return (size_t)0L;      /* Can't access? */
        return (size_t)info.resident_size;

    #elif defined(__linux__) || defined(__linux) || defined(linux) || \
    defined(__gnu_linux__)
        /* Linux ---------------------------------------------------- */
        long rss = 0L;
        FILE* fp = NULL;
        if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
            return (size_t)0L;      /* Can't open? */
        if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
        {
            fclose( fp );
            return (size_t)0L;      /* Can't read? */
        }
        fclose( fp );
        return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE) / 1024;

    #else
        /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
        return (size_t)0L;          /* Unsupported. */
    #endif
    }

}

//----------------------------------------------------------------------------//

namespace memory
{
    //========================================================================//
    struct memory_usage
    {
        typedef memory_usage    this_type;
        typedef std::size_t     size_type;

        int evt_id;
        size_type stat_vm;
        size_type stat_rss;
        size_type psinfo_peak_rss;
        size_type psinfo_curr_rss;
        size_type smaps_heap_pss;

        memory_usage()
        : evt_id(0),
          stat_vm(0), stat_rss(0),
          psinfo_peak_rss(0), psinfo_curr_rss(0),
          smaps_heap_pss(0)
        { }

        memory_usage(int _id, size_type minus = 0)
        : evt_id(_id),
          stat_vm(0), stat_rss(0),
          psinfo_peak_rss(0), psinfo_curr_rss(0),
          smaps_heap_pss(0)
        {
            record();
            if(minus > 0)
            {
                stat_rss -= minus;
                if(minus < psinfo_curr_rss)
                    psinfo_curr_rss -= minus;
                else
                    psinfo_curr_rss = 1;

                if(minus < psinfo_peak_rss)
                    psinfo_peak_rss -= minus;
                else
                    psinfo_peak_rss = 1;

                if(minus < smaps_heap_pss)
                    smaps_heap_pss -= minus;
                else
                    smaps_heap_pss = 1;

            }
        }

        memory_usage(const memory_usage& rhs)
        : evt_id(rhs.evt_id),
          stat_vm(rhs.stat_vm), stat_rss(rhs.stat_rss),
          psinfo_peak_rss(rhs.psinfo_peak_rss),
          psinfo_curr_rss(rhs.psinfo_curr_rss),
          smaps_heap_pss(rhs.smaps_heap_pss)
        { }

        memory_usage& operator=(const memory_usage& rhs)
        {
            if(this != &rhs)
            {
                stat_vm = rhs.stat_vm;
                stat_rss = rhs.stat_rss;
                psinfo_peak_rss = rhs.psinfo_peak_rss;
                psinfo_curr_rss = rhs.psinfo_curr_rss;
                smaps_heap_pss = rhs.smaps_heap_pss;
            }
            return *this;
        }

        void record();

        friend bool operator<(const this_type& lhs, const this_type& rhs)
        { return lhs.evt_id < rhs.evt_id; }
        friend bool operator==(const this_type& lhs, const this_type& rhs)
        { return lhs.evt_id == rhs.evt_id; }
        friend bool operator!=(const this_type& lhs, const this_type& rhs)
        { return !(lhs.evt_id == rhs.evt_id); }
        friend bool operator>(const this_type& lhs, const this_type& rhs)
        { return rhs.evt_id < lhs.evt_id; }
        friend bool operator<=(const this_type& lhs, const this_type& rhs)
        { return !(lhs > rhs); }
        friend bool operator>=(const this_type& lhs, const this_type& rhs)
        { return !(lhs < rhs); }
        bool operator()(const this_type& rhs) const
        { return *this < rhs; }

        friend this_type operator-(const this_type& lhs, const this_type& rhs)
        {
            this_type r = lhs;
            r.stat_vm -= rhs.stat_vm;
            r.stat_rss -= rhs.stat_rss;

            if(rhs.psinfo_peak_rss < r.psinfo_peak_rss)
                r.psinfo_peak_rss -= rhs.psinfo_peak_rss;
            else
                r.psinfo_peak_rss = 1;

            if(rhs.psinfo_curr_rss < r.psinfo_curr_rss)
                r.psinfo_curr_rss -= rhs.psinfo_curr_rss;
            else
                r.psinfo_curr_rss = 1;

            if(rhs.smaps_heap_pss < r.smaps_heap_pss)
                r.smaps_heap_pss -= rhs.smaps_heap_pss;
            else
                r.smaps_heap_pss = 1;

            return r;
        }

        this_type& operator-=(const this_type& rhs)
        {
            stat_vm -= rhs.stat_vm;
            stat_rss -= rhs.stat_rss;
            psinfo_peak_rss -= rhs.psinfo_peak_rss;
            psinfo_curr_rss -= rhs.psinfo_curr_rss;
            smaps_heap_pss -= rhs.smaps_heap_pss;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const memory_usage& m)
        {
            if(m.psinfo_curr_rss == 0)
                const_cast<memory_usage&>(m).record();

            using std::setw;

            unsigned _w = 20;
            unsigned prec = os.precision();
            os.precision(2);

            os << "Memory usage: " << std::endl;

            os << "\t" << setw(_w) << "Virtual memory: " << std::fixed
               << setw(_w) << (m.stat_vm/(1024.0*1024.0)) << " GB" << std::endl;

            os << "\t" << setw(_w) << "Resident set: "
               << setw(_w) << (m.stat_rss/1024.0) << " MB" << std::endl;

            os << "\t" << setw(_w) << "Peak RSS: "
               << setw(_w) << (m.psinfo_peak_rss/1024.0) << " MB" << std::endl;

            os << "\t" << setw(_w) << "Current RSS: "
               << setw(_w) << (m.psinfo_curr_rss/1024.0) << " MB" << std::endl;

            os << "\t" << setw(_w) << "Heap allocation: "
               << setw(_w) << (m.smaps_heap_pss/1024.0) << " MB" << std::endl;

            os.unsetf(std::ios_base::fixed);
            os.precision(prec);
            return os;

        }
    };
    //========================================================================//

    inline
    void memory_usage::record()
    {
        // everything is kB
        memory::process_mem_usage(stat_vm, stat_rss);
        psinfo_peak_rss = memory::getPeakRSS();
        psinfo_curr_rss = memory::getCurrentRSS();
        smaps_heap_pss = memory::getHeapPss();
    }

    //========================================================================//

}

//----------------------------------------------------------------------------//

} // namespace mad

//============================================================================//

#endif
