cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(madthreading C CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules
                      ${PROJECT_SOURCE_DIR}/cmake/Packages
                      ${CMAKE_MODULE_PATH})

set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
    ${CMAKE_SOURCE_DIR}/cmake/Modules/MakeRulesCXX.cmake)


include(MakeRulesCXX)
include(GenericCMakeFunctions)
SET_PROJECT_VERSION("2" "0" "0" "Multithreading API using pthreads. Task-based with thread-pool")
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_INSTALL_MESSAGE LAZY)

message("")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${${PROJECT_NAME}_VERSION}")
message("")

include(VersionUpdate)
include(InstallDirs)
include(GenericCMakeOptions)
include(Packages)
include(LibraryBuildOptions)
include(MacroUtilities)
include(LibrarySuffixes)

OPTION_USE_OPENMP(ON)
OPTION_RETURN_TYPE_COMPILER_WARNINGS(OFF NO_FEATURE)
OPTION_EXTRA_COMPILER_WARNINGS(OFF NO_FEATURE)
OPTION_GPROF(OFF NO_FEATURE)
ADD_OPTION(BUILD_EXAMPLES "Build the examples" ON)

string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_BUILD_TYPE)
add_feature(EXTRA_LIBS "Extra link libraries")
add_feature(CMAKE_INSTALL_PREFIX "Installation prefix")
add_feature(CMAKE_CXX_FLAGS "CXX flags")
add_feature(CMAKE_BUILD_TYPE "Build type")
add_feature(CMAKE_CXX_FLAGS_${UPPER_BUILD_TYPE} "CXX flags for ${CMAKE_BUILD_TYPE}")

foreach(_INCLUDE_DIR ${EXTERNAL_INCLUDE_DIRS})
    include_directories(SYSTEM ${_INCLUDE_DIR})
endforeach()

#-----------------------------------------------------------------------
# BUILD_CXXSTD
# Choose C++ Standard to build against, if supported.
# Mark as advanced because most users will not need it.
include(ConfigureCXXSTD)
set(CMAKE_CXX_FLAGS "-std=${BUILD_CXXSTD} ${CMAKE_CXX_FLAGS}")

add_subdirectory(madthreading)
add_subdirectory(unit_test)

#------------------------------------------------------------------------------#

option(GENERATE_PYTHON_INTERFACES "Generate Python interfaces via SWIG" OFF)
add_feature(GENERATE_PYTHON_INTERFACES "Generate Python interfaces via SWIG")

#------------------------------------------------------------------------------#

if(GENERATE_PYTHON_INTERFACES)

    # used by SWIG Modules
    set(PROJECT_LIBRARIES ${PROJECT_NAME}-shared ${EXTRA_LIBS})

    string(REPLACE "-Wshadow " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    # modules
    get_property(SWIG_MODULE_FILES GLOBAL
        PROPERTY SWIG_MODULE_FILES)

    # remove duplicates
    if(NOT "${SWIG_MODULE_FILES}" STREQUAL "")
        list(REMOVE_DUPLICATES SWIG_MODULE_FILES)
    endif()

    # iterate
    foreach(_swig_file ${SWIG_MODULE_FILES})
        include(${_swig_file})
    endforeach()
endif()

#------------------------------------------------------------------------------#

include(Documentation)
if(DOXYGEN_DOCS)
    SET(CMAKE_INSTALL_MESSAGE NEVER)
    Generate_Documentation(Doxyfile.${PROJECT_NAME})
    SET(CMAKE_INSTALL_MESSAGE LAZY)
endif()

print_features()

include(CMakeUninstallTarget)

set(Madthreading_DIR ${PROJECT_BINARY_DIR})
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
