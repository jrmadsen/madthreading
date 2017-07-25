
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_6)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")

find_package(Madthreading REQUIRED)

ConfigureRootSearchPath(TBB)
find_package(TBB COMPONENTS malloc QUIET)
if(TBB_FOUND)
    include_directories(${TBB_INCLUDE_DIRS})
    list(APPEND EXTERNAL_LIBRARIES ${TBB_LIBRARIES})
    list(APPEND EXTERNAL_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
    add_definitions(-DUSE_TBB)
endif()

find_package(OpenMP QUIET)
if(OpenMP_FOUND)
    # Add the OpenMP-specific compiler and linker flags
    set(TARGET_CXX_FLAGS "${OpenMP_CXX_FLAGS}")
    set(TARGET_EXE_LINKER_FLAGS "${OpenMP_EXE_LINKER_FLAGS}")
    add_definitions(-DUSE_OPENMP)
endif()

#------------------------------------------------------------------------------#

include_directories(${Madthreading_INCLUDE_DIRS})

#------------------------------------------------------------------------------#
set(executables pi_serial
                pi_lambda_serial
                pi_pthreads
                pi_cxx11
                pi_pthread_pool
                pi_pthread_pool_joiner_1
                pi_pthread_pool_joiner_2)
if(TBB_FOUND)
    list(APPEND executables pi_tbb pi_lambda_tbb)
endif()

if(OpenMP_FOUND)
    list(APPEND executables omp_pi_loop omp_pi_task
        omp_pi_spmd_simple omp_pi_spmd_final)
endif()

foreach(executable ${executables})
    add_executable(${executable} ${PROJECT_SOURCE_DIR}/${executable}.cc
        ${PROJECT_SOURCE_DIR}/../Common.hh)
    target_link_libraries(${executable} ${Madthreading_LIBRARIES}
        ${TBB_LIBRARY} ${TBB_MALLOC_LIBRARY})
    set_target_properties(${executable} PROPERTIES
        COMPILE_FLAGS "${TARGET_CXX_FLAGS}"
        LINK_FLAGS "${TARGET_EXE_LINKER_FLAGS}")
endforeach()
#------------------------------------------------------------------------------#

enable_testing()
foreach(exe ${executables})
    add_test(NAME ${exe}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ./${exe})
endforeach()

find_program(CTEST_EXE ctest)
if(NOT CTEST_EXE)
    message(FATAL_ERROR "The ctest executable could not be found")
else()
    add_custom_target(run_tests ${TESTING_ALL_TARGET}
            COMMAND ${CTEST_EXE} --output-on-failure -O ctest_output_0.log
            DEPENDS ${executables}
            WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}
            COMMENT "\nRunning ctest...\n"
            VERBATIM)
endif()




