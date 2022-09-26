target_sources(
  ${KIT_LIBRARY}
    PRIVATE
      input_buffer.c threads.win32.c time.c atomic.win32.c
      threads.posix.c condition_variable.c move_back.c input_stream.c
      lower_bound.c file.c string_ref.c async_function.c allocator.c
      array_ref.c dynamic_array.c mutex.c mersenne_twister_64.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/mutex.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/time.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/atomic.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/allocator.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/string_ref.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/threads.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/dynamic_array.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/condition_variable.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/async_function.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/move_back.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/input_stream.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/input_buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lower_bound.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/file.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/options.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/array_ref.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/mersenne_twister_64.h>)
