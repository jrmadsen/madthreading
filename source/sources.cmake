
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/atomics)
include_directories(${PROJECT_SOURCE_DIR}/source/vectorization)
include_directories(${PROJECT_SOURCE_DIR}/source/utility)

include(MacroDefineModule)

DEFINE_MODULE(NAME mad
                   HEADER_EXT ".h;.hh"
                   SOURCE_EXT ".cc;.cpp"
)
