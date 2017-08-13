
include_directories(${PROJECT_SOURCE_DIR})

include(MacroDefineModule)

DEFINE_MODULE(NAME mad
    EXCLUDE config
    HEADER_EXT ".h;.hh"
    SOURCE_EXT ".cc;.cpp"
)
