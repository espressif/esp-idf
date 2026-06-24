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

#ifndef ZEPHYR_INCLUDE_SYS_UTIL_H_
#define ZEPHYR_INCLUDE_SYS_UTIL_H_

#include <zephyr/autoconf.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/toolchain.h>

/* needs to be outside _ASMLANGUAGE so 'true' and 'false' can turn
 * into '1' and '0' for asm or linker scripts
 */
#include <stdbool.h>

#ifndef _ASMLANGUAGE

#include <zephyr/sys/__assert.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/** @brief Number of bits that make up a type */
#define NUM_BITS(t) (sizeof(t) * BITS_PER_BYTE)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sys-util Utility Functions
 * @since 2.4
 * @version 0.1.0
 * @ingroup utilities
 * @{
 */

/** @brief Cast @p x, a pointer, to an unsigned integer. */
#define POINTER_TO_UINT(x) ((uintptr_t) (x))
/** @brief Cast @p x, an unsigned integer, to a <tt>void*</tt>. */
#define UINT_TO_POINTER(x) ((void *) (uintptr_t) (x))
/** @brief Cast @p x, a pointer, to a signed integer. */
#define POINTER_TO_INT(x)  ((intptr_t) (x))
/** @brief Cast @p x, a signed integer, to a <tt>void*</tt>. */
#define INT_TO_POINTER(x)  ((void *) (intptr_t) (x))

#if !(defined(__CHAR_BIT__) && defined(__SIZEOF_LONG__) && defined(__SIZEOF_LONG_LONG__))
#   error Missing required predefined macros for BITS_PER_LONG calculation
#endif

/** Number of bits in a byte. */
#define BITS_PER_BYTE (__CHAR_BIT__)

/** Number of bits in a nibble. */
#define BITS_PER_NIBBLE (__CHAR_BIT__ / 2)

/** Number of nibbles in a byte. */
#define NIBBLES_PER_BYTE (BITS_PER_BYTE / BITS_PER_NIBBLE)

/** Number of bits in a long int. */
#define BITS_PER_LONG   (__CHAR_BIT__ * __SIZEOF_LONG__)

/** Number of bits in a long long int. */
#define BITS_PER_LONG_LONG  (__CHAR_BIT__ * __SIZEOF_LONG_LONG__)

/**
 * @brief Create a contiguous bitmask starting at bit position @p l
 *        and ending at position @p h.
 */
#define GENMASK(h, l) (((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

/**
 * @brief Create a contiguous 64-bit bitmask starting at bit position @p l
 *        and ending at position @p h.
 */
#define GENMASK64(h, l) (((~0ULL) - (1ULL << (l)) + 1) & (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

/** @brief 0 if @p cond is true-ish; causes a compile error otherwise. */
#define ZERO_OR_COMPILE_ERROR(cond) ((int)sizeof(char[1 - (2 * !(cond))]) - 1)

#if defined(__cplusplus)

/* The built-in function used below for type checking in C is not
 * supported by GNU C++.
 */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#else /* __cplusplus */

/**
 * @brief Zero if @p array has an array type, a compile error otherwise
 *
 * This macro is available only from C, not C++.
 */
#define IS_ARRAY(array) \
    ZERO_OR_COMPILE_ERROR( \
        !__builtin_types_compatible_p(__typeof__(array), __typeof__(&(array)[0])))

#ifndef ARRAY_SIZE
/**
 * @brief Number of elements in the given @p array
 *
 * In C++, due to language limitations, this will accept as @p array
 * any type that implements <tt>operator[]</tt>. The results may not be
 * particularly meaningful in this case.
 *
 * In C, passing a pointer as @p array causes a compile error.
 */
#define ARRAY_SIZE(array) ((size_t)(IS_ARRAY(array) + (sizeof(array) / sizeof((array)[0]))))
#endif

#endif /* __cplusplus */

/**
 * @brief Declare a flexible array member.
 *
 * This macro declares a flexible array member in a struct. The member
 * is named @p name and has type @p type.
 *
 * Since C99, flexible arrays are part of the C standard, but for historical
 * reasons many places still use an older GNU extension that is declare
 * zero length arrays.
 *
 * Although zero length arrays are flexible arrays, we can't blindly
 * replace [0] with [] because of some syntax limitations. This macro
 * workaround these limitations.
 *
 * It is specially useful for cases where flexible arrays are
 * used in unions or are not the last element in the struct.
 */
#define FLEXIBLE_ARRAY_DECLARE(type, name) \
    struct { \
        struct {                                                                           \
        } __unused_##name;                                                                 \
        type name[]; \
    }

/**
 * @brief Whether @p ptr is an element of @p array
 *
 * This macro can be seen as a slightly stricter version of @ref PART_OF_ARRAY
 * in that it also ensures that @p ptr is aligned to an array-element boundary
 * of @p array.
 *
 * In C, passing a pointer as @p array causes a compile error.
 *
 * @param array the array in question
 * @param ptr the pointer to check
 *
 * @return 1 if @p ptr is part of @p array, 0 otherwise
 */
#define IS_ARRAY_ELEMENT(array, ptr)                                                               \
    ((ptr) && POINTER_TO_UINT(array) <= POINTER_TO_UINT(ptr) &&                          \
     POINTER_TO_UINT(ptr) < POINTER_TO_UINT(&(array)[ARRAY_SIZE(array)]) &&                    \
     (POINTER_TO_UINT(ptr) - POINTER_TO_UINT(array)) % sizeof((array)[0]) == 0)

/**
 * @brief Index of @p ptr within @p array
 *
 * With `CONFIG_ASSERT=y`, this macro will trigger a runtime assertion
 * when @p ptr does not fall into the range of @p array or when @p ptr
 * is not aligned to an array-element boundary of @p array.
 *
 * In C, passing a pointer as @p array causes a compile error.
 *
 * @param array the array in question
 * @param ptr pointer to an element of @p array
 *
 * @return the array index of @p ptr within @p array, on success
 */
#define ARRAY_INDEX(array, ptr)                                                                    \
    ({                                                                                         \
        __ASSERT_NO_MSG(IS_ARRAY_ELEMENT(array, ptr));                                     \
        (__typeof__((array)[0]) *)(ptr) - (array);                                         \
    })

/**
 * @brief Check if a pointer @p ptr lies within @p array.
 *
 * In C but not C++, this causes a compile error if @p array is not an array
 * (e.g. if @p ptr and @p array are mixed up).
 *
 * @param array an array
 * @param ptr a pointer
 * @return 1 if @p ptr is part of @p array, 0 otherwise
 */
#define PART_OF_ARRAY(array, ptr)                                                                  \
    ((ptr) && POINTER_TO_UINT(array) <= POINTER_TO_UINT(ptr) &&                                \
     POINTER_TO_UINT(ptr) < POINTER_TO_UINT(&(array)[ARRAY_SIZE(array)]))

/**
 * @brief Array-index of @p ptr within @p array, rounded down
 *
 * This macro behaves much like @ref ARRAY_INDEX with the notable
 * difference that it accepts any @p ptr in the range of @p array rather than
 * exclusively a @p ptr aligned to an array-element boundary of @p array.
 *
 * With `CONFIG_ASSERT=y`, this macro will trigger a runtime assertion
 * when @p ptr does not fall into the range of @p array.
 *
 * In C, passing a pointer as @p array causes a compile error.
 *
 * @param array the array in question
 * @param ptr pointer to an element of @p array
 *
 * @return the array index of @p ptr within @p array, on success
 */
#define ARRAY_INDEX_FLOOR(array, ptr)                                                              \
    ({                                                                                         \
        __ASSERT_NO_MSG(PART_OF_ARRAY(array, ptr));                                        \
        (POINTER_TO_UINT(ptr) - POINTER_TO_UINT(array)) / sizeof((array)[0]);              \
    })

/**
 * @brief Iterate over members of an array using an index variable
 *
 * @param array the array in question
 * @param idx name of array index variable
 */
#define ARRAY_FOR_EACH(array, idx) for (size_t idx = 0; (idx) < ARRAY_SIZE(array); ++(idx))

/**
 * @brief Iterate over members of an array using a pointer
 *
 * @param array the array in question
 * @param ptr pointer to an element of @p array
 */
#define ARRAY_FOR_EACH_PTR(array, ptr)                                                             \
    for (__typeof__(*(array)) *ptr = (array); (size_t)((ptr) - (array)) < ARRAY_SIZE(array);   \
         ++(ptr))

/**
 * @brief Validate if two entities have a compatible type
 *
 * @param a the first entity to be compared
 * @param b the second entity to be compared
 * @return 1 if the two elements are compatible, 0 if they are not
 */
#define SAME_TYPE(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))

/**
 * @brief Validate CONTAINER_OF parameters, only applies to C mode.
 */
#ifndef __cplusplus
#define CONTAINER_OF_VALIDATE(ptr, type, field)               \
    BUILD_ASSERT(SAME_TYPE(*(ptr), ((type *)0)->field) || SAME_TYPE(*(ptr), void),             \
             "pointer type mismatch in CONTAINER_OF");
#else
#define CONTAINER_OF_VALIDATE(ptr, type, field)
#endif

/**
 * @brief Get a pointer to a structure containing the element
 *
 * Example:
 *
 *  struct foo {
 *      int bar;
 *  };
 *
 *  struct foo my_foo;
 *  int *ptr = &my_foo.bar;
 *
 *  struct foo *container = CONTAINER_OF(ptr, struct foo, bar);
 *
 * Above, @p container points at @p my_foo.
 *
 * @param ptr pointer to a structure element
 * @param type name of the type that @p ptr is an element of
 * @param field the name of the field within the struct @p ptr points to
 * @return a pointer to the structure that contains @p ptr
 */
#define CONTAINER_OF(ptr, type, field)                               \
    ({                                                           \
        CONTAINER_OF_VALIDATE(ptr, type, field)              \
        ((type *)(((char *)(ptr)) - offsetof(type, field))); \
    })

/**
 * @brief Report the size of a struct field in bytes.
 *
 * @param type The structure containing the field of interest.
 * @param member The field to return the size of.
 *
 * @return The field size.
 */
#define SIZEOF_FIELD(type, member) sizeof((((type *)0)->member))

/**
 * @brief Concatenate input arguments
 *
 * Concatenate provided tokens into a combined token during the preprocessor pass.
 * This can be used to, for ex., build an identifier out of multiple parts,
 * where one of those parts may be, for ex, a number, another macro, or a macro argument.
 *
 * @param ... Tokens to concatencate
 *
 * @return Concatenated token.
 */
#define CONCAT(...) UTIL_CAT(_CONCAT_, NUM_VA_ARGS_LESS_1(__VA_ARGS__))(__VA_ARGS__)

/**
 * @brief Check if @p ptr is aligned to @p align alignment
 */
#define IS_ALIGNED(ptr, align) (((uintptr_t)(ptr)) % (align) == 0)

/**
 * @brief Value of @p x rounded up to the next multiple of @p align.
 */
#define ROUND_UP(x, align)                                   \
    ((((unsigned long)(x) + ((unsigned long)(align) - 1)) / (unsigned long)(align)) *          \
     (unsigned long)(align))

/**
 * @brief Value of @p x rounded down to the previous multiple of @p align.
 */
#define ROUND_DOWN(x, align)                                 \
    (((unsigned long)(x) / (unsigned long)(align)) * (unsigned long)(align))

/** @brief Value of @p x rounded up to the next word boundary. */
#define WB_UP(x) ROUND_UP(x, sizeof(void *))

/** @brief Value of @p x rounded down to the previous word boundary. */
#define WB_DN(x) ROUND_DOWN(x, sizeof(void *))

/**
 * @brief Divide and round up.
 *
 * Example:
 * @code{.c}
 * DIV_ROUND_UP(1, 2); // 1
 * DIV_ROUND_UP(3, 2); // 2
 * @endcode
 *
 * @param n Numerator.
 * @param d Denominator.
 *
 * @return The result of @p n / @p d, rounded up.
 */
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

/**
 * @brief Divide and round to the nearest integer.
 *
 * Example:
 * @code{.c}
 * DIV_ROUND_CLOSEST(5, 2); // 3
 * DIV_ROUND_CLOSEST(5, -2); // -3
 * DIV_ROUND_CLOSEST(5, 3); // 2
 * @endcode
 *
 * @param n Numerator.
 * @param d Denominator.
 *
 * @return The result of @p n / @p d, rounded to the nearest integer.
 */
#define DIV_ROUND_CLOSEST(n, d) \
    (((((__typeof__(n))-1) < 0) && (((__typeof__(d))-1) < 0) && ((n) < 0) ^ ((d) < 0))         \
         ? ((n) - ((d) / 2)) / (d)                                                         \
         : ((n) + ((d) / 2)) / (d))

/**
 * @cond INTERNAL_HIDDEN
 */
#define Z_INTERNAL_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define Z_INTERNAL_MIN(a, b) (((a) < (b)) ? (a) : (b))

#define _minmax_unique(op, a, b, ua, ub) ({ \
        __typeof__(a) ua = (a);     \
        __typeof__(b) ub = (b);     \
        op(ua, ub);                 \
    })

