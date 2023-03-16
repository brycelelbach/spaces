// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <spaces/config.hpp>
#include <spaces/mdspan.hpp>
#include <spaces/index_md_range.hpp>

void memset_2d_index_known_distance_iterators(
  spaces::mdspan<double, spaces::dextents<2>> A
  ) noexcept
{
  auto&& r   = spaces::index_2d_range(A.extent(0), A.extent(1));
  auto first = r.begin();

  spaces::index_type const dist = A.size();
  for (spaces::index_type d = 0; d < dist; ++d, ++first)
  {
    auto pos = *first;
    A(pos[0], pos[1]) = 0.0;
  }
}

