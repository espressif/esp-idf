/*
 * SPDX-FileCopyrightText: 2010-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* esp_compiler.h needs sdkconfig.h, which the prebuilt lib must not depend on;
 * the lib defines what it uses from there itself. */
#ifndef BLE_AUDIO_LIB_BUILD
#include "esp_compiler.h"
#endif /* BLE_AUDIO_LIB_BUILD */

#include <zephyr/types.h>
#include <zephyr/sys/atomic.h>

#ifndef BUILD_ASSERT
/**
 * @brief Check if the compiler supports the built-in function \a x.
 *
 * This macro is for use with conditional compilation to enable code using a
 * builtin function that may not be available in every compiler.
 *
 * @param x Built-in function identifier, including the leading underscores.
 * @return Nonzero if the built-in function is supported, or zero otherwise.
 */
#ifdef __has_builtin
#define HAS_BUILTIN(x)                  __has_builtin(x)
#else /* __has_builtin */
/*
 * The compiler doesn't provide the __has_builtin() macro, so instead we depend
 * on the toolchain-specific headers to define HAS_BUILTIN_x for the builtins
 * supported.
 */
#define HAS_BUILTIN(x)                  HAS_BUILTIN_##x
#endif /* __has_builtin */

#define BUILD_ASSERT(EXPR, MSG...)      _Static_assert(EXPR, ## MSG)
#endif /* BUILD_ASSERT */

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
