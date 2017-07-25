
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_4)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()

find_package(Madthreading REQUIRED)

set(VEC_FLAGS )
if(CMAKE_COMPILER_IS_GNUCXX)
    set(VEC_FLAGS "-Wvector-operation-performance -fopt-info-vec-optimized")
else()
    set(VEC_FLAGS "-axCOMMON-AVX512")
endif()

include_directories(${Madthreading_INCLUDE_DIRS})

add_executable(ex4 ex4.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex4 ${Madthreading_LIBRARIES})
set_target_properties(ex4 PROPERTIES COMPILE_FLAGS "-std=c++11 ${VEC_FLAGS}")

