#  script for generating
#  @PROJECT_NAME@ version.hh configuration header file  ----------------------//

#  (C) Copyright Jonathan Madsen 2015.

# Variables that must be defined
#   PROJECT_NAME
#   MAJOR_VERSION
#   MINOR_VERSION
#   PATCH_VERSION
#   OUTPUT_DIR
#
# Variables that are created in this script
#   MAJOR_VERSN_00
#   MINOR_VERSN_00
#   PATCH_VERSN_00
#   VERSION_STRING

macro(check_defined VAR)
  if(NOT DEFINED ${VAR})
    message(FATAL_ERROR "Variable ${VAR} must be defined to run Version.cmake")
  endif()
endmacro()

check_defined(PROJECT_NAME)
check_defined(MAJOR_VERSION)
check_defined(MINOR_VERSION)
check_defined(PATCH_VERSION)
check_defined(OUTPUT_DIR)

macro(define_version_00 PREFIX)
  if(${${PREFIX}_VERSION} LESS 10)
    set(${PREFIX}_VERSN_00 "0${${PREFIX}_VERSION}")
  else()
    set(${PREFIX}_VERSN_00 "${${PREFIX}_VERSION}")
  endif()
endmacro()

define_version_00(MAJOR)
define_version_00(MINOR)
define_version_00(PATCH)

if(${PATCH_VERSION} GREATER 0)
  set(VERSION_STRING "${MAJOR_VERSION}_${MINOR_VERSION}_${PATCH_VERSION}")
else()
  set(VERSION_STRING "${MAJOR_VERSION}_${MINOR_VERSION}")
endif()

set(GENERATE TRUE)
# a check to see if version has changed
if(EXISTS ${OUTPUT_DIR}/version.hh)
  FILE(READ "${OUTPUT_DIR}/version.hh" CURRENT_VERSION_FILE)
  STRING(REPLACE "\n" ";" CURRENT_VERSION_FILE ${CURRENT_VERSION_FILE})
  foreach(_str ${CURRENT_VERSION_FILE})
    string(REGEX MATCH "VERSION_STRING" CURRENT_VERSION_STRING ${_str})
    if(DEFINED CURRENT_VERSION_STRING AND
       NOT ${CURRENT_VERSION_STRING} STREQUAL "")
      STRING(REPLACE " " ";" _str "${_str}")
      STRING(REPLACE ";;" ";" _str "${_str}")
      list(GET _str 2 CURRENT_VERSION_STRING)
    endif()
    if("${CURRENT_VERSION_STRING}" STREQUAL "${VERSION_STRING}")
      set(GENERATE FALSE)
    endif()
  endforeach()
endif()

if(GENERATE)
  configure_file(${CMAKE_CURRENT_LIST_DIR}/../Templates/version.hh.in
                 ${OUTPUT_DIR}/version.hh
                 @ONLY)
endif()

