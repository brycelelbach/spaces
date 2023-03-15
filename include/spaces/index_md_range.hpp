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

SPACES_BEGIN_NAMESPACE

struct index_2d_range
{
  struct sentinel
  {
    constexpr sentinel(index_type nj_) noexcept : nj(nj_) {}

    index_type const nj;
  };

  struct iterator
  {
    struct dimension
    {
      constexpr dimension() noexcept : extent(0), idx(0) {}

      constexpr dimension(index_type extent_, index_type idx_) noexcept
        : extent(extent_), idx(idx_)
      {}

      constexpr dimension(dimension const&) noexcept = default;
      constexpr dimension(dimension&&)      noexcept = default;

      index_type const extent;
      index_type       idx;
    };

    constexpr iterator(dimension i_, dimension j_) noexcept
      : i(i_), j(j_)
    {}

    constexpr iterator& operator++() noexcept
    {
        SPACES_ASSUME(i.idx    >= 0);
        SPACES_ASSUME(i.extent >  0);

        ++i.idx;                // Inner loop iteration-expression.

        if (i.extent == i.idx)  // Inner loop condition.
        {
            ++j.idx;            // Outer loop increment.
            i.idx = 0;          // Inner loop init-statement.
        }

        return *this;
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    friend constexpr index_type operator-(
        iterator const& l
      , iterator const& r
        ) noexcept
    {
        SPACES_ASSUME(l.i.idx    >= 0);
        SPACES_ASSUME(r.i.idx    >= 0);
        SPACES_ASSUME(l.j.idx    >= 0);
        SPACES_ASSUME(r.j.idx    >= 0);
        SPACES_ASSUME(l.i.extent >  0);
        SPACES_ASSUME(l.j.extent >  0);

        return (l.j.idx - r.j.idx) * (l.i.extent) - (l.i.idx - r.i.idx);
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    friend constexpr index_type operator-(
        iterator const& l
      , sentinel r
        ) noexcept
    {
        SPACES_ASSUME(l.i.idx    >= 0);
        SPACES_ASSUME(l.j.idx    >= 0);
        SPACES_ASSUME(l.i.extent >  0);
        SPACES_ASSUME(r.nj       >  0);

        return (l.j.idx - r.nj) * (l.i.extent) - (l.i.idx - l.i.extent);
    }
    friend constexpr index_type operator-(
        sentinel r
      , iterator const& l
        ) noexcept
    {
        SPACES_ASSUME(l.i.idx    >= 0);
        SPACES_ASSUME(l.j.idx    >= 0);
        SPACES_ASSUME(l.i.extent >  0);
        SPACES_ASSUME(r.nj       >  0);

        return (r.nj - l.j.idx) * (l.i.extent) - (l.i.extent - l.i.idx);
    }

    friend constexpr iterator operator+(
        iterator it
      , index_type d
        ) noexcept
    {
        return iterator(
          dimension(it.i.extent, it.i.idx + it.i.extent % d)
        , dimension(it.j.extent, it.j.idx + d / it.i.extent)
        );
    }

    // NOTE: ICPC requires this when we use an iterator-sentinel range.
    constexpr iterator& operator+=(
        index_type d
        ) noexcept
    {
        i.idx += i.extent % d;
        j.idx += j.idx + d / i.extent;
        return *this;
    }

    constexpr position<2> operator[](
        index_type d
        ) noexcept
    {
        return *(*this + d);
    }

    constexpr position<2> operator*() const noexcept
    {
        return position<2>(i.idx, j.idx);
    }

    friend constexpr bool
    operator==(iterator const& l, iterator const& r) noexcept
    {
        SPACES_ASSUME(l.i.idx    >= 0);
        SPACES_ASSUME(r.i.idx    >= 0);
        SPACES_ASSUME(l.j.idx    >= 0);
        SPACES_ASSUME(r.j.idx    >= 0);
        SPACES_ASSUME(l.i.extent >  0);
        SPACES_ASSUME(r.i.extent >  0);
        SPACES_ASSUME(l.j.extent >  0);
        SPACES_ASSUME(r.j.extent >  0);

        return l.i.extent == r.i.extent && l.j.extent == r.j.extent
            && l.i.idx    == r.i.idx    && l.j.idx    == r.j.idx;
    }
    friend constexpr bool operator!=(
        iterator const& l
      , iterator const& r
        ) noexcept
    {
        return !(l == r);
    }

    friend constexpr bool operator==(
        iterator const& l
      , sentinel r
        ) noexcept
    {
        SPACES_ASSUME(l.j.idx >= 0);
        SPACES_ASSUME(r.nj    >  0);

        return l.j.idx == r.nj;
    }
    friend constexpr bool operator!=(
        iterator const& l
      , sentinel r
        ) noexcept
    {
        return !(l == r);
    }

  private:
    dimension i;
    dimension j;
  };

  constexpr index_2d_range(
      index_type ni
    , index_type nj
      ) noexcept
    : first(iterator(iterator::dimension(ni, 0), iterator::dimension(nj, 0)))
    , last(nj)
  {}

  constexpr iterator begin() const noexcept { return first; }

  constexpr sentinel end() const noexcept { return last; }

private:
  iterator first;
  sentinel last;
};

SPACES_END_NAMESPACE

