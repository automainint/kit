target_sources(
  ${KIT_TEST_SUITE}
    PRIVATE
      async_function.test.c test_duration.test.c main.test.c
      string_ref.test.c atomic.test.c thread.test.c array_ref.test.c
      input_stream.test.c lower_bound.test.c input_buffer.test.c
      dynamic_array.test.c)
