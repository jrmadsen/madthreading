
include_directories(${PROJECT_SOURCE_DIR})

include(CMakeParseArguments)
include(MacroDefineSWIGModule)

DEFINE_PYTHON_SWIG_MODULE(NAME atomic_array
              FILE atomic_array.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME atomic_deque
              FILE atomic_deque.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME atomic
              FILE atomic.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME atomic_map
              FILE atomic_map.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
