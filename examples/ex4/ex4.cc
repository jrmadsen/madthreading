//
//
//	Serial example (for comparison with MT examples)
//
//


#ifdef USE_OPENMP
    #include <omp.h>
    #define SIMD omp simd
#else
    #define SIMD
#endif

#include <iostream>
#include <iomanip>

#include <madthreading/types.hh>
#include <madthreading/timer.hh>
#include <madthreading/threading/thread_manager.hh>
#include <madthreading/vectorization/vectorization_typedefs.hh>
#include <madthreading/allocator/allocator.hh>
#include <chrono>
#include <thread>

using namespace std;
using namespace mad;

#include "../Common.hh"

#define nsize 3
#define SLEEP_TIME 1

class tv : public mad::Allocator
{
public:
    tv()
    {
        m_mask = 0xFF;
        for(int i = 0; i < nsize; ++i)
            m_data.d[i] = double(0.0);
    }

    tv(const double& val)
    {
        m_mask = 0xFF;
        m_data.sd = _mm256_set1_pd(val);
    }

    tv(const double& val1, const double& val2, const double& val3)
    {
        m_mask = 0xFF;
        double rhs_array[nsize] = { val1, val2, val3 };
        for(int i = 0; i < nsize; ++i)
            m_data.d[i] = rhs_array[i];
    }

    tv(const tv& rhs)
    {
        m_mask = 0xFF;
        for(int i = 0; i < nsize; ++i)
            m_data.d[i] = rhs.m_data.d[i];
    }

    ~tv() {}

    tv& operator=(const tv& rhs)
    {
        if(this != &rhs)
        {
            m_mask = 0xFF;
            for(int i = 0; i < nsize; ++i)
                m_data.d[i] = rhs.m_data.d[i];
        }
        return *this;
    }

    double&       operator[](const int& i)       { return m_data.d[i%3]; }
    const double& operator[](const int& i) const { return m_data.d[i%3]; }

    inline
    tv& loop(const double& val)
    {
        //std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME));
        m_data.sd = _mm256_add_pd(m_data.sd, _mm256_set1_pd(val));
        return *this;
    }

    inline
    tv& indiv(const double& val)
    {
        //std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME));
        (*this)[0] += val;
        (*this)[1] += val;
        (*this)[2] += val;
        return *this;
    }

    inline
    tv& loop(const tv& val)
    {
        //std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME));
        m_data.sd = _mm256_add_pd(m_data.sd, val.m_data.sd);
        return *this;
    }

    inline
    tv& indiv(const tv& val)
    {
        //std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME));
        (*this)[0] += val[0];
        (*this)[1] += val[1];
        (*this)[2] += val[2];
        return *this;
    }

    std::string str() const
    {
        std::stringstream ss;
        ss.precision(4);
        ss << std::scientific;
        //ss << std::fixed;
        ss << "("
           << std::setw(10) << m_data.d[0] << ", "
           << std::setw(10) << m_data.d[1] << ", "
           << std::setw(10) << m_data.d[2] << ")";
        return ss.str();
    }

private:
    vec::mask8_t    m_mask;
    vec::w256_t     m_data;

};

//============================================================================//

int main(int argc, char** argv)
{
    ulong_type num_steps = GetEnvNumSteps(5000000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);

    tmcout << "Number of steps: " << num_steps << "..." << std::endl;
    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps; ++i)
            sum.indiv((static_cast<double>(i)-0.5)*step);
        report(num_steps, sum.str(), t.stop_and_return(),
               string(argv[0]) + " - double");
    }
    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps; ++i)
            sum.loop((static_cast<double>(i)-0.5)*step);
        report(num_steps, sum.str(), t.stop_and_return(),
               string(argv[0]) + " - SIMD (double)");
    }
    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps; ++i)
            sum.indiv(tv(static_cast<double>(i-0.5)*(step/3.0)));
        report(num_steps, sum.str(), t.stop_and_return(),
               string(argv[0]) + " - tv");
    }
    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps; ++i)
            sum.loop(tv(static_cast<double>(i-0.5)*(step/3.0)));
        report(num_steps, sum.str(), t.stop_and_return(),
               string(argv[0]) + " - SIMD (tv)");
    }
    //========================================================================//


}

//============================================================================//

