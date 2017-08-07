
include_directories(${PROJECT_SOURCE_DIR})

include(MacroDefineModule)

DEFINE_MODULE(NAME mad.allocator
              HEADER_EXT ".h;.hh"
              SOURCE_EXT ".cc;.cpp")
