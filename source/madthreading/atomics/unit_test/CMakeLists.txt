
if(NOT ENABLE_UNIT_TEST)
    return()
endif()

include(TestingMacros)
include(GenericFunctions)

if(BATCH_MODE)
    Add_Unit_Test(BATCH
                  NAME
                    "atomics_tester"
                  LINK_LIBRARIES
                    ${LIBRARIES} ${PROJECT_NAME}-shared)
else()
    Add_Unit_Test(NAME
                    "atomics_tester"
                  LINK_LIBRARIES
                    ${LIBRARIES} ${PROJECT_NAME}-shared)
endif()
