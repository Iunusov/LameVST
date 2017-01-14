cmake_minimum_required(VERSION 3.0)

project (LameVST_Solution)

set(CMAKE_BUILD_TYPE Release)

if((${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC))
  set(MYFLAGS "/O2 /WX- /MT")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MYFLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MYFLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MYFLAGS}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${MYFLAGS}")
  include(CheckSymbolExists)
  check_symbol_exists(snprintf "stdio.h" HAVE_SNPRINTF)
  if(NOT HAVE_SNPRINTF)
  add_definitions(-Dsnprintf=_snprintf)
  endif()
else()
  set(MYFLAGS "-Ofast")
  if(MINGW)
    set(MYFLAGS "-static")
  endif(MINGW)
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${MYFLAGS}")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MYFLAGS}")
  set(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} ${MYFLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} ${MYFLAGS}")
endif()

include_directories (${CMAKE_CURRENT_BINARY_DIR})


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/CMake)
