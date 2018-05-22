
add_option(BUILD_TESTING "Build testing for dashboard" OFF)
mark_as_advanced(BUILD_TESTING)

# testing
ENABLE_TESTING()
if(BUILD_TESTING)
    include(CTest)
endif(BUILD_TESTING)

# if this is directory we are running CDash (don't set to ON)
add_option(DASHBOARD_MODE "Internally used to skip generation of CDash files" OFF NO_FEATURE)
mark_as_advanced(DASHBOARD_MODE)

# ------------------------------------------------------------------------ #
# -- Miscellaneous
# ------------------------------------------------------------------------ #
if(NOT DASHBOARD_MODE AND BUILD_TESTING)
    add_option(CTEST_LOCAL_CHECKOUT "Use the local source tree for CTest/CDash"
        OFF NO_FEATURE)
    if(CTEST_LOCAL_CHECKOUT)
        set(CMAKE_LOCAL_DIRECTORY "${CMAKE_SOURCE_DIR}")
        set(CTEST_MODEL "Continuous" CACHE STRING "Model for CTest")
    else(CTEST_LOCAL_CHECKOUT)
        set(CTEST_MODEL "Nightly" CACHE STRING "Model for CTest")
    endif(CTEST_LOCAL_CHECKOUT)
    mark_as_advanced(CTEST_MODEL)
    mark_as_advanced(CTEST_LOCAL_CHECKOUT)
endif(NOT DASHBOARD_MODE AND BUILD_TESTING)


# ------------------------------------------------------------------------ #
# -- Function to create a temporary directory
# ------------------------------------------------------------------------ #
function(GET_TEMPORARY_DIRECTORY DIR_VAR DIR_BASE DIR_MODEL)
    # create a root working directory
    set(_TMP_ROOT "${CMAKE_BINARY_DIR}/cdash/${DIR_MODEL}")
    set(${DIR_VAR} "${_TMP_ROOT}" PARENT_SCOPE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${_TMP_ROOT})
endfunction()


# ------------------------------------------------------------------------ #
# -- Configure Branch label
# ------------------------------------------------------------------------ #
if(BUILD_TESTING)
    find_package(Git REQUIRED)

    execute_process(COMMAND ${GIT_EXECUTABLE} branch --color=never --contains
        OUTPUT_VARIABLE CMAKE_SOURCE_BRANCH
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "* " "" CMAKE_SOURCE_BRANCH "${CMAKE_SOURCE_BRANCH}")
    string(REPLACE "*" "" CMAKE_SOURCE_BRANCH "${CMAKE_SOURCE_BRANCH}")
    # removes things such as : '(HEAD detached at 3109eab) master' -> 'master'
    STRING(REGEX REPLACE "(\\\(.*\\\) )" "" CMAKE_SOURCE_BRANCH "${CMAKE_SOURCE_BRANCH}")

endif(BUILD_TESTING)


# ------------------------------------------------------------------------ #
# -- Add options
# ------------------------------------------------------------------------ #
macro(add_ctest_options VARIABLE )

    get_cmake_property(_vars CACHE_VARIABLES)
    get_cmake_property(_nvars VARIABLES)
    foreach(_var ${_nvars})
        list(APPEND _vars ${_var})
    endforeach(_var ${_nvars})

    list(REMOVE_DUPLICATES _vars)
    list(SORT _vars)
    set(_set_vars ${ARGN})
    foreach(_var ${_vars})
        STRING(REGEX MATCH "^(USE_|ENABLE_UNIT_)" _use_found "${_var}")
        STRING(REGEX MATCH ".*(_ROOT|_LIBRARY|_INCLUDE_DIR|_EXECUTABLE)$"
            _root_found "${_var}")
        STRING(REGEX MATCH "^(DOXYGEN_|QT_|PREVIOUS_|CMAKE_|OSX_|DEFAULT_|EXTERNAL_|_|CTEST_)"
            _skip_prefix "${_var}")
        STRING(REGEX MATCH ".*(_AVAILABLE|_LIBRARIES|_INCLUDE_DIRS)$"
            _skip_suffix "${_var}")

        if(_skip_prefix OR _skip_suffix)
            continue()
        endif(_skip_prefix OR _skip_suffix)

        if(_use_found OR _root_found)
            list(APPEND _set_vars ${_var})
        endif(_use_found OR _root_found)
    endforeach(_var ${_vars})

    list(REMOVE_DUPLICATES _set_vars)
    list(SORT _set_vars)
    foreach(_var ${_set_vars})
        if(NOT "${${_var}}" STREQUAL "" AND
            NOT "${${_var}}" STREQUAL "${_var}-NOTFOUND")
            add(${VARIABLE} "-D${_var}='${${_var}}'")
        endif(NOT "${${_var}}" STREQUAL "" AND
            NOT "${${_var}}" STREQUAL "${_var}-NOTFOUND")
    endforeach(_var ${_set_vars})

    unset(_vars)
    unset(_nvars)
    unset(_set_vars)

