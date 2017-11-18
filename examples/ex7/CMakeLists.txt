cmake_minimum_required(VERSION 3.1.3)
project(ex7-pyasync CXX)

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
  ${PROJECT_BINARY_DIR}/async.py COPYONLY)

#add_executable(ex7 ex7.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
#target_link_libraries(ex7 ${Madthreading_LIBRARIES})

#execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
#  ${PROJECT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)
  
