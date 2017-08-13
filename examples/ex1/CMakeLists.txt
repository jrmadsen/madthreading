
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_1)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")

find_package(Madthreading REQUIRED)
if(Madthreading_FOUND AND Madthreading_CMAKE_CONFIG_FILE)
    add_madthreading_cxx_flags()
    add_madthreading_definitions()
endif()

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex1 ex1.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex1 ${Madthreading_LIBRARIES})
