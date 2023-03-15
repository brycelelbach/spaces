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
#include <spaces/meta.hpp>

#include <type_traits>
#include <functional>
#include <optional>

SPACES_BEGIN_NAMESPACE

template <typename T>
struct add_optional_impl { using type = std::optional<T>; };

template <typename T>
struct add_optional_impl<std::optional<T>> { using type = std::optional<T>; };

template <typename T>
using add_optional = add_optional_impl<T>::type;

template <typename T>
struct remove_optional_impl { using type = T; };

template <typename T>
struct remove_optional_impl<std::optional<T>> { using type = T; };

template <typename T>
using remove_optional = remove_optional_impl<T>::type;

// `invoke_o(f, t)`: Invokes `f` with `t` (if `t` isn't an `optional`) or
// `t.value()` (if `t` is an `optional` and non-empty), and wraps the result
// in an `optional`. If `t` is an empty optional, `f` is not invoked and an
// empty optional is returned.
inline constexpr auto invoke_o =
  [] <typename F, typename T> (F&& f, T&& t)
  -> std::conditional_t<
       std::same_as<std::invoke_result_t<F&&, remove_optional<T>>, void>,
       std::nullopt_t,
       add_optional<std::invoke_result_t<F&&, remove_optional<T>>>
     >
{
  if constexpr (std::same_as<std::invoke_result_t<F&&, remove_optional<T>>, void>) {
    if constexpr (specialization_of<T, std::optional>) {
      if (t.has_value()) std::invoke((F&&)f, ((T&&)t).value());
    } else {
      std::invoke((F&&)f, (T&&)t);
    }
    return std::nullopt;
  } else {
    if constexpr (specialization_of<T, std::optional>) {
      if (t.has_value()) return std::invoke((F&&)f, ((T&&)t).value());
      else return std::nullopt;
    } else {
      return std::invoke((F&&)f, (T&&)t);
    }
  }
};

SPACES_END_NAMESPACE

