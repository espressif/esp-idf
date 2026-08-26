/*
 * SPDX-FileCopyrightText: 2011-2014 Wind River Systems, Inc.
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Misc utilities
 *
 * Repetitive or obscure helper macros needed by sys/util.h.
 */

#ifndef ZEPHYR_INCLUDE_SYS_UTIL_INTERNAL_H_
#define ZEPHYR_INCLUDE_SYS_UTIL_INTERNAL_H_

#include "util_loops.h"

/* IS_ENABLED() helpers */

/* This is called from IS_ENABLED(), and sticks on a "_XXXX" prefix,
 * it will now be "_XXXX1" if config_macro is "1", or just "_XXXX" if it's
 * undefined.
 *   ENABLED:   Z_IS_ENABLED2(_XXXX1)
 *   DISABLED   Z_IS_ENABLED2(_XXXX)
 */
#define Z_IS_ENABLED1(config_macro) Z_IS_ENABLED2(_XXXX##config_macro)

/* Here's the core trick, we map "_XXXX1" to "_YYYY," (i.e. a string
 * with a trailing comma), so it has the effect of making this a
 * two-argument tuple to the preprocessor only in the case where the
 * value is defined to "1"
 *   ENABLED:    _YYYY,    <--- note comma!
 *   DISABLED:   _XXXX
 */
#define _XXXX1 _YYYY,

/* Then we append an extra argument to fool the gcc preprocessor into
 * accepting it as a varargs macro.
 *                         arg1   arg2  arg3
 *   ENABLED:   Z_IS_ENABLED3(_YYYY,    1,    0)
 *   DISABLED   Z_IS_ENABLED3(_XXXX 1,  0)
 */
#define Z_IS_ENABLED2(one_or_two_args) Z_IS_ENABLED3(one_or_two_args 1, 0)

/* And our second argument is thus now cooked to be 1 in the case
 * where the value is defined to 1, and 0 if not:
 */
#define Z_IS_ENABLED3(ignore_this, val, ...) val

/* Implementation of IS_EQ(). Returns 1 if _0 and _1 are the same integer from
 * 0 to 4096, 0 otherwise.
 */
#define Z_IS_EQ(_0, _1) Z_HAS_COMMA(Z_CAT4(Z_IS_, _0, _EQ_, _1)())

