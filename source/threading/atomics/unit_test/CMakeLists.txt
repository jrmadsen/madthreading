
include(TestingMacros)
include(GenericCMakeFunctions)

message(STATUS "Adding unit test for ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "Libraries are : ${LIBRARIES}")
if(BATCH_MODE)
    Add_Unit_Test(BATCH
                  Name
                    "atomics_tester"
                  LinkLibraries
                    ${LIBRARIES} ${PROJECT_NAME}-SHARED)
else()
    Add_Unit_Test(Name
                    "atomics_tester"
                  LinkLibraries
                    ${LIBRARIES} ${PROJECT_NAME}-SHARED)
endif()
