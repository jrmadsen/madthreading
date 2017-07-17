
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/atomics)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/threading/task)
include_directories(${PROJECT_SOURCE_DIR}/source/vectorization)
include_directories(${PROJECT_SOURCE_DIR}/source/utility)

include(CMakeMacroParseArguments)
include(MacroDefineSWIGModule)

DEFINE_PYTHON_SWIG_MODULE(NAME auto_lock
              FILE auto_lock.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME condition
              FILE condition.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME mutex
              FILE mutex.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME thread_manager
              FILE thread_manager.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME thread_pool
              FILE thread_pool.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