/* Used internally by COND_CODE_1 and COND_CODE_0. */
#define Z_COND_CODE_1(_flag, _if_1_code, _else_code) \
    __COND_CODE(_XXXX##_flag, _if_1_code, _else_code)
#define Z_COND_CODE_0(_flag, _if_0_code, _else_code) \
    __COND_CODE(_ZZZZ##_flag, _if_0_code, _else_code)
#define _ZZZZ0 _YYYY,
#define __COND_CODE(one_or_two_args, _if_code, _else_code) \
    __GET_ARG2_DEBRACKET(one_or_two_args _if_code, _else_code)

/* Gets second argument and removes brackets around that argument. It
 * is expected that the parameter is provided in brackets/parentheses.
 */
#define __GET_ARG2_DEBRACKET(ignore_this, val, ...) __DEBRACKET val

/* Used to remove brackets from around a single argument. */
#define __DEBRACKET(...) __VA_ARGS__

/* Used by IS_EMPTY() */
/* reference: https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/ */
#define Z_HAS_COMMA(...) \
    NUM_VA_ARGS_LESS_1_IMPL(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, \
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define Z_TRIGGER_PARENTHESIS_(...) ,
#define Z_IS_EMPTY_(...) \
    Z_IS_EMPTY__( \
        Z_HAS_COMMA(__VA_ARGS__), \
        Z_HAS_COMMA(Z_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
        Z_HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
        Z_HAS_COMMA(Z_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)))
#define Z_CAT4(_0, _1, _2, _3) _0 ## _1 ## _2 ## _3
#define Z_CAT5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define Z_IS_EMPTY__(_0, _1, _2, _3) \
    Z_HAS_COMMA(Z_CAT5(Z_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define Z_IS_EMPTY_CASE_0001 ,

/* Used by LIST_DROP_EMPTY() */
/* Adding ',' after each element would add empty element at the end of
 * list, which is hard to remove, so instead precede each element with ',',
 * this way first element is empty, and this one is easy to drop.
 */
#define Z_LIST_ADD_ELEM(e) EMPTY, e
#define Z_LIST_DROP_FIRST(...) GET_ARGS_LESS_N(1, __VA_ARGS__)
#define Z_LIST_NO_EMPTIES(e) \
    COND_CODE_1(IS_EMPTY(e), (), (Z_LIST_ADD_ELEM(e)))

#define UTIL_CAT(a, ...) UTIL_PRIMITIVE_CAT(a, __VA_ARGS__)
#define UTIL_PRIMITIVE_CAT(a, ...) a##__VA_ARGS__
#define UTIL_CHECK_N(x, n, ...) n
#define UTIL_CHECK(...) UTIL_CHECK_N(__VA_ARGS__, 0,)
#define UTIL_NOT(x) UTIL_CHECK(UTIL_PRIMITIVE_CAT(UTIL_NOT_, x))
#define UTIL_NOT_0 ~, 1,
#define UTIL_COMPL(b) UTIL_PRIMITIVE_CAT(UTIL_COMPL_, b)
#define UTIL_COMPL_0 1
#define UTIL_COMPL_1 0
#define UTIL_BOOL(x) UTIL_COMPL(UTIL_NOT(x))

#define UTIL_EVAL(...) __VA_ARGS__
#define UTIL_EXPAND(...) __VA_ARGS__
#define UTIL_REPEAT(...) UTIL_LISTIFY(__VA_ARGS__)

#define _CONCAT_0(arg, ...) arg
#define _CONCAT_1(arg, ...) UTIL_CAT(arg, _CONCAT_0(__VA_ARGS__))
#define _CONCAT_2(arg, ...) UTIL_CAT(arg, _CONCAT_1(__VA_ARGS__))
#define _CONCAT_3(arg, ...) UTIL_CAT(arg, _CONCAT_2(__VA_ARGS__))
#define _CONCAT_4(arg, ...) UTIL_CAT(arg, _CONCAT_3(__VA_ARGS__))
#define _CONCAT_5(arg, ...) UTIL_CAT(arg, _CONCAT_4(__VA_ARGS__))
#define _CONCAT_6(arg, ...) UTIL_CAT(arg, _CONCAT_5(__VA_ARGS__))
#define _CONCAT_7(arg, ...) UTIL_CAT(arg, _CONCAT_6(__VA_ARGS__))

/* Implementation details for NUM_VA_ARGS_LESS_1 */
#define NUM_VA_ARGS_LESS_1_IMPL(                \
    _ignored,                       \
    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,        \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,   \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,   \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,   \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,   \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60,   \
    _61, _62, N, ...) N

/* Used by MACRO_MAP_CAT */
#define MACRO_MAP_CAT_(...)                     \
    /* To make sure it works also for 2 arguments in total */   \
    MACRO_MAP_CAT_N(NUM_VA_ARGS_LESS_1(__VA_ARGS__), __VA_ARGS__)
#define MACRO_MAP_CAT_N_(N, ...) UTIL_CAT(MACRO_MC_, N)(__VA_ARGS__,)
#define MACRO_MC_0(...)
#define MACRO_MC_1(m, a, ...)  m(a)
#define MACRO_MC_2(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_1(m, __VA_ARGS__,))
#define MACRO_MC_3(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_2(m, __VA_ARGS__,))
#define MACRO_MC_4(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_3(m, __VA_ARGS__,))
#define MACRO_MC_5(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_4(m, __VA_ARGS__,))
#define MACRO_MC_6(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_5(m, __VA_ARGS__,))
#define MACRO_MC_7(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_6(m, __VA_ARGS__,))
#define MACRO_MC_8(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_7(m, __VA_ARGS__,))
#define MACRO_MC_9(m, a, ...)  UTIL_CAT(m(a), MACRO_MC_8(m, __VA_ARGS__,))
#define MACRO_MC_10(m, a, ...) UTIL_CAT(m(a), MACRO_MC_9(m, __VA_ARGS__,))
#define MACRO_MC_11(m, a, ...) UTIL_CAT(m(a), MACRO_MC_10(m, __VA_ARGS__,))
#define MACRO_MC_12(m, a, ...) UTIL_CAT(m(a), MACRO_MC_11(m, __VA_ARGS__,))
#define MACRO_MC_13(m, a, ...) UTIL_CAT(m(a), MACRO_MC_12(m, __VA_ARGS__,))
#define MACRO_MC_14(m, a, ...) UTIL_CAT(m(a), MACRO_MC_13(m, __VA_ARGS__,))
#define MACRO_MC_15(m, a, ...) UTIL_CAT(m(a), MACRO_MC_14(m, __VA_ARGS__,))

/* Used by Z_IS_EQ */
#include "util_internal_is_eq.h"

/*
 * Generic sparse list of odd numbers (check the implementation of
 * GPIO_DT_RESERVED_RANGES_NGPIOS as a usage example)
 */
#define Z_SPARSE_LIST_ODD_NUMBERS       \
    EMPTY,  1, EMPTY,  3, EMPTY,  5, EMPTY,  7, \
    EMPTY,  9, EMPTY, 11, EMPTY, 13, EMPTY, 15, \
    EMPTY, 17, EMPTY, 19, EMPTY, 21, EMPTY, 23, \
    EMPTY, 25, EMPTY, 27, EMPTY, 29, EMPTY, 31, \
    EMPTY, 33, EMPTY, 35, EMPTY, 37, EMPTY, 39, \
    EMPTY, 41, EMPTY, 43, EMPTY, 45, EMPTY, 47, \
    EMPTY, 49, EMPTY, 51, EMPTY, 53, EMPTY, 55, \
    EMPTY, 57, EMPTY, 59, EMPTY, 61, EMPTY, 63

/*
 * Generic sparse list of even numbers (check the implementation of
 * GPIO_DT_RESERVED_RANGES_NGPIOS as a usage example)
 */
#define Z_SPARSE_LIST_EVEN_NUMBERS      \
     0, EMPTY,  2, EMPTY,  4, EMPTY,  6, EMPTY, \
     8, EMPTY, 10, EMPTY, 12, EMPTY, 14, EMPTY, \
    16, EMPTY, 18, EMPTY, 20, EMPTY, 22, EMPTY, \
    24, EMPTY, 26, EMPTY, 28, EMPTY, 30, EMPTY, \
    32, EMPTY, 34, EMPTY, 36, EMPTY, 38, EMPTY, \
    40, EMPTY, 42, EMPTY, 44, EMPTY, 46, EMPTY, \
    48, EMPTY, 50, EMPTY, 52, EMPTY, 54, EMPTY, \
    56, EMPTY, 58, EMPTY, 60, EMPTY, 62, EMPTY

/* Used by UTIL_INC */
#include "util_internal_util_inc.h"

/* Used by UTIL_DEC */
#include "util_internal_util_dec.h"

/* Used by UTIL_X2 */
#include "util_internal_util_x2.h"

#endif /* ZEPHYR_INCLUDE_SYS_UTIL_INTERNAL_H_ */
