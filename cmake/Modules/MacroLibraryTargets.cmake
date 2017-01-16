# - Define useful macros
#
# A custom compile definition "DEVELOPER_<CONFIG>" is set on
# each target using the target property COMPILE_DEFINITIONS_<CONFIG>
# target property

if(__MACROLIBRARYTARGETS_ISLOADED)
  return()
endif()
set(__MACROLIBRARYTARGETS_ISLOADED TRUE)

include(CMakeMacroParseArguments)
include(MacroDefineModule)

#-----------------------------------------------------------------------
# function compile_definitions_config(<target>)
#          Set a custom compile definition for a  target on a
#          per configuration basis:
#            For mode <CONFIG>, define DEVELOPER_<CONFIG>
#
function(compile_definitions_config _target)
    if(NOT TARGET ${_target})
        message(FATAL_ERROR "compile_definitions_config passed target '${_target}' which is not a valid CMake target")
    endif()

    if(CMAKE_CONFIGURATION_TYPES)
        # - Multimode tools
        foreach(_mode ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${_mode} _mode_upper)
            set_property(TARGET ${_target}
                         APPEND PROPERTY COMPILE_DEFINITIONS_${_mode_upper} DEVELOPER_${_mode_upper}
                        )
        endforeach()
    elseif(CMAKE_BUILD_TYPE)
        # - Single mode tools, only if set
        string(TOUPPER ${CMAKE_BUILD_TYPE} _mode_upper)
        set_property(TARGET ${_target}
                     APPEND PROPERTY COMPILE_DEFINITIONS_${_mode_upper} DEVELOPER_${_mode_upper}
                    )
    endif()
endfunction()

#-----------------------------------------------------------------------
# - OBJECT_TARGET
# General build of a  library target
#
macro(OBJECT_TARGET)
    cmake_parse_arguments(OBJTARG
                          ""
                          "NAME"
                          "SOURCES"
                          ${ARGN}
                          )

    # We have to distinguish the static from shared lib, so use -static in
    # name. Link its dependencies, and ensure we actually link to the
    # -static targets (We should strictly do this for the external
    # libraries as well if we want a pure static build).
    add_library(${OBJTARG_NAME} OBJECT ${OBJTARG_SOURCES})
    #compile_definitions_config(${LIBTARG_NAME})

    # from DEFINE_MODULE
    get_property(SOURCE_GROUPS GLOBAL PROPERTY SOURCE_GROUPS)
    foreach(_src_grp ${SOURCE_GROUPS})
        get_property(HEADER_FOLDER GLOBAL PROPERTY ${_src_grp}_HEADER_FOLDER)
        get_property(SOURCE_FOLDER GLOBAL PROPERTY ${_src_grp}_SOURCE_FOLDER)
        get_property(HEADER_FILES GLOBAL PROPERTY ${_src_grp}_HEADER_FILES)
        get_property(SOURCE_FILES GLOBAL PROPERTY ${_src_grp}_SOURCE_FILES)
        #message(STATUS "${_src_grp} has headers in ${HEADER_FOLDER} (${HEADER_FILES}) ")
        source_group("${HEADER_FOLDER}" FILES ${HEADER_FILES})
        #message(STATUS "${_src_grp} has headers in ${SOURCE_FOLDER} (${SOURCE_FILES}) ")
        source_group("${SOURCE_FOLDER}" FILES ${SOURCE_FILES})
    endforeach()
    # But we can rename the output library to the correct name
    # On WIN32 we *retain* the -static postfix because otherwise
    # we'll conflict with the .lib from the DLL build...
    # We could also install differently...
    #if(NOT WIN32)
    #  set_target_properties(${LIBTARG}
    #        PROPERTIES OUTPUT_NAME ${LIBTARG_NAME})
    #endif()

    set_property(GLOBAL APPEND
      PROPERTY OBJECT_TARGETS ${OBJTARGET_NAME})

endmacro()

