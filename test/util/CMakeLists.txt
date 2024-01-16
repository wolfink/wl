add_executable(util_test tester.c)

target_link_libraries(util_test util)

add_test(NAME test_arena_allocate COMMAND util_test arena_alloc)
add_test(NAME test_arena_init COMMAND util_test arena_init)
add_test(NAME test_strnew COMMAND util_test strnew)
add_test(NAME test_strcat COMMAND util_test strcat)
