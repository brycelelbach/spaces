// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>
#include <spaces/mdrange.hpp>

#include <concepts>
#include <utility>
#include <functional>

SPACES_BEGIN_NAMESPACE

template <typename Space, index_type I, typename Factory>
struct space_binder
{
  static_assert(I < mdrank<Space>);

private:
  Space underlying;
  Factory factory;

public:
  template <typename USpace, typename UFactory>
  constexpr space_binder(USpace&& underlying_, UFactory&& factory_)
    : underlying((USpace&&)underlying_), factory((UFactory&&)factory) {}

  constexpr space_binder(space_binder const& other)
    : underlying(other.underlying), factory(other.factory) {}
  constexpr space_binder(space_binder&& other)
    : underlying(std::move(other.underlying))
    , factory(std::move(other.factory)) {}

  template <index_type J, typename USpace, typename OuterTuple>
    requires(std::convertible_to<USpace, space_binder>)
  friend constexpr auto mdrange(USpace&& space, OuterTuple&& outer)
  {
    static_assert(J < mdrank<USpace>);
    if constexpr (I == J) {
      return std::invoke(
        std::forward<decltype(space.factory)>(space.factory)
      , mdrange<I>(
          std::forward<decltype(space.underlying)>(space.underlying)
        , (OuterTuple&&)outer
        )
      );
    } else {
      return mdrange<I>(
        std::forward<decltype(space.underlying)>(space.underlying)
      , (OuterTuple&&)outer
      );
    }
  }
};

template <typename Space, index_type I, typename Factory>
struct mdrank_t<space_binder<Space, I, Factory>> : mdrank_t<Space> {};

template <typename Space, typename Factory>
constexpr auto space_bind(Space&& space, Factory&& factory)
{
  using T = space_binder<
    std::remove_cvref_t<Space>, 0, std::remove_cvref_t<Factory>
  >;
  return T((Space&&)space, (Factory&&)factory);
}

SPACES_END_NAMESPACE

