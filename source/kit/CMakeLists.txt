target_sources(
  ${KIT_LIBRARY}
    PRIVATE
      atomic.c input_buffer.c input_stream.c lower_bound.c
      string_ref.c async_function.c allocator.c array_ref.c dynamic_array.c
      mersenne_twister_64.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/atomic.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/allocator.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/string_ref.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/dynamic_array.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/async_function.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/input_stream.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/input_buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lower_bound.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/array_ref.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/mersenne_twister_64.h>)
