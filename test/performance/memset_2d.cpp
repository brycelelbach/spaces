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

extern void memset_2d_reference(
  double* __restrict__ A
, spaces::index_type N
, spaces::index_type M
  ) noexcept;

void set_to_initial_state(spaces::mdspan<double, spaces::dextents<2>> A) {
  for (spaces::index_type j = 0; j != A.extent(1); ++j)
    for (spaces::index_type i = 0; i != A.extent(0); ++i)
      A(i, j) = A.mapping()(i, j);
}

void validate_state(spaces::mdspan<double, spaces::dextents<2>> A) {
  for (spaces::index_type j = 0; j != A.extent(1); ++j)
    for (spaces::index_type i = 0; i != A.extent(0); ++i)
      SPACES_TEST(A(i, j) == 0.0);
}

int main() {
  constexpr spaces::index_type N = 2048;
  constexpr spaces::index_type M = 1024;

  std::vector<double> data(N * M);
  spaces::mdspan A(data.data(), N, M);

  set_to_initial_state(A);
  memset_2d_reference(A.data_handle(), A.extent(0), A.extent(1));
  validate_state(A);

  return spaces::test_report_errors();
}

