
include_directories(${PROJECT_SOURCE_DIR})
include(CMakeParseArguments)

set(DIR ${CMAKE_CURRENT_LIST_DIR})

pybind11_add_module(pyutility SHARED ${DIR}/pyutility.cc)
target_link_libraries(pyutility PRIVATE ${PROJECT_LIBRARIES})
