cmake_minimum_required(VERSION 3.22)
project(bhcc LANGUAGES C)

set(CMAKE_BUILD_TYPE "Debug")

set(BHCC_SRC_DIR "${CMAKE_SOURCE_DIR}/bhcc")
file(GLOB SOURCES "${BHCC_SRC_DIR}/*.c")
include_directories(bhcc)
add_executable(bhcc ${SOURCES})

add_custom_target(
  test
  COMMAND bhcc ../test_file.c
  DEPENDS bhcc)
