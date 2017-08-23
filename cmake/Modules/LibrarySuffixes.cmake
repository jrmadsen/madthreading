#
#
#

if(__librarysuffixes_isloaded)
    return()
endif(__librarysuffixes_isloaded)
set(__librarysuffixes_isloaded ON)

STRING(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type)

set(DEFAULT_SHARED_LIBRARY_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}"
    CACHE STRING "Default suffix of shared libraries")
set(DEFAULT_STATIC_LIBRARY_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}"
    CACHE STRING "Default suffix of static libraries")

if("${_build_type}" STREQUAL "DEBUG")
    set(CMAKE_SHARED_LIBRARY_SUFFIX "-debug${DEFAULT_SHARED_LIBRARY_SUFFIX}")
    set(CMAKE_STATIC_LIBRARY_SUFFIX "-debug${DEFAULT_STATIC_LIBRARY_SUFFIX}")
elseif("${_build_type}" STREQUAL "RELWITHDEBINFO")
    set(CMAKE_SHARED_LIBRARY_SUFFIX "-rdebug${DEFAULT_SHARED_LIBRARY_SUFFIX}")
    set(CMAKE_STATIC_LIBRARY_SUFFIX "-rdebug${DEFAULT_STATIC_LIBRARY_SUFFIX}")
elseif("${_build_type}" STREQUAL "MINSIZEREL")
    set(CMAKE_SHARED_LIBRARY_SUFFIX "-mrel${DEFAULT_SHARED_LIBRARY_SUFFIX}")
    set(CMAKE_STATIC_LIBRARY_SUFFIX "-mrel${DEFAULT_STATIC_LIBRARY_SUFFIX}")
endif()

unset(_build_type)
