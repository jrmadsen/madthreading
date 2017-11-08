
include(MacroLibraryTargets)

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0043 NEW)
endif()

set(LIBRARIES ${EXTERNAL_LIBRARIES})

GLOBAL_OBJECT_TARGET(INSTALL_HEADERS
    NAME ${PROJECT_NAME}-OBJECT
    COMPONENTS
        sources.cmake
        allocator/sources.cmake
        atomics/sources.cmake
        threading/sources.cmake
        threading/task/sources.cmake
        vectorization/sources.cmake
        utility/sources.cmake
)

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
# Set the library suffix
set(_SUFFIX "")
string(TOUPPER "${CMAKE_BUILD_TYPE}" _BUILD_TYPE)
if("${_BUILD_TYPE}" STREQUAL "DEBUG")
    set(_SUFFIX "-d")
elseif("${_BUILD_TYPE}" STREQUAL "RELWITHDEBINFO")
    set(_SUFFIX "-rd")
elseif("${_BUILD_TYPE}" STREQUAL "MINSIZEREL")
    set(_SUFFIX "-ms")
endif()


#------------------------------------------------------------------------------#

if(BUILD_SHARED_LIBS)

    add_library(${PROJECT_NAME}-shared SHARED ${GLOBAL_OBJECT_TARGETS})

    set_target_properties(${PROJECT_NAME}-shared
        PROPERTIES OUTPUT_NAME      "${PROJECT_NAME}"
                   VERSION          "${${PROJECT_NAME}_SHORT_VERSION}"
                   SOVERSION        "${${PROJECT_NAME}_MAJOR_VERSION}"
                   LINK_FLAGS       "${EXTERNAL_CXX_LINK_FLAGS}"
                   LINKER_LANGUAGE  CXX)

    target_link_libraries(${PROJECT_NAME}-shared
        ${EXTRA_LIBS} ${EXTERNAL_LIBRARIES})

    set(PROJECT_SHARED_LIBRARY ${PROJECT_NAME}-shared)

endif()

#------------------------------------------------------------------------------#

if(BUILD_STATIC_LIBS)

    add_library(${PROJECT_NAME}-static STATIC ${GLOBAL_OBJECT_TARGETS})

    set_target_properties(${PROJECT_NAME}-static
        PROPERTIES OUTPUT_NAME     "${PROJECT_NAME}"
                   VERSION         "${${PROJECT_NAME}_SHORT_VERSION}"
                   POSITION_INDEPENDENT_CODE ON
                   LINK_FLAGS       "${EXTERNAL_CXX_LINK_FLAGS}"
                   LINKER_LANGUAGE CXX)

    target_link_libraries(${PROJECT_NAME}-static
        ${EXTRA_LIBS} ${EXTERNAL_LIBRARIES})

    set(PROJECT_STATIC_LIBRARY ${PROJECT_NAME}-static)

endif()

get_directory_property(compile_defs COMPILE_DEFINITIONS)

#------------------------------------------------------------------------------#

STRING(TOLOWER "${PROJECT_NAME}" LOWER_PROJECT_NAME)
# write a build specific config.hh header file
write_config_file("config.hh" ${PROJECT_BINARY_DIR}/InstallTreeFiles
    ${CMAKE_INSTALL_CMAKEDIR}/${CMAKE_BUILD_TYPE}/${LOWER_PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(PACKAGE_VERSION "${${PROJECT_NAME}_VERSION}")
set(config_target ${PROJECT_NAME}LibraryDepends)

set(_TARGETS )
set(PROJECT_LIBRARY_NAMES )

if(BUILD_SHARED_LIBS)
    list(APPEND _TARGETS ${PROJECT_NAME}-shared)
endif()
if(BUILD_STATIC_LIBS )
    list(APPEND _TARGETS ${PROJECT_NAME}-static)
endif()

list(GET _TARGETS 0 PROJECT_LIBRARY_NAMES)

# =============================================================================#
# BUILD TREE
# =============================================================================#
# Add all targets to the build-tree export set
export(TARGETS ${_TARGETS} FILE "${PROJECT_BINARY_DIR}/${CPROJECT_NAME}LibraryDepends.cmake")
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${CPROJECT_NAME}LibraryDepends)
# Create the Config.cmake and ConfigHeader-${CMAKE_BUILD_TYPE}.cmake for the build tree
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/Config.cmake.in
    "${PROJECT_BINARY_DIR}/${CPROJECT_NAME}Config.cmake"
    @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/ConfigHeader.cmake.in
    "${PROJECT_BINARY_DIR}/${CPROJECT_NAME}ConfigHeader-${CMAKE_BUILD_TYPE}.cmake"
    @ONLY)

# =============================================================================#
# INSTALL TREE
# =============================================================================#
# Create the Config.cmake and ConfigHeader-${CMAKE_BUILD_TYPE}.cmake for the install tree
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/Config.cmake.in
    "${PROJECT_BINARY_DIR}/InstallTreeFiles/${CPROJECT_NAME}Config.cmake"
    @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/ConfigHeader.cmake.in
    "${PROJECT_BINARY_DIR}/InstallTreeFiles/${CPROJECT_NAME}ConfigHeader-${CMAKE_BUILD_TYPE}.cmake"
    @ONLY)

# Create the ConfigVersion.cmake for both
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/ConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/${CPROJECT_NAME}ConfigVersion.cmake" @ONLY)

# Install the ${PROJECT_NAME}Config*.cmake files
install(FILES
    "${PROJECT_BINARY_DIR}/InstallTreeFiles/${CPROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/InstallTreeFiles/${CPROJECT_NAME}ConfigHeader-${CMAKE_BUILD_TYPE}.cmake"
    "${PROJECT_BINARY_DIR}/${CPROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    COMPONENT dev)
# install the libraries
install(TARGETS ${_TARGETS}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    EXPORT ${CPROJECT_NAME}LibraryDepends)
# Install the export set for use with the install-tree
install(EXPORT ${CPROJECT_NAME}LibraryDepends
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    COMPONENT dev)

# export the packages
export(PACKAGE ${PROJECT_NAME})

foreach(_cmake_file Compilers MacroUtilities)
    # Install CMake files
    install(FILES ${PROJECT_SOURCE_DIR}/cmake/Modules/${_cmake_file}.cmake
        DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
        COMPONENT dev)
endforeach(_cmake_file Compilers MacroUtilities)

foreach(_cmake_file FindMKL FindSSE FindTBB)
    # Install CMake files
    install(FILES ${PROJECT_SOURCE_DIR}/cmake/Packages/${_cmake_file}.cmake
        DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
        COMPONENT dev)
endforeach(_cmake_file FindMKL FindSSE FindTBB)
