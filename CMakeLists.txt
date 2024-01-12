cmake_minimum_required(VERSION 3.10)
project(WolfinkLangCompiler C)

set(WLC_VERSION "0.1.0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS_DEBUG "-ggdb -g3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB UTIL   ${SRC_DIR}/util/*.c)
file(GLOB LEXER  ${SRC_DIR}/lexer/*.c)
file(GLOB PARSER ${SRC_DIR}/parser/*.c)

add_library(util   OBJECT ${UTIL})
add_library(lexer  OBJECT ${LEXER})
add_library(parser OBJECT ${PARSER})

add_executable(WolfinkLangCompiler ${SRC_DIR}/wlc.c)
target_link_libraries(WolfinkLangCompiler util)
target_link_libraries(WolfinkLangCompiler lexer)
target_link_libraries(WolfinkLangCompiler parser)

#
#target_compile_options(WolfinkLang)
#
#target_include_directories(WolfinkLang)
#target_link_directories(WolfinkLang)
