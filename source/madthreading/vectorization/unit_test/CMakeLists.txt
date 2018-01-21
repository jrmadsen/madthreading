
if(NOT ENABLE_UNIT_TEST)
    return()
endif()

include(TestingMacros)
include(GenericFunctions)

set(BATCH_FLAG )
if(BATCH_MODE)
    set(BATCH_FLAG BATCH)
endif(BATCH_MODE)

Add_Unit_Test(${BATCH_FLAG}
    NAME "vectorization_tester"
    LINK_LIBRARIES ${LIBRARIES} ${PROJECT_NAME}-shared)
