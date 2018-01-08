/*
 * Copyright (c) 2011-2014, Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Misc utilities
 *
 * Misc utilities usable by the kernel and application code.
 */

#ifndef _UTIL__H_
#define _UTIL__H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include <stddef.h>
#include "mesh_types.h"
#include "mesh_trace.h"
#include "soc/soc.h"

/* Helper to pass a int as a pointer or vice-versa.
 * Those are available for 32 bits architectures:
 */
#define POINTER_TO_UINT(x) ((u32_t)  (x))
#define UINT_TO_POINTER(x) ((void *) (x))
#define POINTER_TO_INT(x)  ((s32_t)  (x))
#define INT_TO_POINTER(x)  ((void *) (x))

/* Evaluates to 0 if cond is true-ish; compile error otherwise */
#define ZERO_OR_COMPILE_ERROR(cond) ((int) sizeof(char[1 - 2 * !(cond)]) - 1)

/* Evaluates to 0 if array is an array; compile error if not array (e.g.
 * pointer)
 */
#define IS_ARRAY(array) \
    ZERO_OR_COMPILE_ERROR( \
        !__builtin_types_compatible_p(__typeof__(array), \
                          __typeof__(&(array)[0])))

/* Evaluates to number of elements in an array; compile error if not
 * an array (e.g. pointer)
 */
#define ARRAY_SIZE(array) \
    ((unsigned long) (IS_ARRAY(array) + \
        (sizeof(array) / sizeof((array)[0]))))

/* Evaluates to 1 if ptr is part of array, 0 otherwise; compile error if
 * "array" argument is not an array (e.g. "ptr" and "array" mixed up)
 */
#define PART_OF_ARRAY(array, ptr) \
    ((ptr) && ((ptr) >= &array[0] && (ptr) < &array[ARRAY_SIZE(array)]))

#define CONTAINER_OF(ptr, type, field) \
    ((type *)(((char *)(ptr)) - offsetof(type, field)))

/* round "x" up/down to next multiple of "align" (which must be a power of 2) */
#define ROUND_UP(x, align)                                   \
    (((unsigned long)(x) + ((unsigned long)align - 1)) & \
     ~((unsigned long)align - 1))
#define ROUND_DOWN(x, align) ((unsigned long)(x) & ~((unsigned long)align - 1))

#define ceiling_fraction(numerator, divider) \
    (((numerator) + ((divider) - 1)) / (divider))

/* Internal helpers only used by the sys_* APIs further below */
#define __bswap_16(x) ((u16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#define __bswap_32(x) ((u32_t) ((((x) >> 24) & 0xff) | \
                   (((x) >> 8) & 0xff00) | \
                   (((x) & 0xff00) << 8) | \
                   (((x) & 0xff) << 24)))
#define __bswap_64(x) ((u64_t) ((((x) >> 56) & 0xff) | \
                   (((x) >> 40) & 0xff00) | \
                   (((x) >> 24) & 0xff0000) | \
                   (((x) >> 8) & 0xff000000) | \
                   (((x) & 0xff000000) << 8) | \
                   (((x) & 0xff0000) << 24) | \
                   (((x) & 0xff00) << 40) | \
                   (((x) & 0xff) << 56)))

#define sys_le16_to_cpu(val) (val)
#define sys_cpu_to_le16(val) (val)
#define sys_be16_to_cpu(val) __bswap_16(val)
#define sys_cpu_to_be16(val) __bswap_16(val)
#define sys_le32_to_cpu(val) (val)
#define sys_cpu_to_le32(val) (val)
#define sys_le64_to_cpu(val) (val)
#define sys_cpu_to_le64(val) (val)
#define sys_be32_to_cpu(val) __bswap_32(val)
#define sys_cpu_to_be32(val) __bswap_32(val)
#define sys_be64_to_cpu(val) __bswap_64(val)
#define sys_cpu_to_be64(val) __bswap_64(val)



#ifdef INLINED
#define INLINE inline
#else
#define INLINE
#endif

#define NOINIT

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

static inline int is_power_of_two(unsigned int x)
{
    return (x != 0) && !(x & (x - 1));
}

static inline s64_t arithmetic_shift_right(s64_t value, u8_t shift)
{
    s64_t sign_ext;

    if (shift == 0) {
        return value;
    }

    /* extract sign bit */
    sign_ext = (value >> 63) & 1;

    /* make all bits of sign_ext be the same as the value's sign bit */
    sign_ext = -sign_ext;

    /* shift value and fill opened bit positions with sign bit */
    return (value >> shift) | (sign_ext << (64 - shift));
}

#endif /* !_ASMLANGUAGE */