#define _minmax_cnt(op, a, b, cnt) \
    _minmax_unique(op, a, b, UTIL_CAT(_value_a_, cnt), UTIL_CAT(_value_b_, cnt))

#define _minmax3_unique(op, a, b, c, ua, ub, uc) ({ \
        __typeof__(a) ua = (a);             \
        __typeof__(b) ub = (b);             \
        __typeof__(c) uc = (c);             \
        op(ua, op(ub, uc));                 \
    })

#define _minmax3_cnt(op, a, b, c, cnt)            \
    _minmax3_unique(op, a, b, c,              \
            UTIL_CAT(_value_a_, cnt), \
            UTIL_CAT(_value_b_, cnt), \
            UTIL_CAT(_value_c_, cnt))
/**
 * @endcond
 */

#ifndef MAX
/**
 * @brief Obtain the maximum of two values.
 *
 * @note Arguments are evaluated twice. Use @ref max for a single evaluation
 * version.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @returns Maximum value of @p a and @p b.
 */
#define MAX(a, b) Z_INTERNAL_MAX(a, b)
#endif

#ifndef __cplusplus
/** @brief Return larger value of two provided expressions.
 *
 * Macro ensures that expressions are evaluated only once.
 *
 * @note Macro has limited usage compared to the standard macro as it cannot be
 *   used:
 *   - to generate constant integer, e.g. __aligned(max(4,5))
 *   - static variable, e.g. array like static uint8_t array[max(...)];
 */
