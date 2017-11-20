//
//
//
//
//
// created by jmadsen on Sun Jan 15 15:56:02 2017
//
//
//
//

#ifndef common_hh_
#define common_hh_

#include <madthreading/utility/timer.hh>
#include <madthreading/utility/constants.hh>
#include <madthreading/utility/tools.hh>

#include <unistd.h>
#include <sys/times.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>
#include <ostream>
#include <istream>

#ifndef PI_EPSILON
#   define PI_EPSILON 1.0e-8
#endif

#ifndef do_pragma
#   define do_pragma(x) _Pragma(#x)
#endif

//============================================================================//
//  OpenMP
//============================================================================//

#if defined(USE_OPENMP) && !defined(__INTEL_COMPILER)
#   include <omp.h>
#   ifndef pragma_simd
#       define pragma_simd(args) do_pragma(omp simd args)
#   endif
#else
#   ifndef pragma_simd
#       define pragma_simd(args) {;}
#   endif
#endif

//----------------------------------------------------------------------------//

template <typename _Step_t, typename _Pi_t>
inline void report(_Step_t num_steps, _Pi_t pi, timer::timer& t,
                   std::string prefix = "",
                   std::ostream& os = std::cout)
{
    os.precision(8);
    os.setf(std::ios::fixed);
    os << " " << std::setw(35)
       << prefix << ((prefix.length() > 0) ? ": " : "")
       << " pi with "
       << num_steps << " steps is "
       << pi << " in ";
    t.report();
    os << std::endl;
    os.unsetf(std::ios::fixed);
}

//----------------------------------------------------------------------------//

template <typename _Size_t>
_Size_t GetEnvNumSteps(_Size_t _default = 500000000)
{
    char* env_nsteps;
    env_nsteps = getenv("NUM_STEPS");
    if(env_nsteps)
    {
        std::string str_nsteps = std::string(env_nsteps);
        std::istringstream iss(str_nsteps);
        _Size_t _n = 0;
        iss >> _n;
        return _n;
    }

    if(_default > 0)
        return _default;
    return 0;

}

//----------------------------------------------------------------------------//

namespace details
{

void fmt(std::ofstream& of, double d[3])
{
    std::stringstream oss;
    oss.precision(6);
    size_t w = 20;
    oss << std::fixed
        << std::setw(w) << d[0] << "  "
        << std::setw(w) << d[1] << "  "
        << std::setw(w) << d[2] << std::endl;
    of << oss.str();
}

}

//----------------------------------------------------------------------------//

template <typename _Step_t>
inline void report(_Step_t num_steps,
                   const double& pi, timer::timer& t,
                   std::string prefix = "",
                   std::ostream& os = std::cout)
{
    os.precision(8);
    double delta = (pi - mad::dat::PI);
    os << " " << std::setw(35)
       << prefix << ((prefix.length() > 0) ? ": " : "")
       << " pi with "
       << num_steps << " steps is "
       << std::fixed << pi
       << " (diff = " << std::setw(15) << std::scientific << delta
       << ")"
       << " in ";
    t.report();
    os << std::endl;

    mad::tools::mkdir("timing");
    std::stringstream iss;
    iss << "timing/" << prefix << ".txt";

    typedef double m_data_t;
    m_data_t m_wall[] = { 0.0, 0.0, 0.0 };
    m_data_t m_user[] = { 0.0, 0.0, 0.0 };
    m_data_t m_cput[] = { 0.0, 0.0, 0.0 };
    m_data_t m_perc[] = { 0.0, 0.0, 0.0 };

    {
        std::ifstream in(iss.str().c_str());
        if(in)
        {
            in >> m_wall[0] >> m_wall[1] >> m_wall[2];
            in >> m_user[0] >> m_user[1] >> m_user[2];
            in >> m_cput[0] >> m_cput[1] >> m_cput[2];
            in >> m_perc[0] >> m_perc[1] >> m_perc[2];
        }
        in.close();
    }

    m_wall[1] += 1.0;
    m_user[1] += 1.0;
    m_cput[1] += 1.0;
    m_perc[1] += 1.0;

    m_wall[0] += t.real_elapsed();
    m_user[0] += t.user_elapsed();
    m_cput[0] += t.system_elapsed() + t.user_elapsed();
    m_perc[0] += ((t.user_elapsed()+t.system_elapsed())/(t.real_elapsed())*100.0);

    m_wall[2] = m_wall[0] / m_wall[1];
    m_user[2] = m_user[0] / m_user[1];
    m_cput[2] = m_cput[0] / m_cput[1];
    m_perc[2] = m_perc[0] / m_perc[1];

    std::ofstream of(iss.str().c_str());


    if(of)
    {
        ::details::fmt(of, m_wall);
        ::details::fmt(of, m_user);
        ::details::fmt(of, m_cput);
        ::details::fmt(of, m_perc);
    }
    of.close();
}

//----------------------------------------------------------------------------//

#endif
