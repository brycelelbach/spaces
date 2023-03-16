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

template <index_type N, typename Space, typename OuterTuple>
constexpr auto mdrange(Space&& space, OuterTuple&& outer);

template <typename Space>
struct mdrank_t : std::rank<Space> {};

template <typename Space>
inline constexpr index_type mdrank = mdrank_t<std::remove_cvref_t<Space>>::value;

SPACES_END_NAMESPACE
