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
 * Therefore, if we call "CHOOSE_MACRO_VA_ARG(_0, ##__VA_ARGS__, MACRO1, MACRO2)(__VA_ARGS__)", the result will be:
 *
 * - MACRO1(__VA_ARGS__) if __VA_ARGS__ was not empty
 * - MACRO2() if __VA_ARGS__ was empty
 *
 * @note In the future, we want to switch to C++20. We also want to become compatible with clang. Hence, we provide two
 * versions of the following macros which are using variadic arguments. The first one is using the GNU extension
 * ##__VA_ARGS__. The second one is using the C++20 feature __VA_OPT__(,). This allows users to compile their code with
 * standard C++20 enabled instead of the GNU extension. Below C++20, we haven't found any good alternative to using
 * ##__VA_ARGS__.
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define CHOOSE_MACRO_VA_ARG(_0 __VA_OPT__(,) __VA_ARGS__, MACRO, ...) MACRO
#else
#define CHOOSE_MACRO_VA_ARG(_0, _1, MACRO, ...) MACRO
#endif
_Static_assert(CHOOSE_MACRO_VA_ARG(_0, x, 0, 1) == 0, "CHOOSE_MACRO_VA_ARG() result does not match for 0 arguments");
_Static_assert(CHOOSE_MACRO_VA_ARG(_0, 0, 1) == 1, "CHOOSE_MACRO_VA_ARG() result does not match for 1 argument");

#ifdef __cplusplus
}
#endif
