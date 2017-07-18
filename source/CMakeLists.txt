
include(MacroLibraryTargets)

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0043 NEW)
endif()

set(LIBRARIES ${EXTERNAL_LIBRARIES})

GLOBAL_OBJECT_TARGET(NAME ${PROJECT_NAME}-OBJECT
                     COMPONENTS
                        sources.cmake
                        allocator/sources.cmake
                        atomics/sources.cmake
                        atomics/details/sources.cmake
                        threading/sources.cmake
                        threading/task/sources.cmake
                        vectorization/sources.cmake
                        utility/sources.cmake)

get_property(GLOBAL_OBJECT_TARGETS GLOBAL PROPERTY GLOBAL_OBJECT_TARGETS)

#------------------------------------------------------------------------------#
# function - capitalize - make a string capitalized (first letter is capital)
#   usage:
#       capitalize("SHARED" CShared)
#   message(STATUS "-- CShared is \"${CShared}\"")
#   $ -- CShared is "Shared"
#
function(capitalize str var)
    # make string lower
    string(TOLOWER "${str}" str)
    string(SUBSTRING "${str}" 0 1 _first)
    string(TOUPPER "${_first}" _first)
    string(SUBSTRING "${str}" 1 -1 _remainder)
    string(CONCAT str "${_first}" "${_remainder}")
    set(${var} "${str}" PARENT_SCOPE)
endfunction()



#------------------------------------------------------------------------------#
capitalize(${PROJECT_NAME} CPROJECT_NAME)



#------------------------------------------------------------------------------#
if(BUILD_SHARED_LIBS)

    add_library(${PROJECT_NAME}-SHARED SHARED ${GLOBAL_OBJECT_TARGETS})

    set_target_properties(${PROJECT_NAME}-SHARED
        PROPERTIES OUTPUT_NAME     "${PROJECT_NAME}"
                   VERSION         "${${PROJECT_NAME}_SHORT_VERSION}"
                   SOVERSION       "${${PROJECT_NAME}_MAJOR_VERSION}"
                   #PUBLIC_HEADER   "${PROJECT_SOURCE_DIR}/source/config.hh"
                   LINKER_LANGUAGE CXX)

    target_link_libraries(${PROJECT_NAME}-SHARED
                          ${EXTRA_LIBS} ${EXTERNAL_LIBRARIES})


    install(TARGETS ${PROJECT_NAME}-SHARED
            EXPORT ${CPROJECT_NAME}LibraryDepends
            RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT bin
            LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR} COMPONENT lib
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR} COMPONENT dev
            DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

endif()

if(BUILD_STATIC_LIBS)

    add_library(${PROJECT_NAME}-STATIC STATIC ${GLOBAL_OBJECT_TARGETS})

    set_target_properties(${PROJECT_NAME}-STATIC
        PROPERTIES OUTPUT_NAME     "${PROJECT_NAME}"
                   VERSION         "${${PROJECT_NAME}_SHORT_VERSION}"
                   #PUBLIC_HEADER   "${PROJECT_SOURCE_DIR}/source/config.hh"
                   POSITION_INDEPENDENT_CODE ON
                   LINKER_LANGUAGE CXX)

    target_link_libraries(${PROJECT_NAME}-STATIC
                          ${EXTRA_LIBS} ${EXTERNAL_LIBRARIES})

    install(TARGETS ${PROJECT_NAME}-STATIC
            EXPORT ${CPROJECT_NAME}LibraryDepends
            RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT bin
            LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR} COMPONENT lib
            ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR} COMPONENT lib
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR} COMPONENT dev
            DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

endif()

get_directory_property(compile_defs COMPILE_DEFINITIONS)

#------------------------------------------------------------------------------#

capitalize(${PROJECT_NAME} _cproject_name)

set(_prefix "${_cproject_name}")
set(_filename "${_prefix}LibraryDepends.cmake")
set(_PROJECT_NAME "${_cproject_name}")
set(PACKAGE_VERSION "${${PROJECT_NAME}_VERSION}")

set(_TARGETS )
set(PROJECT_LIBRARY_NAMES )

if(BUILD_SHARED_LIBS)
    list(APPEND _TARGETS ${PROJECT_NAME}-SHARED)
endif()
if(BUILD_STATIC_LIBS )
    list(APPEND _TARGETS ${PROJECT_NAME}-STATIC)
endif()

list(GET _TARGETS 0 PROJECT_LIBRARY_NAMES)

# ===============================
# Add all targets to the build-tree export set
export(TARGETS ${_TARGETS} FILE "${PROJECT_BINARY_DIR}/${_filename}")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${_prefix}LibraryDepends)

# Create the Config.cmake and ConfigVersion.cmake
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_CMAKEDIR}"
     "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/Config.cmake.in
    "${PROJECT_BINARY_DIR}/${_prefix}Config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${${_PROJECT_NAME}_CMAKE_DIR}/${REL_INCLUDE_DIR};\${${_PROJECT_NAME}_CMAKE_DIR}/${REL_INCLUDE_DIR}/madthreading")
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/Config.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_prefix}Config.cmake" @ONLY)

# ... for both
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/ConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/${_prefix}ConfigVersion.cmake" @ONLY)

# Install the Config.cmake and ConfigVersion.cmake
install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_prefix}Config.cmake"
    "${PROJECT_BINARY_DIR}/${_prefix}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_FULL_CMAKEDIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT ${CPROJECT_NAME}LibraryDepends
    DESTINATION "${CMAKE_INSTALL_FULL_CMAKEDIR}" COMPONENT dev)
