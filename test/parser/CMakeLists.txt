add_executable(parser_test tester.c)

target_link_libraries(parser_test util)
target_link_libraries(parser_test lexer)
target_link_libraries(parser_test parser)

add_test(NAME parser_test_commands_solo COMMAND parser_test solo)
add_test(NAME parser_test_example_01 COMMAND parser_test file ${CMAKE_SOURCE_DIR}/examples/01_helloworld.wl)
add_test(NAME parser_test_example_02 COMMAND parser_test file ${CMAKE_SOURCE_DIR}/examples/02_ftoc.wl)
add_test(NAME parser_test_example_03 COMMAND parser_test file ${CMAKE_SOURCE_DIR}/examples/03_repeat.wl)
add_test(NAME parser_test_example_04 COMMAND parser_test file ${CMAKE_SOURCE_DIR}/examples/04_char_count.wl)
add_test(NAME parser_test_example_05 COMMAND parser_test file ${CMAKE_SOURCE_DIR}/examples/05_pow.wl)
add_test(NAME parser_test_example_08 COMMAND parser_test file ${CMAKE_SOURCE_DIR}/examples/08_ranges.wl)
