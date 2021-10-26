/*
 * SPDX-FileCopyrightText: 2011-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Misc utilities
 *
 * Misc utilities usable by the kernel and application code.
 */

#ifndef _BLE_MESH_UTIL_H_
#define _BLE_MESH_UTIL_H_

#include <stddef.h>
#include "esp_bit_defs.h"
#include "mesh_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Helper to pass a int as a pointer or vice-versa.
 * Those are available for 32 bits architectures:
 */
#ifndef POINTER_TO_UINT
#define POINTER_TO_UINT(x) ((uint32_t)  (x))
#endif
#ifndef UINT_TO_POINTER
#define UINT_TO_POINTER(x) ((void *) (x))
#endif
#ifndef POINTER_TO_INT
#define POINTER_TO_INT(x)  ((int32_t)  (x))
#endif
#ifndef INT_TO_POINTER
#define INT_TO_POINTER(x)  ((void *) (x))
#endif

/* Evaluates to 0 if cond is true-ish; compile error otherwise */
#ifndef ZERO_OR_COMPILE_ERROR
#define ZERO_OR_COMPILE_ERROR(cond) ((int) sizeof(char[1 - 2 * !(cond)]) - 1)
#endif

/* Evaluates to 0 if array is an array; compile error if not array (e.g.
 * pointer)
 */
#ifndef IS_ARRAY
#define IS_ARRAY(array) \
        ZERO_OR_COMPILE_ERROR( \
        !__builtin_types_compatible_p(__typeof__(array), \
            __typeof__(&(array)[0])))
#endif

/* Evaluates to number of elements in an array; compile error if not
 * an array (e.g. pointer)
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

/* Evaluates to 1 if ptr is part of array, 0 otherwise; compile error if
 * "array" argument is not an array (e.g. "ptr" and "array" mixed up)
 */
#ifndef PART_OF_ARRAY
#define PART_OF_ARRAY(array, ptr) \
        ((ptr) && ((ptr) >= &array[0] && (ptr) < &array[ARRAY_SIZE(array)]))
#endif

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, field) \
        ((type *)(((char *)(ptr)) - offsetof(type, field)))
#endif

/* round "x" up/down to next multiple of "align" (which must be a power of 2) */
#ifndef ROUND_UP
#define ROUND_UP(x, align) \
        (((unsigned long)(x) + ((unsigned long)align - 1)) & \
        ~((unsigned long)align - 1))
#endif

#ifndef ROUND_DOWN
#define ROUND_DOWN(x, align) ((unsigned long)(x) & ~((unsigned long)align - 1))
#endif

/* round up/down to the next word boundary */
#ifndef WB_UP
#define WB_UP(x) ROUND_UP(x, sizeof(void *))
#endif

#ifndef WB_DN
#define WB_DN(x) ROUND_DOWN(x, sizeof(void *))
#endif

#ifndef ceiling_fraction
#define ceiling_fraction(numerator, divider) \
        (((numerator) + ((divider) - 1)) / (divider))
#endif

#ifndef CHECKIF
#define CHECKIF(expr)   if (expr)
#endif

/** @brief Return larger value of two provided expressions.
 *
 * @note Arguments are evaluated twice. See Z_MAX for GCC only, single
 * evaluation version.
 */
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/** @brief Return smaller value of two provided expressions.
 *
 * @note Arguments are evaluated twice. See Z_MIN for GCC only, single
 * evaluation version.
 */
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef BIT
#define BIT(n)      (1UL << (n))
#endif

#ifndef BIT_MASK
#define BIT_MASK(n) (BIT(n) - 1)
#endif

/**
 * @brief Check for macro definition in compiler-visible expressions
 *
 * This trick was pioneered in Linux as the config_enabled() macro.
 * The madness has the effect of taking a macro value that may be
 * defined to "1" (e.g. CONFIG_MYFEATURE), or may not be defined at
 * all and turning it into a literal expression that can be used at
 * "runtime".  That is, it works similarly to
 * "defined(CONFIG_MYFEATURE)" does except that it is an expansion
 * that can exist in a standard expression and be seen by the compiler
 * and optimizer.  Thus much ifdef usage can be replaced with cleaner
 * expressions like:
 *
 *     if (IS_ENABLED(CONFIG_MYFEATURE))
 *             myfeature_enable();
 *
 * INTERNAL
 * First pass just to expand any existing macros, we need the macro
 * value to be e.g. a literal "1" at expansion time in the next macro,
 * not "(1)", etc...  Standard recursive expansion does not work.
 */
#define IS_ENABLED(config_macro) Z_IS_ENABLED1(config_macro)

/* Now stick on a "_XXXX" prefix, it will now be "_XXXX1" if config_macro
 * is "1", or just "_XXXX" if it's undefined.
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
#define Z_IS_ENABLED2(one_or_two_args) Z_IS_ENABLED3(one_or_two_args true, false)

/* And our second argument is thus now cooked to be 1 in the case
 * where the value is defined to 1, and 0 if not:
 */
#define Z_IS_ENABLED3(ignore_this, val, ...) val

const char *bt_hex(const void *buf, size_t len);

void mem_rcopy(uint8_t *dst, uint8_t const *src, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_UTIL_H_ */
