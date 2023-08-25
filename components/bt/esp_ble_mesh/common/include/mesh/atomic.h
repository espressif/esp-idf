/* atomic operations */

/*
 * SPDX-FileCopyrightText: 1997-2015 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_ATOMIC_H_
#define _BLE_MESH_ATOMIC_H_

#include "mesh/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bt_mesh_atomic_t bt_mesh_atomic_val_t;

/**
 * @defgroup atomic_apis Atomic Services APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 *
 * @brief Atomic increment.
 *
 * This routine performs an atomic increment by 1 on @a target.
 *
 * @param target Address of atomic variable.
 *
 * @return Previous value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline bt_mesh_atomic_val_t bt_mesh_atomic_inc(bt_mesh_atomic_t *target)
{
    return bt_mesh_atomic_add(target, 1);
}
#else
extern bt_mesh_atomic_val_t bt_mesh_atomic_inc(bt_mesh_atomic_t *target);
#endif

/**
 *
 * @brief Atomic decrement.
 *
 * This routine performs an atomic decrement by 1 on @a target.
 *
 * @param target Address of atomic variable.
 *
 * @return Previous value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline bt_mesh_atomic_val_t bt_mesh_atomic_dec(bt_mesh_atomic_t *target)
{
    return bt_mesh_atomic_sub(target, 1);
}
#else
extern bt_mesh_atomic_val_t bt_mesh_atomic_dec(bt_mesh_atomic_t *target);
#endif

/**
 *
 * @brief Atomic get.
 *
 * This routine performs an atomic read on @a target.
 *
 * @param target Address of atomic variable.
 *
 * @return Value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline bt_mesh_atomic_val_t bt_mesh_atomic_get(const bt_mesh_atomic_t *target)
{
    return __atomic_load_n(target, __ATOMIC_SEQ_CST);
}
#else
extern bt_mesh_atomic_val_t bt_mesh_atomic_get(const bt_mesh_atomic_t *target);
#endif

/**
 *
 * @brief Atomic get-and-set.
 *
 * This routine atomically sets @a target to @a value and returns
 * the previous value of @a target.
 *
 * @param target Address of atomic variable.
 * @param value Value to write to @a target.
 *
 * @return Previous value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline bt_mesh_atomic_val_t bt_mesh_atomic_set(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value)
{
    /* This builtin, as described by Intel, is not a traditional
     * test-and-set operation, but rather an atomic exchange operation. It
     * writes value into *ptr, and returns the previous contents of *ptr.
     */
    return __atomic_exchange_n(target, value, __ATOMIC_SEQ_CST);
}
#else
extern bt_mesh_atomic_val_t bt_mesh_atomic_set(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value);
#endif

/**
 *
 * @brief Atomic bitwise inclusive OR.
 *
 * This routine atomically sets @a target to the bitwise inclusive OR of
 * @a target and @a value.
 *
 * @param target Address of atomic variable.
 * @param value Value to OR.
 *
 * @return Previous value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline bt_mesh_atomic_val_t bt_mesh_atomic_or(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value)
{
    return __atomic_fetch_or(target, value, __ATOMIC_SEQ_CST);
}
#else
extern bt_mesh_atomic_val_t bt_mesh_atomic_or(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value);
#endif

/**
 *
 * @brief Atomic bitwise AND.
 *
 * This routine atomically sets @a target to the bitwise AND of @a target
 * and @a value.
 *
 * @param target Address of atomic variable.
 * @param value Value to AND.
 *
 * @return Previous value of @a target.
 */
#ifdef CONFIG_ATOMIC_OPERATIONS_BUILTIN
static inline bt_mesh_atomic_val_t bt_mesh_atomic_and(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value)
{
    return __atomic_fetch_and(target, value, __ATOMIC_SEQ_CST);
}
#else
extern bt_mesh_atomic_val_t bt_mesh_atomic_and(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value);
#endif

/**
 * @cond INTERNAL_HIDDEN
 */

#define BLE_MESH_ATOMIC_BITS            (sizeof(bt_mesh_atomic_val_t) * 8)
#define BLE_MESH_ATOMIC_MASK(bit)       (1 << ((bit) & (BLE_MESH_ATOMIC_BITS - 1)))
#define BLE_MESH_ATOMIC_ELEM(addr, bit) ((addr) + ((bit) / BLE_MESH_ATOMIC_BITS))

