
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${TIMEMORY_INCLUDE_DIRS})
include(MacroDefineModule)

get_filename_component(DIRNAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
file(GLOB MADPY_EXCLUDE "${CMAKE_CURRENT_LIST_DIR}/py${DIRNAME}.cc")

DEFINE_MODULE(NAME mad.threading.task
    EXCLUDE ${MADPY_EXCLUDE}
    HEADER_EXT .h .hh .tcc
    SOURCE_EXT .cc .cpp
)