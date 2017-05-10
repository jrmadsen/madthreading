

include(CMakeMacroParseArguments)

cmake_policy(PUSH)
if(NOT CMAKE_VERSION VERSION_LESS 3.1)
    cmake_policy(SET CMP0054 NEW)
endif()

################################################################################
#   OPTIONS TAKING STRING
################################################################################
function(ParseCommandLineOptions)

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
    message("CMAKE_ARGS: ${CMAKE_ARGS}")

endfunction()

################################################################################
#   Propagate to parent scope
################################################################################
macro(Propagate SET_VARIABLE)
    set(${SET_VARIABLE} ${${SET_VARIABLE}} PARENT_SCOPE)
endmacro()


################################################################################
#   Propagate to parent scope
################################################################################
macro(PropagateSG SET_VARIABLE)
    set(SG_${SET_VARIABLE}_H ${SG_${SET_VARIABLE}_H} PARENT_SCOPE)
    set(SG_${SET_VARIABLE}_I ${SG_${SET_VARIABLE}_I} PARENT_SCOPE)
    set(PROJECT_FOLDERS ${PROJECT_FOLDERS} ${SET_VARIABLE})
    set(PROJECT_FOLDERS ${PROJECT_FOLDERS} PARENT_SCOPE)
    #message(STATUS "Adding ${SET_VARIABLE} to PROJECT_FOLDERS : ${PROJECT_FOLDERS}")
endmacro()


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
endmacro()


################################################################################
#    Undefined set
################################################################################
macro(undefset _ARG)
    if(NOT DEFINED ${_ARG})
        set(${_ARG} )
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
#    Add definitions if <OPTION> is true
################################################################################
macro(ADD_DEFINITIONS_IF OPT)
    if(${OPT})
        set(DEFS ${ARGN})
        if("${DEFS}" STREQUAL "")
            set(DEFS ${OPT})
        endif("${DEFS}" STREQUAL "")
        foreach(_ARG ${DEFS})
            add_definitions(-D${_ARG})
        endforeach(_ARG ${ARGN})
    endif(${OPT})
endmacro()

################################################################################
#    Remove duplicates if string exists
################################################################################
macro(REMOVE_DUPLICATES _ARG)
    if(NOT "${${_ARG}}" STREQUAL "")
        list(REMOVE_DUPLICATES ${_ARG})
    endif()
endmacro()

