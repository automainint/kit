cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 4 CACHE STRING "" FORCE)
endif()

option(KIT_ENABLE_TESTING "Enable testing" ON)
option(KIT_FORCE_ANSI_C "Force ANSI C" ${KIT_ENABLE_TESTING})

option(KIT_DISABLE_SYSTEM_MALLOC "Disable system memory allocator" OFF)
option(KIT_DISABLE_SYSTEM_THREADS "Disable system threads" OFF)

if(KIT_FORCE_ANSI_C)
  set(CMAKE_C_STANDARD 90)
  set(CMAKE_C_EXTENSIONS OFF)
endif()

project(
  kit
  VERSION 0.1.1
  DESCRIPTION "A collection of C libraries"
  LANGUAGES C)

add_library(kit STATIC)
add_library(kit::kit ALIAS kit)
target_include_directories(
  kit INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)
if(WIN32)
  target_link_libraries(kit PUBLIC Shlwapi)
endif()

include(CheckCSourceRuns)

check_c_source_runs(
  "
  __attribute__((weak)) int foo();
  int main() { return 0; }
  "
  KIT_HAVE_FUNC_ATTRIBUTE_WEAK
)

set(CMAKE_REQUIRED_LIBRARIES "pthread")
check_c_source_runs(
  "
  #define _GNU_SOURCE
  #include <pthread.h>
  int main() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    return 0;
  }
  "
  KIT_HAVE_PTHREAD_MUTEXATTR_SETTYPE
)
set(CMAKE_REQUIRED_LIBRARIES "")

check_c_source_runs(
  "
  #include <pthread.h>
  int main() {
    return 0;
  }
  "
  KIT_HAVE_PTHREAD
)

check_c_source_runs(
  "
  #include <Windows.h>
  int main() {
    return 0;
  }
  "
  KIT_HAVE_WINDOWS
)

check_c_source_runs(
  "
  #include <stdlib.h>
  int main() {
    void *p = malloc(10);
    free(p);
    return 0;
  }
  "
  KIT_HAVE_MALLOC
)

if(NOT KIT_HAVE_PTHREAD AND NOT KIT_HAVE_WINDOWS)
  set(KIT_DISABLE_SYSTEM_THREADS ON)
endif()

if(NOT KIT_HAVE_MALLOC)
  set(KIT_DISABLE_SYSTEM_MALLOC ON)
endif()

if(KIT_HAVE_FUNC_ATTRIBUTE_WEAK)
  target_compile_definitions(kit PUBLIC KIT_HAVE_FUNC_ATTRIBUTE_WEAK)
endif()

if(KIT_HAVE_PTHREAD_MUTEXATTR_SETTYPE)
  target_compile_definitions(kit PUBLIC KIT_HAVE_PTHREAD_MUTEXATTR_SETTYPE)
endif()

if(KIT_DISABLE_SYSTEM_MALLOC)
  target_compile_definitions(kit PUBLIC KIT_DISABLE_SYSTEM_MALLOC)
endif()

if(KIT_DISABLE_SYSTEM_THREADS)
  target_compile_definitions(kit PUBLIC KIT_DISABLE_SYSTEM_THREADS)
endif()

if(NOT KIT_DISABLE_SYSTEM_THREADS)
  find_package(Threads REQUIRED)
  target_link_libraries(kit PUBLIC Threads::Threads)
endif()

add_library(kit_test STATIC)
add_library(kit::kit_test ALIAS kit_test)
target_include_directories(
  kit_test INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)

enable_testing()

if(KIT_ENABLE_TESTING)
  add_executable(kit_test_suite)
  add_executable(kit::kit_test_suite ALIAS kit_test_suite)
  target_link_libraries(kit_test_suite PRIVATE kit kit_test Threads::Threads)

  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      kit PUBLIC
      -fsanitize=undefined,address
      --coverage -O0 -g)
    target_link_options(
      kit PUBLIC
      -fsanitize=undefined,address
      --coverage)
  endif()

  add_test(
    NAME kit_unittests
    COMMAND kit_test_suite --no-carriage-return)
  set_tests_properties(
    kit_unittests
    PROPERTIES
    TIMEOUT "120")
endif()

add_subdirectory(source)

include(GNUInstallDirs)

file(
  GLOB_RECURSE headers_
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/kit
  ${CMAKE_CURRENT_SOURCE_DIR}/source/kit/*.h)

foreach(path_ ${headers_})
  get_filename_component(dir_ "${path_}" DIRECTORY)

  install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/kit/${path_}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kit/${dir_})
endforeach()

unset(headers_)
unset(path_)
unset(dir_)

install(
  TARGETS kit kit_test
  EXPORT kit-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  EXPORT kit-targets
  FILE kit-targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(
  EXPORT kit-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/kit-targets.cmake"
  NAMESPACE ${PROJECT_NAME}::)

include(CMakePackageConfigHelpers)

string(TOLOWER ${PROJECT_NAME} project_lower_)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion)

install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

unset(project_lower_)