#define max(a, b) _minmax_cnt(Z_INTERNAL_MAX, a, b, __COUNTER__)
#endif

/** @brief Return larger value of three provided expressions.
 *
 * Macro ensures that expressions are evaluated only once. See @ref max for
 * macro limitations.
 */
#define max3(a, b, c) _minmax3_cnt(Z_INTERNAL_MAX, a, b, c, __COUNTER__)

#ifndef MIN
/**
 * @brief Obtain the minimum of two values.
 *
 * @note Arguments are evaluated twice. Use @ref min for a single evaluation
 * version.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @returns Minimum value of @p a and @p b.
 */
#define MIN(a, b) Z_INTERNAL_MIN(a, b)
#endif

#ifndef __cplusplus
/** @brief Return smaller value of two provided expressions.
 *
 * Macro ensures that expressions are evaluated only once. See @ref max for
 * macro limitations.
 */
#define min(a, b) _minmax_cnt(Z_INTERNAL_MIN, a, b, __COUNTER__)
#endif

/** @brief Return smaller value of three provided expressions.
 *
 * Macro ensures that expressions are evaluated only once. See @ref max for
 * macro limitations.
 */
#define min3(a, b, c) _minmax3_cnt(Z_INTERNAL_MIN, a, b, c, __COUNTER__)

