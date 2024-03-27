/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/*
 * NOTE: This file is deprecated and will be removed in the future.
 *       Use include sys/cdefs.h instead, it will also include
 *       the corresponding libbsd header. TODO: IDF-9391
 */

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_LINUX && !defined(__containerof)
#define __containerof(ptr, type, member) ({         \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifdef __cplusplus
}
#endif
