
macro(CONFIGURE_EXAMPLE)
    if("${CMAKE_MODULE_PATH}" STREQUAL "")
        # set the module path
        set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules
            ${CMAKE_MODULE_PATH})

        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

        include(GenericCMakeFunctions)
        include(GenericCMakeOptions)
        include(InstallDirs)
        include(Packages)

        ConfigureRootSearchPath(Madthreading)
        find_package(Madthreading)
        if(Madthreading_FOUND)
            include_directories(${Madthreading_INCLUDE_DIRS})
        endif()
    endif()
endmacro(CONFIGURE_EXAMPLE)
