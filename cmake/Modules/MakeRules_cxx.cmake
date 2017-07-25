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



#------------------------------------------------------------------------------#
# macro for adding flags to variable
#------------------------------------------------------------------------------#
macro(add _VAR _FLAG)
    set(${_VAR} "${${_VAR}} ${_FLAG}")
endmacro()

#------------------------------------------------------------------------------#
# macro for finding the x86intrin.h header path that needs to be explicitly
# in the search path
#   - Otherwise, icc/icpc will use the GNU x86intrin.h header and
#     this header causes quite a bit of compilation problems
# - The Intel compiler has alot of links and not an easy structure to search
#   so this macro pretty much assumes the x86intrinc.h file will be in
#   an icc/ folder although (icpc/ and icl/) are also permitted)
# - When explicitly setting this folder, be sure to include icc/ or whatever
#   the parent directory is
#------------------------------------------------------------------------------#
macro(get_intel_intrinsic_include_dir)

    #--------------------------------------------------------------------------#
    if(NOT DEFINED INTEL_ICC_CPATH)
        get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" PATH)
        string(TOLOWER "${CMAKE_SYSTEM_NAME}" LSYSNAME)
        set(SYSNAME "${CMAKE_SYSTEM_NAME}")
        find_path(INTEL_ICC_CPATH
            NAMES icc/x86intrin.h icpc/x86intrin.h icl/x86intrin.h
            HINTS
                ENV INTEL_ICC_CPATH
                ${COMPILER_DIR}
                ${COMPILER_DIR}/..
                ${COMPILER_DIR}/../..
                ENV INTEL_ROOT
            PATHS
                ENV CPATH
            PATH_SUFFIXES
                include
                include/intel64
                include/ia32
                include
                ${SYSNAME}/include
                ${SYSNAME}/compiler
                ${SYSNAME}/compiler/include
                ${LSYSNAME}/include
                ${LSYSNAME}/compiler
                ${LSYSNAME}/compiler/include
            NO_SYSTEM_ENVIRONMENT_PATH
            DOC "Include path for the ICC Compiler (need to correctly compile intrinsics)")
    endif()
    #--------------------------------------------------------------------------#

    #--------------------------------------------------------------------------#
    macro(cat_intrin_file _VAR ARGS)
        set(_ICC_PATH ${_VAR})
        set(_FILE x86intrin.h)
        foreach(_arg ${ARGS} ${_FILE})
            set(_ICC_PATH "${_ICC_PATH}/${_arg}")
        endforeach()
    endmacro()
    #--------------------------------------------------------------------------#

    #--------------------------------------------------------------------------#
    if(NOT INTEL_ICC_CPATH)
        set(_expath "/opt/intel/compilers_and_libraries/linux/compiler/include/icc")
        set(_msg "INTEL_ICC_CPATH was not found! Please specify the path to \"x86intrin.h\"")
        add(_msg "in the Intel ICC compiler path. Using the GNU header for this file")
        add(_msg "typically results in a failure to compile.\nExample:\n")
        add(_msg "\t-D${_expath}")
        add(_msg "\nfor \"${_expath}/x86intrin.h\"\n")
        message(WARNING "${_msg}")
    else()
        foreach(_postfix icc icpc icl "")
            cat_intrin_file(${INTEL_ICC_CPATH} "${_postfix}")
            if(EXISTS "${_ICC_PATH}" AND NOT IS_DIRECTORY "${_ICC_PATH}")
                set(_ICC_PATH "${INTEL_ICC_CPATH}/${_postfix}")
                string(REGEX REPLACE "/$" "" _ICC_PATH "${_ICC_PATH}")
                INCLUDE_DIRECTORIES(${_ICC_PATH})
                break()
            endif()
        endforeach()
    endif()
    #--------------------------------------------------------------------------#

endmacro()

#------------------------------------------------------------------------------#
# DEFAULT FLAG SETTING
#------------------------------------------------------------------------------#
# GNU C++ or LLVM/Clang Compiler on all(?) platforms
# NB: At present, only identifies clang correctly on CMake > 2.8.1
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    set(_def_cxx  "-Wno-deprecated -Wno-unused-function")
    set(_verbose_cxx_flags  "-Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wshadow -pipe")
    set(_extra_cxx_flags    "-pedantic -Wno-non-virtual-dtor -Wno-long-long -Wno-variadic-macros")
    set(_fast_flags         "")
    set(_fast_verb_flags    "")
    if(CMAKE_COMPILER_IS_GNUCXX)
        add(_def_cxx "-Wno-unused-local-typedefs")
        add(_rwdi_flags    "-fno-expensive-optimizations")
        add(_fast_flags    "-ftree-vectorize -ftree-loop-vectorize")
        #add(_fast_flags    "-fopt-info-vec-optimized")
    else()
        add(_def_cxx "-Qunused-arguments")
    endif()

    set(CMAKE_CXX_FLAGS_INIT                "-Wall ${_def_cxx}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "-g -DDEBUG ${_verbose_cxx_flags}")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG ${_fast_flags}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g ${_fast_flags} ${_fwdi_flags}")
    set(CMAKE_CXX_FLAGS_TESTRELEASE_INIT    "-g -DDEBUG_VERBOSE -DFPE_DEBUG")
    set(CMAKE_CXX_FLAGS_MAINTAINER_INIT     "-g ${_verbose_cxx_flags} ${_extra_cxx_flags}")

