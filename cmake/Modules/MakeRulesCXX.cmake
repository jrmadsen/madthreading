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
# GNU C++ or LLVM/Clang Compiler on all(?) platforms
#
if(CMAKE_CXX_COMPILER_IS_GNU OR CMAKE_CXX_COMPILER_IS_CLANG)

    set(_def_cxx        "-Wno-deprecated -Wno-unused-function -Wno-unused-variable")
    add(_def_cxx        "-Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-parameter")
    set(_verb_cxx_flags "-Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wshadow -pipe -pedantic")
    if(CMAKE_COMPILER_IS_GNUCXX)
        add(_def_cxx      "-Wno-unused-local-typedefs")
        add(_fast_flags   "-ftree-vectorize -ftree-loop-vectorize")
    else()
        add(_def_cxx "-Qunused-arguments")
    endif()

    set(CMAKE_CXX_FLAGS_INIT                "-W -Wall ${_def_cxx}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-g -O2 ${_fast_flags}")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG ${_fast_flags}")
    set(CMAKE_CXX_FLAGS_VERBOSEDEBUG_INIT   "-g3 -DDEBUG ${_verb_cxx_flags}")


#------------------------------------------------------------------------------#
# Intel C++ Compilers
#
elseif(CMAKE_CXX_COMPILER_IS_INTEL)

    set(_def_cxx "-Wno-unknown-pragmas -Wno-deprecated")
    set(_extra_cxx_flags "-Wno-non-virtual-dtor -Wpointer-arith -Wwrite-strings -fp-model precise")

    add(_def_cxx "-xHOST -ipo -w0 -qno-offload")
    if(USE_SSE)
        add(_def_cxx "-use-intel-optimized-headers")
    endif()

    get_intel_intrinsic_include_dir()

    # -cxxlib -gcc-name -gxx-name -fabi-version -no-gcc
    set(_def_cxx )
    foreach(TYPE GCC GXX)

        # executable default name
        set(EXE "gcc")
        if("${TYPE}" STREQUAL "GXX")
            set(EXE "g++")
        endif()

        if(NOT ${TYPE} AND NOT "$ENV{${TYPE}}" STREQUAL "")
            set(${TYPE} $ENV{${TYPE}} CACHE PATH "Intel ${TYPE} path")
        endif()

        if("${${TYPE}}" STREQUAL "")
            set(_msg "\nPlease specify the ${TYPE} environment variable")
            add(_msg "(GCC C/C++ compiler to use with the Intel compiler)\n")
            message(AUTHOR_WARNING "${_msg}")
            unset(_msg)
        endif()

        if("${${TYPE}}" STREQUAL "")

            # find gcc/g++
            find_program(${TYPE}_PATH ${EXE})

            if(NOT ${TYPE}_PATH)
                # kill if not found
                message(FATAL_ERROR "Failure finding \"${LTYPE}\"")
            else()
                # warning them is not explicitly defined but not if -Wno-dev
                message(AUTHOR_WARNING "Using \"${${TYPE}_PATH}\" for ${TYPE}")
            endif()

            # don't cache
            set(${TYPE} ${${TYPE}_PATH})

        else()

            # make sure it is cached
            set(${TYPE} ${TYPE} CACHE PATH "Intel ${TYPE} compiler path")

        endif()

        # only guaranteed on GNU standard bin layout (e.g. /usr/bin/gcc)
        if(UNIX AND NOT ${TYPE}_ROOT)
            get_filename_component(${TYPE}_ROOT "${${TYPE}}"        DIRECTORY)
            get_filename_component(${TYPE}_ROOT "${${TYPE}_ROOT}"   DIRECTORY)
        else()
            if("${TYPE}" STREQUAL "GXX")
                set(_msg "\nPlease specify the ${TYPE}_ROOT environment variable")
                add(_msg "(GCC C/C++ compiler root directory to use with the Intel compiler)\n")
                message(FATAL_ERROR "${_msg}")
            endif()
        endif()

        get_filename_component(GCCN "${${TYPE}}" NAME)

        if("${TYPE}" STREQUAL "GCC")
            add(_def_cxx "-gcc-name=${GCCN}")
        else()
            add(_def_cxx "-cxxlib=${${TYPE}_ROOT} -gxx-name=${GCCN}")
        endif()

        unset(${TYPE}_ROOT) # won't affect cache version
        unset(GCCN)

    endforeach()

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


