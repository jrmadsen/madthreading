
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${TIMEMORY_INCLUDE_DIRS})
include(MacroDefineModule)

file(GLOB MADPY_EXCLUDE "${CMAKE_CURRENT_LIST_DIR}/madpy_*")

DEFINE_MODULE(NAME mad.atomics
    EXCLUDE ${MADPY_EXCLUDE}
    HEADER_EXT ".h;.hh"
    SOURCE_EXT ".cc;.cpp")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/unit_test)
