cmake_minimum_required(VERSION 3.22)
project(bhcc LANGUAGES C)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_COMPILER clang)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BHCC_SRC_DIR "${CMAKE_SOURCE_DIR}/bhcc")
file(GLOB SOURCES "${BHCC_SRC_DIR}/*.c")
include_directories(bhcc)
add_executable(bhcc ${SOURCES})

# Core executable
add_custom_target(
  run
  COMMAND bhcc ../test_file.c
  DEPENDS bhcc)

# Testing targets
set(BHCC_TEST_DIR "${CMAKE_SOURCE_DIR}/test")
file(GLOB TEST_SOURCES "${BHCC_TEST_DIR}/*.c")
add_executable(tests ${TEST_SOURCES})
add_custom_target(
  test
  COMMAND tests
  DEPENDS tests)