#-----------------------------------------------------------------------
# - LIBRARY_TARGET
# General build of a  library target
#
macro(LIBRARY_TARGET)
    cmake_parse_arguments(LIBTARG
                          ""
                          "NAME"
                          "SOURCES;LINK_LIBRARIES"
                          ${ARGN}
                          )

    # We have to distinguish the static from shared lib, so use -static in
    # name. Link its dependencies, and ensure we actually link to the
    # -static targets (We should strictly do this for the external
    # libraries as well if we want a pure static build).
    add_library(${LIBTARG_NAME} STATIC ${LIBTARG_SOURCES})
    #compile_definitions_config(${LIBTARG_NAME})
    target_link_libraries(${LIBTARG_NAME}
                          ${LIBTARG_LINK_LIBRARIES})

    # But we can rename the output library to the correct name
    # On WIN32 we *retain* the -static postfix because otherwise
    # we'll conflict with the .lib from the DLL build...
    # We could also install differently...
    #if(NOT WIN32)
    #  set_target_properties(${LIBTARG}
    #        PROPERTIES OUTPUT_NAME ${LIBTARG_NAME})
    #endif()

    set_target_properties(${LIBTARG_NAME}
      PROPERTIES CLEAN_DIRECT_OUTPUT 1
                 LINKER_LANGUAGE CXX
                 INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR})

    install(TARGETS ${LIBTARG_NAME}
      EXPORT LibraryDepends
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development)

    set_property(GLOBAL APPEND
      PROPERTY EXPORTED_TARGETS ${LIBTARGET_NAME})

endmacro()


#-----------------------------------------------------------------------
# - HEADER_TARGET
# General build of a header-only target
#
macro(HEADER_TARGET)
    cmake_parse_arguments(HEADTARG
                          ""
                          "NAME"
                          "SOURCES;LINK_LIBRARIES"
                          ${ARGN}
                          )

    # We have to distinguish the static from shared lib, so use -static in
    # name. Link its dependencies, and ensure we actually link to the
    # -static targets (We should strictly do this for the external
    # libraries as well if we want a pure static build).
    if(BUILD_SHARED_LIBS)
        add_library(${HEADTARG_NAME} SHARED ${HEADTARG_SOURCES})
    endif()

    if(BUILD_STATIC_LIBS)
        add_library(${HEADTARG_NAME}-static STATIC ${HEADTARG_SOURCES})
    endif()

    # from DEFINE_MODULE
    get_property(SOURCE_GROUPS GLOBAL PROPERTY SOURCE_GROUPS)
    foreach(_src_grp ${SOURCE_GROUPS})
        get_property(HEADER_FOLDER GLOBAL PROPERTY ${_src_grp}_HEADER_FOLDER)
        get_property(SOURCE_FOLDER GLOBAL PROPERTY ${_src_grp}_SOURCE_FOLDER)
        get_property(HEADER_FILES GLOBAL PROPERTY ${_src_grp}_HEADER_FILES)
        get_property(SOURCE_FILES GLOBAL PROPERTY ${_src_grp}_SOURCE_FILES)
        #message(STATUS "${_src_grp} has headers in ${HEADER_FOLDER} (${HEADER_FILES}) ")
        source_group("${HEADER_FOLDER}" FILES ${HEADER_FILES})
        #message(STATUS "${_src_grp} has headers in ${SOURCE_FOLDER} (${SOURCE_FILES}) ")
        source_group("${SOURCE_FOLDER}" FILES ${SOURCE_FILES})
    endforeach()

    if(BUILD_SHARED_LIBS)
        set_target_properties(${HEADTARG_NAME}
                              PROPERTIES
                              LINKER_LANGUAGE CXX )
    endif()

    if(BUILD_STATIC_LIBS)
        set_target_properties(${HEADTARG_NAME}-static
                              PROPERTIES
                              LINKER_LANGUAGE CXX )
    endif()




endmacro()

