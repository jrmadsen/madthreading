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

option(USE_TBB "Enable Intel Thread Building Blocks (TBB)" OFF)
add_feature(USE_TBB "Intel Thread Building Blocks library")

if(USE_TBB)
    if(NOT CMAKE_COMPILER_IS_INTEL_ICC AND NOT CMAKE_COMPILER_IS_INTEL_ICPC)
        ConfigureRootSearchPath(TBB)

        find_package(TBB REQUIRED)
        if(TBB_FOUND)
            include_directories(${TBB_INCLUDE_DIRS})
            list(APPEND EXTERNAL_LIBRARIES ${TBB_LIBRARIES})
            list(APPEND EXTERNAL_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
        endif()

        add_feature(TBB_ROOT "Root directory of TBB install")
    endif()
    add_package_definitions(TBB)
endif()



################################################################################
#
#        SSE
#
################################################################################
include(FindSSE)
set(SSE OFF CACHE BOOL "SSE support")
add_feature(SSE "SSE Support")
foreach(type SSE2 SSE3 SSSE3 SSE4_1 AVX AVX2)
    if(${type}_FOUND)
        set(SSE ON CACHE BOOL "SSE support" FORCE)
    endif()
    add_subfeature(SSE ${type}_FOUND "Hardware support for ${type}")
endforeach()

if(CMAKE_COMPILER_IS_INTEL_ICC OR CMAKE_COMPILER_IS_INTEL_ICPC)
    find_program(INTEL_LINKER xild HINTS ENV PATH PATH_SUFFIXES bin bin/intel64 bin/ia32)
    find_program(INTEL_AR     xiar HINTS ENV PATH PATH_SUFFIXES bin bin/intel64 bin/ia32)
    #set(CMAKE_LINKER ${INTEL_LINKER} CACHE FILEPATH "Intel C++ linker" FORCE)
    #set(CMAKE_AR ${INTEL_AR} CACHE FILEPATH "Intel C++ archiver" FORCE)
    set(_FLAGS )
    foreach(type SSE2 SSE3 SSSE3 SSE4_1 AVX)
        string(TOLOWER "${type}" _flag)
        string(REPLACE "_" "." _flag "${_flag}")
        set(${type}_FLAGS "-m${_flag}")
        if(${type}_FOUND)
            set(_FLAGS "${${type}_FLAGS}")
            add_definitions(-DHAS_${type})
        endif()
    endforeach()

    if(AVX2_FOUND)
        set(_FLAGS "-march=core-avx2 -axCOMMON-AVX512")
        add_definitions(-DHAS_AVX2)
    endif()

    if(USE_TBB)
        set(_FLAGS "${_FLAGS} -tbb")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_FLAGS}")

    #find_library(LLVM_LIBRARY
    #    NAMES clang
    #    HINTS ENV LD_LIBRARY_PATH
    #    ENV LIBRARY_PATH
    #    ENV DYLD_LIBRARY_PATH
    #    PATH_SUFFIXES
    #    llvm-3.9 llvm-3.8 llvm-3.7 lib lib64 lib32
    #    llvm-3.9/lib llvm-3.8/lib llvm-3.7/lib
    #    llvm-3.9/lib64 llvm-3.8/lib64 llvm-3.7/lib64
    #    llvm-3.9/lib32 llvm-3.8/lib32 llvm-3.7/lib32)

    #mark_as_advanced(LLVM_LIBRARY)
    #include(FindPackageHandleStandardArgs)
    #FIND_PACKAGE_HANDLE_STANDARD_ARGS(LLVM  REQUIRED_VARS LLVM_LIBRARY)
    #if(NOT LLVM_FOUND)
    #    message(FATAL_ERROR "LLVM Library not found")
    #else()
    #    list(APPEND EXTERNAL_LIBRARIES ${LLVM_LIBRARY})
    #endif()

elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

    foreach(type SSE2 SSE3 SSSE3 SSE4_1 AVX AVX2)
        string(TOLOWER "${type}" _flag)
        string(REPLACE "_" "." _flag "${_flag}")
        set(${type}_FLAGS "-m${_flag}")
        if(${type}_FOUND)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${type}_FLAGS}")
            add_definitions(-DHAS_${type})
        endif()
    endforeach()

endif(CMAKE_COMPILER_IS_INTEL_ICC OR CMAKE_COMPILER_IS_INTEL_ICPC)



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
