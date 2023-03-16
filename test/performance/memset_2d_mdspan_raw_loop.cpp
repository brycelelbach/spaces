// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <spaces/config.hpp>
#include <spaces/mdspan.hpp>

void memset_2d_mdspan_raw_loop(
  spaces::mdspan<double, spaces::dextents<2>> A
  ) noexcept
{
  for (spaces::index_type j = 0; j != A.extent(1); ++j)
    for (spaces::index_type i = 0; i != A.extent(0); ++i)
      A(i, j) = 0.0;
}

