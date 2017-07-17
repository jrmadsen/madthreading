
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/atomics)
include_directories(${PROJECT_SOURCE_DIR}/source/vectorization)

include(CMakeMacroParseArguments)
include(MacroDefineSWIGModule)

#DEFINE_PYTHON_SWIG_MODULE(NAME AutoLock
#              FILE AutoLock.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
