
cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
project(example_3)

include(${PROJECT_SOURCE_DIR}/../ExternalBuild.cmake)
configure_example()
set(CMAKE_CXX_STANDARD "11")

find_package(Madthreading REQUIRED)
if(Madthreading_FOUND AND Madthreading_CMAKE_CONFIG_FILE)
    add_madthreading_cxx_flags()
    add_madthreading_definitions()
endif()

include_directories(${Madthreading_INCLUDE_DIRS})

include(Compilers)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_COMPILER_IS_INTEL_ICC OR CMAKE_COMPILER_IS_INTEL_ICPC)
    add_compile_options("-Wno-unknown-pragmas")
endif()

add_executable(ex3a ex3a.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex3a ${Madthreading_LIBRARIES})

add_executable(ex3b ex3b.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex3b ${Madthreading_LIBRARIES})
