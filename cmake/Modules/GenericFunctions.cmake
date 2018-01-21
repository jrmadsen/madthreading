
# - Include guard
if(__genericfunctions_isloaded)
  return()
endif()
set(__genericfunctions_isloaded YES)

include(CMakeParseArguments)

cmake_policy(PUSH)
if(NOT CMAKE_VERSION VERSION_LESS 3.1)
    cmake_policy(SET CMP0054 NEW)
endif()

################################################################################
#   OPTIONS TAKING STRING
################################################################################
function(parse_command_line_options)

    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    foreach(CACHE_VAR ${CACHE_VARS})

        get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
        if(CACHE_VAR_HELPSTRING STREQUAL "No help, variable specified on the command line.")
            get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
            if(CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
                set(CACHE_VAR_TYPE)
            else()
                set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
            endif()
            set(CMAKE_ARGS "${CMAKE_ARGS} -D${CACHE_VAR}${CACHE_VAR_TYPE}=\"${${CACHE_VAR}}\"")
        endif()
    endforeach()
    set(CMAKE_ARGS "${CMAKE_ARGS}" PARENT_SCOPE)

endfunction()


################################################################################
#   Macro to add to string
################################################################################
macro(add _VAR _FLAG)
    if(NOT "${_FLAG}" STREQUAL "")
        if("${${_VAR}}" STREQUAL "")
            set(${_VAR} "${_FLAG}")
        else()
            set(${_VAR} "${${_VAR}} ${_FLAG}")
        endif()
    endif()
endmacro()


################################################################################
#    Add Package definitions
################################################################################
macro(ADD_PACKAGE_DEFINITIONS PKG)
    add_definitions(-DUSE_${PKG})
endmacro()


################################################################################
#    Remove Package definitions
################################################################################
macro(REMOVE_PACKAGE_DEFINITIONS PKG)
    remove_definitions(-DUSE_${PKG})
endmacro()


################################################################################
# function - capitalize - make a string capitalized (first letter is capital)
#   usage:
#       capitalize("SHARED" CShared)
#   message(STATUS "-- CShared is \"${CShared}\"")
#   $ -- CShared is "Shared"
################################################################################
function(capitalize str var)
    # make string lower
    string(TOLOWER "${str}" str)
    string(SUBSTRING "${str}" 0 1 _first)
    string(TOUPPER "${_first}" _first)
    string(SUBSTRING "${str}" 1 -1 _remainder)
    string(CONCAT str "${_first}" "${_remainder}")
    set(${var} "${str}" PARENT_SCOPE)
endfunction()


################################################################################
#   Set project version
################################################################################
macro(SET_PROJECT_VERSION _MAJOR _MINOR _PATCH _DESCRIPTION)
    set(${PROJECT_NAME}_MAJOR_VERSION "${_MAJOR}")
    set(${PROJECT_NAME}_MINOR_VERSION "${_MINOR}")
    set(${PROJECT_NAME}_PATCH_VERSION "${_PATCH}")
    set(${PROJECT_NAME}_VERSION
        "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION}")
    set(${PROJECT_NAME}_SHORT_VERSION
        "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}")
    set(${PROJECT_NAME}_DESCRIPTION "${_DESCRIPTION}")
    set(PROJECT_VERSION ${${PROJECT_NAME}_VERSION})
endmacro()


################################################################################
#    Get version components
################################################################################
function(GET_VERSION_COMPONENTS OUTPUT_VAR VARIABLE)
    string(REPLACE "." ";" VARIABLE_LIST "${VARIABLE}")
    list(LENGTH VARIABLE_LIST VAR_LENGTH)
    if(VAR_LENGTH GREATER 0)
        list(GET VARIABLE_LIST 0 MAJOR)
        set(${OUTPUT_VAR}_MAJOR_VERSION "${MAJOR}" PARENT_SCOPE)
    endif()
    if(VAR_LENGTH GREATER 1)
        list(GET VARIABLE_LIST 1 MINOR)
        set(${OUTPUT_VAR}_MINOR_VERSION "${MINOR}" PARENT_SCOPE)
        set(${OUTPUT_VAR}_SHORT_VERSION "${MAJOR}.${MINOR}" PARENT_SCOPE)
    endif()
    if(VAR_LENGTH GREATER 2)
        list(GET VARIABLE_LIST 2 PATCH)
        set(${OUTPUT_VAR}_PATCH_VERSION "${PATCH}" PARENT_SCOPE)
    endif()
endfunction()


################################################################################
#    Remove duplicates if string exists
################################################################################
macro(REMOVE_DUPLICATES _ARG)
    if(NOT "${${_ARG}}" STREQUAL "")
        list(REMOVE_DUPLICATES ${_ARG})
    endif()
endmacro()


################################################################################
#   set the two digit version
################################################################################
function(define_version_00 PREFIX POSTFIX)
    if("${PREFIX}" STREQUAL "" OR "${POSTFIX}" STREQUAL "")
        message(WARNING "Unable to define version_00")
        return()
    endif()

    set(_full ${PREFIX}_${POSTFIX}_VERSION)
    if(${_full} LESS 10)
        set(${_full}_00 "0${${_full}}" PARENT_SCOPE)
    else()
        set(${_full}_00 "${${_full}}" PARENT_SCOPE)
    endif()

endfunction()


################################################################################
#   Write a config file for installation (writes preprocessor definitions)
#
#  Usage: write_config_file("config.hh" ${PROJECT_SOURCE_DIR}/source)
#       ARG1 - the name of the file
#       ARG2 - the file to be written
#   An optional 3rd argument
#
################################################################################
function(write_config_file FILE_NAME WRITE_PATH)

    get_directory_property(_defs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS)

    STRING(TOLOWER ${PROJECT_NAME} _project)
    STRING(TOLOWER ${FILE_NAME} _file)
    set(_name "${_project}_${_file}")
    STRING(REPLACE "." "_" _name "${_name}")
    STRING(REPLACE "/" "_" _name "${_name}")
    STRING(TOLOWER "${_name}" _name)

    set(header1 "#ifndef ${_name}_")
    set(header2 "#define ${_name}_")

    set(_output "//\n//\n//\n//\n//\n\n${header1}\n${header2}\n")

    if(DEFINED ${PROJECT_NAME}_VERSION)
        define_version_00(${PROJECT_NAME} MAJOR)
        define_version_00(${PROJECT_NAME} MINOR)
        define_version_00(${PROJECT_NAME} PATCH)

        set(_prefix ${PROJECT_NAME})
        if("${${_prefix}_PATCH}" GREATER 0)
          set(VERSION_STRING "${${_prefix}_MAJOR_VERSION}_${${_prefix}_MINOR_VERSION}_${${_prefix}_PATCH_VERSION}")
        else()
          set(VERSION_STRING "${${_prefix}_MAJOR_VERSION}_${${_prefix}_MINOR_VERSION}")
        endif()

        set(s1 "//")
        set(s2 "//  Caution, this is the only ${PROJECT_NAME} header that is guaranteed")
        set(s3 "//  to change with every release, including this header")
        set(s4 "//  will cause a recompile every time a new ${PROJECT_NAME} version is")
        set(s5 "//  released.")
        set(s6 "//")
        set(s7 "//  ${PROJECT_NAME}_VERSION % 100 is the patch level")
        set(s8 "//  ${PROJECT_NAME}_VERSION / 100 % 1000 is the minor version")
        set(s9 "//  ${PROJECT_NAME}_VERSION / 100000 is the major version")

        set(_output "${_output}\n${s1}\n${s2}\n${s3}\n${s4}\n${s5}\n${s6}\n${s7}\n${s8}\n${s9}\n")
        set(_output "${_output}\n#define ${PROJECT_NAME}_VERSION")
        set(_output "${_output} ${${_prefix}_MAJOR_VERSION_00}${${_prefix}_MINOR_VERSION_00}${${_prefix}_PATCH_VERSION_00}\n")

        set(s1 "//")
        set(s2 "//  ${PROJECT_NAME}_LIB_VERSION must be defined to be the same as")
        set(s3 "//  ${PROJECT_NAME}_VERSION but as a *string* in the form \"x_y[_z]\" where x is")
        set(s4 "//  the major version number, y is the minor version number, and z is the patch")
        set(s5 "//  level if not 0.")

        set(_output "${_output}\n${s1}\n${s2}\n${s3}\n${s4}\n${s5}\n")
        set(_output "${_output}\n#define ${PROJECT_NAME}_LIB_VERSION \"${VERSION_STRING}\"\n")
    endif()

    set(_output "${_output}\n\n//\n// DEFINES\n//\n")

    # if DEBUG, add ${PROJECT_NAME}_DEBUG and same for NDEBUG
    foreach(_def ${_defs})
        if("${_def}" STREQUAL "DEBUG")
            list(APPEND _defs ${PROJECT_NAME}_DEBUG)
        elseif("${_def}" STREQUAL "NDEBUG")
            list(APPEND _defs ${PROJECT_NAME}_NDEBUG)
        endif()
    endforeach()

    list(REMOVE_ITEM _defs DEBUG)
    list(REMOVE_ITEM _defs NDEBUG)

    foreach(_def ${_defs})
        set(_str1 "#if !defined(${_def})")
        set(_str2 "#   define ${_def}")
        set(_str3 "#endif")
        set(_output "${_output}\n${_str1}\n${_str2}\n${_str3}\n")
    endforeach()

    list(APPEND _defs ${CMAKE_PROJECT_NAME}_DEBUG)
    list(APPEND _defs ${CMAKE_PROJECT_NAME}_NDEBUG)
    list(REMOVE_DUPLICATES _defs)

    set(_output "${_output}\n\n//\n// To avoid any of the definitions, define DONT_{definition}\n//\n")
    foreach(_def ${_defs})
        set(_str1 "#if defined(DONT_${_def})")
        set(_str2 "#   if defined(${_def})")
        set(_str3 "#       undef ${_def}")
        set(_str4 "#   endif")
        set(_str5 "#endif")
        set(_output "${_output}\n${_str1}\n${_str2}\n${_str3}\n${_str4}\n${_str5}\n")
    endforeach()

    set(_output "${_output}\n\n#endif // end ${_name}_\n\n")

    get_filename_component(_fname ${FILE_NAME} NAME)
    if(NOT EXISTS ${WRITE_PATH}/${_fname})
        file(WRITE ${WRITE_PATH}/${_fname} ${_output})
    else()
        file(WRITE ${CMAKE_BINARY_DIR}/.config_diff/${_fname} ${_output})
        file(STRINGS ${WRITE_PATH}/${_fname} _existing)
        file(STRINGS ${CMAKE_BINARY_DIR}/.config_diff/${_fname} _just_written)
        string(COMPARE EQUAL "${_existing}" "${_just_written}" _diff)
        if(NOT _diff)
            file(WRITE ${WRITE_PATH}/${_fname} ${_output})
        endif()
    endif()

    if(NOT "${ARGN}" STREQUAL "")
        list(GET ARGN 0 INSTALL_PATH)
        install(FILES ${WRITE_PATH}/${_fname} DESTINATION ${INSTALL_PATH})
    endif()

endfunction()

################################################################################
#   remove a file from a list of files
################################################################################
macro(REMOVE_FILE _LIST _NAME)

    # loop over the list
    foreach(_FILE ${${_LIST}})
        # get the base filename
        get_filename_component(_BASE ${_FILE} NAME)
        # check if full filename or base filename is exact match for
        # name provided
        if("${_BASE}" STREQUAL "${_NAME}" OR
           "${_FILE}" STREQUAL "${_NAME}")
            # remove from list
            list(REMOVE_ITEM ${_LIST} ${_FILE})

        endif("${_BASE}" STREQUAL "${_NAME}" OR
              "${_FILE}" STREQUAL "${_NAME}")

    endforeach(_FILE ${${_LIST}})

endmacro(REMOVE_FILE _LIST _NAME)

################################################################################
#   check a return value
################################################################################

function(check_return _VAR)
    if("${${_VAR}}" GREATER 0)
        message(WARNING "Error code for ${_VAR} is greater than zero: ${${_VAR}}")
    endif("${${_VAR}}" GREATER 0)
endfunction(check_return _VAR)

################################################################################
#   numerically sort a list
################################################################################

function(numeric_sort _VAR)
    set(_LIST ${ARGN})
    set(NOT_FINISHED ON)
    while(NOT_FINISHED)
        set(_N 0)
        list(LENGTH _LIST _L)
        math(EXPR _L "${_L}-1")
        set(NOT_FINISHED OFF)
        while(_N LESS _L)
            math(EXPR _P "${_N}+1")
            list(GET _LIST ${_N} _A)
            list(GET _LIST ${_P} _B)
            if(_B LESS _A)
                list(REMOVE_AT _LIST ${_P})
                list(INSERT _LIST ${_N} ${_B})
                set(NOT_FINISHED ON)
            endif(_B LESS _A)
            math(EXPR _N "${_N}+1")
        endwhile(_N LESS _L)
    endwhile(NOT_FINISHED)
    set(${_VAR} ${_LIST} PARENT_SCOPE)
endfunction(numeric_sort _LIST)

################################################################################
#   Get a parameter from a CMake file
#   - useful when variable is set to CACHE but needed as non-cache
################################################################################

function(GET_PARAMETER _VAR _FILE _PARAM)

    execute_process(COMMAND ${CMAKE_COMMAND} -DFILE=${_FILE} -DQUERY=${_PARAM}
        -P ${CMAKE_SOURCE_DIR}/cmake/Scripts/GetParam.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_VARIABLE PARAM
    RESULT_VARIABLE RET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "^-- " "" PARAM "${PARAM}")
    if(NOT RET GREATER 0)
        set(${_VAR} ${PARAM} PARENT_SCOPE)
    else(NOT RET GREATER 0)
        message(FATAL_ERROR "Error retrieving ${_PARAM} value from \"${_FILE}\"")
    endif(NOT RET GREATER 0)

endfunction(GET_PARAMETER _VAR _FILE _PARAM)

################################################################################
#   get value in [0-9][0-9] format
################################################################################

function(GET_00 _VAR _VAL)
    if(${_VAL} LESS 10)
        set(${_VAR} "0${_VAL}" PARENT_SCOPE)
    else()
        set(${_VAR} "${_VAL}" PARENT_SCOPE)
    endif()
endfunction(GET_00 _VAR _VAL)

################################################################################
# Get the hostname
################################################################################

function(GET_HOSTNAME VAR)
    find_program(HOSTNAME_COMMAND NAMES hostname)
    execute_process(COMMAND ${HOSTNAME_COMMAND}
        OUTPUT_VARIABLE HOSTNAME WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(${VAR} ${HOSTNAME} PARENT_SCOPE)
endfunction(GET_HOSTNAME VAR)

################################################################################
# Function for usine "uname" for OS data
################################################################################

function(GET_UNAME NAME FLAG)
    find_program(UNAME_COMMAND NAMES uname)
    # checking if worked
    set(_RET 1)
    # iteration limiting
    set(_NITER 0)
    # empty
    set(_NAME "")
    while(_RET GREATER 0 AND _NITER LESS 100 AND "${_NAME}" STREQUAL "")
        execute_process(COMMAND ${UNAME_COMMAND} ${FLAG}
            OUTPUT_VARIABLE _NAME
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _RET)
        math(EXPR _NITER "${_NITER}+1")
    endwhile(_RET GREATER 0 AND _NITER LESS 100 AND "${_NAME}" STREQUAL "")
    # fail if not successful
    if(_RET GREATER 0)
        message(FATAL_ERROR
            "Unable to successfully execute: '${UNAME_COMMAND} ${FLAG}'")
    endif(_RET GREATER 0)
    # set the variable in parent scope
    set(${NAME} ${_NAME} PARENT_SCOPE)
endfunction(GET_UNAME NAME FLAG)

################################################################################
# Get the OS system name (e.g. Darwin, Linux)
################################################################################

function(GET_OS_SYSTEM_NAME VAR)
    get_uname(_VAR -s)
    set(${VAR} ${_VAR} PARENT_SCOPE)
endfunction(GET_OS_SYSTEM_NAME VAR)

################################################################################
# Get the OS node name (e.g. JRM-macOS-DOE.local, cori09)
################################################################################

function(GET_OS_NODE_NAME VAR)
    get_uname(_VAR -n)
    set(${VAR} ${_VAR} PARENT_SCOPE)
endfunction(GET_OS_NODE_NAME VAR)

################################################################################
# Get the OS machine hardware name (e.g. x86_64)
################################################################################

function(GET_OS_MACHINE_HARDWARE_NAME VAR)
    get_uname(_VAR -m)
    set(${VAR} ${_VAR} PARENT_SCOPE)
endfunction(GET_OS_MACHINE_HARDWARE_NAME VAR)


cmake_policy(POP)
