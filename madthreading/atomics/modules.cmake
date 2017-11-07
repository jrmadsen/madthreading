
include_directories(${PROJECT_SOURCE_DIR})

include(CMakeParseArguments)
include(MacroDefineSWIGModule)

DEFINE_PYTHON_SWIG_MODULE(NAME atomic
              FILE atomic.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
