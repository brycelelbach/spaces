// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/preprocessor.hpp>

#include <cstddef>

#define SPACES_ABI_VERSION 0

#define SPACES_BEGIN_NAMESPACE                                                \
  namespace spaces { inline namespace SPACES_CAT(__v, SPACES_ABI_VERSION) {   \
  /**/

#define SPACES_END_NAMESPACE }}

SPACES_BEGIN_NAMESPACE

using index_type = std::size_t;

SPACES_END_NAMESPACE