#ifndef MAX_FROM_LIST
/**
 * @brief Returns the maximum of a single value (base case).
 * @param a The value.
 * @returns The value `a`.
 */
#define Z_MAX_1(a) a

/**
 * @brief Returns the maximum of two values.
 *
 * @note Arguments are evaluated multiple times.
 *
 * @param a First value.
 * @param b Second value.
 * @returns Maximum value of @p a and @p b.
 */
#define Z_MAX_2(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Returns the maximum of three values.
 * @note Arguments may be evaluated multiple times.
 * @param a First value.
 * @param b Second value.
 * @param c Third value.
 * @returns Maximum value of @p a, @p b, and @p c.
 */
#define Z_MAX_3(a, b, c) Z_MAX_2(a, Z_MAX_2(b, c))

/**
 * @brief Returns the maximum of four values.
 * @note Arguments may be evaluated multiple times.
 * @param a First value.
 * @param b Second value.
 * @param c Third value.
 * @param d Fourth value.
 * @returns Maximum value of @p a, @p b, @p c, and @p d.
 */
#define Z_MAX_4(a, b, c, d) Z_MAX_2(Z_MAX_2(a, b), Z_MAX_2(c, d))

/**
 * @brief Returns the maximum of five values.
 * @note Arguments may be evaluated multiple times.
 */
#define Z_MAX_5(a, b, c, d, e) Z_MAX_2(Z_MAX_4(a, b, c, d), e)

/**
 * @brief Returns the maximum of six values.
 * @note Arguments may be evaluated multiple times.
 */
#define Z_MAX_6(a, b, c, d, e, f) Z_MAX_2(Z_MAX_5(a, b, c, d, e), f)

/**
 * @brief Returns the maximum of seven values.
 * @note Arguments may be evaluated multiple times.
 */
#define Z_MAX_7(a, b, c, d, e, f, g) Z_MAX_2(Z_MAX_6(a, b, c, d, e, f), g)

/**
 * @brief Returns the maximum of eight values.
 * @note Arguments may be evaluated multiple times.
 */
#define Z_MAX_8(a, b, c, d, e, f, g, h) Z_MAX_2(Z_MAX_7(a, b, c, d, e, f, g), h)

/**
 * @brief Returns the maximum of nine values.
 * @note Arguments may be evaluated multiple times.
 */
#define Z_MAX_9(a, b, c, d, e, f, g, h, i) Z_MAX_2(Z_MAX_8(a, b, c, d, e, f, g, h), i)

/**
 * @brief Returns the maximum of ten values.
 * @note Arguments may be evaluated multiple times.
 */
