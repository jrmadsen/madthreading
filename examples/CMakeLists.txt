
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(Madthreading_Examples)

add_subdirectory(ex1)
add_subdirectory(ex2)
add_subdirectory(ex3)
if(USE_SSE AND AVX_FOUND)
    add_subdirectory(ex4)
endif()
add_subdirectory(ex5)
add_subdirectory(ex6)
