
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/atomics)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/threading/task)
include_directories(${PROJECT_SOURCE_DIR}/source/vectorization)
include_directories(${PROJECT_SOURCE_DIR}/source/utility)

include(MacroDefineModule)

set(COMPAT_FOLDER "cxx11-compat")

if("${BUILD_CXXSTD}" STREQUAL "c++98")
    set(COMPAT_FOLDER "cxx98-compat")
endif()

file(GLOB LOCAL_HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.hh)
DEFINE_MODULE(NAME mad.threading.task
    HEADER_DIR ${COMPAT_FOLDER}
    SOURCE_DIR ${COMPAT_FOLDER}
    HEADER_EXT .h .hh
    SOURCE_EXT .cc .cpp
    HEADERS ${LOCAL_HEADERS}
)
