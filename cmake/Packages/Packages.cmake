# - Setup core required and optional components of ${CMAKE_PROJECT_NAME}
#
# Here we provide options to enable and configure required and optional
# components, which may require third party libraries.
#
# We don't configure User Interface options here because these require
# a higher degree of configuration so to keep things neat these have their
# own Module.
#
# Options configured here:
#
#

function(print _name)
    message(STATUS "${_name}: ")
    foreach(_path ${${_name}})
        message(STATUS "\t${_path}")
    endforeach()
endfunction()

################################################################################
function(git_submodule_update RESULT_VARIABLE)

    find_package(Git)
    set(_RET 0)
    if(Git_FOUND)
        message(STATUS "Checking out submodules...")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE RET
            OUTPUT_QUIET)
        set(_RET ${RET})
    endif(Git_FOUND)
    set(${RESULT_VARIABLE} ${_RET} PARENT_SCOPE)
    
endfunction(git_submodule_update)
################################################################################


include(MacroUtilities)
include(GenericFunctions)
include(CMakeDependentOption)
include(Compilers)
include(FindPackageHandleStandardArgs)


################################################################################
#
#        TiMemory
#
################################################################################
# git submodule
add_option(USE_TIMEMORY "Use TiMemory subpackage" ON)
if(USE_TIMEMORY)
    set(TIMEMORY_DIR ${PROJECT_SOURCE_DIR}/source/timemory)
    set(TIMEMORY_INCLUDE_DIR ${TIMEMORY_DIR}/source 
        ${TIMEMORY_DIR}/source/cereal/include ${TIMEMORY_DIR}/pybind11/include)

    function(error_with_timemory)

        message(STATUS "TiMemory is a git submodule and has not been cloned")
        message(STATUS "run: git submodule update --init --remote --recursive")
        message(STATUS "  NOTE: ${ARGN}")
        message(FATAL_ERROR "TiMemory not found")

    endfunction(error_with_timemory)
    
    # set TIMEMORY_FOUND
    set(TIMEMORY_FOUND OFF)
    if(NOT EXISTS "${TIMEMORY_DIR}/CMakeLists.txt")
        git_submodule_update(RET)
        
        if(RET)
            error_with_timemory("Submodule update failed")
        endif(RET)
        
        if(EXISTS "${TIMEMORY_DIR}/CMakeLists.txt")
            set(TIMEMORY_FOUND ON)
        else(EXISTS "${TIMEMORY_DIR}/CMakeLists.txt")
            set(TIMEMORY_FOUND OFF)
        endif(EXISTS "${TIMEMORY_DIR}/CMakeLists.txt")

    else(NOT EXISTS "${TIMEMORY_DIR}/CMakeLists.txt")
    
        set(TIMEMORY_FOUND ON)
        
    endif(NOT EXISTS "${TIMEMORY_DIR}/CMakeLists.txt")

    # check if found
    if(NOT TIMEMORY_FOUND)
    
        error_with_timemory("Submodule update worked but pybind11 still failed")
        
    endif(NOT TIMEMORY_FOUND)

    set(TIMEMORY_INCLUDE_DIRS ${TIMEMORY_INCLUDE_DIR})
    
endif(USE_TIMEMORY)


################################################################################
#
#        Threading
#
################################################################################

set(CMAKE_THREAD_PREFER_PTHREADS ON)
find_package(Threads REQUIRED)


################################################################################
#
#        UnitTest++
#
################################################################################

add_option(ENABLE_UNIT_TEST "Enable unit testing" OFF)

