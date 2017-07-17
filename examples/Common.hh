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

//----------------------------------------------------------------------------//

template <typename _Step_t, typename _Pi_t>
inline void report(_Step_t num_steps, _Pi_t pi, timer::timer& t,
                   std::string prefix = "",
                   std::ostream& os = std::cout)
{
    os.precision(8);
    os.setf(std::ios::fixed);
    os << " " << std::setw(40)
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

#endif
