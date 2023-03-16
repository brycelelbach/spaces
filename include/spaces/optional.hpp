// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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

// `invoke_o(f, t)` - Invokes `f` with `t` (if `t` isn't an `optional`) or
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
    // `f` returns `void`.
    if constexpr (specialization_of<T, std::optional>) {
      // `t` is an `optional`, so we need to invoke `f` only if it's got a value.
      if (t.has_value()) std::invoke((F&&)f, ((T&&)t).value());
    } else {
      // `t` is a value, so always invoke `f`.
      std::invoke((F&&)f, (T&&)t);
    }
    return std::nullopt;
  } else {
    // `F` returns non-`void`.
    if constexpr (specialization_of<T, std::optional>) {
      // `t` is an `optional`, so we need to invoke `f` only if it's got a value.
      if (t.has_value()) return std::invoke((F&&)f, ((T&&)t).value());
      else return std::nullopt;
    } else {
      // `t` is a value, so always invoke `f`.
      return std::invoke((F&&)f, (T&&)t);
    }
  }
};

SPACES_END_NAMESPACE

