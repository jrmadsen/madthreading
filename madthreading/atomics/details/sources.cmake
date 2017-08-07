
include_directories(${PROJECT_SOURCE_DIR})

include(MacroDefineModule)

DEFINE_MODULE(NAME mad.atomics.details
              HEADER_EXT ".h;.hh;.tcc"
              SOURCE_EXT ".cc;.cpp")

