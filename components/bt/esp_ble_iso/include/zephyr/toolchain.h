/*
 * SPDX-FileCopyrightText: 2010-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_compiler.h"

#include <zephyr/types.h>
#include <zephyr/sys/atomic.h>

#define SYS_INIT(init_fn, level, prio)  int init_fn ## _v2(void) { return init_fn(); }

#ifndef BUILD_ASSERT
#define BUILD_ASSERT(EXPR, MSG...)      _Static_assert(EXPR, ## MSG)
#endif

#ifndef _DO_CONCAT
#define _DO_CONCAT(x, y)                x ## y
#endif

#ifndef _CONCAT
#define _CONCAT(x, y)                   _DO_CONCAT(x, y)
#endif

#ifndef ARG_UNUSED
#define ARG_UNUSED(x)                   (void)(x)
#endif

#ifndef __fallthrough
#define __fallthrough                   __attribute__((fallthrough))
#endif

#ifndef __packed
#define __packed                        __attribute__((__packed__))
#endif

#ifndef _LIB_ONLY
#define _LIB_ONLY   /* Used to mark functions only used by BLE Audio LIB */
#endif

#ifndef _IDF_ONLY
#define _IDF_ONLY   /* Used to mark functions only used by BLE Audio IDF */
#endif

#ifndef _LIB_IDF
#define _LIB_IDF    /* Used to mark functions used by BLE Audio LIB and IDF */
#endif

#ifndef _NOT_USED
#define _NOT_USED   /* Used to mark functions currently not used */
#endif

static inline unsigned int find_msb_set(uint32_t op)
{
    if (op == 0) {
        return 0;
    }

    return 32 - __builtin_clz(op);
}

static inline unsigned int find_lsb_set(uint32_t op)
{
    return __builtin_ffs(op);
}
