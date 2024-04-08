add_executable(gen_test tester.c)

target_link_libraries(gen_test util)
target_link_libraries(gen_test lexer)
target_link_libraries(gen_test parser)
target_link_libraries(gen_test gen)

add_test(NAME gen_test_cfg_solo COMMAND gen_test cfg_solo)
