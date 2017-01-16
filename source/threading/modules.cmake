
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/allocator)
include_directories(${PROJECT_SOURCE_DIR}/source/threading)
include_directories(${PROJECT_SOURCE_DIR}/source/threading/atomics)

include(CMakeMacroParseArguments)
include(MacroDefineSWIGModule)

DEFINE_PYTHON_SWIG_MODULE(NAME AutoLock
              FILE AutoLock.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME condition
#              FILE condition.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME joining_task
#              FILE joining_task.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME mutex
#              FILE mutex.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME mutex_typedefs
#              FILE mutex_typedefs.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME task
#              FILE task.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME task_tree
#              FILE task_tree.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME threading
#              FILE threading.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
DEFINE_PYTHON_SWIG_MODULE(NAME thread_manager
              FILE thread_manager.hh
              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME thread_pool
#              FILE thread_pool.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})
#DEFINE_PYTHON_SWIG_MODULE(NAME tls
#              FILE tls.hh
#              LINK_LIBRARIES ${PROJECT_LIBRARIES})

