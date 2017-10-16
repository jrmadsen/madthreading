# Try to find the UnitTest++ library and headers
# Usage of this module is as follows
#
#     find_package( UnitTest++ )
#     if(UnitTest++_FOUND)
#         include_directories(${UnitTest++_INCLUDE_DIRS})
#         add_executable(foo foo.cc)
#         target_link_libraries(foo ${UnitTest++_LIBRARIES})
#     endif()
#
# You can provide a minimum version number that should be used.
# If you provide this version number and specify the REQUIRED attribute,
# this module will fail if it can't find a UnitTest++ of the specified version
# or higher. If you further specify the EXACT attribute, then this module
# will fail if it can't find a UnitTest++ with a version eaxctly as specified.
#
# ===========================================================================
# Variables used by this module which can be used to change the default
# behaviour, and hence need to be set before calling find_package:
#
#  UnitTest++_ROOT (not cached) The preferred installation prefix for searching for
#  UnitTest++_ROOT_DIR (cached) UnitTest++. Set this if the module has problems finding
#                         the proper UnitTest++ installation.
#
# If you don't supply UnitTest++_ROOT, the module will search on the standard
# system paths.
#
# ============================================================================
# Variables set by this module:
#
#  UnitTest++_FOUND           System has UnitTest++.
#
#  UnitTest++_INCLUDE_DIRS    UnitTest++ include directories: not cached.
#
#  UnitTest++_LIBRARIES       Link to these to use the UnitTest++ library: not cached.
#
# ===========================================================================
# If UnitTest++ is installed in a non-standard way, e.g. a non GNU-style install
# of <prefix>/{lib,include}, then this module may fail to locate the headers
# and libraries as needed. In this case, the following cached variables can
# be editted to point to the correct locations.
#
#  UnitTest++_INCLUDE_DIR    The path to the UnitTest++ include directory: cached
#
#  UnitTest++_LIBRARY        The path to the UnitTest++ library: cached
#
# You should not need to set these in the vast majority of cases
#

#------------------------------------------------------------------------------#

find_path(UnitTest++_INCLUDE_DIR
          NAMES
              UnitTest++/UnitTest++.h
              unittest++/UnitTest++.h
          HINTS
              ${UnitTest++_ROOT}
              ${UNITTEST++_ROOT}
              ${UnitTestpp_ROOT}
              ${UNITTESTPP_ROOT}
              ENV UnitTestpp_ROOT
              ENV UNITTESTPP_ROOT
              ENV PATH
              ENV LD_LIBRARY_PATH
              ENV LIBRARY_PATH
              ENV DYLD_LIBRARY_PATH
          PATH_SUFFIXES
              include
              ../include
          DOC
              "Path to the UnitTest++ headers")

#------------------------------------------------------------------------------#

find_library(UnitTest++_LIBRARY
             NAMES
                 UnitTest++
                 unittest++
             HINTS
                 ${UnitTest++_ROOT}
                 ${UNITTEST++_ROOT}
                 ENV UnitTest++_ROOT
                 ENV UnitTest++_ROOT
                 ${UnitTestpp_ROOT}
                 ${UNITTESTPP_ROOT}
                 ENV UnitTestpp_ROOT
                 ENV UNITTESTPP_ROOT
                 ENV PATH
                 ENV LD_LIBRARY_PATH
                 ENV LIBRARY_PATH
                 ENV DYLD_LIBRARY_PATH
             PATH_SUFFIXES
                 lib
                 lib64
                 ../lib
                 ../lib64
             DOC
                 "Path to the UnitTest++ library")
#------------------------------------------------------------------------------#

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set UnitTest++_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(UnitTest++ DEFAULT_MSG
    UnitTest++_LIBRARY UnitTest++_INCLUDE_DIR)

#------------------------------------------------------------------------------#

if(UnitTest++_FOUND)
    get_filename_component(UnitTest++_INCLUDE_DIRS
        ${UnitTest++_INCLUDE_DIR} REALPATH)
    get_filename_component(UnitTest++_LIBRARIES
        ${UnitTest++_LIBRARY} REALPATH)
endif()

mark_as_advanced(UnitTest++_INCLUDE_DIR UnitTest++_LIBRARY)

#------------------------------------------------------------------------------#
