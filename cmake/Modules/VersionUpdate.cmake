# generates version.hh if it does not exist

execute_process(COMMAND ${CMAKE_COMMAND}
    -DPROJECT_NAME=${PROJECT_NAME}
    -DMAJOR_VERSION=${${PROJECT_NAME}_MAJOR_VERSION}
    -DMINOR_VERSION=${${PROJECT_NAME}_MINOR_VERSION}
    -DPATCH_VERSION=${${PROJECT_NAME}_PATCH_VERSION}
    -DOUTPUT_DIR=${PROJECT_BINARY_DIR}/source/madthreading
    -P ${PROJECT_SOURCE_DIR}/cmake/Scripts/Version.cmake
)

include_directories(${PROJECT_BINARY_DIR}/source/madthreading)
