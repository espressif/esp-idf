/*
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Internals for looping macros
 *
 * Repetitive or obscure helper macros needed by sys/util.h.
 */

#ifndef ZEPHYR_INCLUDE_SYS_UTIL_LOOPS_H_
#define ZEPHYR_INCLUDE_SYS_UTIL_LOOPS_H_

#define Z_FOR_LOOP_GET_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, \
                _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                _21, _22, _23, _24, _25, _26, _27, _28, _29, \
                _30, _31, _32, _33, _34, _35, _36, _37, _38, \
                _39, _40, _41, _42, _43, _44, _45, _46, _47, \
                _48, _49, _50, _51, _52, _53, _54, _55, _56, \
                _57, _58, _59, _60, _61, _62, _63, _64, N, ...) N

#define Z_FOR_LOOP_0(z_call, sep, fixed_arg0, fixed_arg1, ...)

#define Z_FOR_LOOP_1(z_call, sep, fixed_arg0, fixed_arg1, x) \
    z_call(0, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_2(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_1(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(1, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_3(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_2(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(2, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_4(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_3(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(3, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_5(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_4(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(4, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_6(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_5(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(5, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_7(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_6(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(6, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_8(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_7(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(7, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_9(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_8(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(8, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_10(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_9(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(9, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_11(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_10(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(10, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_12(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_11(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(11, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_13(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_12(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(12, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_14(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_13(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(13, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_15(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_14(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(14, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_16(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_15(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(15, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_17(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_16(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(16, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_18(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_17(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(17, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_19(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_18(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(18, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_20(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_19(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(19, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_21(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_20(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(20, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_22(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_21(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(21, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_23(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_22(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(22, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_24(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_23(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(23, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_25(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_24(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(24, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_26(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_25(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(25, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_27(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_26(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(26, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_28(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_27(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(27, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_29(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_28(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(28, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_30(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_29(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(29, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_31(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_30(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(30, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_32(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_31(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(31, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_33(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_32(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(32, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_34(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_33(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(33, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_35(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_34(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(34, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_36(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_35(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(35, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_37(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_36(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(36, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_38(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_37(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(37, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_39(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_38(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(38, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_40(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_39(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(39, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_41(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_40(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(40, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_42(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_41(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(41, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_43(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_42(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(42, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_44(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_43(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(43, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_45(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_44(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(44, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_46(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_45(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(45, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_47(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_46(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(46, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_48(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_47(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(47, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_49(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_48(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(48, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_50(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_49(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(49, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_51(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_50(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(50, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_52(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_51(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(51, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_53(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_52(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(52, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_54(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_53(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(53, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_55(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_54(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(54, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_56(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_55(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(55, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_57(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_56(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(56, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_58(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_57(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(57, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_59(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_58(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(58, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_60(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_59(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(59, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_61(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_60(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(60, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_62(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_61(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(61, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_63(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_62(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(62, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_64(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
    Z_FOR_LOOP_63(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    __DEBRACKET sep \
    z_call(63, x, fixed_arg0, fixed_arg1)

#define Z_FOR_EACH_ENGINE(x, sep, fixed_arg0, fixed_arg1, ...) \
    Z_FOR_LOOP_GET_ARG(__VA_ARGS__, \
        Z_FOR_LOOP_64, \
        Z_FOR_LOOP_63, \
        Z_FOR_LOOP_62, \
        Z_FOR_LOOP_61, \
        Z_FOR_LOOP_60, \
        Z_FOR_LOOP_59, \
        Z_FOR_LOOP_58, \
        Z_FOR_LOOP_57, \
        Z_FOR_LOOP_56, \
        Z_FOR_LOOP_55, \
        Z_FOR_LOOP_54, \
        Z_FOR_LOOP_53, \
        Z_FOR_LOOP_52, \
        Z_FOR_LOOP_51, \
        Z_FOR_LOOP_50, \
        Z_FOR_LOOP_49, \
        Z_FOR_LOOP_48, \
        Z_FOR_LOOP_47, \
        Z_FOR_LOOP_46, \
        Z_FOR_LOOP_45, \
        Z_FOR_LOOP_44, \
        Z_FOR_LOOP_43, \
        Z_FOR_LOOP_42, \
        Z_FOR_LOOP_41, \
        Z_FOR_LOOP_40, \
        Z_FOR_LOOP_39, \
        Z_FOR_LOOP_38, \
        Z_FOR_LOOP_37, \
        Z_FOR_LOOP_36, \
        Z_FOR_LOOP_35, \
        Z_FOR_LOOP_34, \
        Z_FOR_LOOP_33, \
        Z_FOR_LOOP_32, \
        Z_FOR_LOOP_31, \
        Z_FOR_LOOP_30, \
        Z_FOR_LOOP_29, \
        Z_FOR_LOOP_28, \
        Z_FOR_LOOP_27, \
        Z_FOR_LOOP_26, \
        Z_FOR_LOOP_25, \
        Z_FOR_LOOP_24, \
        Z_FOR_LOOP_23, \
        Z_FOR_LOOP_22, \
        Z_FOR_LOOP_21, \
        Z_FOR_LOOP_20, \
        Z_FOR_LOOP_19, \
        Z_FOR_LOOP_18, \
        Z_FOR_LOOP_17, \
        Z_FOR_LOOP_16, \
        Z_FOR_LOOP_15, \
        Z_FOR_LOOP_14, \
        Z_FOR_LOOP_13, \
        Z_FOR_LOOP_12, \
        Z_FOR_LOOP_11, \
        Z_FOR_LOOP_10, \
        Z_FOR_LOOP_9, \
        Z_FOR_LOOP_8, \
        Z_FOR_LOOP_7, \
        Z_FOR_LOOP_6, \
        Z_FOR_LOOP_5, \
        Z_FOR_LOOP_4, \
        Z_FOR_LOOP_3, \
        Z_FOR_LOOP_2, \
        Z_FOR_LOOP_1, \
        Z_FOR_LOOP_0)(x, sep, fixed_arg0, fixed_arg1, __VA_ARGS__)

#define Z_GET_ARG_1(_0, ...) _0

#define Z_GET_ARG_2(_0, _1, ...) _1

#define Z_GET_ARG_3(_0, _1, _2, ...) _2

#define Z_GET_ARG_4(_0, _1, _2, _3, ...) _3

#define Z_GET_ARG_5(_0, _1, _2, _3, _4, ...) _4

#define Z_GET_ARG_6(_0, _1, _2, _3, _4, _5, ...) _5

#define Z_GET_ARG_7(_0, _1, _2, _3, _4, _5, _6, ...) _6

#define Z_GET_ARG_8(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7

#define Z_GET_ARG_9(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8

#define Z_GET_ARG_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9

#define Z_GET_ARG_11(_0, _1, _2, _3, _4, _5, \
              _6, _7, _8, _9, _10, ...) _10

#define Z_GET_ARG_12(_0, _1, _2, _3, _4, _5, _6,\
              _7, _8, _9, _10, _11, ...) _11

#define Z_GET_ARG_13(_0, _1, _2, _3, _4, _5, _6, \
              _7, _8, _9, _10, _11, _12, ...) _12

#define Z_GET_ARG_14(_0, _1, _2, _3, _4, _5, _6, \
              _7, _8, _9, _10, _11, _12, _13, ...) _13

#define Z_GET_ARG_15(_0, _1, _2, _3, _4, _5, _6, _7, \
              _8, _9, _10, _11, _12, _13, _14, ...) _14

#define Z_GET_ARG_16(_0, _1, _2, _3, _4, _5, _6, _7, \
              _8, _9, _10, _11, _12, _13, _14, _15, ...) _15

#define Z_GET_ARG_17(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, ...) _16

#define Z_GET_ARG_18(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, ...) _17

#define Z_GET_ARG_19(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, ...) _18

#define Z_GET_ARG_20(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              ...) _19

#define Z_GET_ARG_21(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, ...) _20

#define Z_GET_ARG_22(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, ...) _21

#define Z_GET_ARG_23(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, ...) _22

#define Z_GET_ARG_24(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, ...) _23

#define Z_GET_ARG_25(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, ...) _24

#define Z_GET_ARG_26(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, ...) _25

#define Z_GET_ARG_27(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, ...) _26

#define Z_GET_ARG_28(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, ...) _27

#define Z_GET_ARG_29(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              ...) _28

#define Z_GET_ARG_30(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, ...) _29

#define Z_GET_ARG_31(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, ...) _30

#define Z_GET_ARG_32(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, ...) _31

#define Z_GET_ARG_33(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, ...) _32

#define Z_GET_ARG_34(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, ...) _33

#define Z_GET_ARG_35(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, ...) _34

#define Z_GET_ARG_36(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, ...) _35

#define Z_GET_ARG_37(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, ...) _36

#define Z_GET_ARG_38(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, ...) _37

#define Z_GET_ARG_39(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, ...) _38

#define Z_GET_ARG_40(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, ...) _39

#define Z_GET_ARG_41(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, ...) _40

#define Z_GET_ARG_42(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, ...) _41

#define Z_GET_ARG_43(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, ...) _42

#define Z_GET_ARG_44(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, ...) _43

#define Z_GET_ARG_45(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, ...) _44

#define Z_GET_ARG_46(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, ...) _45

#define Z_GET_ARG_47(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, ...) _46

#define Z_GET_ARG_48(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, ...) _47

#define Z_GET_ARG_49(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, ...) _48

#define Z_GET_ARG_50(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, ...) _49

#define Z_GET_ARG_51(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, ...) _50

#define Z_GET_ARG_52(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, ...) _51

#define Z_GET_ARG_53(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, ...) _52

#define Z_GET_ARG_54(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, ...) _53

#define Z_GET_ARG_55(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, ...) _54

#define Z_GET_ARG_56(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, ...) _55

#define Z_GET_ARG_57(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, ...) _56

#define Z_GET_ARG_58(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, _57, ...) _57

#define Z_GET_ARG_59(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, _57, _58, ...) _58

#define Z_GET_ARG_60(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, _57, _58, _59, ...) _59

#define Z_GET_ARG_61(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, _57, _58, _59, _60, ...) _60

#define Z_GET_ARG_62(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, _57, _58, _59, _60, _61, ...) _61

#define Z_GET_ARG_63(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, _57, _58, _59, _60, _61, _62, ...) _62

#define Z_GET_ARG_64(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
              _11, _12, _13, _14, _15, _16, _17, _18, _19, \
              _20, _21, _22, _23, _24, _25, _26, _27, _28, \
              _29, _30, _31, _32, _33, _34, _35, _36, _37, \
              _38, _39, _40, _41, _42, _43, _44, _45, _46, \
              _47, _48, _49, _50, _51, _52, _53, _54, _55, \
              _56, _57, _58, _59, _60, _61, _62, _63, ...) _63

#define Z_GET_ARGS_LESS_0(...) __VA_ARGS__

#define Z_GET_ARGS_LESS_1(_0, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_2(_0, _1, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_3(_0, _1, _2, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_4(_0, _1, _2, _3, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_5(_0, _1, _2, _3, _4, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_6(_0, _1, _2, _3, _4, _5, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_7(_0, _1, _2, _3, _4, _5, _6, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_8(_0, _1, _2, _3, _4, _5, \
                _6, _7, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_9(_0, _1, _2, _3, _4, _5, \
                _6, _7, _8, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_10(_0, _1, _2, _3, _4, _5, \
                _6, _7, _8, _9, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_11(_0, _1, _2, _3, _4, _5, \
                _6, _7, _8, _9, _10, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_12(_0, _1, _2, _3, _4, _5, _6,\
                _7, _8, _9, _10, _11, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_13(_0, _1, _2, _3, _4, _5, _6, \
                _7, _8, _9, _10, _11, _12, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_14(_0, _1, _2, _3, _4, _5, _6, \
                _7, _8, _9, _10, _11, _12, _13, \
                ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_15(_0, _1, _2, _3, _4, _5, _6, _7, \
                _8, _9, _10, _11, _12, _13, _14, \
                ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_16(_0, _1, _2, _3, _4, _5, _6, _7, \
                _8, _9, _10, _11, _12, _13, _14, _15, ...) \
                __VA_ARGS__

#define Z_GET_ARGS_LESS_17(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_18(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, ...) \
                __VA_ARGS__

#define Z_GET_ARGS_LESS_19(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, ...) \
                __VA_ARGS__

#define Z_GET_ARGS_LESS_20(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_21(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_22(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_23(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_24(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_25(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_26(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_27(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, ...) \
                __VA_ARGS__

#define Z_GET_ARGS_LESS_28(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, \
                ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_29(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_30(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_31(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_32(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_33(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_34(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_35(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_36(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_37(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_38(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_39(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_40(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_41(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_42(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_43(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_44(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_45(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_46(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_47(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_48(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_49(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_50(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_51(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_52(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_53(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_54(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_55(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_56(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_57(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_58(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, _57, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_59(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, _57, _58, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_60(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, _57, _58, _59, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_61(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, _57, _58, _59, _60, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_62(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, _57, _58, _59, _60, _61, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_63(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, _57, _58, _59, _60, _61, _62, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_64(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, \
                _29, _30, _31, _32, _33, _34, _35, _36, _37, \
                _38, _39, _40, _41, _42, _43, _44, _45, _46, \
                _47, _48, _49, _50, _51, _52, _53, _54, _55, \
                _56, _57, _58, _59, _60, _61, _62, _63, ...) __VA_ARGS__

#define Z_FOR_EACH_IDX_FIXED_ARG_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(idx, x, fixed_arg1)

#define Z_FOR_EACH_IDX_FIXED_ARG(F, sep, fixed_arg, ...) \
    Z_FOR_EACH_ENGINE(Z_FOR_EACH_IDX_FIXED_ARG_EXEC, sep, \
                 F, fixed_arg, __VA_ARGS__)

#define Z_FOR_EACH_FIXED_ARG_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(x, fixed_arg1)

#define Z_FOR_EACH_FIXED_ARG(F, sep, fixed_arg, ...) \
    Z_FOR_EACH_ENGINE(Z_FOR_EACH_FIXED_ARG_EXEC, sep, \
                 F, fixed_arg, __VA_ARGS__)

#define Z_FOR_EACH_IDX_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(idx, x)

#define Z_FOR_EACH_IDX(F, sep, ...) \
    Z_FOR_EACH_ENGINE(Z_FOR_EACH_IDX_EXEC, sep, F, _, __VA_ARGS__)

#define Z_FOR_EACH_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(x)

#define Z_FOR_EACH(F, sep, ...) \
    Z_FOR_EACH_ENGINE(Z_FOR_EACH_EXEC, sep, F, _, __VA_ARGS__)

#define Z_BYPASS(x) x

/* Set of UTIL_LISTIFY particles */
#include "util_listify.h"

#endif /* ZEPHYR_INCLUDE_SYS_UTIL_LOOPS_H_ */
