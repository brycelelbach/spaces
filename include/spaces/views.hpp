// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>
#include <spaces/optional.hpp>
#include <spaces/overloaded.hpp>

#include <ranges>

SPACES_BEGIN_NAMESPACE

// `transform_o(rng, f)` or `rng | transform_o(f)` returns a range that, for
// each element `e` of `rng`, contains a corresponding element that is:
// * `nullopt` if `e` is an empty `optional`.
// * An `optional` containing the result of `apply_or_invoke(f, *e)` if `e` is a
//   non-empty `optional`.
// * An `optional` containing the result of invoking `apply_or_invoke(f, e)` if
//   `e` isn't an `optional`.
inline constexpr auto transform_o =
overloaded(
  [] <typename Range, typename F> (Range&& rng, F&& f)
  {
    return std::views::transform(
      (Range&&)rng
    , [f = (F&&)f] <typename T> (T&& t)
      {
        return apply_or_invoke_o(f, (T&&)t);
      }
    );
  },
  [] <typename F> (F&& f)
  {
    return std::views::transform(
      [f = (F&&)f] <typename T> (T&& t)
      {
        return apply_or_invoke_o(f, (T&&)t);
      }
    );
  }
);

inline constexpr auto filter_o =
overloaded(
  [] <typename Range, typename F> (Range&& rng, F&& f)
  {
    return std::views::transform(
      (Range&&)rng
    , [f = (F&&)f] <typename T> (T&& t) -> add_optional<T>
      {
        if (apply_or_invoke(f, t)) return (T&&)t;
        else return std::nullopt;
      }
    );
  }
, [] <typename F> (F&& f)
  {
    return std::views::transform(
      [f = (F&&)f] <typename T> (T&& t) -> add_optional<T>
      {
        if (apply_or_invoke(f, t)) return (T&&)t;
        else return std::nullopt;
      }
    );
  }
);

SPACES_END_NAMESPACE

