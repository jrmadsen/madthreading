#
# Simplified interface for finding Qt components
#
#	Default Qt Components:
#		QtCore, QtGui, QtXml, QtWidgets, QtPrintSupport, and QtNetwork
#
#	Default Library Variable:
#		${PROJECT_NAME}_EXTERNAL_LIBRARIES
#
#	Default Include Directory Variable:
#		None - calls INCLUDE_DIRECTORIES(...)
#
#	To change the Qt components, define:
#		QtPackages (without Qt prefix)
#
#	To change the library variable, define:
#		EXTERNAL_LIBRARIES_VARIABLE
#
#	To use the include directory variable, define:
#		EXTERNAL_INCLUDE_DIRS_VARIABLE
#
#	example:
#		#----------------------------------------------------------------------#
#		# using defaults
#		#----------------------------------------------------------------------#
#		include(Qt)
#		...
#		add_executable(<target> <file> ...)
#		target_link_libraries(<target> ${${PROJECT_NAME}_EXTERNAL_LIBRARIES})
#		#----------------------------------------------------------------------#
#
#		#----------------------------------------------------------------------#
#		# using custom package, include, and library variables
#		#----------------------------------------------------------------------#
#		set(QtPackages Core Gui PrintSupport Widgets)
#		set(EXTERNAL_INCLUDE_DIRS_VARIABLE QT_EXTERNAL_INCLUDE_DIRS)
#		set(EXTERNAL_LIBRARIES_VARIABLE QT_EXTERNAL_LIBRARIES)
#		include(Qt)
#		...
#		include_directories(${QT_EXTERNAL_INCLUDE_DIRS})
#		add_executable(<target> <file> ...)
#		target_link_libraries(<target> ${QT_EXTERNAL_LIBRARIES})
#		#----------------------------------------------------------------------#
#

include(MacroUtilities)

if(QT5_ONLY)
    set(ALLOW_QT4 OFF)
else()
    set(ALLOW_QT4 ON)
endif()

#------------------------------------------------------------------------------#
# Configure CMAKE_PREFIX_PATH for Qt
ConfigureRootSearchPath(QT)
if(ALLOW_QT4)
    ConfigureRootSearchPath(QT4)
endif()
ConfigureRootSearchPath(QT5)
mark_as_advanced(QT_ROOT Qt_ROOT QT4_ROOT QT5_ROOT)

#------------------------------------------------------------------------------#
# use QMake option
option(USE_QT_QMAKE "Use qmake" OFF)
mark_as_advanced(USE_QT_QMAKE)

#------------------------------------------------------------------------------#
# define default EXTERNAL_LIBRARIES_VARIABLE if not provided
if(NOT DEFINED EXTERNAL_LIBRARIES_VARIABLE)
    set(EXTERNAL_LIBRARIES_VARIABLE ${PROJECT_NAME}_EXTERNAL_LIBRARIES)
endif()

#------------------------------------------------------------------------------#
# define default packages if not provided
if(NOT DEFINED QtPackages)
    set(QtPackages Core Gui Xml Widgets PrintSupport Network)
endif()
set(QtPackagePaths )

#------------------------------------------------------------------------------#
# Qt components list built from QtPackages
set(QtComponents )
foreach(package ${QtPackages})
    list(APPEND QtComponents Qt${package})
    mark_as_advanced(Qt5${package}_DIR)
endforeach()

#------------------------------------------------------------------------------#
# macro for including directories
macro(_qt_include)
    if(DEFINED EXTERNAL_INCLUDE_DIRS_VARIABLE)
        list(APPEND ${EXTERNAL_INCLUDE_DIRS_VARIABLE} ${ARGN})
    else()
        INCLUDE_DIRECTORIES(${ARGN})
    endif()
endmacro()
#------------------------------------------------------------------------------#

if(NOT USE_QT_QMAKE AND NOT CMAKE_VERSION VERSION_LESS 3.0)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)

    if(ALLOW_QT4)
        option(USE_QT4 "Force use of Qt4 always" OFF)
        add_feature(USE_QT4 "Force explicit use of Qt4 (don't search for Qt5)")
    else()
        set(USE_QT4 OFF)
    endif()

    set(Qt5_PACKAGES_FOUND ON)
    if(NOT USE_QT4)
        foreach(package ${QtPackages})
            if(NOT ALLOW_QT)
                find_package(Qt5${package} REQUIRED)
            else()
                find_package(Qt5${package} QUIET)
            endif()
            if(NOT Qt5${package}_FOUND)
                set(Qt5_PACKAGES_FOUND OFF)
            else()
                set(Qt_${package}_FOUND ON)
            endif()
        endforeach()
    endif()

    if(NOT USE_QT4 AND Qt5_PACKAGES_FOUND)
        # include the directories
        foreach(package ${QtPackages})
            _qt_include(${Qt5${package}_INCLUDE_DIRS})
            list(APPEND ${EXTERNAL_LIBRARIES_VARIABLE} ${Qt5${package}_LIBRARIES})
        endforeach()
        # get the package paths
        foreach(package ${QtPackages})
            foreach(lib ${Qt5${package}_LIBRARIES})
                get_target_property(location ${lib} LOCATION)
                list(APPEND QtPackagePaths ${location})
            endforeach()
        endforeach()

    elseif(ALLOW_QT4)	# Qt4 or not all Qt5 packages were found

        foreach(package ${QtPackages})
            unset(Qt5${package}_DIR CACHE)
        endforeach()
        # find package components
        find_package(Qt4 REQUIRED COMPONENTS ${QtComponents})
        # set these for checking later, if not found REQUIRED will fail
        foreach(package ${QtPackages})
            set(Qt_${package}_FOUND ON)
        endforeach()
        # add include dirs and libraries
        foreach(component ${QtComponents})
            string(TOUPPER ${component} UPPCOMPONENT)
            _qt_include(${QT_${UPPCOMPONENT}_INCLUDE_DIR})
            list(APPEND ${EXTERNAL_LIBRARIES_VARIABLE} Qt4::${component})
        endforeach()
        # get paths
        foreach(package ${QtPackages})
            get_filename_component(_package_path Qt4::Qt${package} PATH)
            list(APPEND QtPackagePaths ${location})
        endforeach()

    endif()

else()
    if(NOT QMAKE_EXE)
        set(QMAKE_EXE "QMAKE_EXE-NOTFOUND")
        # look for Qt5-qmake
        find_package(Qt5Core QUIET)
        if(Qt5Core_FOUND)
            set(QMAKE_EXE ${Qt5Core_QMAKE_EXECUTABLE} CACHE STRING "QMake executable" FORCE)
        endif()
        # if not Qt5-qmake, search for Qt4
        if("${QMAKE_EXE}" STREQUAL "QMAKE_EXE-NOTFOUND")
            find_package(Qt4)
            set(QMAKE_EXE ${QT_QMAKE_EXECUTABLE} CACHE STRING "QMake executable" FORCE)
        endif()
        if(ALLOW_QT4)
            # if not Qt5-qmake and not Qt4, look for qmake explicitly
            if("${QMAKE_EXE}" STREQUAL "QMAKE_EXE-NOTFOUND")
                find_program(QMAKE_EXE
                    NAMES qmake-qt4 qmake
                    DOC "QMake executable")
            endif()
        endif()
    endif()
    add_feature(QMAKE_EXE "QMake executable")

endif()

mark_as_advanced(QT_QMAKE_EXECUTABLE)
