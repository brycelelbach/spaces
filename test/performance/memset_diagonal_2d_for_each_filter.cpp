// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <spaces/config.hpp>
#include <spaces/mdspan.hpp>
#include <spaces/cursor.hpp>
#include <spaces/for_each.hpp>

#include <ranges>

void memset_diagonal_2d_for_each_filter(
  spaces::mdspan<double, spaces::dextents<2>> A
  ) noexcept
{
  spaces::for_each(
    spaces::cursor<2>(A.extent(0), A.extent(1))
  | std::views::filter([] (auto idx) { auto [i, j] = idx; return i == j; })
  , [=] (auto i, auto j) { A(i, j) = 0.0; }
  );
}

