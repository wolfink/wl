cmake_minimum_required(VERSION 3.10)
project(WolfinkLangCompiler C)

include(CTest)

set(WLC_VERSION "0.1.0")

option(TEST_ALL OFF)
option(TEST_LEXER "Test the lexer" OFF)
option(TEST_PARSER "Test the parser" OFF)
option(TEST_UTIL "Test the utility library" OFF)
option(TEST_GEN  "Test the code generator" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS_DEBUG "-ggdb -g3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB UTIL_FILES   ${SRC_DIR}/util/*.c)
file(GLOB LEXER_FILES  ${SRC_DIR}/lexer/*.c)
file(GLOB PARSER_FILES ${SRC_DIR}/parser/*.c)
file(GLOB GEN_FILES    ${SRC_DIR}/gen/*.c)
# file(GLOB SOURCE_FILES
#   ${SRC_DIR}/*.c
#   ${UTIL_FILES}
#   ${LEXER_FILES}
#   ${PARSER_FILES}
#   ${GEN_FILES})

add_library(util   OBJECT ${UTIL_FILES})
add_library(lexer  OBJECT ${LEXER_FILES})
add_library(parser OBJECT ${PARSER_FILES})
add_library(gen    OBJECT ${GEN_FILES})

add_executable(WolfinkLangCompiler ${SRC_DIR}/wlc.c)
target_link_libraries(WolfinkLangCompiler util)
target_link_libraries(WolfinkLangCompiler lexer)
target_link_libraries(WolfinkLangCompiler parser)
target_link_libraries(WolfinkLangCompiler gen)

if(TEST_ALL OR TEST_LEXER OR TEST_PARSER OR TEST_UTIL OR TEST_GEN)
  add_subdirectory(test)
endif()

#
#target_compile_options(WolfinkLang)
#
#target_include_directories(WolfinkLang)
#target_link_directories(WolfinkLang)
