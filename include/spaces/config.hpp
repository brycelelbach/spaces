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

#include <cstddef>

#include <spaces/preprocessor.hpp>

#define SPACES_ABI_VERSION 0

#define SPACES_BEGIN_NAMESPACE                                                \
  namespace spaces { inline namespace SPACES_CAT(__v, SPACES_ABI_VERSION) {   \
  /**/

#define SPACES_END_NAMESPACE }}

SPACES_BEGIN_NAMESPACE

using index_type = std::ptrdiff_t;

SPACES_END_NAMESPACE

