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

message(STATUS "")

################################################################################


include(MacroUtilities)
include(GenericCMakeFunctions)
include(CMakeDependentOption)


################################################################################
#
#        Compilers
#
################################################################################
if(CMAKE_CXX_COMPILER MATCHES "icc.*")
    set(CMAKE_COMPILER_IS_INTEL_ICC ON)
endif()
if(CMAKE_CXX_COMPILER MATCHES "icpc.*")
    set(CMAKE_COMPILER_IS_INTEL_ICPC ON)
endif()


################################################################################
#
#        Threading
#
################################################################################

set(CMAKE_THREAD_PREFER_PTHREADS ON)
find_package(Threads REQUIRED)
if(Threads_FOUND)
    add_definitions(-DENABLE_THREADING)
endif()
list(APPEND EXTERNAL_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})


################################################################################
#
#        UnitTest++
#
################################################################################

option(ENABLE_UNIT_TEST "Enable unit testing" OFF)
add_feature(ENABLE_UNIT_TEST "Enable unit testing")

if(ENABLE_UNIT_TEST)
  find_package(UnitTest++ REQUIRED)
  file(GLOB_RECURSE unittest++_headers ${UnitTest++_INCLUDE_DIRS}/*.h)
endif()


################################################################################
#
#        TCMALLOC
#
################################################################################

option(USE_TCMALLOC "Enable gperftools TCMALLOC - an efficient thread-caching malloc" ON)
add_feature(USE_TCMALLOC "Enable gperftools TCMALLOC - an efficient thread-caching malloc")

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
    list(APPEND EXTERNAL_LIBRARIES ${TCMALLOC_LIBRARIES})
endif()


################################################################################
#
#        BOOST
#
################################################################################

option(USE_BOOST "Enable BOOST Libraries" OFF)
add_feature(USE_BOOST "Enable Boost libraries")

foreach(_component chrono python serialization signals system thread timer)
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
        list(APPEND EXTERNAL_LIBRARIES ${Boost_LIBRARIES})
        list(APPEND EXTERNAL_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
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
    if(NOT CMAKE_COMPILER_IS_INTEL_ICC AND NOT CMAKE_COMPILER_IS_INTEL_ICPC)
        ConfigureRootSearchPath(TBB)

        find_package(TBB REQUIRED)
        if(TBB_FOUND)
            include_directories(${TBB_INCLUDE_DIRS})
            list(APPEND EXTERNAL_LIBRARIES ${TBB_LIBRARIES})
            list(APPEND EXTERNAL_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
        endif()

        add_subfeature(USE_TBB TBB_ROOT "Root directory of TBB install")
    elseif(CMAKE_COMPILER_IS_INTEL_ICC OR CMAKE_COMPILER_IS_INTEL_ICPC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -tbb")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -tbb")
    endif()
    add_package_definitions(TBB)
    add_package_definitions(TBB_MALLOC)
endif()


################################################################################
#
#        MKL - Intel Math Kernel Library
#
################################################################################

add_option(USE_MKL "Enable Intel Math Kernel Library (MKL)" OFF)
if(USE_MKL)
    if(NOT CMAKE_COMPILER_IS_INTEL_ICC AND NOT CMAKE_COMPILER_IS_INTEL_ICPC)
        ConfigureRootSearchPath(MKL)

        set(MKL_THREADING "Sequential")
        find_package(MKL REQUIRED)
        if(MKL_FOUND)
            include_directories(${MKL_INCLUDE_DIRS})
            list(APPEND EXTERNAL_LIBRARIES ${MKL_LIBRARIES})
            list(APPEND EXTERNAL_INCLUDE_DIRS ${MKL_INCLUDE_DIRS})
        endif()

        add_subfeature(USE_MKL MKL_ROOT "Root directory of MKL install")
        foreach(_def ${MKL_DEFINITIONS})
            add_definitions(-D${_def})
        endforeach()
        list(APPEND EXTERNAL_LINK_FLAGS "${MKL_CXX_LINK_FLAGS}")
    elseif(CMAKE_COMPILER_IS_INTEL_ICC OR CMAKE_COMPILER_IS_INTEL_ICPC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mkl")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mkl")
    endif()
    add_package_definitions(MKL)
endif()


################################################################################
#
#        SSE
#
################################################################################
option(USE_SSE "Enable SSE support" OFF)
add_feature(USE_SSE "Enable SSE/AVX support")

if(USE_SSE)
    include(FindSSE)
    foreach(type SSE2 SSE3 SSSE3 SSE4_1 AVX AVX2)
        add_subfeature(USE_SSE ${type}_FOUND "Hardware support for ${type}")
    endforeach()

    GET_SSE_COMPILE_FLAGS(CMAKE_CXX_FLAGS_EXTRA SSE_DEFINITIONS)
    foreach(_DEF ${SSE_DEFINITIONS})
        add_definitions(-D${_DEF})
    endforeach()
    unset(SSE_DEFINITIONS)

else()
    foreach(type SSE2 SSE3 SSSE3 SSE4_1 AVX AVX2)
        remove_definitions(-DHAS_${type})
    endforeach()
endif()


################################################################################
#
#        clean up...
#
################################################################################
set(_types LIBRARIES INCLUDE_DIRS)
foreach(_type ${_types})
    if(NOT "${EXTERNAL_${_type}}" STREQUAL "")
        list(REMOVE_DUPLICATES EXTERNAL_${_type})
    endif()
endforeach()
unset(_types)

if(USE_BOOST)
    if(Boost_FOUND)
        set(Boost_FOUND ON)
    endif()
endif()
