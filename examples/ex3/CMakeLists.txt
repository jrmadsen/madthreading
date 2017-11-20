cmake_minimum_required(VERSION 3.1.3)
project(ex3-pyasync CXX)

option(BLOCKING_OUTPUT "Enable mutex around std::cout in C++" ON)
if(BLOCKING_OUTPUT)
    add_definitions(-DBLOCKING_OUTPUT)
endif(BLOCKING_OUTPUT)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
    # not in build tree
    find_package(pybind11 REQUIRED)
    include_directories(${pybind11_INCLUDE_DIRS})
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")

find_package(Madthreading REQUIRED)
include_directories(${Madthreading_INCLUDE_DIRS})

pybind11_add_module(async MODULE ${PROJECT_SOURCE_DIR}/async.cc)
target_link_libraries(async PRIVATE ${Madthreading_LIBRARIES})

configure_file(${PROJECT_SOURCE_DIR}/async.py
  ${CMAKE_BINARY_DIR}/async.py COPYONLY)
