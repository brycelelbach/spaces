// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

// SPACES_CAT(args, ...) - Concatenate the tokens together.
#define SPACES_CAT_IMPL(args, ...) args##__VA_ARGS__
#define SPACES_CAT(args, ...) SPACES_CAT_IMPL(args, __VA_ARGS__)

// SPACES_STRINGIZE(expr) - Return `expr` as a string literal.
#define SPACES_STRINGIZE_IMPL(expr) #expr
#define SPACES_STRINGIZE(expr) SPACES_STRINGIZE_IMPL(expr)

// SPACES_PP_PRAGMA(args) - Emits the pragma `args`.
#define SPACES_PRAGMA(args) _Pragma(SPACES_STRINGIZE(args))

// SPACES_CURRENT_FUNCTION - Expands to a string literal naming the current
// function.
#if defined(__PRETTY_FUNCTION__)
  #define SPACES_CURRENT_FUNCTION __PRETTY_FUNCTION__
#else
  #define SPACES_CURRENT_FUNCTION __func__
#endif

