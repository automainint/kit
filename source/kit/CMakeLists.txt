target_sources(
  ${KIT_LIBRARY}
    PRIVATE
      async_function.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/async_function.h>)