#define Z_MAX_10(a, b, c, d, e, f, g, h, i, j) Z_MAX_2(Z_MAX_9(a, b, c, d, e, f, g, h, i), j)

/**
 * @brief Helper macro to select the correct MAX_N macro.
 *
 * This macro uses the argument-counting trick to pick the correct
 * `Z_MAX_N` macro name from the arguments provided to `MAX_FROM_LIST`.
 * The 10th argument (or 11th including `NAME`) effectively becomes the
 * macro name to use.
 *
 * @param _1 Positional argument 1.
 * @param _2 Positional argument 2.
 * @param _3 Positional argument 3.
 * @param _4 Positional argument 4.
 * @param _5 Positional argument 5.
 * @param _6 Positional argument 6.
 * @param _7 Positional argument 7.
 * @param _8 Positional argument 8.
 * @param _9 Positional argument 9.
 * @param _10 Positional argument 10.
 * @param NAME The macro name to be selected.
 * @param ... Additional arguments.
 * @returns The selected macro name `NAME`.
 */
#define Z_GET_MAX_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

/**
 * @brief Finds the maximum value from a list of 1 to 10 arguments.
 *
 * Dispatches to the appropriate internal `Z_MAX_N` macro based on the number of
 * arguments provided.
 *
 * Example Usage:
 *   MAX_FROM_LIST(1, 5, 2)
 *   MAX_FROM_LIST(10)
 *
 * @note Arguments may be evaluated multiple times by the underlying
 *       `Z_MAX_N` macros. Avoid expressions with side effects.
 *
 * @param ... A list of 1 to 10 values to compare.
 * @returns The maximum value among the arguments.
 */
#define MAX_FROM_LIST(...)                                                                         \
    Z_GET_MAX_MACRO(__VA_ARGS__, Z_MAX_10, Z_MAX_9, Z_MAX_8, Z_MAX_7, Z_MAX_6, Z_MAX_5,        \
            Z_MAX_4, Z_MAX_3, Z_MAX_2, Z_MAX_1)(__VA_ARGS__)
#endif

#ifndef CLAMP
/**
 * @brief Clamp a value to a given range.
 *
 * @note Arguments are evaluated multiple times. Use @ref clamp for a single
 * evaluation version.
 *
 * @param val Value to be clamped.
 * @param low Lowest allowed value (inclusive).
 * @param high Highest allowed value (inclusive).
 *
 * @returns Clamped value.
 */
#define CLAMP(val, low, high) (((val) <= (low)) ? (low) : Z_INTERNAL_MIN(val, high))
#endif

#ifndef __cplusplus
/** @brief Return a value clamped to a given range.
 *
 * Macro ensures that expressions are evaluated only once. See @ref max for
 * macro limitations.
 */
#define clamp(val, low, high) ({                                               \
        /* random suffix to avoid naming conflict */                   \
        __typeof__(val) _value_val_ = (val);                           \
        __typeof__(low) _value_low_ = (low);                           \
        __typeof__(high) _value_high_ = (high);                        \
        (_value_val_ < _value_low_)  ? _value_low_ :                   \
        (_value_val_ > _value_high_) ? _value_high_ :                  \
                           _value_val_;                    \
    })
#endif

/**
 * @brief Checks if a value is within range.
 *
 * @note @p val is evaluated twice.
 *
 * @param val Value to be checked.
 * @param min Lower bound (inclusive).
 * @param max Upper bound (inclusive).
 *
 * @retval true If value is within range
 * @retval false If the value is not within range
 */
#define IN_RANGE(val, min, max) ((val) >= (min) && (val) <= (max))

/**
 * Find number of contiguous bits which are not set in the bit mask (32 bits).
 *
 * It is possible to return immediately when requested number of bits is found or
 * iterate over whole mask and return the best fit (smallest from available options).
 *
 * @param[in] mask 32 bit mask.
 * @param[in] num_bits Number of bits to find.
 * @param[in] total_bits Total number of LSB bits that can be used in the mask.
 * @param[in] first_match If true returns when first match is found, else returns the best fit.
 *
 * @retval -1 Contiguous bits not found.
 * @retval non-negative Starting index of the bits group.
 */
int bitmask_find_gap(uint32_t mask, size_t num_bits, size_t total_bits, bool first_match);

/**
 * @brief Is @p x a power of two?
 * @param x value to check
 * @return true if @p x is a power of two, false otherwise
 */
static inline bool is_power_of_two(unsigned int x)
{
    return IS_POWER_OF_TWO(x);
}

