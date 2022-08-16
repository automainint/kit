target_sources(
  ${KIT_LIBRARY}
    PRIVATE
      time.c threads_posix.c threads_win32.c
      threads_win32_tls_callback.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/threads_win32.h>)
