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

option(ENABLE_UNIT_TEST "Enable unit testing" ON)
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
    ConfigureRootSearchPath(TBB)

    find_package(TBB REQUIRED)
    if(TBB_FOUND)
        include_directories(${TBB_INCLUDE_DIRS})
        list(APPEND EXTERNAL_LIBRARIES ${TBB_LIBRARIES})
        list(APPEND EXTERNAL_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
        add_package_definitions(TBB)
    endif()

    add_feature(TBB_ROOT "Root directory of TBB install")
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
