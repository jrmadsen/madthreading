
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/threading/atomics)

include(MacroDefineModule)

DEFINE_MODULE(NAME mad.allocator
              HEADER_EXT ".h;.hh"
              SOURCE_EXT ".cc;.cpp")
