
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/threading/atomics)

include(CMakeMacroParseArguments)
include(MacroDefineSWIGModule)

#DEFINE_PYTHON_SWIG_MODULE(NAME atomic_array
#              FILE atomic_array.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME atomic_deque
#              FILE atomic_deque.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME atomic
#              FILE atomic.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME atomic_map
#              FILE atomic_map.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME atomic_typedefs
#              FILE atomic_typedefs.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME mutexed_pod
#              FILE mutexed_pod.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME omp_atomic
#              FILE omp_atomic.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})

