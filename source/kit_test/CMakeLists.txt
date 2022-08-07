target_sources(
  ${KIT_TEST_LIBRARY}
    PRIVATE
      test.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/test.h>)
