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

#include <array>

SPACES_BEGIN_NAMESPACE

struct storage_2d_range
{
  struct iterator
  {
  private:
    index_type location;
    std::array<index_type, 2> const extents;

  public:
    constexpr iterator(
      index_type location_
    , std::array<index_type, 2> extents_
    ) noexcept
      : location(location_), extents{extents_}
    {}

    constexpr iterator& operator++() noexcept
    {
      ++location;
      return *this;
    }

    constexpr std::array<index_type, 2> operator*() const noexcept
    {
      return std::array{location % extents[1], location / extents[1]};
    }

    constexpr bool operator!=(iterator const& other) noexcept
    {
      return location != other.location
          || extents[0] != other.extents[0]
          || extents[1] != other.extents[1];
    }
  };

private:
  iterator const first;
  iterator const last;

public:
  constexpr storage_2d_range(
      index_type ni
    , index_type nj
      ) noexcept
    : first(0, std::array<index_type, 2>{ni, nj})
    , last(ni * nj, std::array<index_type, 2>{ni, nj})
  {}

  constexpr iterator begin() const noexcept { return first; }

  constexpr iterator end() const { return last; }
};

SPACES_END_NAMESPACE

