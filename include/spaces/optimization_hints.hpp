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

#include <spaces/preprocessor.hpp>

// "Portable" Bryce-to-vectorizer communication facilities.
// I never leave home without 'em!

// SPACES_DEMAND_VECTORIZATION - Insist that the compiler disregard loop-carried
// dependency analysis and cost modelling and vectorize the loop directly
// following the macro. Using this incorrectly can silently cause bogus codegen
// that blows up in unexpected ways. Usage:
//
// SPACES_DEMAND_VECTORIZATION for (/* ... */) { /* ... */ }
//
// NOTE: Unlike Clang and Intel, GCC doesn't have a stronger hint than ivdep,
// so this is the best we can do. It is not clear if this overrides GCC's cost
// modeling.
#if   defined(__INTEL_COMPILER)
  #define SPACES_DEMAND_VECTORIZATION                                         \
    SPACES_PRAGMA(simd)                                                       \
    /**/
#elif defined(__clang__)
  #define SPACES_DEMAND_VECTORIZATION                                         \
    SPACES_PRAGMA(clang loop vectorize(enable) interleave(enable))            \
    /**/
#else
  #define SPACES_DEMAND_VECTORIZATION                                         \
    SPACES_PRAGMA(GCC ivdep)                                                  \
    /**/
#endif

// SPACES_PREVENT_VECTORIZATION - Tell the compiler to not vectorize a loop.
// Usage:
//
// SPACES_PREVENT_VECTORIZATION for (/* ... */) { /* ... */ }
//
// NOTE: Unlike Clang and Intel, GCC doesn't seem to have a way to do this.
#if   defined(__INTEL_COMPILER)
  #define SPACES_PREVENT_VECTORIZATION                                        \
    SPACES_PRAGMA(novector)                                                   \
    /**/
#elif defined(__clang__)
  #define SPACES_PREVENT_VECTORIZATION                                        \
    SPACES_PRAGMA(clang loop vectorize(disable) interleave(disable))          \
    /**/
#else
    #define SPACES_PREVENT_VECTORIZATION
#endif

// Sometimes it is nice to check that our brash and bold claims are, in fact,
// correct. Defining SPACES_CHECK_ASSUMPTIONS does that (e.g. assumption will be
// asserted before they are assumed).
#if defined(SPACES_CHECK_ASSUMPTIONS)
    #include <cassert>
    #include <stdint>
    #define SPACES_ASSERT_ASSUMPTION(expr) assert(expr)
#else
    #define SPACES_ASSERT_ASSUMPTION(expr)
#endif

// SPACES_ASSUME(expr) - Tell the compiler to assume that expr is true.
// Useful for telling the compiler that the trip count for a loop is division
// by a unrolling/vectorizing-friendly number:
//
//   SPACES_ASSUME((N % 32) == 0); for (auto i = 0; i != N; ++i) /* ... */
//
// SPACES_ASSUME_ALIGNED(ptr, align) - Tell the compiler to
// assume that ptr is aligned to align bytes. ptr must be an lvalue non-const
// pointer.
//
// NOTE: These used to have ridiculous exponential-in-number-of-uses
// compile-time costs with Clang/LLVM. For example, a 10k line project with
// ~100 SPACES_ASSUME/SPACES_ASSUME_ALIGNED usages would take ~20
// seconds to build with ICPC and ~5-10 minutes with Clang/LLVM. I believe the
// issue has now been fixed, but you'll run into it with older versions.
//
// NOTE: To the best of my knowledge, ICPC's __assume_aligned() is an
// assumption about the first argument, while Clang/GCC's
// __builtin_assume_aligned() is an assumption about the return value of the
// intrinsic.
#if   defined(__INTEL_COMPILER)
  #define SPACES_ASSUME(expr)                                                 \
    SPACES_ASSERT_ASSUMPTION(expr)                                            \
    __assume(expr)                                                            \
    /**/
  #define SPACES_ASSUME_ALIGNED(ptr, align)                                   \
    SPACES_ASSERT_ASSUMPTION(0 == (std::uintptr_t(ptr) % alignment))          \
    __assume_aligned(ptr, align)                                              \
    /**/
#elif defined(__clang__)
  #define SPACES_ASSUME(expr)                                                 \
    SPACES_ASSERT_ASSUMPTION(expr)                                            \
    __builtin_assume(expr)                                                    \
    /**/
  #define SPACES_ASSUME_ALIGNED(ptr, align)                                   \
    SPACES_ASSERT_ASSUMPTION(0 == (std::uintptr_t(ptr) % alignment))          \
    {                                                                         \
        ptr = reinterpret_cast<decltype(ptr)>(                                \
            __builtin_assume_aligned(ptr, align)                              \
        );                                                                    \
    }                                                                         \
    /**/
#else // GCC
  #define SPACES_ASSUME(expr)                                                 \
    SPACES_ASSERT_ASSUMPTION(expr)                                            \
    do { if (!(expr)) __builtin_unreachable(); } while (0)                    \
    /**/
  #define SPACES_ASSUME_ALIGNED(ptr, align)                                   \
    SPACES_ASSERT_ASSUMPTION(0 == (std::uintptr_t(ptr) % alignment))          \
    {                                                                         \
      ptr = reinterpret_cast<decltype(ptr)>(                                  \
        __builtin_assume_aligned(ptr, align)                                  \
      );                                                                      \
    }                                                                         \
    /**/
#endif

