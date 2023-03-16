// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <spaces/config.hpp>
#include <spaces/optimization_hints.hpp>

void memset_diagonal_2d_reference(
  double* __restrict__ A
, spaces::index_type N
, spaces::index_type M
  ) noexcept
{
  SPACES_ASSUME_ALIGNED(A, 32);
  SPACES_ASSUME((N % 32) == 0);
  SPACES_ASSUME((M % 32) == 0);

  SPACES_DEMAND_VECTORIZATION
  for (spaces::index_type j = 0; j != M; ++j)
    SPACES_DEMAND_VECTORIZATION
    for (spaces::index_type i = 0; i != N; ++i)
      if (i == j) A[i + j * N] = 0.0;
}

