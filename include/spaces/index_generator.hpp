// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>

#if !(defined(__INTEL_LLVM_COMPILER) || defined(__INTEL_COMPILER))

#include <array>
#include <coroutine>

SPACES_BEGIN_NAMESPACE

template <index_type N>
struct index_generator
{
  static_assert(N != 0, "N must be greater than 0.");

  struct promise_type
  {
    using return_type = index_generator;

    std::array<index_type, N> pos;

    constexpr std::suspend_always yield_value(
      std::array<index_type, N> pos_
    ) noexcept
    {
      pos = pos_;
      return {};
    }

    constexpr std::suspend_always initial_suspend() const noexcept
    {
      return {};
    }

    constexpr std::suspend_always final_suspend() const noexcept
    {
      return {};
    }

    index_generator get_return_object() noexcept
    {
      return index_generator(this);
    }

    constexpr void return_void() noexcept {}
    constexpr void unhandled_exception() noexcept {}
  };

  struct iterator
  {
    std::coroutine_handle<promise_type> coro;
    bool done;

    constexpr iterator(
      std::coroutine_handle<promise_type> coro_
    , bool done_
      )
      : coro(coro_), done(done_)
    {}

    iterator& operator++()
    {
      coro.resume();
      done = coro.done();
      return *this;
    }

    std::array<index_type, N> operator*() const
    {
      return coro.promise().pos;
    }

    constexpr bool operator==(iterator const& rhs) const noexcept
    {
      return done == rhs.done;
    }
    constexpr bool operator!=(iterator const& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };

  iterator begin()
  {
    p.resume();
    return iterator(p, p.done());
  }

  constexpr iterator end()
  {
    return iterator(p, true);
  }

  constexpr index_generator(index_generator&& rhs) noexcept : p(rhs.p)
  {
    rhs.p = nullptr;
  }

  ~index_generator()
  {
    if (p) p.destroy();
  }

private:
  explicit index_generator(promise_type* p) noexcept
    : p(std::coroutine_handle<promise_type>::from_promise(*p))
  {}

  std::coroutine_handle<promise_type> p;
};

inline index_generator<1> generate_indices(
  std::array<index_type, 2> ni
  ) noexcept
{
  index_type const nilo = ni[0];
  index_type const nihi = ni[1];
  SPACES_ASSUME(nilo >= 0);
  SPACES_ASSUME(nihi >  nilo);
  for (index_type i = nilo; i != nihi; ++i)
      co_yield std::array<index_type, 1>{i};
}

inline index_generator<1> generate_indices(
  index_type ni
  ) noexcept
{
  return generate_indices(std::array<index_type, 2>{0, ni});
}

inline index_generator<2> generate_indices(
  std::array<index_type, 2> ni
, std::array<index_type, 2> nj
  ) noexcept
{
  index_type const nilo = ni[0];
  index_type const nihi = ni[1];
  index_type const njlo = nj[0];
  index_type const njhi = nj[1];
  SPACES_ASSUME(njlo >= 0);
  SPACES_ASSUME(nilo >= 0);
  SPACES_ASSUME(njhi >  njlo);
  SPACES_ASSUME(nihi >  nilo);
  for (index_type j = njlo; j != njhi; ++j)
      for (index_type i = nilo; i != nihi; ++i)
          co_yield std::array<index_type, 2>{i, j};
}

inline index_generator<2> generate_indices(
  index_type ni
, index_type nj
  ) noexcept
{
  return generate_indices(
      std::array<index_type, 2>{0, ni}
    , std::array<index_type, 2>{0, nj}
  );
}

SPACES_END_NAMESPACE

#endif

