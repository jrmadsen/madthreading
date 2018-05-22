
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_1)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")

find_package(Madthreading REQUIRED)

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex1 ex1.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex1 ${Madthreading_LIBRARIES})

install(TARGETS ex1 DESTINATION bin)
