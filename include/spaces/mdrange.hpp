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

template <index_type N, typename Space, typename OuterTuple>
constexpr auto mdrange(Space&& space, OuterTuple&& outer);

template <typename Space>
struct mdrank_t : std::rank<Space> {};

template <typename Space>
inline constexpr index_type mdrank = mdrank_t<std::remove_cvref_t<Space>>::value;

SPACES_END_NAMESPACE
