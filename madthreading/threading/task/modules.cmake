
include_directories(${PROJECT_SOURCE_DIR})

include(CMakeParseArguments)
include(MacroDefineSWIGModule)

DEFINE_PYTHON_SWIG_MODULE(NAME task_tree
              FILE task_tree.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME task_group
              FILE task_group.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})