/**
 * @brief Is @p p equal to ``NULL``?
 *
 * Some macros may need to check their arguments against NULL to support
 * multiple use-cases, but NULL checks can generate warnings if such a macro
 * is used in contexts where that particular argument can never be NULL.
 *
 * The warnings can be triggered if:
 * a) all macros are expanded (e.g. when using CONFIG_COMPILER_SAVE_TEMPS=y)
 * or
 * b) tracking of macro expansions are turned off (-ftrack-macro-expansion=0)
 *
 * The warnings can be circumvented by using this inline function for doing
 * the NULL check within the macro. The compiler is still able to optimize the
 * NULL check out at a later stage.
 *
 * @param p Pointer to check
 * @return true if @p p is equal to ``NULL``, false otherwise
 */
static inline bool is_null_no_warn(void *p)
{
    return p == NULL;
}

/**
 * @brief Arithmetic shift right
 * @param value value to shift
 * @param shift number of bits to shift
 * @return @p value shifted right by @p shift; opened bit positions are
 *         filled with the sign bit
 */
static inline int64_t arithmetic_shift_right(int64_t value, uint8_t shift)
{
    int64_t sign_ext;

    if (shift == 0U) {
        return value;
    }

    /* extract sign bit */
    sign_ext = (value >> 63) & 1;

    /* make all bits of sign_ext be the same as the value's sign bit */
    sign_ext = -sign_ext;

    /* shift value and fill opened bit positions with sign bit */
    return (value >> shift) | (sign_ext << (64 - shift));
}

/**
 * @brief byte by byte memcpy.
 *
 * Copy `size` bytes of `src` into `dest`. This is guaranteed to be done byte by byte.
 *
 * @param dst Pointer to the destination memory.
 * @param src Pointer to the source of the data.
 * @param size The number of bytes to copy.
 */
static inline void bytecpy(void *dst, const void *src, size_t size)
{
    size_t i;

    for (i = 0; i < size; ++i) {
        ((volatile uint8_t *)dst)[i] = ((volatile const uint8_t *)src)[i];
    }
}

/**
 * @brief byte by byte swap.
 *
 * Swap @a size bytes between memory regions @a a and @a b. This is
 * guaranteed to be done byte by byte.
 *
 * @param a Pointer to the first memory region.
 * @param b Pointer to the second memory region.
 * @param size The number of bytes to swap.
 */
static inline void byteswp(void *a, void *b, size_t size)
{
    uint8_t t;
    uint8_t *aa = (uint8_t *)a;
    uint8_t *bb = (uint8_t *)b;

    for (; size > 0; --size) {
        t = *aa;
        *aa++ = *bb;
        *bb++ = t;
    }
}

/**
 * @brief      Convert a single character into a hexadecimal nibble.
 *
 * @param c     The character to convert
 * @param x     The address of storage for the converted number.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int char2hex(char c, uint8_t *x);

/**
 * @brief      Convert a single hexadecimal nibble into a character.
 *
 * @param c     The number to convert
 * @param x     The address of storage for the converted character.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int hex2char(uint8_t x, char *c);

/**
 * @brief      Convert a binary array into string representation.
 *
 * @param buf     The binary array to convert
 * @param buflen  The length of the binary array to convert
 * @param hex     Address of where to store the string representation.
 * @param hexlen  Size of the storage area for string representation.
 *
 * @return     The length of the converted string, or 0 if an error occurred.
 */
size_t bin2hex(const uint8_t *buf, size_t buflen, char *hex, size_t hexlen);

/**
 * @brief      Convert a hexadecimal string into a binary array.
 *
 * @param hex     The hexadecimal string to convert
 * @param hexlen  The length of the hexadecimal string to convert.
 * @param buf     Address of where to store the binary data
 * @param buflen  Size of the storage area for binary data
 *
 * @return     The length of the binary array, or 0 if an error occurred.
 */
size_t hex2bin(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen);

/**
 * @brief Convert a binary coded decimal (BCD 8421) value to binary.
 *
 * @param bcd BCD 8421 value to convert.
 *
 * @return Binary representation of input value.
 */
static inline uint8_t bcd2bin(uint8_t bcd)
{
    return ((10 * (bcd >> 4)) + (bcd & 0x0F));
}

/**
 * @brief Convert a binary value to binary coded decimal (BCD 8421).
 *
 * @param bin Binary value to convert.
 *
 * @return BCD 8421 representation of input value.
 */
static inline uint8_t bin2bcd(uint8_t bin)
{
    return (((bin / 10) << 4) | (bin % 10));
}

/**
 * @brief      Convert a uint8_t into a decimal string representation.
 *
 * Convert a uint8_t value into its ASCII decimal string representation.
 * The string is terminated if there is enough space in buf.
 *
 * @param buf     Address of where to store the string representation.
 * @param buflen  Size of the storage area for string representation.
 * @param value   The value to convert to decimal string
 *
 * @return     The length of the converted string (excluding terminator if
 *             any), or 0 if an error occurred.
 */
