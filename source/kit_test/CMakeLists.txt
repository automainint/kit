target_sources(
  kit_test
    PRIVATE
      test.c bench.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/bench.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/test.h>)