/* KB, MB, GB */
#define KB(x) ((x) << 10)
#define MB(x) (KB(x) << 10)
#define GB(x) (MB(x) << 10)

/* KHZ, MHZ */
#define KHZ(x) ((x) * 1000)
#define MHZ(x) (KHZ(x) * 1000)

#ifndef BIT
#define BIT(n)  (1UL << (n))
#endif

#define BIT_MASK(n) (BIT(n) - 1)

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
#define IS_ENABLED(config_macro) _IS_ENABLED1(config_macro)

/* Now stick on a "_XXXX" prefix, it will now be "_XXXX1" if config_macro
 * is "1", or just "_XXXX" if it's undefined.
 *   ENABLED:   _IS_ENABLED2(_XXXX1)
 *   DISABLED   _IS_ENABLED2(_XXXX)
 */
#define _IS_ENABLED1(config_macro) _IS_ENABLED2(_XXXX##config_macro)

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
 *   ENABLED:   _IS_ENABLED3(_YYYY,    1,    0)
 *   DISABLED   _IS_ENABLED3(_XXXX 1,  0)
 */
#define _IS_ENABLED2(one_or_two_args) _IS_ENABLED3(one_or_two_args 1, 0)

/* And our second argument is thus now cooked to be 1 in the case
 * where the value is defined to 1, and 0 if not:
 */
#define _IS_ENABLED3(ignore_this, val, ...) val

/**
 * Macros for doing code-generation with the preprocessor.
 *
 * Generally it is better to generate code with the preprocessor than
 * to copy-paste code or to generate code with the build system /
 * python script's etc.
 *
 * http://stackoverflow.com/a/12540675
 */
#define UTIL_EMPTY(...)
#define UTIL_DEFER(...) __VA_ARGS__ UTIL_EMPTY()
#define UTIL_OBSTRUCT(...) __VA_ARGS__ UTIL_DEFER(UTIL_EMPTY)()
#define UTIL_EXPAND(...) __VA_ARGS__

#define UTIL_EVAL(...)  UTIL_EVAL1(UTIL_EVAL1(UTIL_EVAL1(__VA_ARGS__)))
#define UTIL_EVAL1(...) UTIL_EVAL2(UTIL_EVAL2(UTIL_EVAL2(__VA_ARGS__)))
#define UTIL_EVAL2(...) UTIL_EVAL3(UTIL_EVAL3(UTIL_EVAL3(__VA_ARGS__)))
#define UTIL_EVAL3(...) UTIL_EVAL4(UTIL_EVAL4(UTIL_EVAL4(__VA_ARGS__)))
#define UTIL_EVAL4(...) UTIL_EVAL5(UTIL_EVAL5(UTIL_EVAL5(__VA_ARGS__)))
#define UTIL_EVAL5(...) __VA_ARGS__

#define UTIL_CAT(a, ...) UTIL_PRIMITIVE_CAT(a, __VA_ARGS__)
#define UTIL_PRIMITIVE_CAT(a, ...) a##__VA_ARGS__

#define UTIL_INC(x) UTIL_PRIMITIVE_CAT(UTIL_INC_, x)
#define UTIL_INC_0 1
#define UTIL_INC_1 2
#define UTIL_INC_2 3
#define UTIL_INC_3 4
#define UTIL_INC_4 5
#define UTIL_INC_5 6
#define UTIL_INC_6 7
#define UTIL_INC_7 8
#define UTIL_INC_8 9
#define UTIL_INC_9 10
#define UTIL_INC_10 11
#define UTIL_INC_11 12
#define UTIL_INC_12 13
#define UTIL_INC_13 14
#define UTIL_INC_14 15
#define UTIL_INC_15 16
#define UTIL_INC_16 17
#define UTIL_INC_17 18
#define UTIL_INC_18 19
#define UTIL_INC_19 19

#define UTIL_DEC(x) UTIL_PRIMITIVE_CAT(UTIL_DEC_, x)
#define UTIL_DEC_0 0
#define UTIL_DEC_1 0
#define UTIL_DEC_2 1
#define UTIL_DEC_3 2
#define UTIL_DEC_4 3
#define UTIL_DEC_5 4
#define UTIL_DEC_6 5
#define UTIL_DEC_7 6
#define UTIL_DEC_8 7
#define UTIL_DEC_9 8
#define UTIL_DEC_10 9
#define UTIL_DEC_11 10
#define UTIL_DEC_12 11
#define UTIL_DEC_13 12
#define UTIL_DEC_14 13
#define UTIL_DEC_15 14
#define UTIL_DEC_16 15
#define UTIL_DEC_17 16
#define UTIL_DEC_18 17
#define UTIL_DEC_19 18

