cmake_minimum_required(VERSION 3.16)
set(NAME kit-integration-fetch-content)
project(${NAME} C)

include(FetchContent)
FetchContent_Declare(
  kit
  GIT_REPOSITORY https://github.com/automainint/kit.git
  GIT_TAG dev)
set(KIT_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(kit)

add_executable(${NAME})
target_sources(${NAME} PRIVATE main.c)
target_link_libraries(${NAME} PRIVATE kit::kit)

enable_testing()
add_test(NAME ${NAME}-run COMMAND ${NAME})
set_tests_properties(${NAME}-run PROPERTIES TIMEOUT "15")
