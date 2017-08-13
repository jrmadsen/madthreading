
if(NOT ENABLE_UNIT_TEST)
    return()
endif()

include(TestingMacros)
include(GenericCMakeFunctions)

include_directories(${PROJECT_SOURCE_DIR})

add_option(BATCH_MODE "Enable building but not running of unit test" OFF)

if(BATCH_MODE)
    set(BATCH_OPTION BATCH)
else()
    set(BATCH_OPTION )
endif()

Add_Unit_Test(${BATCH_OPTION}
    NAME "madthreading_tester"
    LINK_LIBRARIES ${EXTERNAL_LIBRARIES} madthreading-shared)

