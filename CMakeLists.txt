# Copyright (c) 2015-2017 Bryce Adelstein Lelbach
# Copyright (c) 2017-2023 NVIDIA Corporation
#
# Distributed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.25)

project(spaces LANGUAGES CXX)

enable_testing()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui.
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(SPACES_MARCH "native" CACHE STRING "Microarchitecture target (e.g. -march=)." FORCE)

add_library(spaces INTERFACE)

target_include_directories(spaces INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(spaces INTERFACE cxx_std_20)

target_compile_options(spaces INTERFACE
  $<$<AND:$<CONFIG:Release,RelWithDebInfo>,$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel,NVHPC>>:
    -O3 -fstrict-aliasing
  >
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel>>:
    -g0 -DNDEBUG
    -fno-exceptions -fno-rtti
    -fomit-frame-pointer -fno-asynchronous-unwind-tables -fno-unwind-tables
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel,NVHPC>:
    -Wall
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,Intel>:
    -Wno-pass-failed
    -Wno-assume
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel>:
    -ftemplate-backtrace-limit=0
    -ferror-limit=0
    -march=${SPACES_MARCH}
  >
)

add_library(spaces_optimization_report INTERFACE)

target_compile_options(spaces_optimization_report INTERFACE
  $<$<CXX_COMPILER_ID:Clang,AppleClang,Intel>:
    -Rpass=loop-vectorize
    -Rpass-missed=loop-vectorize
    -Rpass-analysis=loop-vectorize
    -fsave-optimization-record
  >
)

add_executable(test.performance.memset_2d
  test/performance/memset_2d.cpp
  test/performance/memset_2d_reference.cpp
)
add_test(
  NAME test.performance.memset_2d
  COMMAND test.performance.memset_2d
)
target_link_libraries(test.performance.memset_2d PRIVATE spaces)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(SPACES_OPTIMIZATION_REPORT_TARGETS
    memset_2d_reference
  )
  foreach(SPACES_TARGET ${SPACES_OPTIMIZATION_REPORT_TARGETS})
    add_library(${SPACES_TARGET}_optimization_report STATIC test/performance/${SPACES_TARGET}.cpp)
    target_link_libraries(${SPACES_TARGET}_optimization_report
      PRIVATE spaces spaces_optimization_report)
    set(SPACES_THIS_BINARY_DIR
      ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${SPACES_TARGET}_optimization_report.dir)
    file(WRITE ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash
      "#! /usr/bin/env bash\n"
      "$@ > ${SPACES_THIS_BINARY_DIR}/compiler_output 2>&1\n")
    file(CHMOD ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash
      FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
    set_target_properties(${SPACES_TARGET}_optimization_report PROPERTIES
      CXX_COMPILER_LAUNCHER ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash)
  endforeach()
endif()

