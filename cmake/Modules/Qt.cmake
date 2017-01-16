
include(MacroUtilities)

  ConfigureRootSearchPath(QT)
  ConfigureRootSearchPath(QT4)
  ConfigureRootSearchPath(QT5)

  option(USE_QT_QMAKE "Use qmake" OFF)
  add_feature(USE_QT_QMAKE "Use qmake")

  set(QtPackages Core Gui Xml Widgets PrintSupport Network)
  set(QtPackagePaths )
  if(NOT USE_QT_QMAKE AND NOT CMAKE_VERSION VERSION_LESS 3.0)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    option(USE_QT4 "Force use of Qt4 always" OFF)
    add_feature(USE_QT4 "Force explicit use of Qt4 (don't search for Qt5)")

    if(NOT USE_QT4)
      find_package(Qt5Core QUIET)
      find_package(Qt5Gui QUIET)
      find_package(Qt5Xml QUIET)
      find_package(Qt5Widgets QUIET)
      find_package(Qt5PrintSupport QUIET)
      find_packagE(Qt5Network QUIET)
    endif()

    if(NOT USE_QT4
       AND Qt5Core_FOUND
       AND Qt5Gui_FOUND
       AND Qt5Widgets_FOUND
       AND Qt5Xml_FOUND
       AND Qt5PrintSupport_FOUND
       AND Qt5Network_FOUND)
      INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS}
                          ${Qt5Gui_INCLUDE_DIRS}
                          ${Qt5Widgets_INCLUDE_DIRS}
                          ${Qt5Xml_INCLUDE_DIRS}
                          ${Qt5PrintSupport_INCLUDE_DIRS}
                          ${Qt5Network_INCLUDE_DIRS})
      list(APPEND ${PROJECT_NAME}_EXTERNAL_LIBRARIES
                            ${Qt5Core_LIBRARIES}
                            ${Qt5Gui_LIBRARIES}
                            ${Qt5Widgets_LIBRARIES}
                            ${Qt5Xml_LIBRARIES}
                            ${Qt5PrintSupport_LIBRARIES}
                            ${Qt5Network_LIBRARIES})
      foreach(package ${QtPackages})
          foreach(lib ${Qt5${package}_LIBRARIES})
              get_target_property(location ${lib} LOCATION)
              list(APPEND QtPackagePaths ${location})
          endforeach()
      endforeach()
    else()
      unset(Qt5Core_DIR CACHE)
      unset(Qt5Gui_DIR CACHE)
      unset(Qt5Widgets_DIR CACHE)
      unset(Qt5Xml_DIR CACHE)
      unset(Qt5PrintSupport_DIR CACHE)
      unset(Qt5Network_DIR CACHE)
      find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtXml QtNetwork)
      INCLUDE_DIRECTORIES(${QT_QTCORE_INCLUDE_DIR}
                          ${QT_QTGUI_INCLUDE_DIR}
                          ${QT_QTXML_INCLUDE_DIR}
                          ${QT_QTPRINTSUPPORT_INCLUDE_DIR}
                          ${QT_QTNETWORK_INCLUDE_DIR})
      list(APPEND ${PROJECT_NAME}_EXTERNAL_LIBRARIES
                            Qt4::QtCore
                            Qt4::QtGui
                            Qt4::QtPrintSupport
                            Qt4::QtXml
                            Qt4::QtNetwork)
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
      # if not Qt5-qmake and not Qt4, look for qmake explicitly
      if("${QMAKE_EXE}" STREQUAL "QMAKE_EXE-NOTFOUND")
    find_program(QMAKE_EXE
          NAMES
          qmake-qt4 qmake
          DOC
          "QMake executable")
      endif()
    endif()

    add_feature(QMAKE_EXE "QMake executable")

  endif()
