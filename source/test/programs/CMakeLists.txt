if(KIT_ENABLE_TESTING)
  add_executable(too_many_tests too_many_tests.c)
  target_link_libraries(too_many_tests ${KIT_TEST_LIBRARY})
  add_test(
    NAME too_many_tests_test
    COMMAND too_many_tests)
  set_tests_properties(
    too_many_tests_test
    PROPERTIES
    TIMEOUT "30")

  add_executable(too_many_assertions too_many_assertions.c)
  target_link_libraries(too_many_assertions ${KIT_TEST_LIBRARY})
  add_test(
    NAME too_many_assertions_test
    COMMAND too_many_assertions)
  set_tests_properties(
    too_many_assertions_test
    PROPERTIES
    TIMEOUT "30")
endif()
