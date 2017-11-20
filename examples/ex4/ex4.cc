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
#include "../Common.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

using namespace std;

typedef uint32_t uint32;
typedef uint64_t uint64;

#define _wid  8
#define _prec 2

//----------------------------------------------------------------------------//
/** 256-bit data structure */
struct alignas(64) W256_T
{
    typedef __m256d intrin;
    union
    {
        intrin  pd;             // packed doubles
        double  d[4];
        struct { double dx, dy, dz, dt; };
    };

    W256_T()
    {
      pd = _mm256_set1_pd(0.0);
    }

    W256_T(const intrin& _pd)
    {
      pd = _pd;
    }

    W256_T(intrin&& _pd)
    {
      pd = std::move(_pd);
    }

};// __attribute__ ((aligned(64)));

//----------------------------------------------------------------------------//
/** 256-bit data structure */
union W256_FAKE_T
{
    double  pd;
    double   d;

    W256_FAKE_T()
    {
      void* mem = (void*) this;
      size_t size = sizeof(*this);
      int ret = posix_memalign(&mem, mad::SIMD_WIDTH, size);
      if (ret != 0)
      {
          std::ostringstream o;
          o << "cannot allocate " << size
            << " bytes of memory with alignment " << mad::SIMD_WIDTH;
          throw std::runtime_error(o.str().c_str());
      }
      memset(mem, 0, size);
    }
};// __attribute__ ((aligned(64)));

/** 256-bit data type */
typedef struct W256_T        w256_t;
typedef union  W256_FAKE_T   f256_t;
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
                                        mad::SIMD_WIDTH));
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd = _mm256_set1_pd(0.0);
    }

    tv_vec(uint64 n, const double& val)
    : m_size(n/4), m_data(nullptr)
    {
        m_data = static_cast<array_type*>(
                     mad::aligned_alloc(m_size * sizeof(array_type),
                                        mad::SIMD_WIDTH));
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd = _mm256_set1_pd(val);
    }

    ~tv_vec()
    {
        mad::aligned_free((void*) m_data);
    }

    tv_vec& operator=(const double& val)
    {
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd = _mm256_set1_pd(val);
        return *this;
    }

    inline
    tv_vec& operator+=(const double& val)
    {
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd = _mm256_add_pd(m_data[i].pd, _mm256_set1_pd(val));
        return *this;
    }

    inline
    tv_vec& operator+=(const tv_vec& val)
    {
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd = _mm256_add_pd(m_data[i].pd, val.m_data[i].pd);
        return *this;
    }

    double&       operator[](const uint32& i)       { return m_data[i/4].d[i%3]; }
    const double& operator[](const uint32& i) const { return m_data[i/4].d[i%3]; }

    std::string str() const
    {
        std::stringstream ss;
        ss.precision(_prec);
        ss << std::scientific;
        ss << "(";
        ss << std::setw(_wid) << (*this)[0] << ", ";
        ss << "... , ";
        ss << std::setw(_wid) << (*this)[4*m_size-1] << ") ";
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
            m_data[i].pd = 0.0;
    }

    tv_array(uint64 n, const double& val)
    : m_size(n), m_data(new array_type[m_size])
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd = val;
    }

    ~tv_array()
    {
        delete [] m_data;
    }

    tv_array& operator=(const double& val)
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd = val;
        return *this;
    }

    inline
    tv_array& operator+=(const double& val)
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd += val;
        return *this;
    }

    inline
    tv_array& operator+=(const tv_array& val)
    {
    #pragma omp simd
        for(uint32 i = 0; i < m_size; ++i)
            m_data[i].pd += val.m_data[i].pd;
        return *this;
    }

    double&       operator[](const uint32& i)       { return m_data[i].d; }
    const double& operator[](const uint32& i) const { return m_data[i].d; }

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

void relative_compute_time(const double& t1, const double& t2,
                           std::ostream& os = std::cout)
{
    using namespace std;
    double _d = ((t2-t1)/t1)*100.0;
    os << " " << setw(35) << "Relative compute time" << ":  ("
       << setprecision(4) << t2 << "s - "
       << setprecision(4) << t1 << "s)/("
       << setprecision(4) << t1 << "s) * 100 = "
       << setprecision(2) << fixed << _d << "%"
       << endl << endl;
    os.unsetf(ios::fixed);
}

//============================================================================//

int main(int argc, char** argv)
{
    uint64_t num_steps = GetEnvNumSteps(1000000000UL);
    double step = 1.0/static_cast<double>(num_steps);
    uint32 size = 24;
    if(argc > 1)
        size = atoi(argv[1]);

    std::cout << "Number of steps: " << num_steps << "..." << std::endl;
    double tw1, tw2, tw3, tw4;

    //========================================================================//
    {
        mad::util::timer t;
        tv_vec sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
            sum += (static_cast<double>(i)-0.5)*step;
        report(num_steps, sum.str(), t.stop_and_return(), "intrin (double)");
        tw1 = t.real_elapsed();
    }
    //========================================================================//
    {
        mad::util::timer t;
        tv_array sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
            sum += (static_cast<double>(i)-0.5)*step;
        report(num_steps, sum.str(), t.stop_and_return(), "array (double)");
        tw2 = t.real_elapsed();
    }
    //========================================================================//
    relative_compute_time(tw1, tw2);
    //========================================================================//
    tv_vec   vincr = tv_vec(size, 0.0);
    tv_array aincr = tv_array(size, 0.0);
    //========================================================================//
    {
        mad::util::timer t;
        tv_vec sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
        {
            vincr = static_cast<double>(i-0.5)*(step/3.0);
            sum += vincr;
        }
        report(num_steps, sum.str(), t.stop_and_return(), "intrin (tv_vec)");
        tw3 = t.real_elapsed();
    }
    //========================================================================//
    {
        mad::util::timer t;
        tv_array sum(size, 0.0);
        for(uint64_t i = 0; i < num_steps; ++i)
        {
            aincr = static_cast<double>(i-0.5)*(step/3.0);
            sum += aincr;
        }
        report(num_steps, sum.str(), t.stop_and_return(), "array (tv_array)");
        tw4 = t.real_elapsed();
    }
    //========================================================================//
    relative_compute_time(tw3, tw4);
    //========================================================================//

}

//============================================================================//

#pragma GCC diagnostic pop
