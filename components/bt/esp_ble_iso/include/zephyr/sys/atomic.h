/*
 * SPDX-FileCopyrightText: 1997-2015 Wind River Systems, Inc.
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_SYS_ATOMIC_H_
#define ZEPHYR_INCLUDE_SYS_ATOMIC_H_

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t atomic_t;
typedef atomic_t atomic_val_t;
typedef void *atomic_ptr_t;
typedef atomic_ptr_t atomic_ptr_val_t;

#define ATOMIC_INIT(i)                  (i)
#define ATOMIC_PTR_INIT(p)              (p)
#define ATOMIC_BITS                     (sizeof(atomic_val_t) * 8)
#define ATOMIC_MASK(bit)                BIT((unsigned long)(bit) & (ATOMIC_BITS - 1U))
#define ATOMIC_ELEM(addr, bit)          ((addr) + ((bit) / ATOMIC_BITS))
#define ATOMIC_BITMAP_SIZE(num_bits)    (ROUND_UP(num_bits, ATOMIC_BITS) / ATOMIC_BITS)
#define ATOMIC_DEFINE(name, num_bits)   atomic_t name[ATOMIC_BITMAP_SIZE(num_bits)]

static inline bool atomic_cas(atomic_t *target,
                              atomic_val_t old_value,
                              atomic_val_t new_value)
{
    if (*target != old_value) {
        return false;
    }

    *target = new_value;
    return true;
}

static inline atomic_val_t atomic_inc(atomic_t *target)
{
    atomic_val_t ret = 0;

    ret = *target;
    (*target)++;

    return ret;
}

static inline atomic_val_t atomic_dec(atomic_t *target)
{
    atomic_val_t ret = 0;

    ret = *target;
    (*target)--;

    return ret;
}

static inline atomic_val_t atomic_get(const atomic_t *target)
{
    return *target;
}

static inline atomic_val_t atomic_set(atomic_t *target, atomic_val_t value)
{
    atomic_val_t ret = 0;

    ret = *target;
    *target = value;

    return ret;
}

static inline atomic_val_t atomic_clear(atomic_t *target)
{
    return atomic_set(target, 0);
}

static inline atomic_val_t atomic_or(atomic_t *target, atomic_val_t value)
{
    atomic_val_t ret = 0;

    ret = *target;
    *target |= value;

    return ret;
}

static inline atomic_val_t atomic_and(atomic_t *target, atomic_val_t value)
{
    atomic_val_t ret = 0;

    ret = *target;
    *target &= value;

    return ret;
}

static inline void atomic_set_bit_to(atomic_t *target, int bit, bool val)
{
    atomic_val_t mask = ATOMIC_MASK(bit);

    if (val) {
        (void)atomic_or(ATOMIC_ELEM(target, bit), mask);
    } else {
        (void)atomic_and(ATOMIC_ELEM(target, bit), ~mask);
    }
}

static inline bool atomic_test_bit(const atomic_t *target, int bit)
{
    atomic_val_t val = atomic_get(ATOMIC_ELEM(target, bit));

    return (1 & (val >> (bit & (ATOMIC_BITS - 1)))) != 0;
}

static inline void atomic_set_bit(atomic_t *target, int bit)
{
    atomic_val_t mask = ATOMIC_MASK(bit);

    (void)atomic_or(ATOMIC_ELEM(target, bit), mask);
}

static inline void atomic_clear_bit(atomic_t *target, int bit)
{
    atomic_val_t mask = ATOMIC_MASK(bit);

    (void)atomic_and(ATOMIC_ELEM(target, bit), ~mask);
}

static inline bool atomic_test_and_set_bit(atomic_t *target, int bit)
{
    atomic_val_t mask = ATOMIC_MASK(bit);
    atomic_val_t old;

    old = atomic_or(ATOMIC_ELEM(target, bit), mask);

    return (old & mask) != 0;
}

static inline bool atomic_test_and_clear_bit(atomic_t *target, int bit)
{
    atomic_val_t mask = ATOMIC_MASK(bit);
    atomic_val_t old;

    old = atomic_and(ATOMIC_ELEM(target, bit), ~mask);

    return (old & mask) != 0;
}

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_SYS_ATOMIC_H_ */