#define UTIL_CHECK_N(x, n, ...) n
#define UTIL_CHECK(...) UTIL_CHECK_N(__VA_ARGS__, 0,)

#define UTIL_NOT(x) UTIL_CHECK(UTIL_PRIMITIVE_CAT(UTIL_NOT_, x))
#define UTIL_NOT_0 ~, 1,

#define UTIL_COMPL(b) UTIL_PRIMITIVE_CAT(UTIL_COMPL_, b)
#define UTIL_COMPL_0 1
#define UTIL_COMPL_1 0

#define UTIL_BOOL(x) UTIL_COMPL(UTIL_NOT(x))

#define UTIL_IIF(c) UTIL_PRIMITIVE_CAT(UTIL_IIF_, c)
#define UTIL_IIF_0(t, ...) __VA_ARGS__
#define UTIL_IIF_1(t, ...) t

#define UTIL_IF(c) UTIL_IIF(UTIL_BOOL(c))

#define UTIL_EAT(...)
#define UTIL_EXPAND(...) __VA_ARGS__
#define UTIL_WHEN(c) UTIL_IF(c)(UTIL_EXPAND, UTIL_EAT)

#define UTIL_REPEAT(count, macro, ...)              \
    UTIL_WHEN(count)                    \
    (                           \
        UTIL_OBSTRUCT(UTIL_REPEAT_INDIRECT) ()      \
        (                       \
            UTIL_DEC(count), macro, __VA_ARGS__ \
        )                       \
        UTIL_OBSTRUCT(macro)                \
        (                       \
            UTIL_DEC(count), __VA_ARGS__        \
        )                       \
    )
#define UTIL_REPEAT_INDIRECT() UTIL_REPEAT

#define _STRINGIFY(x) #x
#define STRINGIFY(s) _STRINGIFY(s)

/* ESP Toolchain doesn't support section */
#define ___in_section(a, b, c)
#define __in_section(a, b, c) ___in_section(a, b, c)

#define __in_section_unique(seg) ___in_section(seg, __FILE__, __COUNTER__)


/* ESP Toolchain doesn't support section */
#define __noinit

#define popcount(x) __builtin_popcount(x)


/**
 *
 * @brief find most significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the most significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return most significant bit set, 0 if @a op is 0
 */

#if defined(__GNUC__)
static inline unsigned int find_msb_set(u32_t op)
{
    if (!op) {
        return 0;
    }
    return 32 - __builtin_clz(op);
}
#endif

/**
 *
 * @brief find least significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the least significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return least significant bit set, 0 if @a op is 0
 */

#if defined(__GNUC__)
static inline unsigned int find_lsb_set(u32_t op)
{
    return __builtin_ffs(op);
}
#endif

