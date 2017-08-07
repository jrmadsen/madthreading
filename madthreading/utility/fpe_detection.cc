//
//
// created by jmadsen on Wed Oct 19 16:38:09 2016
//
//
//
//


#include "madthreading/utility/fpe_detection.hh"
#include <sstream>
#include <string>
#include <cstdlib>


#if (defined(__GNUC__) && !defined(__clang__)) // compatible compiler
#   if !defined(FPE_COMPAT_COMPILER)
#       define FPE_COMPAT_COMPILER
#   endif
#endif

#if (defined(__linux__) || defined(__MACH__)) // compatible operating system
#   if !defined(FPE_COMPAT_OS)
#       define FPE_COMPAT_OS
#   endif
#endif

#if defined(FPE_COMPAT_COMPILER) && defined(FPE_COMPAT_OS)
#   if !defined(FPE_AVAILABLE)
#       define FPE_AVAILABLE
#   endif
#endif

//============================================================================//

int GetEnvironmentVariable(const std::string& ID, int _default)
{
    char* env_var = std::getenv(ID.c_str());
    if(env_var)
    {
        std::string str_var = std::string(env_var);
        std::istringstream iss(str_var);
        int var = 0;
        iss >> var;
        return var;
    }
    // return default if not specified in environment
    return _default;
}

//============================================================================//

std::set<fpe> fpe_settings::fpe_default = { fpe::divbyzero, fpe::invalid,
                                            fpe::overflow                     };

//============================================================================//

std::set<fpe> fpe_settings::fpe_enabled = fpe_settings::fpe_default;

//============================================================================//

std::set<fpe> fpe_settings::fpe_disabled = { fpe::downward, fpe::inexact,
                                             fpe::tonearest, fpe::towardzero,
                                             fpe::upward, fpe::underflow      };

//============================================================================//

void insert_and_remove(const fpe& _type,            // fpe type
                       fpe_settings::fpe_set* _ins, // set to insert into
                       fpe_settings::fpe_set* _rem  // set to remove from
                       )
{
  _ins->insert(_type);
  auto itr = _rem->find(_type);
  if(itr != _rem->end())
    _rem->erase(itr);
}

//============================================================================//

void fpe_settings::enable(const fpe& _type)
{
  insert_and_remove(_type, &fpe_enabled, &fpe_disabled);
}

//============================================================================//

void fpe_settings::disable(const fpe& _type)
{
  insert_and_remove(_type, &fpe_disabled, &fpe_enabled);
}

//============================================================================//

void fpe_settings::check_environment()
{
    typedef std::pair<std::string, fpe> match_t;

    auto _list = { match_t("DIVBYZERO",     fpe::divbyzero),
                   match_t("INEXACT",       fpe::inexact),
                   match_t("INVALID",       fpe::invalid),
                   match_t("OVERFLOW",      fpe::overflow),
                   match_t("TONEAREST",     fpe::tonearest),
                   match_t("TOWARDZERO",    fpe::towardzero),
                   match_t("UNDERFLOW",     fpe::underflow),
                   match_t("UPWARD",        fpe::upward) };

    for(auto itr : _list)
    {
        int _enable = GetEnvironmentVariable("FPE_ENABLE_" + itr.first, 0);
        int _disable = GetEnvironmentVariable("FPE_DISABLE_" + itr.first, 0);

        if(_enable > 0)
            fpe_settings::enable(itr.second);
        if(_disable > 0)
            fpe_settings::disable(itr.second);
    }

    int _enable_all = GetEnvironmentVariable("FPE_ENABLE_ALL", 0);
    if(_enable_all > 0)
        for(const auto& itr : fpe_settings::fpe_disabled)
            fpe_settings::enable(itr);

    int _disable_all = GetEnvironmentVariable("FPE_DISABLE_ALL", 0);
    if(_disable_all > 0)
        for(const auto& itr : fpe_settings::fpe_enabled)
            fpe_settings::disable(itr);

}

//============================================================================//

std::string fpe_settings::str(const fpe& _type)
{
  std::stringstream ss;
  switch (_type)
  {
    case fpe::divbyzero:
      ss << "Divide by zero";
      break;
    case fpe::downward:
      ss << "Downward rounding mode detection";
      break;
    case fpe::inexact:
      ss << "Inexact";
      break;
    case fpe::invalid:
      ss << "Invalid (e.g. sqrt(-1))";
      break;
    case fpe::overflow:
      ss << "Overflow - number too large to represent";
      break;
    case fpe::tonearest:
      ss << "To nearest rounding mode detection";
      break;
    case fpe::towardzero:
      ss << "Toward zero rounding mode detection";
      break;
    case fpe::underflow:
      ss << "Underflow - number too small to represent";
      break;
    case fpe::upward:
      ss << "Upward rounding mode detection";
      break;
  }

  return ss.str();
}

//============================================================================//

std::string fpe_settings::str()
{

  std::stringstream ss;
  auto spacer = [&] () { return "    "; };

#if defined(FPE_AVAILABLE)

  ss << std::endl
     << spacer() << "FPE detection activated. Floating point exception settings:\n"
     << spacer() << "***  NOTE on rounding mode detection:                                        ***\n"
     << spacer() << "***   rounding mode does not affect implicit conversion casting,             ***\n"
     << spacer() << "***   const expressions (always nearest), or library calls (e.g. std::round) ***\n"
     << std::endl;

  ss << spacer() << "Enabled:" << std::endl;
  for(const auto& itr : fpe_enabled)
    ss << spacer() << spacer() << fpe_settings::str(itr) << std::endl;

  ss << "\n" << spacer() << "Disabled:" << std::endl;
  for(const auto& itr : fpe_disabled)
    ss << spacer() << spacer() << fpe_settings::str(itr) << std::endl;

#else

  ss << std::endl
     << spacer()
     << "FPE detection not available" << std::endl;

#endif

  return ss.str();
}

//============================================================================//
