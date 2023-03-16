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

