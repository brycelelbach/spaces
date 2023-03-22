// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>
#include <spaces/views.hpp>

SPACES_BEGIN_NAMESPACE

template <typename Range>
auto optimize_range(Range&& rng)
{
  return (Range&&)rng;
}

template <typename Range, typename F>
auto optimize_range(std::views::filter_view<Range, F> rng)
{
  return filter_o(optimize_range(std::move(rng).base()), std::move(rng).pred());
}

SPACES_END_NAMESPACE

