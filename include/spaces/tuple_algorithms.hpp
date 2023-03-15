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

#include <tuple>
#include <functional>

SPACES_BEGIN_NAMESPACE

template <std::ptrdiff_t I, typename Tuple, typename T, typename F>
auto tuple_reduce_impl(Tuple&& tuple, T&& init, F&& f)
{
  if constexpr (I > 0) {
    return std::invoke(
      f
    , std::get<I>(tuple)
    , tuple_reduce_impl<I - 1>((Tuple&&)tuple, (T&&)init, (F&&)op)
    );
  } else {
    return std::invoke(
      (F&&)f
    , (T&&)init
    , std::get<I>((Tuple&&)tuple)
    );
  }
}

// `tuple_reduce(tuple, init, f) == f(..., f(tuple[1], f(tuple[0], init))))`.
template <typename Tuple, typename T, typename F>
auto tuple_reduce(Tuple&& tuple, T&& init, F&& f)
{
  return tuple_reduce_impl<std::tuple_size_v<Tuple> - 1>(
    (Tuple&&)tuple, (T&&)init, (F&&)f
  );
}

SPACES_END_NAMESPACE

