target_sources(
  ${KIT_TEST_SUITE}
    PRIVATE
      async_function.test.c mutex.test.c test_duration.test.c
      main.test.c string_ref.test.c atomic.test.c thread.test.c
      array_ref.test.c input_stream.test.c lower_bound.test.c
      condition_variable.test.c mersenne_twister_64.test.c input_buffer.test.c
      move_back.test.c dynamic_array.test.c file.test.c)
