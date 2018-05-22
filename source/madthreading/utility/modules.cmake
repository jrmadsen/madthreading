
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${TIMEMORY_INCLUDE_DIRS})
include(CMakeParseArguments)

set(DIR ${CMAKE_CURRENT_LIST_DIR})

pybind11_add_module(utility SHARED ${DIR}/pyutility.cc)
target_link_libraries(utility PRIVATE ${PROJECT_LIBRARIES})

install(TARGETS utility DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/madthreading)
set_target_properties(utility PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/madthreading
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/madthreading
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/madthreading)
