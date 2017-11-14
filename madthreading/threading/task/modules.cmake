
include_directories(${PROJECT_SOURCE_DIR})
include(CMakeParseArguments)

set(DIR ${CMAKE_CURRENT_LIST_DIR})
pybind11_add_module(pytask MODULE ${DIR}/pytask.cc)
target_link_libraries(pytask PRIVATE ${PROJECT_LIBRARIES})
