//
//
//
//
//
// created by jrmadsen on Mon Jul 13 14:17:08 2015
//
//
//
//


#ifndef cache_line_size_hh_
#define cache_line_size_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module cache_line_size
%{
    #include "cache_line_size.hh"
%}
#endif
//----------------------------------------------------------------------------//

// Author: Nick Strupat
// Date: October 29, 2010
// Returns the cache line size (in bytes) of the processor, or 0 on failure

#include <stddef.h>

namespace mad { namespace cache { static size_t cache_line_size(); } }

#if defined(__APPLE__)

#include <sys/sysctl.h>

static size_t mad::cache::cache_line_size()
{
    size_t line_size = 0;
    size_t sizeof_line_size = sizeof(line_size);
    sysctlbyname("hw.cachelinesize", &line_size, &sizeof_line_size, 0, 0);
    return line_size;
}

#elif defined(_WIN32)

#include <stdlib.h>
#include <windows.h>
static size_t mad::cache::cache_line_size()
{
    size_t line_size = 0;
    DWORD buffer_size = 0;
    DWORD i = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
    GetLogicalProcessorInformation(&buffer[0], &buffer_size);

    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            line_size = buffer[i].Cache.LineSize;
            break;
        }
    }

    free(buffer);
    return line_size;
}

#elif defined(__linux__) || defined(SWIG)

#include <stdio.h>
static inline size_t mad::cache::cache_line_size()
{
    FILE * p = 0;
    p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
    int i = 32;
    int ret = -1;
    if (p)
    {
        ret = fscanf(p, "%d", &i);
        fclose(p);
        if(ret != 1)
            ret = printf("Unable to read cache line size\n");
    }
    return i;
}

#else
#error Unrecognized platform
#endif

//----------------------------------------------------------------------------//


#endif