if(ENABLE_UNIT_TEST)
  find_package(UnitTest++ REQUIRED)
  file(GLOB_RECURSE unittest++_headers ${UnitTest++_INCLUDE_DIRS}/*.h)
endif()


################################################################################
#
#        TCMALLOC
#
################################################################################

add_option(USE_TCMALLOC "Enable gperftools TCMALLOC - an efficient thread-caching malloc" OFF)

if(USE_TCMALLOC)
    ConfigureRootSearchPath(TCMALLOC)
    find_library(TCMALLOC_LIBRARY
        NAMES tcmalloc_minimal
        HINTS ${TCMALLOC_ROOT} ENV TCMALLOC_ROOT
        PATH_SUFFIXES lib64 lib
        DOC "TCMALLOC library"
    )
    mark_as_advanced(TCMALLOC_LIBRARY)
    find_package_handle_standard_args(TCMALLOC DEFAULT_MSG
        TCMALLOC_LIBRARY)

    if(TCMALLOC_FOUND)
        set(TCMALLOC_LIBRARIES ${TCMALLOC_LIBRARY})
    else()
        message(FATAL_ERROR "Unable to find tcmalloc library")
    endif()
endif()


################################################################################
#
#        BOOST
#
################################################################################

add_option(USE_BOOST "Enable BOOST Libraries" OFF)

foreach(_component serialization signals system)
    STRING(TOUPPER ${_component} _COMPONENT)
    if(USE_BOOST)
        add_subfeature(USE_BOOST USE_BOOST_${_COMPONENT} "Use Boost ${_component} library")
        set(USE_BOOST_${_COMPONENT} ON)
    elseif(USE_BOOST_${_COMPONENT})
        add_feature(USE_BOOST_${_COMPONENT} "Use Boost ${_component} library")
    endif()
    if(USE_BOOST_${_COMPONENT})
        list(APPEND Boost_COMPONENTS ${_component})
    endif()
endforeach()

if(USE_BOOST)
    message(STATUS "Finding Boost components : ${Boost_COMPONENTS}")
    ConfigureRootSearchPath(BOOST)

    set(Boost_NO_BOOST_CMAKE OFF)
    find_package(Boost 1.53 REQUIRED COMPONENTS ${Boost_COMPONENTS})
    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
        foreach(_component ${Boost_COMPONENTS})
            STRING(TOUPPER ${_component} _COMPONENT)
            add_definitions(-DUSE_BOOST_${_COMPONENT})
        endforeach()
    else()
        message(FATAL_ERROR "BOOST NOT FOUND")
    endif(Boost_FOUND)

    if(NOT BOOST_ROOT)
        GET_FILENAME_COMPONENT(BOOST_ROOT "${Boost_INCLUDE_DIR}" PATH)
    endif()

    add_feature(BOOST_ROOT "The root location of Boost - ${BOOST_ROOT}")
endif()



################################################################################
#
#        TBB - Intel Thread Building Blocks
#
################################################################################

add_option(USE_TBB "Enable Intel Thread Building Blocks (TBB)" OFF)

if(USE_TBB)

    ConfigureRootSearchPath(TBB)
    find_package(TBB REQUIRED COMPONENTS malloc)

    add_definitions(-DUSE_TBB)
    if(TBB_MALLOC_FOUND)
        add_definitions(-DUSE_TBB_MALLOC)
    endif()

endif()


################################################################################
#
#        MKL - Intel Math Kernel Library
#
################################################################################

add_option(USE_MKL "Enable Intel Math Kernel Library (MKL)" OFF)
if(USE_MKL)

    ConfigureRootSearchPath(MKL)

    set(MKL_COMPONENTS rt) # mkl_rt as default

    if(CMAKE_C_COMPILER_IS_GNU OR CMAKE_CXX_COMPILER_IS_GNU)
        set_ifnot(MKL_THREADING OpenMP)
        set(MKL_COMPONENTS gnu_thread core intel_lp64)
        find_package(GOMP REQUIRED)
    endif()

    find_package(MKL REQUIRED COMPONENTS ${MKL_COMPONENTS})

    foreach(_def ${MKL_DEFINITIONS})
        add_definitions(-D${_def})
    endforeach()
    add(EXTERNAL_LINK_FLAGS "${MKL_CXX_LINK_FLAGS}")
    add_definitions(-DUSE_MKL)

endif()


################################################################################
#
#        MPI
#
################################################################################

if(USE_MPI)
    find_package(MPI REQUIRED)

    # Add the MPI-specific compiler and linker flags
    add(CMAKE_CXX_FLAGS  "${MPI_CXX_COMPILE_FLAGS}")
    add(CMAKE_EXE_LINKER_FLAGS "${MPI_CXX_LINK_FLAGS}")
    list(APPEND EXTERNAL_INCLUDE_DIRS
        ${MPI_INCLUDE_PATH} ${MPI_C_INCLUDE_PATH} ${MPI_CXX_INCLUDE_PATH})
    foreach(_DIR ${EXTERNAL_INCLUDE_DIRS})
        include_directories(SYSTEM ${_DIR})
    endforeach(_DIR ${EXTERNAL_INCLUDE_DIRS})

    set(MPI_LIBRARIES )
    foreach(_TYPE C_LIBRARIES CXX_LIBRARIES EXTRA_LIBRARY)
        set(_TYPE MPI_${_TYPE})
        if(${_TYPE})
            list(APPEND MPI_LIBRARIES ${${_TYPE}})
        endif(${_TYPE})
    endforeach(_TYPE C_LIBRARIES CXX_LIBRARIES EXTRA_LIBRARY)
endif(USE_MPI)


################################################################################
# --- setting definitions: EXTERNAL_INCLUDE_DIRS,   ---------------------------#
#                          EXTERNAL_LIBRARIES       ---------------------------#
################################################################################

set(EXTERNAL_INCLUDE_DIRS
    ${MKL_INCLUDE_DIRS}
    ${TBB_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${TIMEMORY_INCLUDE_DIRS}
)

set(EXTERNAL_LIBRARIES ${CMAKE_THREAD_LIBS_INIT}
    ${MKL_LIBRARIES}
    ${GOMP_LIBRARIES}
    ${TBB_LIBRARIES}
    ${Coverage_LIBRARIES}
    ${TCMALLOC_LIBRARIES}
    ${Boost_LIBRARIES}
    ${MPI_LIBRARIES}
)

REMOVE_DUPLICATES(EXTERNAL_INCLUDE_DIRS)
REMOVE_DUPLICATES(EXTERNAL_LIBRARIES)


################################################################################
#
#        SSE
#
################################################################################
add_option(USE_SSE "Enable SSE support" OFF)

if(USE_SSE)

    include(FindSSE)

    GET_SSE_COMPILE_FLAGS(SSE_FLAGS SSE_DEFINITIONS)
    foreach(_DEF ${SSE_DEFINITIONS})
        add_definitions(-D${_DEF})
    endforeach()
    unset(SSE_DEFINITIONS)

    add(CMAKE_C_FLAGS_EXTRA "${SSE_FLAGS}")
    add(CMAKE_CXX_FLAGS_EXTRA "${SSE_FLAGS}")

else(USE_SSE)

    foreach(type SSE2 SSE3 SSSE3 SSE4_1 AVX AVX2)
        remove_definitions(-DHAS_${type})
    endforeach()

endif(USE_SSE)


################################################################################
#
#        Architecture Flags
#
################################################################################
add_option(USE_ARCH "Enable architecture optimizations" OFF)

if(USE_ARCH OR USE_SSE)
    include(Architecture)

    ArchitectureFlags(ARCH_FLAGS)
    add(CMAKE_C_FLAGS_EXTRA "${ARCH_FLAGS}")
    add(CMAKE_CXX_FLAGS_EXTRA "${ARCH_FLAGS}")

endif(USE_ARCH OR USE_SSE)

if(USE_BOOST)
    if(Boost_FOUND)
        set(Boost_FOUND ON)
    endif()
endif()

add_c_flags(CMAKE_C_FLAGS_EXTRA "${CMAKE_C_FLAGS_EXTRA}")
add_cxx_flags(CMAKE_CXX_FLAGS_EXTRA "${CMAKE_CXX_FLAGS_EXTRA}")

