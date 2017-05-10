# - Macros for organizing and specializing code in modules
#
# This file defines the following macros for  developers needing
# utlities for specializing source file properties
#
#
# It will also add the module include directory to the list of directories
# using include_directories
#
#
# ADD_COMPILE_DEFINITIONS - add compile defintions to a list of files
# ================================
# ADD_COMPILE_DEFINITIONS(SOURCES src1 src2 ...
#                                COMPILE_DEFINITIONS def1 def 2)
#
# Here, SOURCES is the list of source files to which compile definitions
# will be added. COMPILE_DEFINITIONS gives the list of definitions that
# should be added. These definitions will be appended to any existing
# definitions given to the sources.
#

# - Include guard
if(__macrodefineswigmodule_isloaded)
  return()
endif()
set(__macrodefineswigmodule_isloaded YES)

include(CMakeMacroParseArguments)

#-----------------------------------------------------------------------
# macro define_module(NAME <name>
#                            HEADER_EXT <header_ext1> <header_ext2> ... <header_extN>
#                            SOURCE_EXT <source_ext1> <source_ext2> ... <source_extN>
#                            LINK_LIBRARIES <lib1> ... <lib2>)
#       Define a  Module's source extensions and what internal and external
#       libraries it links to.
#
macro(DEFINE_PYTHON_SWIG_MODULE)
    cmake_parse_arguments(MODULE
                          ""
                          "NAME;PYTHON_VERSION;SWIG_FLAGS;FILE"
                          "LINK_LIBRARIES;OBJECT_LIBRARIES;SOURCES"
                          ${ARGN}
                         )
    #
    if(NOT DEFINED MODULE_PYTHON_VERSION)
        set(MODULE_PYTHON_VERSION 2.7)
    endif()
    #
    FIND_PACKAGE(SWIG)
    if(NOT SWIG_FOUND)
        return()
    endif()
    #
    INCLUDE(${SWIG_USE_FILE})
    #
    FIND_PACKAGE(PythonLibs ${MODULE_PYTHON_VERSION})
    #
    if(NOT PythonLibs_FOUND)
        return()
    endif()
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

    # get the path
    #get_filename_component(${MODULE_NAME}_BASEDIR ${CMAKE_CURRENT_LIST_FILE} PATH)
    #message(STATUS "DIR (1) - ${CMAKE_CURRENT_LIST_DIR}")
    #message(STATUS "DIR (2) - ${CMAKE_CURRENT_SOURCE_DIR}")

    if(NOT EXISTS ${MODULE_FILE})
        set(MODULE_FILE ${CMAKE_CURRENT_LIST_DIR}/${MODULE_FILE})
    endif()

    get_directory_property(_defs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS)
    set(defs)
    foreach(_def ${_defs})
        list(APPEND defs -D${_def})
    endforeach()

    if(NOT WIN32)
        list(APPEND defs -D__unix__)
    endif()
    if(APPLE)
        list(APPEND defs -D__MACH__)
        list(APPEND defs -D__APPLE__)
    else()
        list(APPEND defs -D__linux__)
    endif()
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        list(APPEND defs -D__x86_64__)
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
        list(APPEND defs -D__clang__)
    elseif(CMAKE_COMPILER_IS_GNUCXX)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
            OUTPUT_VARIABLE _gnucxx_version
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(REPLACE "." ";" _gnucxx_version "${_gnucxx_version}")
        list(GET _gnucxx_version 0 _gnucxx_version_major)
        list(GET _gnucxx_version 1 _gnucxx_version_minor)
        list(APPEND defs -D__GNUC__=${_gnucxx_version_major})
        list(APPEND defs -D__GNUC_MINOR__=${_gnucxx_version_minor})
    endif()

    SET(CMAKE_SWIG_FLAGS "")
    SET(CMAKE_SWIG_OUTDIR ${PROJECT_BINARY_DIR}/python)
    SET_SOURCE_FILES_PROPERTIES(${MODULE_FILE}
                                PROPERTIES
                                  CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${MODULE_FILE}
                                PROPERTIES
                                  SWIG_FLAGS "-cpperraswarn;${defs}")

    #
    set(MODULE_OBJ_LIBRARIES )
    foreach(_obj ${MODULE_OBJECT_LIBRARIES})
        if(NOT "${_obj}" MATCHES ".*TARGET_OBJECTS:.*")
            list(APPEND MODULE_OBJ_LIBRARIES "$<TARGET_OBJECTS:${_obj}>")
        else()
            list(APPEND MODULE_OBJ_LIBRARIES "${_obj}")
        endif()
    endforeach()
    #
    set(_${MODULE_NAME}_DIR ${CMAKE_CURRENT_LIST_DIR})
    SWIG_ADD_MODULE(${MODULE_NAME} python
                    ${MODULE_FILE}
                    ${MODULE_SOURCES}
                    ${MODULE_OBJ_LIBRARIES})

    SWIG_LINK_LIBRARIES(${MODULE_NAME} ${PYTHON_LIBRARIES}
                                       ${MODULE_LINK_LIBRARIES})
    # installation
    install(TARGETS ${SWIG_MODULE_${MODULE_NAME}_REAL_NAME}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
    install(FILES ${CMAKE_SWIG_OUTDIR}/${MODULE_NAME}.py
            DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
    # init file
    if(NOT EXISTS "${CMAKE_SWIG_OUTDIR}/__init__.py")
        file(WRITE "${CMAKE_SWIG_OUTDIR}/__init__.py")
        install(FILES ${CMAKE_SWIG_OUTDIR}/__init__.py
                DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
    endif()
endmacro()

