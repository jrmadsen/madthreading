
include_directories(${PROJECT_SOURCE_DIR})

include(MacroDefineModule)

DEFINE_MODULE(NAME mad.atomics
    HEADER_EXT ".h;.hh"
    SOURCE_EXT ".cc;.cpp")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/unit_test)
