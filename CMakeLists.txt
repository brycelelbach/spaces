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

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

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
    -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel,NVHPC>:
    -Wall
    -fno-builtin
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
    -Rpass=loop
    -Rpass-missed=loop
    -Rpass-analysis=loop
    -fsave-optimization-record
  >
)

add_subdirectory(test/performance)