uint8_t u8_to_dec(char *buf, uint8_t buflen, uint8_t value);

/**
 * @brief Sign extend an 8, 16 or 32 bit value using the index bit as sign bit.
 *
 * @param value The value to sign expand.
 * @param index 0 based bit index to sign bit (0 to 31)
 */
static inline int32_t sign_extend(uint32_t value, uint8_t index)
{
    __ASSERT_NO_MSG(index <= 31);

    uint8_t shift = 31 - index;

    return (int32_t)(value << shift) >> shift;
}

/**
 * @brief Sign extend a 64 bit value using the index bit as sign bit.
 *
 * @param value The value to sign expand.
 * @param index 0 based bit index to sign bit (0 to 63)
 */
static inline int64_t sign_extend_64(uint64_t value, uint8_t index)
{
    __ASSERT_NO_MSG(index <= 63);

    uint8_t shift = 63 - index;

    return (int64_t)(value << shift) >> shift;
}

#define __z_log2d(x) (32 - __builtin_clz(x) - 1)
#define __z_log2q(x) (64 - __builtin_clzll(x) - 1)
#define __z_log2(x) (sizeof(__typeof__(x)) > 4 ? __z_log2q(x) : __z_log2d(x))

/**
 * @brief Compute log2(x)
 *
 * @note This macro expands its argument multiple times (to permit use
 *       in constant expressions), which must not have side effects.
 *
 * @param x An unsigned integral value to compute logarithm of (positive only)
 *
 * @return log2(x) when 1 <= x <= max(x), -1 when x < 1
 */
#define LOG2(x) ((x) < 1 ? -1 : __z_log2(x))

/**
 * @brief Compute ceil(log2(x))
 *
 * @note This macro expands its argument multiple times (to permit use
 *       in constant expressions), which must not have side effects.
 *
 * @param x An unsigned integral value
 *
 * @return ceil(log2(x)) when 1 <= x <= max(type(x)), 0 when x < 1
 */
#define LOG2CEIL(x) ((x) <= 1 ? 0 : __z_log2((x) - 1) + 1)

/**
 * @brief Compute next highest power of two
 *
 * Equivalent to 2^ceil(log2(x))
 *
 * @note This macro expands its argument multiple times (to permit use
 *       in constant expressions), which must not have side effects.
 *
 * @param x An unsigned integral value
 *
 * @return 2^ceil(log2(x)) or 0 if 2^ceil(log2(x)) would saturate 64-bits
 */
#define NHPOT(x) ((x) < 1 ? 1 : ((x) > (1ULL<<63) ? 0 : 1ULL << LOG2CEIL(x)))

/**
 * @brief Determine if a buffer exceeds highest address
 *
 * This macro determines if a buffer identified by a starting address @a addr
 * and length @a buflen spans a region of memory that goes beyond the highest
 * possible address (thereby resulting in a pointer overflow).
 *
 * @param addr Buffer starting address
 * @param buflen Length of the buffer
 *
 * @return true if pointer overflow detected, false otherwise
 */
#define Z_DETECT_POINTER_OVERFLOW(addr, buflen)  \
    (((buflen) != 0) && ((UINTPTR_MAX - (uintptr_t)(addr)) <= ((uintptr_t)((buflen) - 1))))

/**
 * @brief XOR n bytes
 *
 * @param dst  Destination of where to store result. Shall be @p len bytes.
 * @param src1 First source. Shall be @p len bytes.
 * @param src2 Second source. Shall be @p len bytes.
 * @param len  Number of bytes to XOR.
 */
static inline void mem_xor_n(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, size_t len)
{
    while (len--) {
        *dst++ = *src1++ ^ *src2++;
    }
}

/**
 * @brief XOR 32 bits
 *
 * @param dst  Destination of where to store result. Shall be 32 bits.
 * @param src1 First source. Shall be 32 bits.
 * @param src2 Second source. Shall be 32 bits.
 */
static inline void mem_xor_32(uint8_t dst[4], const uint8_t src1[4], const uint8_t src2[4])
{
    mem_xor_n(dst, src1, src2, 4U);
}

/**
 * @brief XOR 128 bits
 *
 * @param dst  Destination of where to store result. Shall be 128 bits.
 * @param src1 First source. Shall be 128 bits.
 * @param src2 Second source. Shall be 128 bits.
 */
static inline void mem_xor_128(uint8_t dst[16], const uint8_t src1[16], const uint8_t src2[16])
{
    mem_xor_n(dst, src1, src2, 16);
}

