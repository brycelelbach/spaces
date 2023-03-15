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

struct index_2d_range
{
  struct sentinel
  {
    index_type const extent1;

    constexpr sentinel(index_type extent1_) noexcept : extent1(extent1_) {}
  };

  struct iterator
  {
    constexpr iterator(
      std::array<index_type, 2> indices_
    , std::array<index_type, 2> extents_
    ) noexcept
      : indices(indices_)
      , extents{extents_}
    {}

    constexpr iterator& operator++() noexcept
    {
      SPACES_ASSUME(indices[0] >= 0);
      SPACES_ASSUME(extents[0] >  0);

      ++indices[0];                 // Inner loop iteration-expression.

      if (extents[0] == indices[0]) // Inner loop condition.
      {
        ++indices[1];               // Outer loop increment.
        indices[0] = 0;             // Inner loop init-statement.
      }

      return *this;
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    friend constexpr index_type
    operator-(iterator const& l, iterator const& r) noexcept
    {
      SPACES_ASSUME(l.indices[0] >= 0);
      SPACES_ASSUME(r.indices[0] >= 0);
      SPACES_ASSUME(l.indices[1] >= 0);
      SPACES_ASSUME(r.indices[1] >= 0);
      SPACES_ASSUME(l.extents[0] >  0);
      SPACES_ASSUME(l.extents[1] >  0);

      return (l.indices[1] - r.indices[1])
          * (l.extents[0]) - (l.indices[0] - r.indices[0]);
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    friend constexpr index_type
    operator-(iterator const& l, sentinel r) noexcept
    {
      SPACES_ASSUME(l.indices[0] >= 0);
      SPACES_ASSUME(l.indices[1] >= 0);
      SPACES_ASSUME(l.extents[0] >  0);
      SPACES_ASSUME(r.extent1    >  0);

      return (l.indices[1] - r.extent1)
           * (l.extents[0]) - (l.indices[0] - l.extents[0]);
    }
    friend constexpr index_type operator-(
        sentinel r
      , iterator const& l
        ) noexcept
    {
      SPACES_ASSUME(l.indices[0] >= 0);
      SPACES_ASSUME(l.indices[1] >= 0);
      SPACES_ASSUME(l.extents[0] >  0);
      SPACES_ASSUME(r.extent1    >  0);

      return (r.extent1 - l.indices[1])
           * (l.extents[0]) - (l.extents[0] - l.indices[0]);
    }

    friend constexpr iterator operator+(iterator it, index_type d) noexcept
    {
      return iterator(
        {it.indices[0] + it.extents[0] % d, it.indices[1] + d / it.extents[0]},
        {it.extents[0], it.extents[1]}
      );
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    constexpr iterator& operator+=(index_type d) noexcept
    {
      indices[0] += extents[0] % d;
      indices[1] += indices[1] + d / extents[0];
      return *this;
    }

    constexpr std::array<index_type, 2> operator[](index_type d) const noexcept
    {
      return *(*this + d);
    }

    constexpr std::array<index_type, 2> operator*() const noexcept
    {
      return indices;
    }

    friend constexpr bool
    operator==(iterator const& l, iterator const& r) noexcept
    {
      SPACES_ASSUME(l.indices[0] >= 0);
      SPACES_ASSUME(r.indices[0] >= 0);
      SPACES_ASSUME(l.indices[1] >= 0);
      SPACES_ASSUME(r.indices[1] >= 0);
      SPACES_ASSUME(l.extents[0] >  0);
      SPACES_ASSUME(r.extents[0] >  0);
      SPACES_ASSUME(l.extents[1] >  0);
      SPACES_ASSUME(r.extents[1] >  0);

      return l.extents[0] == r.extents[0] && l.extents[1] == r.extents[1]
          && l.indices[0] == r.indices[0] && l.indices[1] == r.indices[1];
    }
    friend constexpr bool operator!=(iterator const& l, iterator const& r) noexcept
    {
      return !(l == r);
    }

    friend constexpr bool operator==(iterator const& l, sentinel r) noexcept
    {
      SPACES_ASSUME(l.indices[1] >= 0);
      SPACES_ASSUME(r.extent1    >  0);

      return l.indices[1] == r.extent1;
    }
    friend constexpr bool operator!=(iterator const& l, sentinel r) noexcept
    {
      return !(l == r);
    }

  private:
    std::array<index_type, 2> indices;
    std::array<index_type, 2> const extents;
  };

private:
  iterator const first;
  sentinel const last;

public:
  constexpr index_2d_range(index_type extent0, index_type extent1) noexcept
    : first(
        std::array<index_type, 2>{{0, 0}}
      , std::array<index_type, 2>{{extent0, extent1}}
      )
    , last(extent1)
  {}

  constexpr iterator begin() const noexcept { return first; }

  constexpr sentinel end() const noexcept { return last; }
};

SPACES_END_NAMESPACE

