
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(example_5)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()

find_package(Madthreading REQUIRED)

set(VEC_FLAGS "-Wopenmp-simd -Wunknown-pragmas")
set(VEC_FLAGS "${VEC_FLAGS} -ftree-loop-vectorize")
set(VEC_FLAGS "${VEC_FLAGS} -ftree-vectorize -Wvector-operation-performance -fopt-info-vec-optimized")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${VEC_FLAGS}")

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex5 ex5.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex5 ${Madthreading_LIBRARIES})