endif()


#------------------------------------------------------------------------------#
# MSVC
#------------------------------------------------------------------------------#
if(MSVC)
    message(FATAL_ERROR "${PROJECT_NAME} does not support Windows")
endif()


#------------------------------------------------------------------------------#
# Intel C++ Compilers - all (?) platforms
#------------------------------------------------------------------------------#
# Sufficient id on all platforms?
if(CMAKE_CXX_COMPILER MATCHES "icpc.*|icc.*")

    set(_def_cxx "-Wno-unknown-pragmas -Wno-deprecated")
    set(_extra_cxx_flags "-Wno-non-virtual-dtor -Wpointer-arith -Wwrite-strings -fp-model precise")

    add(_def_cxx "-xHOST")
    #add(_def_cxx "-cxxlib-nostd")
    #add(_def_cxx "-vecabi=intel")
    add(_def_cxx "-ipo")
    #add(_def_cxx "-no-gcc-include-dir")
    #add(_def_cxx "-gcc-sys")
    #add(_def_cxx "-no-icc")
    #add(_def_cxx "-no-gcc")
    #add(_def_cxx "-nostdinc++")
    #add(_def_cxx "-X")

    #add(_def_cxx "-march=core-avx-i")
    #add(_def_cxx "-m64")
    #add(_def_cxx "-m32")
    #add(_def_cxx "-mmmx")
    #add(_def_cxx "-mdspr2")
    #add(_def_cxx "-mpaired-single")
    #add(_def_cxx "-maes")
    #add(_def_cxx "-mpclmul")
    #add(_def_cxx "-mfsgsbase")
    #add(_def_cxx "-mrdrnd")
    #add(_def_cxx "-msse4a")
    #add(_def_cxx "-mxop")
    #add(_def_cxx "-mfma4")
    #add(_def_cxx "-mlwp")
    #add(_def_cxx "-mbmi")
    #add(_def_cxx "-mbmi2")
    #add(_def_cxx "-mlzcnt")
    #add(_def_cxx "-mtbm")
    #add(_def_cxx "-m3dnow")
    #add(_def_cxx "-mrtm")
    #add(_def_cxx "-mavx")
    #add(_def_cxx "-H")
    add(_def_cxx "-w0")
    #add(_def_cxx "-v")
    add(_def_cxx "-qno-offload")
    add(_def_cxx "-use-intel-optimized-headers")

    get_intel_intrinsic_include_dir()

    # -cxxlib -gcc-name -gxx-name -fabi-version -no-gcc
    set(GCC $ENV{GCC})
    if("${GCC}" STREQUAL "")
        set(_msg "\nPlease specify the GCC and GXX environment variables")
        add(_msg "(GCC C and C++ compilers to use with the Intel compiler)\n")
        message(FATAL_ERROR "${_msg}")
    endif()

    get_filename_component(GDIR "${GCC}" DIRECTORY)
    get_filename_component(GDIR "${GDIR}" DIRECTORY)
    get_filename_component(GCC "${GCC}" NAME)
    set(GXX $ENV{GXX})
    get_filename_component(GXX "${GXX}" NAME)
    add(_def_cxx "-cxxlib=${GDIR} -gcc-name=${GCC} -gxx-name=${GXX}")

    set(CMAKE_CXX_FLAGS_INIT "${_def_cxx}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Ofast -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

    # Extra modes
    set(CMAKE_CXX_FLAGS_TESTRELEASE_INIT "-g -DEBUG_VERBOSE")
    set(CMAKE_CXX_FLAGS_MAINTAINER_INIT "-g")

    # Linker flags
    #set(CMAKE_EXE_LINKER_FLAGS "-i-dynamic -limf")
endif()


#-----------------------------------------------------------------------
# Ye Olde *NIX/Compiler Systems
# NB: *NOT* Supported... Only provided as legacy.
# None are tested...
# Whilst these use flags taken from existing  setup, may want to see if
# CMake defaults on these platforms are good enough...
#
#------------------------------------------------------------------------------#
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

