// Copyright (c) 2015-2017 Bryce Adelstein Lelbach
// Copyright (c) 2017-2023 NVIDIA Corporation
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <spaces/config.hpp>
#include <spaces/meta.hpp>
#include <spaces/space_bind.hpp>

SPACES_BEGIN_NAMESPACE

template <typename Index, typename Factory>
struct on_extent_factory
{
private:
  static constexpr Index index{};
  Factory underlying;

public:
  template <typename UFactory>
  explicit constexpr on_extent_factory(UFactory&& underlying_)
    : underlying(underlying_) {}

  constexpr on_extent_factory(on_extent_factory const&) = default;
  constexpr on_extent_factory(on_extent_factory&&) = default;

  template <typename Space, typename UFactory>
    requires(specialization_of<UFactory, on_extent_factory>)
  friend constexpr auto space_bind(Space&& space, UFactory&& factory)
  {
    using T = space_binder<
      std::remove_cvref_t<Space>, UFactory::index, decltype(factory.underlying)
    >;
    return T((Space&&)space, ((UFactory&&)factory).underlying);
  }
};

template <index_type I, typename Space, typename Factory>
constexpr auto on_extent(Space&& space, Factory&& factory)
{
  using T = space_binder<
    std::remove_cvref_t<Space>, I, std::remove_cvref_t<Factory>
  >;
  return T((Space&&)space, (Factory&&)factory);
}

template <index_type I, typename Factory>
constexpr auto on_extent(Factory&& factory)
{
  using T = on_extent_factory<constant<I>, std::remove_cvref_t<Factory>>;
  return T((Factory&&)factory);
}

SPACES_END_NAMESPACE

