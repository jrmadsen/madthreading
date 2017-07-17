
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(example_4)

find_package(Madthreading REQUIRED)

set(VEC_FLAGS "-march=corei7-avx -Wopenmp-simd")
set(VEC_FLAGS "${VEC_FLAGS} -ftree-loop-vectorize")
set(VEC_FLAGS "${VEC_FLAGS} -ftree-vectorize -Wvector-operation-performance -fopt-info-vec-optimized")
#set(VEC_FLAGS "-mavx -mavx2 -mavx512f -mavx512pf -mavx512er -mavx512cd")
#set(VEC_FLAGS "${VEC_FLAGS} -mavx256-split-unaligned-load -mavx256-split-unaligned-store")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${VEC_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mavx -Wunknown-pragmas")

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex4 ex4.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex4 ${Madthreading_LIBRARIES})
#set_target_properties(ex4 PROPERTIES
#    COMPILE_FLAGS "-fwhole-program -flto")

