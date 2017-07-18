
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(example_4)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()

find_package(Madthreading REQUIRED)

set(VEC_FLAGS )
if(CMAKE_COMPILER_IS_GNUCXX)
    set(VEC_FLAGS "-Wvector-operation-performance -fopt-info-vec-optimized")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${VEC_FLAGS}")

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex4 ex4.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex4 ${Madthreading_LIBRARIES})
#set_target_properties(ex4 PROPERTIES
#    COMPILE_FLAGS "-fwhole-program -flto")

