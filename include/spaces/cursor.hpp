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
#include <spaces/tuple.hpp>
#include <spaces/mdrange.hpp>
#include <spaces/space_bind.hpp>

#include <type_traits>
#include <concepts>
#include <utility>
#include <iterator>
#include <array>
#include <ranges>

SPACES_BEGIN_NAMESPACE

template <index_type N>
struct cursor
{
private:
  std::array<index_type, N> data;

public:
  template <typename... Ts>
    requires(std::convertible_to<Ts, index_type> && ...)
  explicit constexpr cursor(Ts&&... ts) : data{(Ts&&)ts...}
  {
    static_assert(sizeof...(Ts) == N);
  }

  constexpr cursor(cursor const& other) : data(other.data) {}
  constexpr cursor(cursor&& other) : data(std::move(other.data)) {}

  template <typename OuterTuple>
  struct range;

  template <typename... Outer>
  struct range<std::tuple<Outer...>>
  {
    struct iterator : std::forward_iterator_tag
    {
      using value_type = std::tuple<index_type, Outer...>;
      using difference_type = index_type;

    private:
      value_type idx;

    public:
      template <typename OuterTuple>
      constexpr iterator(index_type ext, OuterTuple&& outer)
        : idx(std::tuple_cat(std::make_tuple(ext), (OuterTuple&&)outer))
      {}

      constexpr iterator() = default;

      constexpr iterator(iterator const& other) : idx(other.idx) {}
      constexpr iterator(iterator&& other) : idx(std::move(other.idx)) {}

      constexpr iterator& operator=(iterator const& other)
      {
        idx = other.idx;
        return *this;
      }
      constexpr iterator& operator=(iterator&& other)
      {
        idx = std::move(other.idx);
        return *this;
      }

      constexpr iterator& operator++()
      {
        ++std::get<0>(idx);
        return *this;
      }

      constexpr iterator operator++(int)
      {
        iterator tmp(*this);
        ++(*this);
        return tmp;
      }

      constexpr iterator operator+(difference_type n) const
      {
        iterator tmp(*this);
        std::get<0>(tmp.idx) += n;
        return tmp;
      }

      constexpr auto operator*() { return idx; }
      constexpr auto operator*() const { return idx; }

      constexpr bool
      operator==(iterator const& it) const { return idx == it.idx; }
      constexpr bool
      operator!=(iterator const& it) const { return idx != it.idx; }
    };

  private:
    iterator first, last;

  public:
    template <typename OuterTuple>
    constexpr range(index_type ext, OuterTuple&& outer)
      : first(0, outer), last(ext, outer)
    {}

    constexpr range() = default;

    constexpr range(range const& other)
      : first(other.first), last(other.last) {}
    constexpr range(range&& other)
      : first(std::move(other.first)), last(std::move(other.last)) {}

    constexpr range& operator=(range const& other)
    {
      first = other.first;
      last = other.last;
      return *this;
    }
    constexpr range& operator=(range&& other)
    {
      first = std::move(other.first);
      last = std::move(other.last);
      return *this;
    }

    constexpr iterator begin() const { return first; }

    constexpr iterator end() const { return last; }

    constexpr auto size() const
    {
      return tuple_reduce(*last, 1, std::multiplies{});
    }
  };

  static_assert(std::ranges::forward_range<range<std::tuple<>>>);

  template <index_type I, typename OuterTuple>
  friend constexpr auto mdrange(cursor space, OuterTuple&& outer) {
    static_assert(I < N);
    using T = range<std::remove_cvref_t<OuterTuple>>;
    return T(space.data[I], (OuterTuple&&)outer);
  }

  template <typename Factory>
  friend constexpr auto operator|(cursor space, Factory&& factory) {
    return space_bind(space, (Factory&&)factory);
  }
};

template <index_type M>
struct mdrank_t<cursor<M>> : std::integral_constant<index_type, M> {};

SPACES_END_NAMESPACE

