cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(cmake_and_cuda LANGUAGES CXX CUDA)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")
find_package(Madthreading REQUIRED)

include(CTest)
include(CMakeParseArguments)

#------------------------------------------------------------------------------#
function(NVCUDA_COMPILE_PTX)
    set(options "")
    set(oneValueArgs TARGET_PATH GENERATED_FILES OUTPUT_DIR)
    set(multiValueArgs NVCC_OPTIONS SOURCES)
    cmake_parse_arguments(NVCUDA_COMPILE_PTX "${options}" "${oneValueArgs}"
        "${multiValueArgs}" ${ARGN})

    # Match the bitness of the ptx to the bitness of the application
    set( MACHINE "--machine=32" )
    if( CMAKE_SIZEOF_VOID_P EQUAL 8)
        set( MACHINE "--machine=64" )
    endif()

    set(_OUTPUT_DIR ${NVCUDA_COMPILE_PTX_OUTPUT_DIR})
    if("${_OUTPUT_DIR}" STREQUAL "")
        set(_OUTPUT_DIR ${CUDA_GENERATED_OUTPUT_DIR})
    endif()

    # Custom build rule to generate ptx files from cuda files
    foreach(input ${NVCUDA_COMPILE_PTX_SOURCES})
        get_filename_component(input_we ${input} NAME_WE)

        # generate the *.ptx files inside "ptx" folder inside
        # the executable's output directory.
        set( output "${_OUTPUT_DIR}/${input_we}.ptx" )

        list(APPEND PTX_FILES  ${output})

        add_custom_command(
            OUTPUT  ${output}
            DEPENDS ${input}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMAND ${CUDA_NVCC_EXECUTABLE} ${MACHINE}
                --ptx ${NVCUDA_COMPILE_PTX_NVCC_OPTIONS} ${input}
                -o ${output}
        )
    endforeach()

    set(${NVCUDA_COMPILE_PTX_GENERATED_FILES} ${PTX_FILES} PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------------#
set(BUILD_LIBRARY_TYPE STATIC)

find_package(CUDA REQUIRED)
foreach(_DIR ${CUDA_INCLUDE_DIRS})
    include_directories(SYSTEM ${_DIR})
endforeach(_DIR ${CUDA_INCLUDE_DIRS})
include_directories(
    SYSTEM ${PROJECT_SOURCE_DIR}/common/inc
    SYSTEM ${PROJECT_SOURCE_DIR}/common/inc/dynlink
    SYSTEM ${Madthreading_INCLUDE_DIRS})

find_library(CUDART_LIBRARY cuda
    PATHS /usr/local/cuda
    HINTS /usr/local/cuda
    PATH_SUFFIXES lib lib64)
set(cuda_libs ${CUDART_LIBRARY})
list(REMOVE_DUPLICATES cuda_libs)
message(STATUS "CUDA runtime lib: ${cuda_libs}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -pthread -std=c++11")

file(GLOB_RECURSE cuda_headers "${PROJECT_SOURCE_DIR}/common/inc/*.h")
file(GLOB_RECURSE cuda_sources "${PROJECT_SOURCE_DIR}/common/src/multithreading.cpp"
    "${PROJECT_SOURCE_DIR}/common/*.c")

set(CUDA_GENERATED_OUTPUT_DIR ${CMAKE_BINARY_DIR})
set(CUDA_SEPARABLE_COMPILATION ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

nvcuda_compile_ptx(
    GENERATED_FILES thread_context_ptx_files
    OUTPUT_DIR ${CMAKE_BINARY_DIR}
    SOURCES
        thread_context_kernel.cu)

nvcuda_compile_ptx(
    GENERATED_FILES particles_ptx_files
    OUTPUT_DIR ${CMAKE_BINARY_DIR}
    SOURCES
        particle.cu v3.cu)

cuda_add_library(particle_simulator ${BUILD_LIBRARY_TYPE}
    randomize.cpp
    randomize.hpp
    particle.hpp
    v3.hpp
    particle.cu
    v3.cu
    advance_particles.hpp
    advance_particles.cu
    particle_simulator.hpp
    particle_simulator.cu)

cuda_add_library(common ${BUILD_LIBRARY_TYPE} ${cuda_sources} ${cuda_headers})
cuda_add_executable(particle_test test.cu)
cuda_add_executable(madcudathreading madcudathreading.cpp)
cuda_add_library(thread_context ${BUILD_LIBRARY_TYPE}
    thread_context.cpp thread_context.hpp ${thread_context_ptx_files})

target_link_libraries(thread_context ${cuda_libs} common)
target_link_libraries(particle_test particle_simulator)
target_link_libraries(madcudathreading ${cuda_libs} common thread_context
    ${Madthreading_LIBRARIES} particle_simulator)

if(BUILD_TESTING)

  add_test(NAME particles_10k COMMAND particle_test 10000 )
  add_test(NAME particles_256k COMMAND particle_test 256000 )

  if(APPLE)
      # We need to add the default path to the driver (libcuda.dylib) as an rpath,
      # so that the ${BUILD_LIBRARY_TYPE} cuda runtime can find it at runtime.
      set_property(TARGET particle_test PROPERTY BUILD_RPATH
          ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})
  endif()
endif()
