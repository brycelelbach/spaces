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

extern void memset_plane_3d_reference(
  double* __restrict__ A
, spaces::index_type N
, spaces::index_type M
, spaces::index_type O
  ) noexcept;

extern void memset_plane_3d_for_each_filter(
  spaces::mdspan<double, spaces::dextents<3>, spaces::layout_left> A
  );

extern void memset_plane_3d_for_each_filter_o(
  spaces::mdspan<double, spaces::dextents<3>, spaces::layout_left> A
  );

void set_to_initial_state(
  spaces::mdspan<double, spaces::dextents<3>, spaces::layout_left> A
) {
  for (spaces::index_type k = 0; k != A.extent(2); ++k)
    for (spaces::index_type j = 0; j != A.extent(1); ++j)
      for (spaces::index_type i = 0; i != A.extent(0); ++i)
        A(i, j, k) = A.mapping()(i, j, k);
}

void validate_state(
  spaces::mdspan<double, spaces::dextents<3>, spaces::layout_left> A
) {
  for (spaces::index_type k = 0; k != A.extent(2); ++k)
    for (spaces::index_type j = 0; j != A.extent(1); ++j)
      for (spaces::index_type i = 0; i != A.extent(0); ++i) {
        if (i == j) SPACES_TEST_EQ(A(i, j, k), 0.0);
        else SPACES_TEST_EQ(A(i, j, k), A.mapping()(i, j, k));
      }
}

int main() {
  constexpr spaces::index_type N = 64;
  constexpr spaces::index_type M = 64;
  constexpr spaces::index_type O = 64;

  std::unique_ptr<double[]> data(
    reinterpret_cast<double*>(std::aligned_alloc(32, N * M * O * sizeof(double)))
  );
  spaces::mdspan A(
    data.get(), spaces::layout_left::mapping{spaces::extents{N, M, O}}
  );

  set_to_initial_state(A);
  memset_plane_3d_reference(
    A.data_handle(), A.extent(0), A.extent(1), A.extent(2)
  );
  validate_state(A);

  set_to_initial_state(A);
  memset_plane_3d_for_each_filter(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_plane_3d_for_each_filter_o(A);
  validate_state(A);

  return spaces::test_report_errors();
}

