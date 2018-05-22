
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/source/timemory/CMakeLists.txt")
    find_package(Git REQUIRED)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE RET)
    if(RET GREATER 0)
        message(FATAL_ERROR "Failure checking out submodules")
    endif(RET GREATER 0)
endif(NOT EXISTS "${PROJECT_SOURCE_DIR}/source/timemory/CMakeLists.txt")

add_option(PYBIND11_INSTALL "PyBind11 installation" ON)
set(PYBIND11_CPP_STANDARD -std=c++${CMAKE_CXX_STANDARD}
    CACHE STRING "PyBind11 CXX standard" FORCE)
set(TIMEMORY_NAMESPACE mad CACHE STRING "Namespace of TiMemory" FORCE)    
set(TIMEMORY_SUBMODULE ON CACHE BOOL "Let TiMemory know it is a submodule")
set(SHARED_PYTHON_LINKING ON CACHE BOOL "TiMemory uses shared library linking")
add_definitions(-DNAME_TIM=${TIMEMORY_NAMESPACE})
add_option(USE_MPI "Compile TiMemory with MPI support" OFF)

add_subdirectory(timemory)

if(NOT PYBIND11_PYTHON_VERSION)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} --version
        OUTPUT_VARIABLE PYTHON_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "[ A-Za-z]" "" PYTHON_VERSION "${PYTHON_VERSION}")
    string(REGEX REPLACE "\.([0-9]+)$" "" PYTHON_VERSION "${PYTHON_VERSION}")
    set(PYBIND11_PYTHON_VERSION "${PYTHON_VERSION}"
        CACHE STRING "Python version" FORCE)
endif(NOT PYBIND11_PYTHON_VERSION)
set(CMAKE_INSTALL_PYTHONDIR
    ${CMAKE_INSTALL_LIBDIR}/python${PYBIND11_PYTHON_VERSION}/site-packages
    CACHE PATH "Installation directory for python" FORCE)
set(CMAKE_INSTALL_FULL_PYTHONDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PYTHONDIR})
set(PYTHON_INSTALL_DIR      ${CMAKE_INSTALL_PYTHONDIR})

set_property(TARGET timemory-static PROPERTY IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/TiMemoryBuild.cmake")
set_property(TARGET timemory-shared PROPERTY IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/TiMemoryBuild.cmake")
#add_library(timemory-library STATIC IMPORTED)
#set_target_properties(timemory-library PROPERTIES
#    IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libtimemory${CMAKE_STATIC_LIBRARY_SUFFIX})
#list(APPEND EXTERNAL_LIBRARIES timemory-library)

add_subdirectory(madthreading)
