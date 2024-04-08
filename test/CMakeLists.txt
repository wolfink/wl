if(TEST_ALL OR TEST_UTIL)
  add_subdirectory(util)
endif()

if(TEST_ALL OR TEST_LEXER)
  add_subdirectory(lexer)
endif()

if(TEST_ALL OR TEST_PARSER)
  add_subdirectory(parser)
endif()

if(TEST_ALL OR TEST_GEN)
  add_subdirectory(gen)
endif()

