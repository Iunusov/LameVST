cmake_minimum_required(VERSION 3.8)

project("LameVST" CXX)

set(CMAKE_CXX_STANDARD 11)

if(MSVC)
  add_compile_options(/MT)
  add_compile_options(/MP)
  add_compile_options(/nologo)

  #add_compile_options("$<$<CONFIG:Release>:/fp:strict>")
  add_compile_options("$<$<CONFIG:Release>:/O2>")
  include(CheckSymbolExists)
  check_symbol_exists(snprintf "stdio.h" HAVE_SNPRINTF)
  if(NOT HAVE_SNPRINTF)
    add_definitions(-Dsnprintf=_snprintf)
  endif()
else()
  add_compile_options("$<$<CONFIG:Release>:-O2>")
  add_compile_options("$<$<CONFIG:Release>:-fomit-frame-pointer>")
  add_compile_options("$<$<CONFIG:Release>:-fno-exceptions>")
  add_compile_options("$<$<CONFIG:Release>:-pipe>")
endif()

set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/LameVST_DLL")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ SUB_SOURCES)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps)

add_library(${PROJECT_NAME} SHARED ${SUB_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE mp3lame VST_SDK)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE -Wall
            -Wextra
            -Wpedantic
            -pedantic
            # -Werror
            -Wfatal-errors
            -Wshadow
            -Wunused
            -Wdouble-promotion
            -Wundef
            -Wconversion
            -fstack-protector-all
            -fstack-protector-strong
            -Werror=format-security
            -D_FORTIFY_SOURCE=2
            -D_GLIBCXX_ASSERTIONS)
endif()
