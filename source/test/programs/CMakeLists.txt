if(KIT_ENABLE_TESTING)
  add_executable(too_many_tests too_many_tests.c)
  target_link_libraries(too_many_tests ${KIT_TEST_LIBRARY})
  add_test(
    NAME too_many_tests_test
    COMMAND too_many_tests --quiet)
  set_tests_properties(
    too_many_tests_test
    PROPERTIES
    TIMEOUT "30")

  add_executable(too_many_assertions too_many_assertions.c)
  target_link_libraries(too_many_assertions ${KIT_TEST_LIBRARY})
  add_test(
    NAME too_many_assertions_test
    COMMAND too_many_assertions --quiet)
  set_tests_properties(
    too_many_assertions_test
    PROPERTIES
    TIMEOUT "30")

  add_executable(cpp_example cpp_example.cpp)
  target_link_libraries(cpp_example ${KIT_TEST_LIBRARY})
  add_test(
    NAME cpp_example_test
    COMMAND cpp_example --quiet)
  set_tests_properties(
    cpp_example_test
    PROPERTIES
    TIMEOUT "30")
endif()