#-----------------------------------------------------------------------
# - GLOBAL_OBJECT_TARGET
# Build and install of a  category library
#
MACRO(GLOBAL_OBJECT_TARGET)
  CMAKE_PARSE_ARGUMENTS(GLOBOBJ
                        ""
                        "NAME"
                        "COMPONENTS"
                        ${ARGN}
                        )

    # We loop over the component sources one at a time,
    # appending properties as we go.
    foreach(_comp ${GLOBOBJ_COMPONENTS})
        include(${_comp})
        # In case we have a global lib with one component, ensure name gets set
        if(NOT GLOBOBJ_NAME)
            set(GLOBOBJ_NAME ${MODULE_NAME})
        endif()

        #message(STATUS "${MODULE_NAME} srcs : ${${MODULE_NAME}_SOURCES}")
        list(APPEND ${GLOBOBJ_NAME}_GLOBAL_SOURCES ${${MODULE_NAME}_SOURCES})
        list(APPEND ${GLOBOBJ_NAME}_GLOBAL_HEADERS ${${MODULE_NAME}_HEADERS})

        #message(STATUS "${MODULE_NAME} MODULE linked libraries are : ${${MODULE_NAME}_LINK_LIBRARIES}")
        list(APPEND ${GLOBOBJ_NAME}_LINK_LIBRARIES ${${MODULE_NAME}_LINK_LIBRARIES})

        #message(STATUS "Include directory for ${MODULE_NAME} :  ${${MODULE_NAME}_INCDIR}")
        list(APPEND ${GLOBOBJ_NAME}_BUILDTREE_INCLUDES ${${MODULE_NAME}_INCDIR})
    endforeach()

    #message(STATUS "${GLOBOBJ_NAME} srcs : ${${GLOBOBJ_NAME}_GLOBAL_SOURCES}")
    # Filter out duplicates in LINK_LIBRARIES
    if(${GLOBOBJ_NAME}_LINK_LIBRARIES)
        list(REMOVE_DUPLICATES ${GLOBOBJ_NAME}_LINK_LIBRARIES)
    endif()

    OBJECT_TARGET(NAME ${GLOBOBJ_NAME}
                      SOURCES
                          ${${GLOBOBJ_NAME}_GLOBAL_SOURCES}
                          ${${GLOBOBJ_NAME}_GLOBAL_HEADERS}
                     )
    set_property(GLOBAL APPEND
                 PROPERTY GLOBAL_OBJECT_TARGETS \$<TARGET_OBJECTS:${GLOBOBJ_NAME}>)

    # Header install?
    foreach(_header ${${GLOBOBJ_NAME}_GLOBAL_HEADERS})
        file(RELATIVE_PATH _fpath "${PROJECT_SOURCE_DIR}/source" "${_header}")
        get_filename_component(_fpath ${_fpath} PATH)
        #message(STATUS "Setting up install for ${_header} into ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${_fpath}...")
        install(FILES ${_header}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${_fpath}
                COMPONENT Development)
    endforeach()
    
    # Store the include path of the component so that the build tree
    # config file can pick up all needed header paths
    set_property(GLOBAL APPEND
                    PROPERTY BUILDTREE_INCLUDE_DIRS ${${GLOBOBJ_NAME}_BUILDTREE_INCLUDES})

    set_property(GLOBAL APPEND
                    PROPERTY INSTALL_HEADER_FILES ${${GLOBOBJNAME}_GLOBAL_HEADERS})

    #get_property(BT_INCDIR GLOBAL PROPERTY BUILDTREE_INCLUDE_DIRS)
    #message(STATUS "BUILDTREE_INCLUDE_DIRS : ${BT_INCDIR}")

ENDMACRO()

#-----------------------------------------------------------------------
# - GLOBAL_LIBRARY_TARGET
# Build and install of a  category library
#
MACRO(GLOBAL_LIBRARY_TARGET)
    CMAKE_PARSE_ARGUMENTS(GLOBLIB
                          ""
                          "NAME"
                          "COMPONENTS"
                          ${ARGN}
                         )

    # We loop over the component sources one at a time,
    # appending properties as we go.
    foreach(_comp ${GLOBLIB_COMPONENTS})
        include(${_comp})
        # In case we have a global lib with one component, ensure name gets set
        if(NOT GLOBLIB_NAME)
        set(GLOBLIB_NAME ${MODULE_NAME})
        endif()

        #message(STATUS "${MODULE_NAME} srcs : ${${MODULE_NAME}_SOURCES}")
        list(APPEND ${GLOBLIB_NAME}_GLOBAL_SOURCES ${${MODULE_NAME}_SOURCES})
        list(APPEND ${GLOBLIB_NAME}_GLOBAL_HEADERS ${${MODULE_NAME}_HEADERS})

        #message(STATUS "${MODULE_NAME} MODULE linked libraries are : ${${MODULE_NAME}_LINK_LIBRARIES}")
        list(APPEND ${GLOBLIB_NAME}_LINK_LIBRARIES ${${MODULE_NAME}_LINK_LIBRARIES})

        list(APPEND ${GLOBLIB_NAME}_BUILDTREE_INCLUDES ${${MODULE_NAME}_INCDIR})
    endforeach()

    #message(STATUS "${GLOBLIB_NAME} srcs : ${${GLOBLIB_NAME}_GLOBAL_SOURCES}")
    # Filter out duplicates in LINK_LIBRARIES
    if(${GLOBLIB_NAME}_LINK_LIBRARIES)
        list(REMOVE_DUPLICATES ${GLOBLIB_NAME}_LINK_LIBRARIES)
    endif()

    #message(STATUS "${GLOBLIB_NAME} link libraries are : ${${GLOBLIB_NAME}_LINK_LIBRARIES}")
    # Now add the library target
    LIBRARY_TARGET(NAME ${GLOBLIB_NAME}
                        SOURCES
                            ${${GLOBLIB_NAME}_GLOBAL_SOURCES}
                            ${${GLOBLIB_NAME}_GLOBAL_HEADERS}
                        LINK_LIBRARIES
                            ${${GLOBOBJ_NAME}_LINK_LIBRARIES}
                     )

    set_property(GLOBAL APPEND
                   PROPERTY GLOBAL_EXPORTED_TARGETS ${GLOBLIB_NAME})

    # Header install?
    install(FILES ${${GLOBLIB_NAME}_GLOBAL_HEADERS}
             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
             COMPONENT Development)

    # Store the include path of the component so that the build tree
    # config file can pick up all needed header paths
    set_property(GLOBAL APPEND
                   PROPERTY BUILDTREE_INCLUDE_DIRS ${${GLOBLIB_NAME}_BUILDTREE_INCLUDES})

