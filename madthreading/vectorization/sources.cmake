
include_directories(${PROJECT_SOURCE_DIR})

include(MacroDefineModule)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_COMPILER_IS_INTEL_ICC OR CMAKE_COMPILER_IS_INTEL_ICPC)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/array.cc
        ${CMAKE_CURRENT_LIST_DIR}/array.hh
        ${CMAKE_CURRENT_LIST_DIR}/func.cc
        ${CMAKE_CURRENT_LIST_DIR}/func.hh
        PROPERTIES COMPILE_FLAGS "-Wno-unknown-pragmas")
endif()

DEFINE_MODULE(NAME mad.vectorization
              HEADER_EXT ".h;.hh"
              SOURCE_EXT ".cc;.cpp")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/unit_test)
