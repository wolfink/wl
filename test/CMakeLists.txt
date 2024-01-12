if(TEST_ALL OR TEST_LEXER)
  add_subdirectory(lexer)
endif()

if(TEST_ALL OR TEST_LEXER)
  add_subdirectory(parser)
endif()

if(TEST_ALL OR TEST_LEXER)
  add_subdirectory(util)
endif()
