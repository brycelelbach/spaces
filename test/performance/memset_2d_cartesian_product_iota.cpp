// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <spaces/config.hpp>
#include <spaces/mdspan.hpp>
#include <spaces/cartesian_product.hpp>

#include <ranges>

void memset_2d_cartesian_product_iota(
  spaces::mdspan<double, spaces::dextents<2>> A
  ) noexcept
{
  auto indices =
    spaces::cartesian_product(
      std::views::iota(0LU, A.extent(0))
    , std::views::iota(0LU, A.extent(1))
    );

  for (auto [i, j] : indices)
    A(i, j) = 0.0;
}