endmacro(add_ctest_options VARIABLE )


# ------------------------------------------------------------------------ #
# -- Configure CTest for CDash
# ------------------------------------------------------------------------ #
if(NOT DASHBOARD_MODE AND BUILD_TESTING)
    # get temporary directory for dashboard testing
    if(NOT DEFINED CMAKE_DASHBOARD_ROOT)
        GET_TEMPORARY_DIRECTORY(CMAKE_DASHBOARD_ROOT
            "${CMAKE_PROJECT_NAME}-cdash" ${CTEST_MODEL})
    endif(NOT DEFINED CMAKE_DASHBOARD_ROOT)

    # set the CMake configure options
    add_ctest_options(CMAKE_CONFIGURE_OPTIONS
        CMAKE_BUILD_TYPE
        CMAKE_C_COMPILER CMAKE_CXX_COMPILER
        MPI_C_COMPILER MPI_CXX_COMPILER
        CTEST_MODEL CTEST_SITE)

    ## -- CTest Config
    if(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")
        configure_file(${CMAKE_SOURCE_DIR}/CTestConfig.cmake
            ${CMAKE_BINARY_DIR}/CTestConfig.cmake @ONLY)
    endif(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")

    set(cdash_templates Init Build Test Submit Glob Stages)
    if(USE_COVERAGE)
        list(APPEND cdash_templates Coverage)
    endif(USE_COVERAGE)
    if(MEMORYCHECK_COMMAND)
        list(APPEND cdash_templates MemCheck)
    endif(MEMORYCHECK_COMMAND)
    foreach(_type ${cdash_templates})
        ## -- CTest Setup
        if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in")
            configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in
                ${CMAKE_BINARY_DIR}/cdash/${_type}.cmake @ONLY)
        endif(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in")
    endforeach(_type Init Build Test Coverage MemCheck Submit Glob Stages)

    ## -- CTest Custom
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")
        configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in
            ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)
    endif(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")

endif(NOT DASHBOARD_MODE AND BUILD_TESTING)


# ------------------------------------------------------------------------ #
# -- Add tests
# ------------------------------------------------------------------------ #
# simplified macro for adding tests
macro(mad_test _NAME)
    set(_CMD ${ARGN})
    if("${_CMD}" STREQUAL "")
        set(_CMD ./${_NAME})
    endif("${_CMD}" STREQUAL "")

    add_test(NAME ${_NAME}
        COMMAND ${_CMD}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
    set_tests_properties(${_NAME} PROPERTIES
        LABELS "example" TIMEOUT 7200)

endmacro(mad_test _NAME)

if(BUILD_EXAMPLES)

    mad_test(Example1 ./ex1)
    mad_test(Example2 ./ex2)
    #mad_test(Example3 ${PYTHON_EXECUTABLE} ./async.py)
    if(USE_SSE AND AVX2_FOUND)
        mad_test(Example4 ./ex4)
    endif(USE_SSE AND AVX2_FOUND)
    mad_test(Example5 ./ex5)

    mad_test(pi_serial)
    mad_test(pi_lambda_serial)
    mad_test(pi_cxx11)
    mad_test(pi_thread_pool)

    if(OpenMP_FOUND)
        mad_test(OpenMP_pi_loop ./omp_pi_loop)
        mad_test(OpenMP_pi_spmd_simple ./omp_pi_spmd_simple)
        mad_test(OpenMP_pi_spmd_final ./omp_pi_spmd_final)
        mad_test(OpenMP_pi_task ./omp_pi_task)
    endif(OpenMP_FOUND)

    if(TBB_FOUND)
        mad_test(pi_lambda_tbb)
        mad_test(pi_tbb)
    endif(TBB_FOUND)

    if(NOT WIN32)
        mad_test(pi_pthreads)
    endif(NOT WIN32)

endif(BUILD_EXAMPLES)

# built from TiMemory
mad_test(test_timing)
if(MPI_FOUND)
    mad_test(mpi_test_timing)
endif(MPI_FOUND)

# unit test binaries
if(ENABLE_UNIT_TEST)
    macro(mad_unit_test _NAME _DIR)
        set(_CMD ${ARGN})
        if("${_CMD}" STREQUAL "")
            set(_CMD ./${_NAME})
        endif("${_CMD}" STREQUAL "")

        add_test(NAME ${_NAME}
            COMMAND ${_CMD}
            WORKING_DIRECTORY ${_DIR})
        set_tests_properties(${_NAME} PROPERTIES
            LABELS "unit_test" TIMEOUT 7200)

    endmacro(mad_unit_test _NAME _DIR)

    mad_unit_test(Madthreading_Test ${PROJECT_BINARY_DIR}/unit_test ./Unitmadthreading_tester)
    mad_unit_test(Atomics_Test ${PROJECT_BINARY_DIR}/source/madthreading/atomics/unit_test ./Unitatomics_tester)
endif(ENABLE_UNIT_TEST)
