
add_executable(lexer_test tester.c)

target_link_libraries(lexer_test util)
target_link_libraries(lexer_test lexer)

add_test(NAME lexer_test_tokens COMMAND lexer_test token)
add_test(NAME lexer_test_commands_solo COMMAND lexer_test solo)
add_test(NAME lexer_test_example_01 COMMAND lexer_test file ${CMAKE_SOURCE_DIR}/examples/01_helloworld.wl)
add_test(NAME lexer_test_example_02 COMMAND lexer_test file ${CMAKE_SOURCE_DIR}/examples/02_ftoc.wl)
add_test(NAME lexer_test_example_03 COMMAND lexer_test file ${CMAKE_SOURCE_DIR}/examples/03_repeat.wl)
add_test(NAME lexer_test_example_04 COMMAND lexer_test file ${CMAKE_SOURCE_DIR}/examples/04_char_count.wl)
add_test(NAME lexer_test_example_05 COMMAND lexer_test file ${CMAKE_SOURCE_DIR}/examples/05_pow.wl)
add_test(NAME lexer_test_example_08 COMMAND lexer_test file ${CMAKE_SOURCE_DIR}/examples/08_ranges.wl)
