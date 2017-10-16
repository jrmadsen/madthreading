#
# - MakeRulesCXX
# Sets the default make rules for a CXX build, specifically the
# initialization of the compiler flags on a platform and compiler
# dependent basis
#
# Settings for each compiler are handled in a dedicated function.
# Whilst we only handle GNU, Clang and Intel, these are sufficiently
# different in the required flags that individual handling is needed.
#

include(Compilers)

#------------------------------------------------------------------------------#
# macro for adding flags to variable
#------------------------------------------------------------------------------#
macro(add _VAR _FLAG)
    set(${_VAR} "${${_VAR}} ${_FLAG}")
endmacro()

#------------------------------------------------------------------------------#
# MSVC
#------------------------------------------------------------------------------#
if(MSVC)
    message(FATAL_ERROR "${PROJECT_NAME} does not support Windows")
endif()

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
        add(_msg "\t-DINTEL_ICC_CPATH=${_expath}")
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
# GNU C++ or LLVM/Clang Compiler on all(?) platforms
#
if(CMAKE_CXX_COMPILER_IS_GNU OR CMAKE_CXX_COMPILER_IS_CLANG)

    set(_def_cxx     "-Wno-deprecated")
    add(_def_cxx     "-Wno-unused-parameter -fdiagnostics-color=always")
    set(_verb_flags  "-Wwrite-strings -Wpointer-arith -Woverloaded-virtual")
    add(_verb_flags  "-pedantic")
    set(_loud_flags  "-Wshadow -Wextra")
    set(_quiet_flags "-Wno-unused-function -Wno-unused-variable")
    if(CMAKE_CXX_COMPILER_IS_GNU)
        add(_def_cxx      "-Wno-unused-but-set-variable -Wno-unused-local-typedefs")
        add(_fast_flags   "-ftree-vectorize -ftree-loop-vectorize")
    else()
        add(_def_cxx "-Qunused-arguments")
        INCLUDE_DIRECTORIES("/usr/include/libcxxabi")
    endif()

    option(ENABLE_GCOV "Enable compilation flags for GNU coverage tool (gcov)" OFF)
    mark_as_advanced(ENABLE_GCOV)
    if(ENABLE_GCOV)
        add(_def_cxx "-fprofile-arcs -ftest-coverage")
    endif(ENABLE_GCOV)

    set(CMAKE_CXX_FLAGS_INIT                "-W -Wall ${_def_cxx}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g -DDEBUG ${_loud_flags}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG ${_quiet_flags}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-g -O2 ${_fast_flags}")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG ${_fast_flags} ${_quiet_flags}")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "-g3 -DDEBUG ${_verb_flags} ${_loud_flags}")

#------------------------------------------------------------------------------#
# Intel C++ Compilers
#
elseif(CMAKE_CXX_COMPILER_IS_INTEL)

    set(_def_cxx "-Wno-unknown-pragmas -Wno-deprecated")
    set(_extra_cxx_flags "-Wno-non-virtual-dtor -Wpointer-arith -Wwrite-strings -fp-model precise")

    add(_def_cxx "-ipo -w0")
    if(USE_SSE)
        add(_def_cxx "-use-intel-optimized-headers")
    endif()

    get_intel_intrinsic_include_dir()

    set(CMAKE_CXX_FLAGS_INIT                "${_def_cxx}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-Ofast -DNDEBUG")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "${CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT}")

#-----------------------------------------------------------------------
# IBM xlC compiler
#
elseif(CMAKE_CXX_COMPILER_IS_XLC)

    set(CMAKE_CXX_FLAGS_INIT "")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g -qdbextra -qcheck=all -qfullpath -qtwolink -+")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-O2 -qtwolink -+")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -qdbextra -qcheck=all -qfullpath -qtwolink -+")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O2 -qtwolink -+")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "${CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT}")

#---------------------------------------------------------------------
# HP aC++ Compiler
#
elseif(CMAKE_CXX_COMPILER_IS_XLC)

    set(CMAKE_CXX_FLAGS_INIT                "+DAportable +W823")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-O3 +Onolimit")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O3 +Onolimit -g")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "+O2 +Onolimit")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "${CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT}")

#---------------------------------------------------------------------
# IRIX MIPSpro CC Compiler
#
elseif(CMAKE_CXX_COMPILER_IS_MIPS)

    set(CMAKE_CXX_FLAGS_INIT                "-ptused -DSOCKET_IRIX_SOLARIS")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-O -OPT:Olimit=5000")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O -OPT:Olimit=5000 -g")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O -OPT:Olimit=5000")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "${CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT}")

endif()


foreach(_init DEBUG VERBOSEDEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    if(NOT "${CMAKE_CXX_FLAGS_${_init}_INIT}" STREQUAL "")
        string(REPLACE "  " " " CMAKE_CXX_FLAGS_${_init}_INIT "${CMAKE_CXX_FLAGS_${_init}_INIT}")
    endif()
endforeach()


