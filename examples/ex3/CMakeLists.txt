
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(example_3)

find_package(Madthreading REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex3 ex3.cc)
target_link_libraries(ex3 ${Madthreading_LIBRARIES})
 
