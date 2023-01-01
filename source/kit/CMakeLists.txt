target_sources(
  kit
    PRIVATE
      input_buffer.c bigint.c status.c secure_random.c
      thread.posix.c atomic.win32.c condition_variable.c thread.win32.c
      move_back.c input_stream.c lower_bound.c file.c string_ref.c
      async_function.c allocator.c array_ref.c dynamic_array.c mutex.c
      mersenne_twister_64.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/mutex.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/status.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/atomic.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/allocator.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/string_ref.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/thread.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/bigint.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/dynamic_array.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/condition_variable.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/async_function.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/move_back.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/secure_random.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/input_stream.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/thread_defs.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/input_buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lower_bound.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/file.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/array_ref.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/mersenne_twister_64.h>)
