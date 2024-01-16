
add_executable(lexer_test tester.c)

target_link_libraries(lexer_test util)
target_link_libraries(lexer_test lexer)

add_test(NAME test_tokens COMMAND lexer_test token)
add_test(NAME test_commands_solo COMMAND lexer_test solo)
