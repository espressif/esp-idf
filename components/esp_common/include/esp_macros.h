/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
This header contains various general purpose helper macros used across ESP-IDF
*/
#include <assert.h>
#include "esp_assert.h"
#include "esp_compiler.h"

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
 * @note In the future, we want to become compatible with clang. Hence, we provide two
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

/* Count number of arguments of __VA_ARGS__
 * - reference https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
 * - __GET_NTH_ARG__() takes args >= N (64) but only expand to Nth one (64th)
 * - ESP_RSEQ_N() is reverse sequential to N to add padding to have Nth
 *   position is the same as the number of arguments
 * - ##__VA_ARGS__ is used to deal with 0 parameter (swallows comma)
 */
#ifndef ESP_VA_NARG
# define ESP_VA_NARG(...)   ESP_NARG(_0, ##__VA_ARGS__, ESP_RSEQ_N())

# define ESP_NARG(...)      ESP_GET_NTH_ARG(__VA_ARGS__)
# define ESP_GET_NTH_ARG( \
        _01,_02,_03,_04,_05,_06,_07,_08,_09,_10, \
        _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
        _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
        _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
        _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
        _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
        _61,_62,_63,N,...) N
# define ESP_RSEQ_N() \
        62,61,60,                      \
        59,58,57,56,55,54,53,52,51,50, \
        49,48,47,46,45,44,43,42,41,40, \
        39,38,37,36,35,34,33,32,31,30, \
        29,28,27,26,25,24,23,22,21,20, \
        19,18,17,16,15,14,13,12,11,10, \
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#endif

#ifndef ESP_UNUSED
#define ESP_UNUSED(x) ((void)(x))
#endif

#define ESP_INFINITE_LOOP() \
    do { \
        ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-infinite-loop") \
        while(1); \
        ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-infinite-loop") \
    } while(1)

#ifdef __cplusplus
}
#endif