#define __ASSERT(test, fmt, ...)                                   \
    do {                                                       \
        if (!(test)) {                                     \
            printk("ASSERTION FAIL [%s] @ %s:%d:\n\t", \
                   _STRINGIFY(test),                   \
                   __FILE__,                           \
                   __LINE__);                          \
            printk(fmt, ##__VA_ARGS__);                \
            for (;;)                                   \
                ; /* spin thread */                \
        }                                                  \
    } while ((0))

/**
 * Generates a sequence of code.
 * Useful for generating code like;
 *
 * NRF_PWM0, NRF_PWM1, NRF_PWM2,
 *
 * @arg LEN: The length of the sequence. Must be defined and less than
 * 20.
 *
 * @arg F(i, F_ARG): A macro function that accepts two arguments.
 *  F is called repeatedly, the first argument
 *  is the index in the sequence, and the second argument is the third
 *  argument given to UTIL_LISTIFY.
 *
 * Example:
 *
 *    \#define FOO(i, _) NRF_PWM ## i ,
 *    { UTIL_LISTIFY(PWM_COUNT, FOO) }
 *    // The above two lines will generate the below:
 *    { NRF_PWM0 , NRF_PWM1 , }
 *
 * @note Calling UTIL_LISTIFY with undefined arguments has undefined
 * behaviour.
 */
#define UTIL_LISTIFY(LEN, F, F_ARG) UTIL_EVAL(UTIL_REPEAT(LEN, F, F_ARG))

/**
 *  @brief Put a 16-bit integer as big-endian to arbitrary location.
 *
 *  Put a 16-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 16-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be16(u16_t val, u8_t dst[2])
{
    dst[0] = val >> 8;
    dst[1] = val;
}

/**
 *  @brief Put a 32-bit integer as big-endian to arbitrary location.
 *
 *  Put a 32-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 32-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be32(u32_t val, u8_t dst[4])
{
    sys_put_be16(val >> 16, dst);
    sys_put_be16(val, &dst[2]);
}

/**
 *  @brief Put a 16-bit integer as little-endian to arbitrary location.
 *
 *  Put a 16-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 16-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le16(u16_t val, u8_t dst[2])
{
    dst[0] = val;
    dst[1] = val >> 8;
}

/**
 *  @brief Put a 32-bit integer as little-endian to arbitrary location.
 *
 *  Put a 32-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 32-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le32(u32_t val, u8_t dst[4])
{
    sys_put_le16(val, dst);
    sys_put_le16(val >> 16, &dst[2]);
}

/**
 *  @brief Put a 64-bit integer as little-endian to arbitrary location.
 *
 *  Put a 64-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 64-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le64(u64_t val, u8_t dst[8])
{
    sys_put_le32(val, dst);
    sys_put_le32(val >> 32, &dst[4]);
}

/**
 *  @brief Get a 16-bit integer stored in big-endian format.
 *
 *  Get a 16-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 16-bit integer to get.
 *
 *  @return 16-bit integer in host endianness.
 */
static inline u16_t sys_get_be16(const u8_t src[2])
{
    return ((u16_t)src[0] << 8) | src[1];
}

/**
 *  @brief Get a 32-bit integer stored in big-endian format.
 *
 *  Get a 32-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 32-bit integer to get.
 *
 *  @return 32-bit integer in host endianness.
 */
static inline u32_t sys_get_be32(const u8_t src[4])
{
    return ((u32_t)sys_get_be16(&src[0]) << 16) | sys_get_be16(&src[2]);
}

/**
 *  @brief Get a 16-bit integer stored in little-endian format.
 *
 *  Get a 16-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 16-bit integer to get.
 *
 *  @return 16-bit integer in host endianness.
 */
static inline u16_t sys_get_le16(const u8_t src[2])
{
    return ((u16_t)src[1] << 8) | src[0];
}

/**
 *  @brief Get a 32-bit integer stored in little-endian format.
 *
 *  Get a 32-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 32-bit integer to get.
 *
 *  @return 32-bit integer in host endianness.
 */
static inline u32_t sys_get_le32(const u8_t src[4])
{
    return ((u32_t)sys_get_le16(&src[2]) << 16) | sys_get_le16(&src[0]);
}

/**
 *  @brief Get a 64-bit integer stored in little-endian format.
 *
 *  Get a 64-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 64-bit integer to get.
 *
 *  @return 64-bit integer in host endianness.
 */
static inline u64_t sys_get_le64(const u8_t src[8])
{
    return ((u64_t)sys_get_le32(&src[4]) << 32) | sys_get_le32(&src[0]);
}


const char *bt_hex(const void *buf, size_t len);

void mem_rcopy(u8_t *dst, u8_t const *src, u16_t len);

void _set(void *to, uint8_t val, unsigned int len);

unsigned int _copy(uint8_t *to, unsigned int to_len,
                   const uint8_t *from, unsigned int from_len);

void _set(void *to, uint8_t val, unsigned int len);

uint8_t _double_byte(uint8_t a);

int _compare(const uint8_t *a, const uint8_t *b, size_t size);

/**
 * @brief Swap one buffer content into another
 *
 * Copy the content of src buffer into dst buffer in reversed order,
 * i.e.: src[n] will be put in dst[end-n]
 * Where n is an index and 'end' the last index in both arrays.
 * The 2 memory pointers must be pointing to different areas, and have
 * a minimum size of given length.
 *
 * @param dst A valid pointer on a memory area where to copy the data in
 * @param src A valid pointer on a memory area where to copy the data from
 * @param length Size of both dst and src memory areas
 */
static inline void sys_memcpy_swap(void *dst, const void *src, size_t length)
{
    __ASSERT(((src < dst && (src + length) <= dst) ||
              (src > dst && (dst + length) <= src)),
             "Source and destination buffers must not overlap");

    src += length - 1;

    for (; length > 0; length--) {
        *((u8_t *)dst++) = *((u8_t *)src--);
    }
}

/**
 * @brief Swap buffer content
 *
 * In-place memory swap, where final content will be reversed.
 * I.e.: buf[n] will be put in buf[end-n]
 * Where n is an index and 'end' the last index of buf.
 *
 * @param buf A valid pointer on a memory area to swap
 * @param length Size of buf memory area
 */
static inline void sys_mem_swap(void *buf, size_t length)
{
    size_t i;

    for (i = 0; i < (length / 2); i++) {
        u8_t tmp = ((u8_t *)buf)[i];

        ((u8_t *)buf)[i] = ((u8_t *)buf)[length - 1 - i];
        ((u8_t *)buf)[length - 1 - i] = tmp;
    }
}


#ifdef __cplusplus
}
#endif

#endif /* _UTIL__H_ */