/**
 * @brief Compare memory areas. The same way as `memcmp` it assume areas to be
 * the same length
 *
 * @param m1 First memory area to compare, cannot be NULL even if length is 0
 * @param m2 Second memory area to compare, cannot be NULL even if length is 0
 * @param n First n bytes of @p m1 and @p m2 to compares
 *
 * @returns true if the @p n first bytes of @p m1 and @p m2 are the same, else
 * false
 */
static inline bool util_memeq(const void *m1, const void *m2, size_t n)
{
    return memcmp(m1, m2, n) == 0;
}

/**
 * @brief Compare memory areas and their length
 *
 * If the length are 0, return true.
 *
 * @param m1 First memory area to compare, cannot be NULL even if length is 0
 * @param len1 Length of the first memory area to compare
 * @param m2 Second memory area to compare, cannot be NULL even if length is 0
 * @param len2 Length of the second memory area to compare
 *
 * @returns true if both the length of the memory areas and their content are
 * equal else false
 */
static inline bool util_eq(const void *m1, size_t len1, const void *m2, size_t len2)
{
    return len1 == len2 && (m1 == m2 || util_memeq(m1, m2, len1));
}

/**
 * @brief Returns the number of bits set in a value
 *
 * @param value The value to count number of bits set of
 * @param len The number of octets in @p value
 */
static inline size_t sys_count_bits(const void *value, size_t len)
{
    size_t cnt = 0U;
    size_t i = 0U;

    for (; i < len / sizeof(unsigned int); i++) {
        unsigned int val;
        (void)memcpy(&val, (const uint8_t *)value + i * sizeof(unsigned int),
                     sizeof(unsigned int));

        cnt += __builtin_popcount(val);
    }
    i *= sizeof(unsigned int); /* convert to a uint8_t index for the remainder (if any) */

    for (; i < len; i++) {
        uint8_t value_u8 = ((const uint8_t *)value)[i];

        /* Implements Brian Kernighan’s Algorithm to count bits */
        while (value_u8) {
            value_u8 &= (value_u8 - 1);
            cnt++;
        }
    }

    return cnt;
}

#ifdef __cplusplus
}
#endif

/* This file must be included at the end of the !_ASMLANGUAGE guard.
 * It depends on macros defined in this file above which cannot be forward declared.
 */

#endif /* !_ASMLANGUAGE */

/** @brief Number of bytes in @p x kibibytes */
#ifdef _LINKER
/* This is used in linker scripts so need to avoid type casting there */
#define KB(x) ((x) << 10)
#else
#define KB(x) (((size_t)(x)) << 10)
#endif
/** @brief Number of bytes in @p x mebibytes */
#define MB(x) (KB(x) << 10)
/** @brief Number of bytes in @p x gibibytes */
#define GB(x) (MB(x) << 10)

/** @brief Number of Hz in @p x kHz */
#define KHZ(x) ((x) * 1000)
/** @brief Number of Hz in @p x MHz */
#define MHZ(x) (KHZ(x) * 1000)

/**
 * @brief For the POSIX architecture add a minimal delay in a busy wait loop.
 * For other architectures this is a no-op.
 *
 * In the POSIX ARCH, code takes zero simulated time to execute,
 * so busy wait loops become infinite loops, unless we
 * force the loop to take a bit of time.
 * Include this macro in all busy wait/spin loops
 * so they will also work when building for the POSIX architecture.
 *
 * @param t Time in microseconds we will busy wait
 */
#if defined(CONFIG_ARCH_POSIX)
#define Z_SPIN_DELAY(t) k_busy_wait(t)
#else
#define Z_SPIN_DELAY(t)
#endif

/**
 * @brief Wait for an expression to return true with a timeout
 *
 * Spin on an expression with a timeout and optional delay between iterations
 *
 * Commonly needed when waiting on hardware to complete an asynchronous
 * request to read/write/initialize/reset, but useful for any expression.
 *
 * @param expr Truth expression upon which to poll, e.g.: XYZREG & XYZREG_EN
 * @param timeout Timeout to wait for in microseconds, e.g.: 1000 (1ms)
 * @param delay_stmt Delay statement to perform each poll iteration
 *                   e.g.: NULL, k_yield(), k_msleep(1) or k_busy_wait(1)
 *
 * @retval expr As a boolean return, if false then it has timed out.
 */
#define WAIT_FOR(expr, timeout, delay_stmt)                                                        \
    ({                                                                                         \
        uint32_t _wf_cycle_count = k_us_to_cyc_ceil32(timeout);                            \
        uint32_t _wf_start = k_cycle_get_32();                                             \
        while (!(expr) && (_wf_cycle_count > (k_cycle_get_32() - _wf_start))) {            \
            delay_stmt;                                                                \
            Z_SPIN_DELAY(10);                                                          \
        }                                                                                  \
        (expr);                                                                            \
    })

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_SYS_UTIL_H_ */
