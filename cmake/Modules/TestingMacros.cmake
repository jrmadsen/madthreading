
cmake_policy(SET CMP0011 NEW)

################################################################################
#
#  Include this template cmake file after specifying any include and library
#  dependencies that the test executable may need
#
################################################################################

include(CMakeMacroParseArguments)
include(DefineColors)

# use full path to libraries
cmake_policy(SET CMP0060 NEW)

macro(ADD_UNIT_TEST)
    CMAKE_PARSE_ARGUMENTS(  Test    # prefix
                            "BATCH"      # options
                            "NAME;COMPILE_FLAGS"  # single-value args
                            "LINK_LIBRARIES;EXCLUDE_SOURCE;FILES" # multi-valued args
                            ${ARGN}      # catches any unexpected arguments
                         )

########################################
#  Get the name of directory
get_filename_component( TestDirName ${CMAKE_CURRENT_SOURCE_DIR} NAME )

set(testname ${Test_NAME})

if( NOT DEFINED testname )
    message( FATAL_ERROR "AddTest.cmake must be called AFTER a testname has been defined." )
endif()

if( TestDirName STREQUAL "unit_test" )
    set( TestType "Unit" )
elseif( TestDirName STREQUAL "fi_test" )
    set( TestType "FIUnit" )
elseif( TestDirName STREQUAL "nightly" )
    set( TestType "Nightly" )
    set( nightly true )
endif()

########################################
#  Application Name

set( appName ${TestType}${testname} )

########################################
#  Gather sources

file( GLOB ${appName}_srcs "*.cpp" "*.cc" )

foreach( src ${Test_EXCLUDE_SOURCE} )
    list( REMOVE_ITEM ${appName}_srcs ${CMAKE_CURRENT_SOURCE_DIR}/${src} )
endforeach()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${UnitTest++_INCLUDE_DIRS})

########################################
#  Add the new test

add_executable( ${appName} ${${appName}_srcs} ${Test_FILES})

set_target_properties( ${appName} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}
  COMPILE_OPTIONS "${Test_COMPILE_FLAGS}"
)

########################################
#  Collect the suite names defined for these tests
set( SuiteNames )
foreach( curSrc ${${appName}_srcs} )
    file( STRINGS ${curSrc} curSuiteNames REGEX "^SUITE" )

    foreach( suiteName ${curSuiteNames} )
        string( REGEX REPLACE "SUITE[^a-zA-Z0-9]+" "" suiteName ${suiteName} )
        string( REGEX MATCHALL "^[a-zA-Z_0-9]+" suiteName ${suiteName} )
        list( APPEND SuiteNames ${suiteName} )
    endforeach()
endforeach()
if( DEFINED SuiteNames )
    list( REMOVE_DUPLICATES SuiteNames )
endif()

########################################
#  Toolkit libraries that need to linked in

# Always try to add parent directories library
target_link_libraries( ${appName} ${Test_LINK_LIBRARIES} ${UnitTest++_LIBRARIES})

######################################################################
#  TESTING

foreach( testFile ${Test_ConfigFiles} )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${testFile}
                   ${CMAKE_CURRENT_BINARY_DIR}/${testFile} COPYONLY)
endforeach()


set( runResults "(success|fail|debug)" )

foreach( suiteName ${SuiteNames} )
      # Register with CTest
      set( CTestID ${appName}_${suiteName} )
      if( DEFINED JobController )
          add_test( NAME ${CTestID} COMMAND ${JobController} $<TARGET_FILE:${appName}> ${suiteName} )
      else()
          add_test( NAME ${CTestID} COMMAND ${appName} ${suiteName} )
      endif()

      #set_property( TEST ${CTestID} APPEND PROPERTY LABELS Serial ${SubProjectName} )
      set_tests_properties(${CTestID} PROPERTIES TIMEOUT 4800.)

      # Specify to the job dispatch system that these are serial
      set_property( TEST ${CTestID} PROPERTY PROCESSORS 1 )

      if( nightly )
          set_property( TEST ${CTestID} APPEND PROPERTY LABELS Nightly SerialNightly )
      else()
          set_property( TEST ${CTestID} APPEND PROPERTY LABELS Quick )
      endif()
endforeach()

# This will cause it to run once after building (for all but nightlies!)
if( NOT nightly AND NOT Test_BATCH )
    set( CmdTag "${Cyan}Testing ${testname}[ ${TestDirName} ]...${ColorReset}" )
    set( CmdTagSuccess "${Cyan}Testing ${testname}[ ${TestDirName} ]... PASS${ColorReset}" )
    
    add_custom_command(
        TARGET ${appName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold ${CmdTag}
        COMMAND ${CMAKE_COMMAND} -DAPP=${appName}${CMAKE_EXECUTABLE_SUFFIX}
        -DTAG=${CmdTag} -P ${PROJECT_SOURCE_DIR}/cmake/Scripts/AnalyzeRun.cmake
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold ${CmdTagSuccess})
else()
    message(STATUS "Skipping running of ${testname}...")
endif()

endmacro()

# stuff from a tutorial

# LIBRARY
#   ADD_LIBRARY( lib_${PROJECT_NAME} ${library_sources} )
# create symbolic lib target for calling target lib_XXX
#   ADD_CUSTOM_TARGET( lib DEPENDS lib_${PROJECT_NAME} )
# change lib_target properties
#   SET_TARGET_PROPERTIES( lib_${PROJECT_NAME} PROPERTIES
# create *nix style library versions + symbolic links
#   VERSION ${${PROJECT_NAME}_VERSION}
#   SOVERSION ${${PROJECT_NAME}_SOVERSION}
# allow creating static and shared libs without conflicts
#   CLEAN_DIRECT_OUTPUT 1
# avoid conflicts between library and binary target names
#   OUTPUT_NAME ${PROJECT_NAME} )
# install library
#   INSTALL( TARGETS lib_${PROJECT_NAME} DESTINATION lib PERMISSIONS
#   OWNER_READ OWNER_WRITE OWNER_EXECUTE
#   GROUP_READ GROUP_EXECUTE
#   WORLD_READ WORLD_EXECUTE )
