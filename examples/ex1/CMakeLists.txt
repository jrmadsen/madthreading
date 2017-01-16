
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(example_1)

find_package(Madthreading REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex1 ex1.cc)
target_link_libraries(ex1 madthreading-SHARED)
 
