add_executable(parser_test tester.c)

target_link_libraries(parser_test util)
target_link_libraries(parser_test lexer)
target_link_libraries(parser_test parser)

add_test(NAME parser_test COMMAND parser_test)
