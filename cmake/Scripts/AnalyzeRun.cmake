################################################################################
# Analyze results of unit test runs

################################################################################
include("${CMAKE_CURRENT_LIST_DIR}/../Modules/DefineColors.cmake")

set( FailTag "${TAG}${Red} FAIL${ColorReset}" )

execute_process( COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${APP}
                 TIMEOUT 60
                 RESULT_VARIABLE result
                 OUTPUT_VARIABLE warnings
                 ERROR_VARIABLE  errors
                 ERROR_FILE      ${APP}.err
                 OUTPUT_FILE     ${APP}.out
               )
#
file( STRINGS ${APP}.err errFile )
set(IGNORE "[NVBLAS] NVBLAS_CONFIG_FILE environment variable is set to '$ENV{NVBLAS_CONFIG_FILE}'")
STRING(REPLACE "${IGNORE}" "" errFile "${errFile}")
STRING(REPLACE ";" "" errFile "${errFile}")
STRING(REGEX REPLACE "[^a-zA-Z0-9]" "" errFile "${errFile}")

file( WRITE ${APP}.read.err "${errFile}" )

STRING(STRIP "${errFile}" errFile)

if( errFile )
    execute_process( COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold ${FailTag} )
    message( SEND_ERROR "Problem running ${APP}.  Error: ${result}.  See details in ${CMAKE_CURRENT_BINARY_DIR}/${APP}.err" )
endif()

file( STRINGS ${APP}.out output )

# If the file ran, but didn't produce output, something strange has happened and should be analyzed
if( NOT output )
    execute_process( COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold ${FailTag} )
    message( SEND_ERROR "${APP} [ UNKNOWN ]" )
endif()

foreach( elem ${output} )
    string( REGEX MATCHALL "Failure" errorLine ${elem} )
    if( errorLine )
        message( ${elem} )
    endif()

    string( REGEX MATCHALL "[Dd]ebug" debugLine ${elem} )
    if( debugLine )
        message( ${elem} )
    endif()
endforeach()

# Check for success
string( REGEX MATCH "Success" wasSuccess "${output}" )

if( wasSuccess )
    return()

else()
    execute_process( COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold ${FailTag} )
    message( SEND_ERROR "ERROR: *** ${result} ***" )

endif()

