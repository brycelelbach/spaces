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
#include <spaces/mdrange.hpp>
#include <spaces/optimization_hints.hpp>

#include <type_traits>
#include <utility>
#include <functional>
#include <tuple>

SPACES_BEGIN_NAMESPACE

template <index_type I, typename Space, typename F, typename OuterTuple>
constexpr void for_each_impl(Space&& space, F&& f, OuterTuple&& outer)
{
  if constexpr (I > 0) {
    SPACES_DEMAND_VECTORIZATION
    for (auto&& e: mdrange<I>(space, (OuterTuple&&)outer)) {
      invoke_o(
        [&] (auto&& t) {
          for_each_impl<I - 1>((Space&&)space, f, std::forward<decltype(t)>(t));
        }
      , std::forward<decltype(e)>(e)
      );
    }
  } else {
    SPACES_DEMAND_VECTORIZATION
    for (auto&& e: mdrange<I>((Space&&)space, (OuterTuple&&)outer)) {
      invoke_o(
        [&] (auto&& t) {
          std::apply((F&&)f, std::forward<decltype(t)>(t));
        }
      , std::forward<decltype(e)>(e)
      );
    }
  }
}

template <typename Space, typename UnaryFunction>
constexpr void for_each(Space&& space, UnaryFunction&& f)
{
  if constexpr (mdrank<Space> > 0)
    for_each_impl<mdrank<Space> - 1>(
      (Space&&)space, (UnaryFunction&&)f, std::tuple<>{}
    );
}

SPACES_END_NAMESPACE

