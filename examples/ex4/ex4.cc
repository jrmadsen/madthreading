//
//
//	Serial example (for comparison with MT examples)
//
//


#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <iostream>
#include <iomanip>
#include <inttypes.h>
#include <cstdint>
#include <cpuid.h>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <x86intrin.h>

#include <madthreading/utility/timer.hh>
#include <madthreading/allocator/aligned_allocator.hh>

using namespace std;

#include "../Common.hh"

typedef uint32_t uint32;
typedef uint64_t uint64;

#define nsize 8
#define lsize 2
#define SLEEP_TIME 1

#define _wid  8
#define _prec 2

//----------------------------------------------------------------------------//
/** 256-bit data structure */
union W256_T
{
    __m256i  si;
    __m256d  sd;
    uint64  u64[4];
    uint32  u32[8];
    double    d[4];

    W256_T() { memset(this, 0, sizeof(*this)); }
};// __attribute__ ((aligned(64)));

//----------------------------------------------------------------------------//
/** 256-bit data structure */
union W256_FAKE_T
{
    uint64  si;
    double  sd;
    uint64 u64;
    uint32 u32;
    double   d;

    W256_FAKE_T() { memset(this, 0, sizeof(*this)); }
};// __attribute__ ((aligned(64)));

/** 256-bit data type */
typedef union W256_T        w256_t;
typedef union W256_FAKE_T   f256_t;
//----------------------------------------------------------------------------//


class tv_vec
{
public:
    typedef uint64  size_type;
    typedef w256_t  array_type;

    size_type   m_size;
    array_type* m_data;

    tv_vec(uint64 n)
    : m_size(n/4), m_data(nullptr)
    {
        m_data = static_cast<array_type*>(
                     mad::aligned_alloc(m_size * sizeof(array_type),
                                        mad::SIMD_ALIGN));
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = _mm256_set1_pd(0.0);
    }

    tv_vec(uint64 n, const double& val)
    : m_size(n/4), m_data(nullptr)
    {
        m_data = static_cast<array_type*>(
                     mad::aligned_alloc(m_size * sizeof(array_type),
                                        mad::SIMD_ALIGN));
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = _mm256_set1_pd(val);
    }

    ~tv_vec()
    {
        mad::aligned_free((void*) m_data);
    }

    tv_vec& operator=(const double& val)
    {
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = _mm256_set1_pd(val);
        return *this;
    }

    inline
    tv_vec& operator+=(const double& val)
    {
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = _mm256_add_pd(m_data[i].sd, _mm256_set1_pd(val));
        return *this;
    }

    inline
    tv_vec& operator+=(const tv_vec& val)
    {
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = _mm256_add_pd(m_data[i].sd, val.m_data[i].sd);
        return *this;
    }

    std::string str() const
    {
        std::stringstream ss;
        ss.precision(_prec);
        ss << std::scientific;
        ss << "(";
        ss << std::setw(_wid) << m_data[0].d[0] << ", ";
        ss << "... , ";
        ss << std::setw(_wid) << m_data[m_size-1].d[3] << ") ";
        return ss.str();
    }

};

//============================================================================//

class tv_array
{
public:
    typedef uint64  size_type;
    typedef f256_t  array_type;

    size_type   m_size;
    array_type* m_data;

    tv_array(uint64 n)
    : m_size(n), m_data(new array_type[m_size])
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = 0.0;
    }

    tv_array(uint64 n, const double& val)
    : m_size(n), m_data(new array_type[m_size])
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = val;
    }

    ~tv_array()
    {
        delete [] m_data;
    }

    tv_array& operator=(const double& val)
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd = val;
        return *this;
    }

    inline
    tv_array& operator+=(const double& val)
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd += val;
        return *this;
    }

    inline
    tv_array& operator+=(const tv_array& val)
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].sd += val.m_data[i].sd;
        return *this;
    }

    std::string str() const
    {
        std::stringstream ss;
        ss.precision(_prec);
        ss << std::scientific;
        ss << "(";
        ss << std::setw(_wid) << m_data[0].d << ", ";
        ss << "... , ";
        ss << std::setw(_wid) << m_data[m_size-1].d << ") ";
        return ss.str();
    }
};

//============================================================================//

int main(int argc, char** argv)
{
    uint64_t num_steps = GetEnvNumSteps(1000000000UL);
    double step = 1.0/static_cast<double>(num_steps);
    uint32 size = 24;
    if(argc > 1)
        size = atoi(argv[1]);

    std::cout << "Number of steps: " << num_steps << "..." << std::endl;
    //========================================================================//
    {
        timer::timer t;
        tv_vec sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
            sum += (static_cast<double>(i)-0.5)*step;
        report(num_steps, sum.str(), t.stop_and_return(), "intrin (double)");
    }
    //========================================================================//
    {
        timer::timer t;
        tv_array sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
            sum += (static_cast<double>(i)-0.5)*step;
        report(num_steps, sum.str(), t.stop_and_return(), "array (double)");
    }
    //========================================================================//
    tv_vec   vincr = tv_vec(size, 0.0);
    tv_array aincr = tv_array(size, 0.0);
    //========================================================================//
    {
        timer::timer t;
        tv_vec sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
        {
            vincr = static_cast<double>(i-0.5)*(step/3.0);
            sum += vincr;
        }
        report(num_steps, sum.str(), t.stop_and_return(), "intrin (tv_vec)");
    }
    //========================================================================//
    {
        timer::timer t;
        tv_array sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
        {
            aincr = static_cast<double>(i-0.5)*(step/3.0);
            sum += aincr;
        }
        report(num_steps, sum.str(), t.stop_and_return(), "array (tv_array)");
    }
    //========================================================================//


}

//============================================================================//

