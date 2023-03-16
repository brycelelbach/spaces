// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>

#include <type_traits>

SPACES_BEGIN_NAMESPACE

template <auto I>
using constant = std::integral_constant<decltype(I), I>;

template <typename T, template <typename...> class Primary>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Primary, typename... Args>
struct is_specialization_of<Primary<Args...>, Primary> : std::true_type {};

template <typename T, template <typename...> class Primary>
concept specialization_of
  = is_specialization_of<std::remove_cvref_t<T>, Primary>::value;

SPACES_END_NAMESPACE
