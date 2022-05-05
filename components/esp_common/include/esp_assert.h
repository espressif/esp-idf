/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ESP_ASSERT_H__
#define __ESP_ASSERT_H__

#include "assert.h"

/* Assert at compile time if possible, runtime otherwise */
#ifndef __cplusplus
/* __builtin_choose_expr() is only in C, makes this a lot cleaner */
#define TRY_STATIC_ASSERT(CONDITION, MSG) do {                                                              \
            _Static_assert(__builtin_choose_expr(__builtin_constant_p(CONDITION), (CONDITION), 1), #MSG);   \
            assert(#MSG && (CONDITION));                                                                    \
        } while(0)
#else
/* for C++, use __attribute__((error)) - works almost as well as _Static_assert */
#define TRY_STATIC_ASSERT(CONDITION, MSG) do {                                                              \
            if (__builtin_constant_p(CONDITION) && !(CONDITION)) {          \
                extern __attribute__((error(#MSG))) void failed_compile_time_assert(void);      \
                failed_compile_time_assert();                                                   \
            }                                                                                   \
            assert(#MSG && (CONDITION));                                                                    \
        } while(0)
#endif /* __cplusplus */

#endif /* __ESP_ASSERT_H__ */
