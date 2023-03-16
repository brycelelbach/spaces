// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <spaces/config.hpp>
#include <spaces/mdspan.hpp>
#include <spaces/index_generator.hpp>

void memset_2d_index_generator(
  spaces::mdspan<double, spaces::dextents<2>, spaces::layout_left> A
  )
{
  for (auto pos : spaces::generate_indices(A.extent(0), A.extent(1)))
    A(pos[0], pos[1]) = 0.0;
}
