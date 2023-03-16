// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>

SPACES_BEGIN_NAMESPACE

// `overloaded(f0, f1, ...)` (where `f0`, `f1`, ... are lambdas or function
// objects) - Produces a function object that dispatches `operator()`
// invocations to `f0`, `f1`, ... by overload resolution, as if they were
// declared as overloads of the same function.
template <typename... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};

template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

SPACES_END_NAMESPACE

