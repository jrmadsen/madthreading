# - Setup of general build options for  Libraries
#
# In addition to the core compiler/linker flags (configured in the
# MakeRules_<LANG>.cmake files), the build may require
# further configuration. This module performs this task whicj includes:
#
#  1) Extra build modes for developers
#  2) Additional compiler definitions to optimize
#     performance.
#  3) Additional compiler flags which may be added optionally.
#  4) Whether to build shared and/or static libraries.
#
#

include(MacroUtilities)

#-----------------------------------------------------------------------
# Set up Build types or configurations
# If further tuning of compiler flags is needed then it should be done here.
# (It can't be done in the make rules override section).
# However, exercise care when doing this not to override existing flags!!
# We don't do this on WIN32 platforms yet because of some teething issues
# with compiler specifics and linker flags
if(NOT WIN32)
  include(BuildModes)
endif()


#-----------------------------------------------------------------------
# Optional compiler flags
#
# - BUILD_CXXSTD
# Choose C++ Standard to build against, if supported.
# Mark as advanced because most users will not need it.
if(CXXSTD_IS_AVAILABLE)
  enum_option(BUILD_CXXSTD
    DOC "C++ Standard to compile against"
    VALUES ${CXXSTD_IS_AVAILABLE}
    CASE_INSENSITIVE
    )
  #mark_as_advanced(BUILD_CXXSTD)
  add_feature(BUILD_CXXSTD "Compiling against C++ Standard '${BUILD_CXXSTD}'")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${BUILD_CXXSTD}_FLAGS}")
endif()


#-----------------------------------------------------------------------
# Setup Shared and/or Static Library builds
# We name these options without a '${PROJECT_NAME}_' prefix because they are
# really higher level CMake options.
# Default to building shared libraries, mark options as advanced because
# most user should not have to touch them.

OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)
OPTION(BUILD_STATIC_LIBS "Build static libraries" ON)

mark_as_advanced(BUILD_SHARED_LIBS BUILD_STATIC_LIBS)

if(BUILD_SHARED_LIBS AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Because both could be switched off accidently, FATAL_ERROR if neither
# option has been selected.
if(NOT BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS)
  message(WARNING "Neither static nor shared libraries will be built")
endif()

# On MSVC, warn if both shared and static are built - this has duplicate
# symbol issues on VS2010 Express.
# TODO: Resolve and understand this issue...
if(MSVC)
  if(BUILD_SHARED_LIBS AND BUILD_STATIC_LIBS)
    message(WARNING " Building shared AND static libraries on VS2010 may result in link errors.
 You are welcome to try building both, but please be aware of this warning.
    ")
  endif()
endif()

#------------------------------------------------------------------------
# Setup symbol visibility (library interface)
# We need to define that we're building
#

# - Testing system only functional on 2.8 and above
if(NOT ${CMAKE_VERSION} VERSION_GREATER 2.7)
  if(${PROJECT_NAME}_ENABLE_TESTING)
    message(STATUS "WARNING: ${PROJECT_NAME}_ENABLE_TESTING requires CMake >= 2.8 -- deactivating")
  endif()
  set(${PROJECT_NAME}_ENABLE_TESTING OFF CACHE INTERNAL "Testing NOT supported on CMake <2.8"
    FORCE)
else()
  option(${PROJECT_NAME}_ENABLE_TESTING "Enable and define all the tests of the project" ON)
  ADD_FEATURE(${PROJECT_NAME}_ENABLE_TESTING "Enable and define all the tests of the project")
  mark_as_advanced(${PROJECT_NAME}_ENABLE_TESTING)

  OPTION(BUILD_TESTS "Run the  test problems as part of `make all`" OFF)
  ADD_FEATURE(BUILD_TESTS "Run the  test problems as part of `make all`")
endif()


# On WIN32, we need to build the genwindef application to create export
# def files for building DLLs.
# We only use it as a helper application at the moment so we exclude it from
# the ALL target.
# TODO: We could move this section into the MacroLibraryTargets.cmake
# if it can be protected so that the genwindef target wouldn't be defined
# more than once... Put it here for now...
if(WIN32)
  add_definitions(-D${PROJECT_NAME}LIB_BUILD_DLL)
  # Assume the sources are co-located
  get_filename_component(_genwindef_src_dir ${CMAKE_CURRENT_LIST_FILE} PATH)
  add_executable(genwindef EXCLUDE_FROM_ALL
    ${_genwindef_src_dir}/genwindef/genwindef.cpp
    ${_genwindef_src_dir}/genwindef/LibSymbolInfo.h
    ${_genwindef_src_dir}/genwindef/LibSymbolInfo.cpp)
endif()


#------------------------------------------------------------------------
# Setup Locations for Build Outputs
# Because of the highly nested structure, targets will be
# distributed throughout this tree, potentially making usage and debugging
# difficult (especially if developers use non-CMake tools).
#
# We therefore set the output directory of runtime, library and archive
# targets to some low level directories under the build tree.
#
# On Unices, we try to make the output directory backward compatible
# with the old style 'SYSTEM-COMPILER' format so that applications may be
# built against the targets in the build tree.
#
# Note that for multi-configuration generators like VS and Xcode, these
# directories will have the configuration type (e.g. Debug) appended to
# them, so are not backward compatible with the old Make toolchain in
# these cases.
#
# Also, we only do this on UNIX because we want to avoid mixing static and
# dynamic libraries on windows until the differences are better understood.
#------------------------------------------------------------------------
# Determine the backward compatible system name
#
if(NOT WIN32)
  set(${PROJECT_NAME}_SYSTEM ${CMAKE_SYSTEM_NAME})
else()
  set(${PROJECT_NAME}_SYSTEM "WIN32")
endif()


#------------------------------------------------------------------------
# Determine the backward compatible compiler name
# NB: At present Clang detection only works on CMake > 2.8.1
if(CMAKE_COMPILER_IS_GNUCXX)
  set(${PROJECT_NAME}_COMPILER "g++")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(${PROJECT_NAME}_COMPILER "clang")
elseif(MSVC)
  set(${PROJECT_NAME}_COMPILER "VC")
elseif(CMAKE_CXX_COMPILER MATCHES "icpc.*|icc.*")
  set(${PROJECT_NAME}_COMPILER "icc")
else()
  set(${PROJECT_NAME}_COMPILER "UNSUPPORTED")
endif()


#-----------------------------------------------------------------------
# Set the output paths to be backward compatible on UNIX
if(NOT UNIX)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/runtime)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/library)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/archive)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif()

