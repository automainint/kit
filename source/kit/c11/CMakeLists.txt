target_sources(
  ${KIT_LIBRARY}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/time.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/threads.h>)
add_subdirectory(impl)
