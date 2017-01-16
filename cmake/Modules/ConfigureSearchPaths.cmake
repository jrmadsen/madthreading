#
#   Sets:
#       CMAKE_PREFIX_PATH
#       CMAKE_INCLUDE_PATH
#       CMAKE_LIBRARY_PATH
#

# - Include guard
if(__configuresearchpaths_is_loaded)
    return()
endif()
set(__configuresearchpaths_is_loaded YES)

set(_BIN_PATH_ $ENV{PATH})
set(_LIB_PATH_ $ENV{LD_LIBRARY_PATH} $ENV{DYLD_LIBRARY_PATH} $ENV{LIBRARY_PATH})
set(_MAN_PATH_ $ENV{MANPATH})

#message(STATUS "_BIN_PATH_ : ${_BIN_PATH_}")
#message(STATUS "_LIB_PATH_ : ${_LIB_PATH_}")
#message(STATUS "_MAN_PATH_ : ${_MAN_PATH_}")

if(NOT "${_BIN_PATH_}" STREQUAL "")
    STRING(REPLACE ":" ";" _BIN_PATH_ ${_BIN_PATH_})
endif()

if(NOT "${_LIB_PATH_}" STREQUAL "")
    STRING(REPLACE ":" ";" _LIB_PATH_ ${_LIB_PATH_})
endif()

if(NOT "${_MAN_PATH_}" STREQUAL "")
    STRING(REPLACE ":" ";" _MAN_PATH_ ${_MAN_PATH_})
endif()

#message(STATUS "_BIN_PATH_ : ${_BIN_PATH_}")
#message(STATUS "_LIB_PATH_ : ${_LIB_PATH_}")
#message(STATUS "_MAN_PATH_ : ${_MAN_PATH_}")

macro(ParsePath _VAR _LIST _RM)
    #set(_tmp ${${_LIST}})
    set(_tmp )
    if(NOT "${_RM}" STREQUAL "")
        # loop of the path list
        foreach(_path ${${_LIST}})
            # get the name of the current directory
            get_filename_component(_path_name ${_path} NAME)
            # loop over the folders to remove
            foreach(_rmstr ${_RM})
                # if the last folder should be remove
                if("${_path_name}" STREQUAL "${_rmstr}")
                    # remove the last folder
                    get_filename_component(_path ${_path} PATH)
                endif()
            endforeach()
            # if it wasn't /bin, /include, /lib, etc.
            if(NOT "${_path}" STREQUAL "/")
                list(APPEND _tmp ${_path})
            endif()
        endforeach()
    endif()
    # append the variable name
    list(APPEND ${_VAR} ${_tmp})
    # remove the duplicates
    if(NOT "${_VAR}" STREQUAL "")
        list(REMOVE_DUPLICATES ${_VAR})
    endif()
endmacro()

ParsePath(_PREFIX_PATH _BIN_PATH_ "sbin;bin")
ParsePath(_PREFIX_PATH _LIB_PATH_ "lib64;lib")
# share and man need to be removed, by adding twice this helps ensure this
ParsePath(_PREFIX_PATH _MAN_PATH_ "share;man;share;man")

set(CMAKE_PREFIX_PATH ${_PREFIX_PATH} CACHE PATH "CMake prefix paths" FORCE)

set(_INCLUDE_PATH ${CMAKE_INCLUDE_PATH})
set(_LIBRARY_PATH ${CMAKE_LIBRARY_PATH})

foreach(_path ${_PREFIX_PATH})
    list(APPEND _INCLUDE_PATH "${_path}/include")
endforeach()

set(CMAKE_INCLUDE_PATH ${_INCLUDE_PATH} CACHE PATH "CMake include paths" FORCE)

foreach(_path ${_PREFIX_PATH})
    list(APPEND _LIBRARY_PATH "${_path}/lib")
    list(APPEND _LIBRARY_PATH "${_path}/lib64")
endforeach()

set(CMAKE_LIBRARY_PATH ${_LIBRARY_PATH} CACHE PATH "CMake include paths" FORCE)

unset(_BIN_PATH_)
unset(_LIB_PATH_)
unset(_MAN_PATH_)

unset(_PREFIX_PATH)
unset(_LIBRARY_PATH)
unset(_INCLUDE_PATH)

mark_as_advanced(CMAKE_PREFIX_PATH CMAKE_INCLUDE_PATH CMAKE_LIBRARY_PATH)
