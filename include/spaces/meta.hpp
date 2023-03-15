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
