
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_2)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")

find_package(Madthreading REQUIRED)

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex2 ex2.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex2 ${Madthreading_LIBRARIES})
