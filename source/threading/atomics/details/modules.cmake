
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/threading/atomics)

include(CMakeMacroParseArguments)
include(MacroDefineSWIGModule)

#DEFINE_PYTHON_SWIG_MODULE(NAME atomic_allocator
#              FILE atomic_allocator.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME atomic_allocator_policy
#              FILE atomic_allocator_policy.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME atomic_allocator_traits
#              FILE atomic_allocator_traits.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})


