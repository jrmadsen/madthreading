
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_3)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")

find_package(Madthreading REQUIRED)

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex3a ex3a.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex3a ${Madthreading_LIBRARIES})

add_executable(ex3b ex3b.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex3b ${Madthreading_LIBRARIES})
