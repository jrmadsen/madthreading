cmake_minimum_required(VERSION 3.1.3)
project(ex3-pyasync CXX)

option(BLOCKING_OUTPUT "Enable mutex around std::cout in C++" ON)
if(BLOCKING_OUTPUT)
    add_definitions(-DBLOCKING_OUTPUT)
endif(BLOCKING_OUTPUT)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
    # not in build tree
    find_package(pybind11 REQUIRED)
    include_directories(${pybind11_INCLUDE_DIRS})
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")

find_package(Madthreading REQUIRED)
include_directories(${Madthreading_INCLUDE_DIRS})

pybind11_add_module(async MODULE
    ${PROJECT_SOURCE_DIR}/async.cc
    ${PROJECT_SOURCE_DIR}/async.py)
target_link_libraries(async PRIVATE ${Madthreading_LIBRARIES})
set_target_properties(async PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/madthreading
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/madthreading
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/madthreading)

add_executable(ex3 ex3.cc ${PROJECT_SOURCE_DIR}/../Common.hh)
target_link_libraries(ex3 ${Madthreading_LIBRARIES})

configure_file(${PROJECT_SOURCE_DIR}/async.py
  ${CMAKE_BINARY_DIR}/ex3.py COPYONLY)

# installation
install(TARGETS ex3 DESTINATION bin)
if(DEFINED CMAKE_INSTALL_PYTHONDIR)
    install(TARGETS async DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/madthreading)
endif(DEFINED CMAKE_INSTALL_PYTHONDIR)
install(FILES ${PROJECT_SOURCE_DIR}/async.py DESTINATION bin
    PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_WRITE GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE)
