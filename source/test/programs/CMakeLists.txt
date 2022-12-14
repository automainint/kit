project(
  ${KIT_PROJECT}_test_programs
  VERSION 0.1.1
  DESCRIPTION "Kit test programs"
  LANGUAGES C CXX)

if(KIT_ENABLE_TESTING)
  add_executable(too_many_tests too_many_tests.c)
  target_link_libraries(too_many_tests kit_test)
  add_test(
    NAME too_many_tests_test
    COMMAND too_many_tests --quiet)
  set_tests_properties(
    too_many_tests_test
    PROPERTIES
    TIMEOUT "15")

  add_executable(too_many_assertions too_many_assertions.c)
  target_link_libraries(too_many_assertions kit_test)
  add_test(
    NAME too_many_assertions_test
    COMMAND too_many_assertions --quiet)
  set_tests_properties(
    too_many_assertions_test
    PROPERTIES
    TIMEOUT "15")

  add_executable(cpp_example cpp_example.cpp)
  target_link_libraries(cpp_example kit_test)
  add_test(
    NAME cpp_example_test
    COMMAND cpp_example --quiet)
  set_tests_properties(
    cpp_example_test
    PROPERTIES
    TIMEOUT "15")

  if(NOT MSVC)
    add_executable(signals signals.cpp)
    target_link_libraries(signals kit_test)
    add_test(
      NAME signals_test
      COMMAND signals --no-carriage-return)
    set_tests_properties(
      signals_test
      PROPERTIES
      TIMEOUT "15")
  endif()
endif()
