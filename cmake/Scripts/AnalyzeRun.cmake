################################################################################
# Analyze results of unit test runs

################################################################################
#get_filename_component( TestDirName ${CMAKE_CURRENT_BINARY_DIR} NAME )

#set( CmdTag "Testing ${UNIT}[ ${TestDirName} ]..." )
#set( FailTag "${CmdTag} FAIL" )
set( FailTag "${TAG}  FAIL" )

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

if( errFile )
    execute_process( COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${FailTag} )
    message( SEND_ERROR "Problem running ${APP}.  Error: ${result}.  See details in ${CMAKE_CURRENT_BINARY_DIR}/${APP}.err" )
endif()

file( STRINGS ${APP}.out output )

# If the file ran, but didn't produce output, something strange has happened and should be analyzed
if( NOT output )
    execute_process( COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${FailTag} )
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
string( REGEX MATCH "Success" wasSuccess ${output} )

if( wasSuccess )
    return()

else()
    execute_process( COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${FailTag} )
    message( SEND_ERROR "ERROR: *** ${result} ***" )

endif()

