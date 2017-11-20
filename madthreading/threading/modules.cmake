
include_directories(${PROJECT_SOURCE_DIR})
include(CMakeParseArguments)

set(DIR ${CMAKE_CURRENT_LIST_DIR})
pybind11_add_module(pythreading MODULE ${DIR}/pythreading.cc)
target_link_libraries(pythreading PRIVATE ${PROJECT_LIBRARIES})
