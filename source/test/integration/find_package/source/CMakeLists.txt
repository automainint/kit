cmake_minimum_required(VERSION 3.16)
set(NAME kit-integration-find-package)
project(${NAME} C)

find_package(kit REQUIRED)

add_executable(${NAME})
target_sources(${NAME} PRIVATE main.c)
target_link_libraries(${NAME} PRIVATE kit::kit)
install(TARGETS ${NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
