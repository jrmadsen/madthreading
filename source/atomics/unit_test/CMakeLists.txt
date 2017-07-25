
include(TestingMacros)
include(GenericCMakeFunctions)

message(STATUS "Adding unit test for ${CMAKE_CURRENT_LIST_DIR}")

if(BATCH_MODE)
    Add_Unit_Test(BATCH
                  NAME
                    "atomics_tester"
                  LINK_LIBRARIES
                    ${LIBRARIES} ${PROJECT_NAME}-SHARED)
else()
    Add_Unit_Test(NAME
                    "atomics_tester"
                  LINK_LIBRARIES
                    ${LIBRARIES} ${PROJECT_NAME}-SHARED)
endif()
