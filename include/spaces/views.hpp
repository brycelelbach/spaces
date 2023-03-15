// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Licensed under the Apache License Version 2.0 with LLVM Exceptions
// (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
//
//   https://llvm.org/LICENSE.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <spaces/config.hpp>
#include <spaces/optional.hpp>
#include <spaces/overloaded.hpp>

#include <ranges>

SPACES_BEGIN_NAMESPACE

// `transform_o(rng, f)` or `rng | transform_o(f)` returns a range that, for
// each element `e` of `rng`, contains a corresponding element that is:
// * `nullopt` if `e` is an empty `optional`.
// * An `optional` containing the result of invoking `f(*e)` if `e` is a
//   non-empty `optional`.
// * An `optional` containing the result of invoking `f(e)` if `e` isn't an
//   `optional`.
inline constexpr auto transform_o =
overloaded(
  [] <typename Range, typename F> (Range&& rng, F&& f)
  {
    return std::views::transform((Range&&)rng, invoke_o((F&&)f));
  },
  [] <typename F> (F&& f)
  {
    return std::views::transform(invoke_o((F&&)f));
  }
);

inline constexpr auto filter_o = [] <typename F> (F&& f)
{
  return std::views::transform(
    [&] <typename T> (T&& t) -> add_optional<T> {
      if constexpr (specialization_of<T, std::optional>) {
        if (t.has_value() && std::invoke((F&&)f, t.value())) return ((T&&)t).value();
        else return std::nullopt;
      } else {
        if (std::invoke((F&&)f, t)) return (T&&)t;
        else return std::nullopt;
      }
    }
  );
};

SPACES_END_NAMESPACE
