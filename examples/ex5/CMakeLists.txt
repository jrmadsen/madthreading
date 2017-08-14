
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_5)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")

find_package(Madthreading REQUIRED)
# if we are not in build tree
if("${CMAKE_BINARY_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    add_madthreading_cxx_flags()
    add_madthreading_definitions()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(VEC_FLAGS "${VEC_FLAGS} -Wopenmp-simd")
    set(VEC_FLAGS "${VEC_FLAGS} -ftree-loop-vectorize -fopt-info-vec-optimized")
    set(VEC_FLAGS "${VEC_FLAGS} -ftree-vectorize -Wvector-operation-performance")
endif()

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex5 ex5.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex5 ${Madthreading_LIBRARIES})
set_target_properties(ex5 PROPERTIES COMPILE_FLAGS "${VEC_FLAGS}")
