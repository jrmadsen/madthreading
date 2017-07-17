
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/threading/atomics)
include_directories(${PROJECT_SOURCE_DIR}/source/vectorization)

include(MacroDefineModule)

DEFINE_MODULE(NAME mad.threading
              HEADER_EXT ".h;.hh"
              SOURCE_EXT ".cc;.cpp")
