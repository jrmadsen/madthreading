#
# Modified/Copied from example in Geant4 version 9.6.2:
#
# http://geant4.web.cern.ch/geant4/
#
# - MakeRules_cxx
# Sets the default make rules for a CXX build, specifically the
# initialization of the compiler flags on a platform and compiler
# dependent basis
#
# Settings for each compiler are handled in a dedicated function.
# Whilst we only handle GNU, Clang and Intel, these are sufficiently
# different in the required flags that individual handling is needed.
#



#-----------------------------------------------------------------------
# macro for adding flags to variable
macro(add_flag _VAR _FLAG)
    set(${_VAR} "${${_VAR}} ${_FLAG}")
endmacro()

#-----------------------------------------------------------------------
# DEFAULT FLAG SETTING
#-----------------------------------------------------------------------
# GNU C++ or LLVM/Clang Compiler on all(?) platforms
# NB: At present, only identifies clang correctly on CMake > 2.8.1
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    set(_default_cxx_flags  "-Wno-deprecated -Wno-unused-function")
    set(_verbose_cxx_flags  "-Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wshadow -pipe")
    set(_extra_cxx_flags    "-pedantic -Wno-non-virtual-dtor -Wno-long-long -Wno-variadic-macros")
    set(_fast_flags         "")
    set(_fast_verb_flags    "")
    if(CMAKE_COMPILER_IS_GNUCXX)
        add_flag(_default_cxx_flags "-Wno-unused-local-typedefs")
        add_flag(_rwdi_flags    "-fno-expensive-optimizations")
        add_flag(_fast_flags    "-ftree-vectorize -ftree-loop-vectorize")
        #add_flag(_fast_flags    "-fopt-info-vec-optimized")
    else()
        add_flag(_default_cxx_flags "-Qunused-arguments")
    endif()

    set(CMAKE_CXX_FLAGS_INIT                "-Wall ${_default_cxx_flags}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "-g -DDEBUG ${_verbose_cxx_flags}")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG ${_fast_flags}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g ${_fast_flags} ${_fwdi_flags}")
    set(CMAKE_CXX_FLAGS_TESTRELEASE_INIT    "-g -DDEBUG_VERBOSE -DFPE_DEBUG")
    set(CMAKE_CXX_FLAGS_MAINTAINER_INIT     "-g ${_verbose_cxx_flags} ${_extra_cxx_flags}")

endif()


#-----------------------------------------------------------------------
# MSVC - all (?) versions
# I don't understand VS flags at all.... Guess try the CMake defaults first
# and see what happens!
if(MSVC)

    # Hmm, WIN32-VC.gmk uses dashes, but cmake uses slashes, latter probably
    # best for native build.
    set(CMAKE_CXX_FLAGS_INIT "/GR /EHsc /Zm200 /nologo /D_CONSOLE /D_WIN32 /DWIN32 /DOS /DXPNET /D_CRT_SECURE_NO_DEPRECATE")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "/MDd /Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "/MD /O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "/MD /Os /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MD /O2 /Zi")

    # Extra modes
    set(CMAKE_CXX_FLAGS_TESTRELEASE_INIT "-MDd -Zi -DEBUG_VERBOSE")
    set(CMAKE_CXX_FLAGS_MAINTAINER_INIT "-MDd -Zi")

    # We may also have to set linker flags....
endif()


#-----------------------------------------------------------------------
# Intel C++ Compilers - all (?) platforms
#
# Sufficient id on all platforms?
if(CMAKE_CXX_COMPILER MATCHES "icpc.*|icc.*")

    set(_default_cxx_flags "-Wno-unknown-pragmas -Wno-deprecated -simd")
    set(_extra_cxx_flags "-Wno-non-virtual-dtor -Wpointer-arith -Wwrite-strings -fp-model precise")

    #add_flag(_default_cxx_flags "-msse2")
    add_flag(_default_cxx_flags "-m64")
    #add_flag(_default_cxx_flags "-march=core-avx-i")
    add_flag(_default_cxx_flags "-xHOST")
    add_flag(_default_cxx_flags "-cxxlib-nostd")
    #add_flag(_default_cxx_flags "-vecabi=gcc")
    #add_flag(_default_cxx_flags "-ipo")
    #add_flag(_default_cxx_flags "-no-gcc-include-dir")
    #add_flag(_default_cxx_flags "-gcc-sys")
    #add_flag(_default_cxx_flags "-no-icc")
    add_flag(_default_cxx_flags "-no-gcc")
    #add_flag(_default_cxx_flags "-nostdinc++")
    #add_flag(_default_cxx_flags "-X")

    set(CMAKE_CXX_FLAGS_INIT "-w1 ${_default_cxx_flags}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Ofast -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

    # Extra modes
    set(CMAKE_CXX_FLAGS_TESTRELEASE_INIT "-g -DEBUG_VERBOSE")
    set(CMAKE_CXX_FLAGS_MAINTAINER_INIT "-g")

    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS "-i-dynamic -limf")
endif()


#-----------------------------------------------------------------------
# Ye Olde *NIX/Compiler Systems
# NB: *NOT* Supported... Only provided as legacy.
# None are tested...
# Whilst these use flags taken from existing  setup, may want to see if
# CMake defaults on these platforms are good enough...
#
if(UNIX AND NOT CMAKE_COMPILER_IS_GNUCXX)

    #---------------------------------------------------------------------
    # IBM xlC compiler
    #
    if(CMAKE_CXX_COMPILER MATCHES "xlC")
    set(CMAKE_CXX_FLAGS_INIT "")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -qdbextra -qcheck=all -qfullpath -qtwolink -+")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2 -qtwolink -+")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-O2 -qtwolink -+")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -qdbextra -qcheck=all -qfullpath -qtwolink -+")
    endif()

    #---------------------------------------------------------------------
    # HP aC++ Compiler
    #
    if(CMAKE_CXX_COMPILER MATCHES "aCC")
    set(CMAKE_CXX_FLAGS_INIT "+DAportable +W823")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "+O2 +Onolimit")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-O3 +Onolimit")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O3 +Onolimit -g")
    endif()

    #---------------------------------------------------------------------
    # IRIX MIPSpro CC Compiler
    #
    if(CMAKE_CXX_COMPILER MATCHES "CC" AND CMAKE_SYSTEM_NAME MATCHES "IRIX")
    set(CMAKE_CXX_FLAGS_INIT "-ptused -DSOCKET_IRIX_SOLARIS")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O -OPT:Olimit=5000")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-O -OPT:Olimit=5000")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O -OPT:Olimit=5000 -g")
    endif()

    #---------------------------------------------------------------------
    # SunOS CC Compiler
    # - CMake may do a reasonable job on its own here...
endif()

#-----------------------------------------------------------------------
# BUILD_CXXSTD
# Choose C++ Standard to build against, if supported.
# Mark as advanced because most users will not need it.
include(ConfigureCXXSTD)
set(CMAKE_CXX_FLAGS "-std=${BUILD_CXXSTD} ${CMAKE_CXX_FLAGS}")
