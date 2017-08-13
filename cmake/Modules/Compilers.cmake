
################################################################################
#
#        Compilers
#
################################################################################
if(CMAKE_CXX_COMPILER MATCHES "icc.*")
    set(CMAKE_COMPILER_IS_INTEL_ICC ON)
endif()
if(CMAKE_CXX_COMPILER MATCHES "icpc.*")
    set(CMAKE_COMPILER_IS_INTEL_ICPC ON)
endif()

