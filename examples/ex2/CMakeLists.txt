
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(example_2)

find_package(Madthreading REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex2 ex2.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex2 madthreading-STATIC)

