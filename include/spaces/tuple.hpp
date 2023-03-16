// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>

#include <functional>
#include <tuple>

SPACES_BEGIN_NAMESPACE

template <index_type I, typename Tuple, typename T, typename F>
auto tuple_reduce_impl(Tuple&& tuple, T&& init, F&& f)
{
  if constexpr (I > 0) {
    return std::invoke(
      f
    , std::get<I>(tuple)
    , tuple_reduce_impl<I - 1>((Tuple&&)tuple, (T&&)init, (F&&)f)
    );
  } else {
    return std::invoke(
      (F&&)f
    , (T&&)init
    , std::get<I>((Tuple&&)tuple)
    );
  }
}

// `tuple_reduce(tuple, init, f) == f(..., f(tuple[1], f(tuple[0], init))))`.
template <typename Tuple, typename T, typename F>
auto tuple_reduce(Tuple&& tuple, T&& init, F&& f)
{
  return tuple_reduce_impl<std::tuple_size_v<Tuple> - 1>(
    (Tuple&&)tuple, (T&&)init, (F&&)f
  );
}

SPACES_END_NAMESPACE

