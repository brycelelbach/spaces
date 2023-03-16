// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>
#include <spaces/optimization_hints.hpp>
#include <spaces/mdrange.hpp>
#include <spaces/optional.hpp>
#include <spaces/tuple.hpp>

#include <type_traits>
#include <utility>
#include <functional>

SPACES_BEGIN_NAMESPACE

template <index_type I, typename Space, typename F, typename OuterTuple>
constexpr void for_each_impl(Space&& space, F&& f, OuterTuple&& outer)
{
  if constexpr (I > 0) {
    SPACES_DEMAND_VECTORIZATION
    for (auto&& e: mdrange<I>(space, (OuterTuple&&)outer)) {
      invoke_o(
        [&] <typename T> (T&& t) {
          for_each_impl<I - 1>((Space&&)space, f, (T&&)t);
        }
      , std::forward<decltype(e)>(e)
      );
    }
  } else {
    SPACES_DEMAND_VECTORIZATION
    for (auto&& e: mdrange<I>((Space&&)space, (OuterTuple&&)outer)) {
      apply_or_invoke_o((F&&)f, std::forward<decltype(e)>(e));
    }
  }
}

template <typename Space, typename UnaryFunction>
constexpr void for_each(Space&& space, UnaryFunction&& f)
{
  if constexpr (mdrank<Space> > 0)
    for_each_impl<mdrank<Space> - 1>(
      (Space&&)space, (UnaryFunction&&)f, std::tuple<>{}
    );
}

SPACES_END_NAMESPACE

