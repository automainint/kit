target_sources(
  kit_test
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/bench.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/test.h>
    PRIVATE
      test.c bench.c)
