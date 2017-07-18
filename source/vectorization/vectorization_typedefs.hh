//
//
//
// MIT License
// Copyright (c) 2017 Jonathan R. Madsen
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// created by jmadsen on Fri Jul  7 02:00:36 2017
//
//
//
//

#ifndef vectorization_typedefs_hh_
#define vectorization_typedefs_hh_

#include <immintrin.h>
#include <inttypes.h>
#include <cstdint>
#include <cpuid.h>
#include <string.h>
#include <stdio.h>

//----------------------------------------------------------------------------//
namespace mad
{
namespace vec
{

//----------------------------------------------------------------------------//
/** 128-bit data structure */
union W128_T
{
#ifdef HAS_AVX2
    __m128i  si;
    __m128d  sd;
#else
    uint64_t si[2];
    double   sd[2];
#endif
    uint64_t u64[2];
    uint32_t u32[4];
    double   d[2];

    W128_T() { memset(this, 0, sizeof(*this)); }
} __attribute__ ((aligned));

/** 128-bit data type */
typedef union W128_T w128_t;
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
/** 256-bit data structure */
union W256_T
{
#ifdef HAS_AVX2
    __m256i  si;
    __m256d  sd;
#else
    uint64_t si[4];
    double   sd[4];
#endif
    uint64_t u64[4];
    uint32_t u32[8];
    double   d[4];
    struct { double x, y, z, t; };

    W256_T() { memset(this, 0, sizeof(*this)); }
} __attribute__ ((aligned));

/** 256-bit data type */
typedef union W256_T w256_t;
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
/** 512-bit data structure */
union W512_T
{
#ifndef __INTEL_COMPILER
    __m512i  si;
    __m512d  sd;
#else
    uint64_t si[8];
    double   sd[8];
#endif
    uint64_t u64[8];
    uint32_t u32[16];
    double   d[8];

    W512_T() { memset(this, 0, sizeof(*this)); }
} __attribute__ ((aligned));

/** 512-bit data type */
typedef union W512_T w512_t;
//----------------------------------------------------------------------------//


#ifndef __INTEL_COMPILER
typedef __mmask8    mask8_t;
typedef __mmask16   mask16_t;
typedef __mmask32   mask32_t;
typedef __mmask64   mask64_t;
#endif

} // namespace vec
} // namespace mad
//----------------------------------------------------------------------------//

#endif
