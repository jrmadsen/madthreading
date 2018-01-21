
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${TIMEMORY_INCLUDE_DIRS})
include(CMakeParseArguments)

set(DIR ${CMAKE_CURRENT_LIST_DIR})
pybind11_add_module(pythreading MODULE ${DIR}/pythreading.cc)
target_link_libraries(pythreading PRIVATE ${PROJECT_LIBRARIES})
