cmake_minimum_required(VERSION 3.10)
project(WolfinkLangCompiler C)

set(WLC_VERSION "0.1.0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS_DEBUG "-ggdb -g3")

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SOURCE_FILES 
  ${SRC_DIR}/*.c
  ${SRC_DIR}/*.h
  ${SRC_DIR}/util/*.c
  ${SRC_DIR}/lexer/*.c
  ${SRC_DIR}/parser/*.c
  )

add_executable(WolfinkLangCompiler ${SOURCE_FILES})

#target_link_libraries(WolfinkLang)
#
#target_compile_options(WolfinkLang)
#
#target_include_directories(WolfinkLang)
#target_link_directories(WolfinkLang)