ENDMACRO()

#-----------------------------------------------------------------------
# - GLOBAL_HEADER_TARGET
# Build and install of a  category library
#
MACRO(GLOBAL_HEADER_TARGET)
    CMAKE_PARSE_ARGUMENTS(GLOBHEAD
                          ""
                          "NAME"
                          "COMPONENTS"
                          ${ARGN}
                         )

    # We loop over the component sources one at a time,
    # appending properties as we go.
    foreach(_comp ${GLOBHEAD_COMPONENTS})
        include(${_comp})
        # In case we have a global lib with one component, ensure name gets set
        if(NOT GLOBHEAD_NAME)
        set(GLOBHEAD_NAME ${MODULE_NAME})
        endif()

        #message(STATUS "${MODULE_NAME} srcs : ${${MODULE_NAME}_SOURCES}")
        list(APPEND ${GLOBHEAD_NAME}_GLOBAL_SOURCES ${${MODULE_NAME}_SOURCES})
        list(APPEND ${GLOBHEAD_NAME}_GLOBAL_HEADERS ${${MODULE_NAME}_HEADERS})

        #message(STATUS "${MODULE_NAME} MODULE linked libraries are : ${${MODULE_NAME}_LINK_LIBRARIES}")
        list(APPEND ${GLOBHEAD_NAME}_LINK_LIBRARIES ${${MODULE_NAME}_LINK_LIBRARIES})

        list(APPEND ${GLOBHEAD_NAME}_BUILDTREE_INCLUDES ${${MODULE_NAME}_INCDIR})
    endforeach()

    #message(STATUS "${GLOBHEAD_NAME} srcs : ${${GLOBHEAD_NAME}_GLOBAL_SOURCES}")
    # Filter out duplicates in LINK_LIBRARIES
    if(${GLOBHEAD_NAME}_LINK_LIBRARIES)
        list(REMOVE_DUPLICATES ${GLOBHEAD_NAME}_LINK_LIBRARIES)
    endif()

    HEADER_TARGET(NAME ${GLOBHEAD_NAME}
                      SOURCES
                          ${${GLOBHEAD_NAME}_GLOBAL_SOURCES}
                          ${${GLOBHEAD_NAME}_GLOBAL_HEADERS}
                      LINK_LIBRARIES
                          ${${GLOBHEAD_NAME}_LINK_LIBRARIES}
                     )

    if(BUILD_SHARED_LIBS)
        set_property(GLOBAL APPEND
                     PROPERTY GLOBAL_EXPORTED_TARGETS ${GLOBHEAD_NAME})
    elseif(BUILD_STATIC_LIBS)
        set_property(GLOBAL APPEND
                     PROPERTY GLOBAL_EXPORTED_TARGETS ${GLOBHEAD_NAME}-static)
    endif()
    #set_property(GLOBAL APPEND
    #               PROPERTY GLOBAL_HEADER_TARGETS ${GLOBHEAD_NAME}_GLOBAL_HEADERS ${GLOBHEAD_NAME}_GLOBAL_SOURCES)

    # Header install?
    install(FILES ${${GLOBHEAD_NAME}_GLOBAL_HEADERS}
             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
             COMPONENT Development)

    # Store the include path of the component so that the build tree
    # config file can pick up all needed header paths
    set_property(GLOBAL APPEND
                   PROPERTY BUILDTREE_INCLUDE_DIRS ${${GLOBHEAD_NAME}_BUILDTREE_INCLUDES})

ENDMACRO()
