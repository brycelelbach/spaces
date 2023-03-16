// Copyright 2018-2023 NVIDIA Corporation
// Copyright 2016-2019 Bryce Adelstein Lelbach aka wash
// Copyright 2002-2014 Peter Dimov
// Copyright 2010-2011 Beman Dawes
// Copyright 2013      Ion Gaztanaga
// Reply-To: Bryce Adelstein Lelbach aka wash <brycelelbach@gmail.com>
//
// Distributed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Based on boost/core/lightweight_test.hpp
// Imported from: git@github.com:boostorg/core.git, f1c51cb, retrieved 2016-10-25

#pragma once

#include <spaces/config.hpp>
#include <spaces/preprocessor.hpp>

#include <cassert>
#include <cstdint>
#include <iostream>

#if !defined(SPACES_TEST_OSTREAM)
  #define SPACES_TEST_OSTREAM std::cerr
#endif

SPACES_BEGIN_NAMESPACE

struct test_report_errors_reminder
{
  bool called_report_errors_function;

  test_report_errors_reminder() : called_report_errors_function(false) {}

  ~test_report_errors_reminder()
  {
    assert(called_report_errors_function);
  }
};

inline test_report_errors_reminder& test_report_errors_remind()
{
  static test_report_errors_reminder r;
  return r;
}

inline std::uint64_t& test_errors_count()
{
  static std::uint64_t x = 0;
  test_report_errors_remind();
  return x;
}

inline void test_error(
  char const* msg
, char const* file
, std::uint64_t line
, char const* function
  )
{
  SPACES_TEST_OSTREAM
    << file << "(" << line << "): " << msg << " in function '"
    << function << "'" << std::endl;
  ++test_errors_count();
}

template <typename T, typename U>
void test_compare_equal(
  char const* expr1
, char const* expr2
, char const* file
, std::uint64_t line
, char const* function
, T&& t
, U&& u
  )
{
  if (t == u)
    test_report_errors_remind();
  else
  {
    SPACES_TEST_OSTREAM
      << file << "(" << line << "): test '" << expr1 << " == " << expr2
      << "' failed in function '" << function << "': "
      << "'" << t << "' != '" << u << "'" << std::endl;
    ++test_errors_count();
  }
}

template <typename T, typename U>
void test_compare_not_equal(
  char const* expr1
, char const* expr2
, char const* file
, std::uint64_t line
, char const* function
, T&& t
, U&& u
  )
{
  if (t != u)
    test_report_errors_remind();
  else
  {
    SPACES_TEST_OSTREAM
      << file << "(" << line << "): test '" << expr1 << " != " << expr2
      << "' failed in function '" << function << "': "
      << "'" << t << "' == '" << u << "'" << std::endl;
    ++test_errors_count();
  }
}

inline int test_report_errors()
{
  test_report_errors_remind().called_report_errors_function = true;

  auto errors = test_errors_count();

  if (errors == 0)
  {
    SPACES_TEST_OSTREAM
      << "No errors detected." << std::endl;
    return 0;
  }
  else
  {
    SPACES_TEST_OSTREAM
      << errors << " error" << (errors == 1 ? "" : "s")
      << " detected." << std::endl;
    return 1;
  }
}

SPACES_END_NAMESPACE

#define SPACES_TEST(expr)                                                     \
  ( (expr)                                                                    \
  ? (void)0                                                                   \
  : ::spaces::test_error(                                                     \
      "test '" #expr "' failed", __FILE__, __LINE__, SPACES_CURRENT_FUNCTION  \
    )                                                                         \
  )                                                                           \
  /**/

#define SPACES_TEST_NOT(expr) SPACES_TEST(!(expr))

#define SPACES_ERROR(msg)                                                     \
  (::spaces::test_error(msg, __FILE__, __LINE__, SPACES_CURRENT_FUNCTION))    \
  /**/

#define SPACES_TEST_EQ(expr0, expr1)                                          \
  (::spaces::test_compare_equal(                                              \
    #expr0, #expr1, __FILE__, __LINE__, SPACES_CURRENT_FUNCTION, expr0, expr1 \
  ))                                                                          \
  /**/

#define SPACES_TEST_NE(expr0, expr1)                                          \
  (::spaces::test_compare_not_equal(                                          \
    #expr0, #expr1, __FILE__, __LINE__, SPACES_CURRENT_FUNCTION, expr0, expr1 \
  ))                                                                          \
  /**/

