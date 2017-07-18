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

#include "types.hh"
#include "timer.hh"
#include "allocator.hh"

using namespace std;
using namespace mad;

#define nsize 3

class tv : public mad::Allocator
{
public:
    tv()
    : m_array(new double[nsize]),
      m_x(m_array[0]), m_y(m_array[1]), m_z(m_array[2])
    {
        for(int i = 0; i < nsize; ++i)
            m_array[i] = double(0.0);
    }

    tv(const double& val)
    : m_array(new double[nsize]),
      m_x(m_array[0]), m_y(m_array[1]), m_z(m_array[2])
    {
        for(int i = 0; i < nsize; ++i)
            m_array[i] = val;
    }

    tv(const double& val1, const double& val2, const double& val3)
    : m_array(new double[nsize]),
      m_x(m_array[0]), m_y(m_array[1]), m_z(m_array[2])
    {
        double rhs_array[nsize] = { val1, val2, val3 };
        for(int i = 0; i < nsize; ++i)
            m_array[i] = rhs_array[i];
    }

    tv(const tv& rhs)
    : m_array(new double[nsize]),
      m_x(m_array[0]), m_y(m_array[1]), m_z(m_array[2])
    {
        for(int i = 0; i < nsize; ++i)
            m_array[i] = rhs.m_array[i];
    }

    ~tv() { delete [] m_array; }

    tv& operator=(const tv& rhs)
    {
        if(this != &rhs)
        {
            for(int i = 0; i < nsize; ++i)
                m_array[i] = rhs.m_array[i];
        }
        return *this;
    }

    double&       operator[](const int& i)       { return m_array[i%3]; }
    const double& operator[](const int& i) const { return m_array[i%3]; }

    inline
    tv& loop(const double& val)
    {
        for(int i = 0; i < nsize; ++i)
            m_array[i] += val;
        return *this;
    }

    inline
    tv& indiv(const double& val)
    {
        m_x += val;
        m_y += val;
        m_z += val;
        return *this;
    }

    //inline
    tv& loop(const tv& val)
    {
        //for(int i = 0; i < nsize; ++i)
        //	m_array[i] += val.m_array[i];
        m_array[0] += val.m_array[0];
        m_array[1] += val.m_array[1];
        m_array[2] += val.m_array[2];
        return *this;
    }

    inline
    tv& indiv(const tv& val)
    {
        m_x += val.m_x;
        m_y += val.m_y;
        m_z += val.m_z;
        return *this;
    }

    std::string str() const
    {
        std::stringstream ss;
        ss.precision(4);
        ss << std::scientific;
        //ss << std::fixed;
        ss << "("
           << std::setw(10) << m_x << ", "
           << std::setw(10) << m_y << ", "
           << std::setw(10) << m_z << ")";
        return ss.str();
    }

private:
    double* m_array;
    double& m_x;
    double& m_y;
    double& m_z;

};

//============================================================================//

int main(int argc, char** argv)
{
    ulong_type num_steps = timer::GetEnvNumSteps(5000000000UL);
    double_type step = 1.0/static_cast<double_type>(num_steps);

    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps; ++i)
            sum.indiv((static_cast<double>(i)-0.5)*step);
        timer::report(num_steps, sum.str(), t.stop_and_return(),
                      string(argv[0]) + " - double");
    }
    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps; ++i)
            sum.loop((static_cast<double>(i)-0.5)*step);
        timer::report(num_steps, sum.str(), t.stop_and_return(),
                      string(argv[0]) + " - SIMD (double)");
    }
    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps/5; ++i)
            sum.indiv(tv(static_cast<double>(i-0.5)*(step/3.0)));
        timer::report(num_steps, sum.str(), t.stop_and_return(),
                      string(argv[0]) + " - tv");
    }
    //========================================================================//
    {
        timer::timer t;
        tv sum(0.0);
        for(ulong_type i = 0; i < num_steps/5; ++i)
            sum.loop(tv(static_cast<double>(i-0.5)*(step/3.0)));
        timer::report(num_steps, sum.str(), t.stop_and_return(),
                      string(argv[0]) + " - SIMD (tv)");
    }
    //========================================================================//


}

//============================================================================//

