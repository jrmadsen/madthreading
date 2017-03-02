cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(madthreading CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules
                      ${CMAKE_MODULE_PATH})

include(MakeRules_cxx)
include(GenericCMakeFunctions)
SET_PROJECT_VERSION("0" "1" "0" "Multithreading API using pthreads")

include(VersionUpdate)
include(InstallDirs)
include(Packages)
include(GenericCMakeOptions)
include(LibraryBuildOptions)

ENABLE_OPENMP_SUPPORT(ON)
OPTION_RETURN_TYPE_COMPILER_WARNINGS(OFF)
OPTION_EXTRA_COMPILER_WARNINGS(OFF)
OPTION_GPROF(OFF)
OPTION_ENABLE_PIC(ON)

add_feature(USE_OPENMP "Enable OpenMP multithreading")
add_feature(EXTRA_COMPILER_WARNINGS "Turn on -Wextra for ${CMAKE_CXX_COMPILER_ID}")
add_feature(RETURN_TYPE_COMPILER_WARNINGS "Turn on -Wreturn-type for ${CMAKE_CXX_COMPILER_ID}")
add_feature(ENABLE_GPROF "Compile using -g -pg for gprof output")
add_feature(EXTRA_LIBS "Extra link libraries")
add_feature(CMAKE_INSTALL_PREFIX "Installation prefix")
add_feature(CMAKE_CXX_FLAGS "CXX flags")
add_feature(CMAKE_BUILD_TYPE "Build type")

foreach(_INCLUDE_DIR ${EXTERNAL_INCLUDE_DIRS})
    include_directories(SYSTEM ${_INCLUDE_DIR})
endforeach()

add_subdirectory(source)

#------------------------------------------------------------------------------#

option(GENERATE_PYTHON_INTERFACES "Generate Python interfaces via SWIG" ON)
add_feature(GENERATE_PYTHON_INTERFACES "Generate Python interfaces via SWIG")

#------------------------------------------------------------------------------#

if(GENERATE_PYTHON_INTERFACES)
    # used by SWIG Modules
    set(PROJECT_LIBRARIES ${PROJECT_NAME}-SHARED ${EXTRA_LIBS})

    string(REPLACE "-Wshadow " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    # modules
    get_property(SWIG_MODULE_FILES
                 GLOBAL
                 PROPERTY SWIG_MODULE_FILES)
    # remove duplicates
    if(NOT "${SWIG_MODULE_FILES}" STREQUAL "")
        list(REMOVE_DUPLICATES SWIG_MODULE_FILES)
    endif()

    # iterate
    foreach(_swig_file ${SWIG_MODULE_FILES})
        message(STATUS "Adding modules from ${_swig_file}...")
        include(${_swig_file})
    endforeach()
endif()

#------------------------------------------------------------------------------#

include(Documentation)
if(DOXYGEN_DOCS)
    Generate_Documentation(Doxyfile.${PROJECT_NAME})
endif()

print_features()

include(CMakeUninstallTarget)

set(Madthreading_DIR ${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(examples)

