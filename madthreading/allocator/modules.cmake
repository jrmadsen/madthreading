
include_directories(${PROJECT_SOURCE_DIR})
include(CMakeMacroParseArguments)
include(MacroDefineSWIGModule)

DEFINE_PYTHON_SWIG_MODULE(NAME allocator
              FILE allocator.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME allocator_list
#              FILE allocator_list.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME allocator_pool
#              FILE allocator_pool.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME cache_line_size
#              FILE cache_line_size.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})