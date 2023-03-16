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
#include <spaces/views.hpp>

void memset_plane_3d_for_each_filter_o(
  spaces::mdspan<double, spaces::dextents<3>, spaces::layout_left> A
  ) noexcept
{
  spaces::for_each(
    spaces::cursor<3>(A.extent(0), A.extent(1), A.extent(2))
  | spaces::filter_o([] (auto i, auto j, auto k) { return i == j; })
  , [=] (auto i, auto j, auto k) { A(i, j, k) = 0.0; }
  );
}

