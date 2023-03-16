# Copyright (c) 2015-2017 Bryce Adelstein Lelbach
# Copyright (c) 2017-2023 NVIDIA Corporation
#
# Distributed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(SPACES_TEST_PERFORMANCE_MEMSET_2D_SOURCES
  memset_2d_reference.cpp
  memset_2d_mdspan_raw_loop.cpp
  memset_2d_index_range_based_for_loop.cpp
  memset_2d_index_forward_iterators.cpp
  memset_2d_index_random_access_iterators.cpp
  memset_2d_index_known_distance_iterators.cpp
  memset_2d_storage_range_based_for_loop.cpp
  memset_2d_cartesian_product_iota.cpp
  memset_2d_index_generator.cpp
  memset_2d_space_based_for_each.cpp
)
add_executable(test.performance.memset_2d
  memset_2d.cpp
  ${SPACES_TEST_PERFORMANCE_MEMSET_2D_SOURCES}
)
add_test(
  NAME test.performance.memset_2d
  COMMAND test.performance.memset_2d
)
target_link_libraries(test.performance.memset_2d PRIVATE spaces)

set(SPACES_TEST_PERFORMANCE_MEMSET_DIAGONAL_2D_SOURCES
  memset_diagonal_2d_reference.cpp
  memset_diagonal_2d_for_each_filter.cpp
  memset_diagonal_2d_for_each_filter_o.cpp
)
add_executable(test.performance.memset_diagonal_2d
  memset_diagonal_2d.cpp
  ${SPACES_TEST_PERFORMANCE_MEMSET_DIAGONAL_2D_SOURCES}
)
add_test(
  NAME test.performance.memset_diagonal_2d
  COMMAND test.performance.memset_diagonal_2d
)
target_link_libraries(test.performance.memset_diagonal_2d PRIVATE spaces)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(SPACES_OPTIMIZATION_REPORT_SOURCES
    ${SPACES_TEST_PERFORMANCE_MEMSET_2D_SOURCES}
    ${SPACES_TEST_PERFORMANCE_MEMSET_DIAGONAL_2D_SOURCES}
  )
  foreach(SPACES_SOURCE ${SPACES_OPTIMIZATION_REPORT_SOURCES})
    get_filename_component(SPACES_TARGET ${SPACES_SOURCE} NAME_WLE)
    add_library(${SPACES_TARGET}_optimization_report STATIC ${SPACES_TARGET}.cpp)
    target_link_libraries(${SPACES_TARGET}_optimization_report
      PRIVATE spaces spaces_optimization_report)
    set(SPACES_THIS_BINARY_DIR
      ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${SPACES_TARGET}_optimization_report.dir)
    file(WRITE ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash
      "#! /usr/bin/env bash\n"
      "$@ > ${CMAKE_BINARY_DIR}/${SPACES_TARGET}.optimization_report 2>&1\n")
    file(CHMOD ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash
      FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
    set_target_properties(${SPACES_TARGET}_optimization_report PROPERTIES
      CXX_COMPILER_LAUNCHER ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash)
  endforeach()
endif()

