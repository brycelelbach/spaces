// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <spaces/config.hpp>
#include <spaces/optimization_hints.hpp>
#include <spaces/mdspan.hpp>
#include <spaces/test.hpp>

#include <cassert>
#include <cstdlib>
#include <memory>
#include <functional>

extern void memset_2d_reference(
  double* __restrict__ A
, spaces::index_type N
, spaces::index_type M
  ) noexcept;

extern void memset_2d_mdspan_raw_loop(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_index_range_based_for_loop(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_index_range_based_for_loop(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_index_forward_iterators(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_index_random_access_iterators(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_index_known_distance_iterators(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_storage_range_based_for_loop(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_cartesian_product_iota(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  ) noexcept;

extern void memset_2d_index_generator(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  );

extern void memset_2d_space_based_for_each(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  );

void set_to_initial_state(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
) {
  for (spaces::index_type j = 0; j != A.extent(1); ++j)
    for (spaces::index_type i = 0; i != A.extent(0); ++i)
      A(i, j) = A.mapping()(i, j);
}

void validate_state(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
) {
  for (spaces::index_type j = 0; j != A.extent(1); ++j)
    for (spaces::index_type i = 0; i != A.extent(0); ++i)
      SPACES_TEST_EQ(A(i, j), 0.0);
}

int main() {
  constexpr spaces::index_type N = 128;
  constexpr spaces::index_type M = 128;

  std::unique_ptr<double[]> data(
    reinterpret_cast<double*>(std::aligned_alloc(32, N * M * sizeof(double)))
  );
  spaces::mdspan A(data.get(), spaces::layout_left::mapping{spaces::extents{N, M}});

  set_to_initial_state(A);
  memset_2d_reference(A.data_handle(), A.extent(0), A.extent(1));
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_mdspan_raw_loop(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_index_range_based_for_loop(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_index_forward_iterators(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_index_random_access_iterators(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_index_known_distance_iterators(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_storage_range_based_for_loop(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_cartesian_product_iota(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_index_generator(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_2d_space_based_for_each(A);
  validate_state(A);

  return spaces::test_report_errors();
}

