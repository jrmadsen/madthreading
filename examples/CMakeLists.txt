
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(Madthreading_Examples)

add_subdirectory(ex1)
add_subdirectory(ex2)
#add_subdirectory(ex3)
if(USE_SSE AND AVX2_FOUND)
    add_subdirectory(ex4)
endif(USE_SSE AND AVX2_FOUND)
add_subdirectory(ex5)
add_subdirectory(ex6)

find_package(CUDA QUIET)
if(CUDA_FOUND)
    option(BUILD_CUDA "Build CUDA examples" ON)
    if(BUILD_CUDA)
        find_program(CMAKE_CUDA_COMPILER nvcc
            PATHS /usr/local/cuda
            HINTS /usr/local/cuda
            PATH_SUFFIXES bin)
        if(CMAKE_CUDA_COMPILER)
            add_subdirectory(cuda)
        endif(CMAKE_CUDA_COMPILER)
    endif(BUILD_CUDA)
endif(CUDA_FOUND)