################################################################################
#    List subdirectories
################################################################################
function(list_subdirectories return_val current_dir return_relative)
    if(NOT EXISTS ${current_dir})
        message(FATAL_ERROR "ERROR in LIST_SUBDIRECTORIES : ${current_dir} does not exist")
    endif()
    # get everything in folder
    file(GLOB sub_dirs ${current_dir}/*)
    #message(STATUS "EVERYTHING: ${sub_dirs}")
    set(list_of_dirs )
    foreach(dir ${sub_dirs})
        #message(STATUS "Checking ${dir} for subdirectories...")
        if(IS_DIRECTORY ${dir})
            #message(STATUS "${dir} is a subdirectory")
            if (${return_relative})
                get_filename_component(dir ${dir} NAME)
                list(APPEND list_of_dirs ${dir})
            else()
                list(APPEND list_of_dirs ${dir})
            endif()
        endif()
    endforeach()
    set(${return_val} ${list_of_dirs} PARENT_SCOPE)
endfunction()

################################################################################
#   Load source files
################################################################################
macro(AddSubdirectoryFiles   NAME
                             NAME_ABBREV
                             SUB_DIR
                             CURRENT_DIR
                             GLOB_HEADER_DIR
                             HEADER_EXT
                             SOURCE_EXT
                             GLOB_HEADERS GLOB_SOURCES
                        )
    set(TOP_DIR ${${PROJECT_NAME}_SOURCE_DIR})
    set(DIR_PATH ${SUB_DIR})
    get_filename_component(TOP_DIR_NAME ${TOP_DIR} NAME)
    get_filename_component(PARENT_DIR ${CURRENT_DIR} PATH)
    #get_filename_component(PARENT_DIR ${PARENT_DIR} NAME)
    string(REPLACE "${TOP_DIR}/" "" PARENT_DIR ${PARENT_DIR})
    message(STATUS "\tAdding ${PARENT_DIR}/${SUB_DIR}")

    # Include the directory
    include_directories(${PROJECT_SOURCE_DIR}/${SUB_DIR} ${${GLOB_HEADER_DIR}})
    # Link the directory for object files
    link_directories(${PROJECT_BINARY_DIR}/${SUB_DIR})

    # Get files
    file(GLOB ${PROJECT_NAME}_${NAME}_HEADERS ${CURRENT_DIR}/*.${HEADER_EXT})
    file(GLOB ${PROJECT_NAME}_${NAME}_SOURCES ${CURRENT_DIR}/*.${SOURCE_EXT})

    #message("\n\t${PROJECT_NAME}_${NAME}_HEADERS are : ${${PROJECT_NAME}_${NAME}_HEADERS}\n")
    #message("\n\t${PROJECT_NAME}_${NAME}_SOURCES are : ${${PROJECT_NAME}_${NAME}_SOURCES}\n")

    # Append files to global set
    set(${GLOB_HEADERS} ${${GLOB_HEADERS}} ${${PROJECT_NAME}_${NAME}_HEADERS} PARENT_SCOPE)
    set(${GLOB_SOURCES} ${${GLOB_SOURCES}} ${${PROJECT_NAME}_${NAME}_SOURCES} PARENT_SCOPE)

    #message("\n\tGLOBAL Headers are : ${${GLOB_HEADERS}}\n")
    #message("\n\tGLOBAL Sources are : ${${GLOB_SOURCES}}\n")

    # set the source group
    set(SG_${NAME_ABBREV}_H ${${PROJECT_NAME}_${NAME}_HEADERS} PARENT_SCOPE)
    set(SG_${NAME_ABBREV}_I ${${PROJECT_NAME}_${NAME}_SOURCES} PARENT_SCOPE)

    #set(${SG_HEADER_SETS} ${${SG_HEADER_SETS}} ${SG_${NAME_ABBREV}_H} PARENT_SCOPE)
    #set(${SG_SOURCE_SETS} ${${SG_SOURCE_SETS}} ${SG_${NAME_ABBREV}_I} PARENT_SCOPE)

    #list(APPEND ${SG_HEADER_NAMES} "${SUB_DIR}/include")
    #list(APPEND ${SG_SOURCE_NAMES} "${SUB_DIR}/src")

    set(${GLOB_HEADER_DIR} ${${GLOB_HEADER_DIR}} ${CURRENT_DIR} PARENT_SCOPE)

endmacro()


################################################################################
#   Load source files
################################################################################
macro(AddSubdirectorySourceSorted  NAME
                             NAME_ABBREV
                             SUB_DIR
                             CURRENT_DIR
                             GLOB_HEADER_DIR
                             HEADER_FOLDER
                             SOURCE_FOLDER
                             HEADER_EXT
                             SOURCE_EXT
                             GLOB_HEADERS GLOB_SOURCES
                             #SG_HEADER_SETS SG_SOURCE_SETS
                             #SG_HEADER_NAMES SG_SOURCE_NAMES
                        )

    message(STATUS "\tAdding ${NAME}")

    # Include the directory
    include_directories(${PROJECT_SOURCE_DIR}/${SUB_DIR}/${HEADER_FOLDER} ${${GLOB_HEADER_DIR}})
    # Link the directory for object files
    link_directories(${PROJECT_BINARY_DIR}/${SUB_DIR}/${SOURCE_FOLDER})

    # Get files
    file(GLOB ${PROJECT_NAME}_${NAME}_HEADERS ${CURRENT_DIR}/${HEADER_FOLDER}/*.${HEADER_EXT})
    file(GLOB ${PROJECT_NAME}_${NAME}_SOURCES ${CURRENT_DIR}/${SOURCE_FOLDER}/*.${SOURCE_EXT})

    message("\n\tHeaders are : ${${PROJECT_NAME}_${NAME}_HEADERS}\n")
    message("\n\tSources are : ${${PROJECT_NAME}_${NAME}_SOURCES}\n")

    # Append files to global set
    set(${GLOB_HEADERS} ${${GLOB_HEADERS}} ${${PROJECT_NAME}_${NAME}_HEADERS} PARENT_SCOPE)
    set(${GLOB_SOURCES} ${${GLOB_SOURCES}} ${${PROJECT_NAME}_${NAME}_SOURCES} PARENT_SCOPE)

    #message("\n\tGLOBAL Headers are : ${${GLOB_HEADERS}}\n")
    #message("\n\tGLOBAL Sources are : ${${GLOB_SOURCES}}\n")

    # set the source group
    set(SG_${NAME_ABBREV}_H ${${PROJECT_NAME}_${NAME}_HEADERS} PARENT_SCOPE)
    set(SG_${NAME_ABBREV}_I ${${PROJECT_NAME}_${NAME}_SOURCES} PARENT_SCOPE)

endmacro()

################################################################################
#   Generate Source Grouping for list of files
################################################################################

macro(GenerateSourceGroupTree STRIP)

    foreach(_dir ${${STRIP}})
        get_filename_component(_dirpath "${_dir}" PATH)
        foreach(_file ${ARGN})

            string(REPLACE "${_dirpath}/" "" _file_r ${_file})

            get_filename_component(_file_r ${_file_r} PATH)
            set(_list )
            set(_path ${_file_r})
            while(NOT "${_path}" STREQUAL "")

                get_filename_component(_tmp ${_path} NAME)
                list(APPEND _list ${_tmp})

                set(_last_path ${_path})
                get_filename_component(_path ${_path} PATH)
                # check that we are not at upper-most directory (infinite loop)
                if("${_path}" STREQUAL "${_last_path}")
                    break()
                endif()
            endwhile()

            if(_list)
                list(REVERSE _list)
                set(_str "")
                foreach(_entry ${_list})
                    set(_str "${_str}\\${_entry}")
                endforeach()

                source_group("${_str}" FILES ${_file})
            endif()
        endforeach()
    endforeach()

endmacro()

################################################################################
#   Find a static library
################################################################################

function(find_static_library OUT LIB_NAME)

    if (WIN32 OR MSVC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    elseif (UNIX)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()

    find_library(FOUND ${LIB_NAME})

    if(NOT FOUND)
        message(SEND_ERROR "Unable to find static library ${LIB_NAME}")
    endif()

    set(${OUT} ${FOUND} PARENT_SCOPE)

endfunction()

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
        #message(STATUS "Installing ${WRITE_PATH}/${_fname} to ${INSTALL_PATH}...")
        install(FILES ${WRITE_PATH}/${_fname} DESTINATION ${INSTALL_PATH})
    endif()

endfunction()

cmake_policy(POP)
