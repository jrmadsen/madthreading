
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)
include(CheckCXXSymbolExists)
include(Compilers)

# Check if SSE/AVX instructions are available on the machine where
# the project is compiled.

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
   EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)

   STRING(REGEX REPLACE "^.*(sse2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse2" "${SSE_THERE}" SSE2_TRUE)
   IF (SSE2_TRUE)
      set(SSE2_FOUND ON CACHE BOOL "SSE2 available on host")
   ELSE (SSE2_TRUE)
      set(SSE2_FOUND OFF CACHE BOOL "SSE2 available on host")
   ENDIF (SSE2_TRUE)

   # /proc/cpuinfo apparently omits sse3 :(
   STRING(REGEX REPLACE "^.*[^s](sse3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse3" "${SSE_THERE}" SSE3_TRUE)
   IF (NOT SSE3_TRUE)
      STRING(REGEX REPLACE "^.*(T2300).*$" "\\1" SSE_THERE ${CPUINFO})
      STRING(COMPARE EQUAL "T2300" "${SSE_THERE}" SSE3_TRUE)
   ENDIF (NOT SSE3_TRUE)

   STRING(REGEX REPLACE "^.*(ssse3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "ssse3" "${SSE_THERE}" SSSE3_TRUE)
   IF (SSE3_TRUE OR SSSE3_TRUE)
      set(SSE3_FOUND ON CACHE BOOL "SSE3 available on host")
   ELSE (SSE3_TRUE OR SSSE3_TRUE)
      set(SSE3_FOUND OFF CACHE BOOL "SSE3 available on host")
   ENDIF (SSE3_TRUE OR SSSE3_TRUE)
   IF (SSSE3_TRUE)
      set(SSSE3_FOUND ON CACHE BOOL "SSSE3 available on host")
   ELSE (SSSE3_TRUE)
      set(SSSE3_FOUND OFF CACHE BOOL "SSSE3 available on host")
   ENDIF (SSSE3_TRUE)

   STRING(REGEX REPLACE "^.*(sse4_1).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse4_1" "${SSE_THERE}" SSE41_TRUE)
   IF (SSE41_TRUE)
      set(SSE4_1_FOUND ON CACHE BOOL "SSE4.1 available on host")
   ELSE (SSE41_TRUE)
      set(SSE4_1_FOUND OFF CACHE BOOL "SSE4.1 available on host")
   ENDIF (SSE41_TRUE)

   STRING(REGEX REPLACE "^.*(sse4_2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse4_2" "${SSE_THERE}" SSE42_TRUE)
   IF (SSE42_TRUE)
      set(SSE4_2_FOUND ON CACHE BOOL "SSE4.2 available on host")
   ELSE (SSE42_TRUE)
      set(SSE4_2_FOUND OFF CACHE BOOL "SSE4.2 available on host")
   ENDIF (SSE42_TRUE)

   STRING(REGEX REPLACE "^.*(avx).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "avx" "${SSE_THERE}" AVX_TRUE)
   IF (AVX_TRUE)
      set(AVX_FOUND ON CACHE BOOL "AVX available on host")
   ELSE (AVX_TRUE)
      set(AVX_FOUND OFF CACHE BOOL "AVX available on host")
   ENDIF (AVX_TRUE)

   STRING(REGEX REPLACE "^.*(avx2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "avx2" "${SSE_THERE}" AVX2_TRUE)
   IF (AVX2_TRUE)
      set(AVX2_FOUND ON CACHE BOOL "AVX2 available on host")
   ELSE (AVX2_TRUE)
      set(AVX2_FOUND OFF CACHE BOOL "AVX2 available on host")
   ENDIF (AVX2_TRUE)

ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
   EXEC_PROGRAM("/usr/sbin/sysctl -n machdep.cpu.features machdep.cpu.leaf7_features" OUTPUT_VARIABLE
      CPUINFO)

   STRING(REGEX REPLACE "^.*[^S](SSE2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE2" "${SSE_THERE}" SSE2_TRUE)
   IF (SSE2_TRUE)
      set(SSE2_FOUND ON CACHE BOOL "SSE2 available on host")
   ELSE (SSE2_TRUE)
      set(SSE2_FOUND OFF CACHE BOOL "SSE2 available on host")
   ENDIF (SSE2_TRUE)

   STRING(REGEX REPLACE "^.*[^S](SSE3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE3" "${SSE_THERE}" SSE3_TRUE)
   IF (SSE3_TRUE)
      set(SSE3_FOUND ON CACHE BOOL "SSE3 available on host")
   ELSE (SSE3_TRUE)
      set(SSE3_FOUND OFF CACHE BOOL "SSE3 available on host")
   ENDIF (SSE3_TRUE)

   STRING(REGEX REPLACE "^.*(SSSE3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSSE3" "${SSE_THERE}" SSSE3_TRUE)
   IF (SSSE3_TRUE)
      set(SSSE3_FOUND ON CACHE BOOL "SSSE3 available on host")
   ELSE (SSSE3_TRUE)
      set(SSSE3_FOUND OFF CACHE BOOL "SSSE3 available on host")
   ENDIF (SSSE3_TRUE)

   STRING(REGEX REPLACE "^.*(SSE4.1).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE4.1" "${SSE_THERE}" SSE41_TRUE)
   IF (SSE41_TRUE)
      set(SSE4_1_FOUND ON CACHE BOOL "SSE4.1 available on host")
   ELSE (SSE41_TRUE)
      set(SSE4_1_FOUND OFF CACHE BOOL "SSE4.1 available on host")
   ENDIF (SSE41_TRUE)

   STRING(REGEX REPLACE "^.*(SSE4.2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE4.2" "${SSE_THERE}" SSE42_TRUE)
   IF (SSE42_TRUE)
      set(SSE4_2_FOUND ON CACHE BOOL "SSE4.2 available on host")
   ELSE (SSE42_TRUE)
      set(SSE4_2_FOUND OFF CACHE BOOL "SSE4.2 available on host")
   ENDIF (SSE42_TRUE)

   STRING(REGEX REPLACE "^.*(AVX).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "AVX" "${SSE_THERE}" AVX_TRUE)
   IF (AVX_TRUE)
      set(AVX_FOUND ON CACHE BOOL "AVX available on host")
   ELSE (AVX_TRUE)
      set(AVX_FOUND OFF CACHE BOOL "AVX available on host")
   ENDIF (AVX_TRUE)

   STRING(REGEX REPLACE "^.*(AVX2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "AVX2" "${SSE_THERE}" AVX2_TRUE)
   IF (AVX2_TRUE)
      set(AVX2_FOUND ON CACHE BOOL "AVX2 available on host")
   ELSE (AVX2_TRUE)
      set(AVX2_FOUND OFF CACHE BOOL "AVX2 available on host")
   ENDIF (AVX2_TRUE)

ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
   # TODO
   set(SSE2_FOUND   ON  CACHE BOOL "SSE2 available on host")
   set(SSE3_FOUND   OFF CACHE BOOL "SSE3 available on host")
   set(SSSE3_FOUND  OFF CACHE BOOL "SSSE3 available on host")
   set(SSE4_1_FOUND OFF CACHE BOOL "SSE4.1 available on host")
   set(SSE4_2_FOUND OFF CACHE BOOL "SSE4.2 available on host")
   set(AVX_FOUND 	OFF CACHE BOOL "AVX available on host")
   set(AVX2_FOUND 	OFF CACHE BOOL "AVX2 available on host")
ELSE(CMAKE_SYSTEM_NAME MATCHES "Linux")
   set(SSE2_FOUND   ON  CACHE BOOL "SSE2 available on host")
   set(SSE3_FOUND   OFF CACHE BOOL "SSE3 available on host")
   set(SSSE3_FOUND  OFF CACHE BOOL "SSSE3 available on host")
   set(SSE4_1_FOUND OFF CACHE BOOL "SSE4.1 available on host")
   set(SSE4_2_FOUND OFF CACHE BOOL "SSE4.2 available on host")
   set(AVX_FOUND 	OFF CACHE BOOL "AVX available on host")
   set(AVX2_FOUND 	OFF CACHE BOOL "AVX2 available on host")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

#foreach(type SSE2 SSE3 SSSE3 SSE4_1 SSE4_2 AVX AVX2)
#    if(NOT ${type}_FOUND)
#        MESSAGE(STATUS "Could not find hardware support for ${type} on this machine.")
#    endif(NOT ${type}_FOUND)
#endforeach()

mark_as_advanced(SSE2_FOUND SSE3_FOUND SSSE3_FOUND
    SSE4_1_FOUND SSE4_2_FOUND AVX_FOUND AVX2_FOUND)

include(Compilers)

#==============================================================================#
#   function to see intrinsic compiles without error
#   - it creates an main that declares the intrinsic and checks if it compiles
#
FUNCTION(CHECK_FOR_INTRIN VAR FLAG _ARG _RET _FUNC)

    get_property(dirs DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

    set(INTRIN_ARG "${_ARG}")
    set(INTRIN_FUNC "${_FUNC}")
    set(INTRIN_RET "${_RET}")

    configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/intrin-test.cc.in
        ${CMAKE_BINARY_DIR}/compile-test/intrin-test.cc @ONLY)

    try_compile(RET
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/compile-test/intrin-test.cc
        CMAKE_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}"
        OUTPUT_VARIABLE RET_OUT)

    IF(RET)
        set(${VAR} ON PARENT_SCOPE)
    ELSE()
        set(${VAR} OFF PARENT_SCOPE)
    ENDIF()

ENDFUNCTION(CHECK_FOR_INTRIN VAR FLAG INTRIN_NAME)


#==============================================================================#
#   function to get SSE/AVX compile flags
#
FUNCTION(GET_SSE_COMPILE_FLAGS _FLAGS_VAR _DEFS_VAR)

    if(CMAKE_CXX_COMPILER_IS_INTEL)

        set(SSE_CXX_FLAGS "-xHOST")

    elseif(CMAKE_CXX_COMPILER_IS_GNU)

        foreach(type SSE2 SSE3 SSSE3 SSE4_1 SSE4_2 AVX AVX2)
            string(TOLOWER "${type}" _flag)
            string(REPLACE "_" "." _flag "${_flag}")
            set(${type}_FLAGS "-m${_flag}")

            CHECK_CXX_COMPILER_FLAG("${${type}_FLAGS}" COMPILER_SUPPORTS_${type})

            if(${type}_FOUND AND COMPILER_SUPPORTS_${type})
                set(SSE_CXX_FLAGS "${SSE_CXX_FLAGS} ${${type}_FLAGS}")
                list(APPEND SSE_DEFINITIONS HAS_${type})
            endif()

        endforeach()

    endif(CMAKE_CXX_COMPILER_IS_INTEL)

    set(${_FLAGS_VAR} "${SSE_CXX_FLAGS}" PARENT_SCOPE)
    set(${_DEFS_VAR} ${SSE_DEFINITIONS} PARENT_SCOPE)
ENDFUNCTION(GET_SSE_COMPILE_FLAGS _FLAGS_VAR _DEFS_VAR)
