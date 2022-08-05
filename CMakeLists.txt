cmake_minimum_required(VERSION 3.16)

option(KIT_ENABLE_LIBRARY "Enable library" ON)
option(KIT_ENABLE_TESTING "Enable testing" ON)

set(KIT_PROJECT kit)
set(KIT_LIBRARY kit)
set(KIT_TEST_LIBRARY kit_test)
set(KIT_TEST_SUITE kit_test_suite)
set(KIT_TARGETS kit-targets)
set(KIT_UNITTESTS kit-unittests)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 4)
endif()

project(
  ${KIT_PROJECT}
  VERSION 0.1.1
  DESCRIPTION "Kit"
  LANGUAGES C)

if(KIT_ENABLE_LIBRARY OR KIT_ENABLE_TESTING)
  add_library(${KIT_LIBRARY} STATIC)
  add_library(${KIT_PROJECT}::${KIT_LIBRARY} ALIAS ${KIT_LIBRARY})
  target_include_directories(
    ${KIT_LIBRARY} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)
  target_compile_features(${KIT_LIBRARY} PUBLIC c_std_11)

  add_library(${KIT_TEST_LIBRARY} STATIC)
  add_library(${KIT_PROJECT}::${KIT_TEST_LIBRARY} ALIAS ${KIT_TEST_LIBRARY})
  target_include_directories(
    ${KIT_TEST_LIBRARY} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)
  target_compile_features(${KIT_TEST_LIBRARY} PUBLIC c_std_11)
endif()

enable_testing()

if(KIT_ENABLE_TESTING)
  add_executable(${KIT_TEST_SUITE})
  add_executable(${KIT_PROJECT}::${KIT_TEST_SUITE} ALIAS ${KIT_TEST_SUITE})
  target_compile_features(${KIT_TEST_SUITE} PRIVATE c_std_11)
  target_link_libraries(${KIT_TEST_SUITE} PRIVATE ${KIT_LIBRARY} ${KIT_TEST_LIBRARY})

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${KIT_LIBRARY} PUBLIC
      -fsanitize=undefined,address
      --coverage -O0 -g)
    target_link_options(
      ${KIT_LIBRARY} PUBLIC
      -fsanitize=undefined,address
      --coverage)
  endif()

  add_test(
    NAME ${KIT_UNITTESTS}
    COMMAND ${KIT_TEST_SUITE})

  set_tests_properties(
    ${KIT_UNITTESTS}
    PROPERTIES
    TIMEOUT "30")
endif()

add_subdirectory(source)

if(KIT_ENABLE_LIBRARY)
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
    TARGETS ${KIT_LIBRARY} ${KIT_TEST_LIBRARY}
    EXPORT ${KIT_TARGETS}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(
    EXPORT ${KIT_TARGETS}
    FILE ${KIT_TARGETS}.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

  export(
    EXPORT ${KIT_TARGETS}
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${KIT_TARGETS}.cmake"
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
endif()
