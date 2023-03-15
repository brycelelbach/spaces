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

#include <array>

#include <spaces/config.hpp>

SPACES_BEGIN_NAMESPACE

template <index_type N>
struct position
{
    constexpr position() noexcept : idxs{{}} {}

    template <typename... Indices>
    constexpr explicit position(Indices... idxs_) noexcept
      : idxs{{static_cast<index_type>(idxs_)...}}
    {
        static_assert(
            sizeof...(Indices) == N
          , "Insufficient index parameters passed to constructor."
        );
    }

    constexpr position(position const&)            noexcept = default;
    constexpr position(position&&)                 noexcept = default;
    constexpr position& operator=(position const&) noexcept = default;
    constexpr position& operator=(position&&)      noexcept = default;

    constexpr index_type& operator[](index_type i) noexcept
    {
        return idxs[i];
    }
    constexpr index_type const& operator[](index_type i) const noexcept
    {
        return idxs[i];
    }

  private:
    std::array<index_type, N> idxs;
};

SPACES_END_NAMESPACE

