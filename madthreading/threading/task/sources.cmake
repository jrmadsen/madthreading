
include_directories(${PROJECT_SOURCE_DIR})

include(MacroDefineModule)

set(COMPAT_FOLDER "cxx11-compat")

if("${BUILD_CXXSTD}" STREQUAL "c++98" OR "${BUILD_CXXSTD}" STREQUAL "c++0x")
    set(COMPAT_FOLDER "cxx98-compat")
endif()

file(GLOB LOCAL_HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.hh)
file(GLOB LOCAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cc)

DEFINE_MODULE(NAME mad.threading.task
    HEADER_DIR ${COMPAT_FOLDER}
    SOURCE_DIR ${COMPAT_FOLDER}
    HEADER_EXT .h .hh
    SOURCE_EXT .cc .cpp
    HEADERS ${LOCAL_HEADERS}
    SOURCES ${LOCAL_SOURCES}
)