/**
 * INTERNAL_HIDDEN @endcond
 */

/**
 * @brief Define an array of atomic variables.
 *
 * This macro defines an array of atomic variables containing at least
 * @a num_bits bits.
 *
 * @note
 * If used from file scope, the bits of the array are initialized to zero;
 * if used from within a function, the bits are left uninitialized.
 *
 * @param name Name of array of atomic variables.
 * @param num_bits Number of bits needed.
 */
#define BLE_MESH_ATOMIC_DEFINE(name, num_bits) \
        bt_mesh_atomic_t name[1 + ((num_bits) - 1) / BLE_MESH_ATOMIC_BITS]

/**
 * @brief Atomically test a bit.
 *
 * This routine tests whether bit number @a bit of @a target is set or not.
 * The target may be a single atomic variable or an array of them.
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return 1 if the bit was set, 0 if it wasn't.
 */
static inline int bt_mesh_atomic_test_bit(const bt_mesh_atomic_t *target, int bit)
{
    bt_mesh_atomic_val_t val = bt_mesh_atomic_get(BLE_MESH_ATOMIC_ELEM(target, bit));

    return (1 & (val >> (bit & (BLE_MESH_ATOMIC_BITS - 1))));
}

/**
 * @brief Atomically test and clear a bit.
 *
 * Atomically clear bit number @a bit of @a target and return its old value.
 * The target may be a single atomic variable or an array of them.
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return 1 if the bit was set, 0 if it wasn't.
 */
static inline int bt_mesh_atomic_test_and_clear_bit(bt_mesh_atomic_t *target, int bit)
{
    bt_mesh_atomic_val_t mask = BLE_MESH_ATOMIC_MASK(bit);
    bt_mesh_atomic_val_t old;

    old = bt_mesh_atomic_and(BLE_MESH_ATOMIC_ELEM(target, bit), ~mask);

    return (old & mask) != 0;
}

/**
 * @brief Atomically set a bit.
 *
 * Atomically set bit number @a bit of @a target and return its old value.
 * The target may be a single atomic variable or an array of them.
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return 1 if the bit was set, 0 if it wasn't.
 */
static inline int bt_mesh_atomic_test_and_set_bit(bt_mesh_atomic_t *target, int bit)
{
    bt_mesh_atomic_val_t mask = BLE_MESH_ATOMIC_MASK(bit);
    bt_mesh_atomic_val_t old;

    old = bt_mesh_atomic_or(BLE_MESH_ATOMIC_ELEM(target, bit), mask);

    return (old & mask) != 0;
}

/**
 * @brief Atomically clear a bit.
 *
 * Atomically clear bit number @a bit of @a target.
 * The target may be a single atomic variable or an array of them.
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return N/A
 */
static inline void bt_mesh_atomic_clear_bit(bt_mesh_atomic_t *target, int bit)
{
    bt_mesh_atomic_val_t mask = BLE_MESH_ATOMIC_MASK(bit);

    (void)bt_mesh_atomic_and(BLE_MESH_ATOMIC_ELEM(target, bit), ~mask);
}

/**
 * @brief Atomically set a bit.
 *
 * Atomically set bit number @a bit of @a target.
 * The target may be a single atomic variable or an array of them.
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return N/A
 */
static inline void bt_mesh_atomic_set_bit(bt_mesh_atomic_t *target, int bit)
{
    bt_mesh_atomic_val_t mask = BLE_MESH_ATOMIC_MASK(bit);

    (void)bt_mesh_atomic_or(BLE_MESH_ATOMIC_ELEM(target, bit), mask);
}

/**
 * @brief Atomically set a bit to a given value.
 *
 * Atomically set bit number @a bit of @a target to value @a val.
 * The target may be a single atomic variable or an array of them.
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 * @param val true for 1, false for 0.
 *
 * @return N/A
 */
static inline void bt_mesh_atomic_set_bit_to(bt_mesh_atomic_t *target, int bit, bool val)
{
    bt_mesh_atomic_val_t mask = BLE_MESH_ATOMIC_MASK(bit);

    if (val) {
        (void)bt_mesh_atomic_or(BLE_MESH_ATOMIC_ELEM(target, bit), mask);
    } else {
        (void)bt_mesh_atomic_and(BLE_MESH_ATOMIC_ELEM(target, bit), ~mask);
    }
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_ATOMIC_H_ */
