
add_executable(lexer_test tester.c)

target_link_libraries(lexer_test util)
target_link_libraries(lexer_test lexer)
