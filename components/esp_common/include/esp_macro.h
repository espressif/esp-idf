/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
This header contains various general purpose helper macros used across ESP-IDF
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro to select different versions of other macros based on whether VA_ARGS has an argument or no argument
 *
 * Some macros (such as in FreeRTOS) have two versions (one that accepts arguments and another that does not). The
 * following "CHOOSE_MACRO_VA_ARG" selector allows automatic selection between two different versions of a macro.
 *
 * "CHOOSE_MACRO_VA_ARG" make use of the fact that "##__VA_ARGS__," will eliminate the trailing comma if there are no
 * arguments, thus allows subsequent arguments in "CHOOSE_MACRO_VA_ARG" to be left shifted in the parameter list.
 *
 * Therefore, if we call the following:
 * - CHOOSE_MACRO_VA_ARG(MACRO_ARGS, MACRO_NO_ARGS, ##__VA_ARGS__)(__VA_ARGS__)
 *
 * The result will be:
 * - MACRO_ARGS(__VA_ARGS__) if __VA_ARGS__ was not empty
 * - MACRO_NO_ARGS() if __VA_ARGS__ was empty
 *
 * @note In the future, we want to switch to C++20. We also want to become compatible with clang. Hence, we provide two
 * versions of the following macros which are using variadic arguments. One is using the GNU extension ##__VA_ARGS__.
 * The other is using the C++20 feature __VA_OPT__(,). This allows users to compile their code with standard C++20
 * enabled instead of the GNU extension. Below C++20, we haven't found any good alternative to using ##__VA_ARGS__.
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define CHOOSE_MACRO_VA_ARG_INN_IMPL(...) __VA_OPT__(0)
#define CHOOSE_MACRO_VA_ARG_INN(one, MACRO1, MACRO2, ...) MACRO1
#define CHOOSE_MACRO_VA_ARG(MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, ...) CHOOSE_MACRO_VA_ARG_INN(CHOOSE_MACRO_VA_ARG_INN_IMPL(__VA_ARGS__) __VA_OPT__(,) MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, 0)
#else
#define CHOOSE_MACRO_VA_ARG_INN(one, two, MACRO1, MACRO2, ...) MACRO1
#define CHOOSE_MACRO_VA_ARG(MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, ...) CHOOSE_MACRO_VA_ARG_INN(0, ##__VA_ARGS__, MACRO_WITH_ARGS, MACRO_WITH_NO_ARGS, 0)
#endif

/* test macros */
#define foo_args(...) 1
#define foo_no_args() 2
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define foo(...) CHOOSE_MACRO_VA_ARG(foo_args, foo_no_args __VA_OPT__(,) __VA_ARGS__)(__VA_ARGS__)
#else
#define foo(...) CHOOSE_MACRO_VA_ARG(foo_args, foo_no_args, ##__VA_ARGS__)(__VA_ARGS__)
#endif

#if defined(__cplusplus)
#define MY_STATIC_ASSERT static_assert
#else
#define MY_STATIC_ASSERT _Static_assert
#endif

MY_STATIC_ASSERT(foo() == 2, "CHOOSE_MACRO_VA_ARG() result does not match for 0 arguments");
MY_STATIC_ASSERT(foo(42) == 1, "CHOOSE_MACRO_VA_ARG() result does not match for 1 argument");
#if defined(__cplusplus) && (__cplusplus >  201703L)
static_assert(foo(42, 87) == 1, "CHOOSE_MACRO_VA_ARG() result does not match for n arguments");
#endif

#undef MY_STATIC_ASSERT
#undef foo
#undef foo_args
#undef foo_no_args

#ifdef __cplusplus
}
#endif
