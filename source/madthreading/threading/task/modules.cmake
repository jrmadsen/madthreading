
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${TIMEMORY_INCLUDE_DIRS})
include(CMakeParseArguments)

set(DIR ${CMAKE_CURRENT_LIST_DIR})

pybind11_add_module(task MODULE ${DIR}/pytask.cc)
target_link_libraries(task PRIVATE ${PROJECT_LIBRARIES})

install(TARGETS task DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/madthreading)
set_target_properties(task PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/madthreading
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/madthreading
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/madthreading)
