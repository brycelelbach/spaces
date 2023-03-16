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
#include <functional>

extern void memset_diagonal_2d_reference(
  double* __restrict__ A
, spaces::index_type N
, spaces::index_type M
  ) noexcept;

extern void memset_diagonal_2d_for_each_filter(
  spaces::mdspan<double, spaces::dextents<2>> A
  );

extern void memset_diagonal_2d_for_each_filter_o(
  spaces::mdspan<double, spaces::dextents<2>> A
  );

void set_to_initial_state(spaces::mdspan<double, spaces::dextents<2>> A) {
  for (spaces::index_type j = 0; j != A.extent(1); ++j)
    for (spaces::index_type i = 0; i != A.extent(0); ++i)
      A(i, j) = A.mapping()(i, j);
}

void validate_state(spaces::mdspan<double, spaces::dextents<2>> A) {
  for (spaces::index_type j = 0; j != A.extent(1); ++j)
    for (spaces::index_type i = 0; i != A.extent(0); ++i) {
      if (i == j) SPACES_TEST_EQ(A(i, j), 0.0);
      else SPACES_TEST_EQ(A(i, j), A.mapping()(i, j));
    }
}

int main() {
  constexpr spaces::index_type N = 128;
  constexpr spaces::index_type M = 128;

  std::vector<double> data(N * M);
  spaces::mdspan A(data.data(), N, M);

  set_to_initial_state(A);
  memset_diagonal_2d_reference(A.data_handle(), A.extent(0), A.extent(1));
  validate_state(A);

  set_to_initial_state(A);
  memset_diagonal_2d_for_each_filter(A);
  validate_state(A);

  set_to_initial_state(A);
  memset_diagonal_2d_for_each_filter_o(A);
  validate_state(A);

  return spaces::test_report_errors();
}

