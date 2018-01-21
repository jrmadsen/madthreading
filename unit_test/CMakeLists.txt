
if(NOT ENABLE_UNIT_TEST)
    return()
endif()

include(TestingMacros)
include(GenericFunctions)

include_directories(${PROJECT_SOURCE_DIR}/source ${TIMEMORY_INCLUDE_DIRS})

add_option(BATCH_MODE "Enable building but not running of unit test" OFF)

if(BATCH_MODE)
    set(BATCH_OPTION BATCH)
else()
    set(BATCH_OPTION )
endif()

include(Compilers)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_COMPILER_IS_INTEL_ICC OR CMAKE_COMPILER_IS_INTEL_ICPC)
    set(TEST_COMPILE_FLAGS "-Wno-unknown-pragmas")
else()
    set(TEST_COMPILE_FLAGS "")
endif()

Add_Unit_Test(${BATCH_OPTION}
    NAME "madthreading_tester"
    COMPILE_FLAGS "${TEST_COMPILE_FLAGS}"
    LINK_LIBRARIES ${LIBRARIES} madthreading-shared)

