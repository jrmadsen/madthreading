

include(CMakeMacroParseArguments)

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
    add_definitions(-DENABLE_${PKG})
endmacro()


################################################################################
#    Remove Package definitions
################################################################################
macro(REMOVE_PACKAGE_DEFINITIONS PKG)
    remove_definitions(-DENABLED_${PKG})
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
        #message(STATUS "Removing duplicates from ${_ARG} -- (${${_ARG}})")
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

    foreach(_file ${ARGN})

        string(REPLACE "${STRIP}/" "" _file_r ${_file})

        get_filename_component(_file_r ${_file_r} PATH)
        set(_list )
        set(_path ${_file_r})
        while(NOT "${_path}" STREQUAL "")
            get_filename_component(_tmp ${_path} NAME)

            list(APPEND _list ${_tmp})
            get_filename_component(_path ${_path} PATH)

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

endmacro()












