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

/*
 * SPDX-FileCopyrightText: 2016 Intel Corporation
 * SPDX-FileCopyrightText: 2011-2014 Wind River Systems, Inc.
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh/atomic.h"
#include "mesh/mutex.h"

#ifndef CONFIG_ATOMIC_OPERATIONS_BUILTIN

/**
*
* @brief Atomic get primitive
*
* @param target memory location to read from
*
* This routine provides the atomic get primitive to atomically read
* a value from <target>. It simply does an ordinary load.  Note that <target>
* is expected to be aligned to a 4-byte boundary.
*
* @return The value read from <target>
*/
bt_mesh_atomic_val_t bt_mesh_atomic_get(const bt_mesh_atomic_t *target)
{
    return *target;
}

/**
 *
 * @brief Atomic get-and-set primitive
 *
 * This routine provides the atomic set operator. The <value> is atomically
 * written at <target> and the previous value at <target> is returned.
 *
 * @param target the memory location to write to
 * @param value the value to write
 *
 * @return The previous value from <target>
 */
bt_mesh_atomic_val_t bt_mesh_atomic_set(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value)
{
    bt_mesh_atomic_val_t ret = 0;

    bt_mesh_atomic_lock();

    ret = *target;
    *target = value;

    bt_mesh_atomic_unlock();

    return ret;
}

/**
 *
 * @brief Atomic bitwise inclusive OR primitive
 *
 * This routine provides the atomic bitwise inclusive OR operator. The <value>
 * is atomically bitwise OR'ed with the value at <target>, placing the result
 * at <target>, and the previous value at <target> is returned.
 *
 * @param target the memory location to be modified
 * @param value the value to OR
 *
 * @return The previous value from <target>
 */
bt_mesh_atomic_val_t bt_mesh_atomic_or(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value)
{
    bt_mesh_atomic_val_t ret = 0;

    bt_mesh_atomic_lock();

    ret = *target;
    *target |= value;

    bt_mesh_atomic_unlock();

    return ret;
}

/**
 *
 * @brief Atomic bitwise AND primitive
 *
 * This routine provides the atomic bitwise AND operator. The <value> is
 * atomically bitwise AND'ed with the value at <target>, placing the result
 * at <target>, and the previous value at <target> is returned.
 *
 * @param target the memory location to be modified
 * @param value the value to AND
 *
 * @return The previous value from <target>
 */
bt_mesh_atomic_val_t bt_mesh_atomic_and(bt_mesh_atomic_t *target, bt_mesh_atomic_val_t value)
{
    bt_mesh_atomic_val_t ret = 0;

    bt_mesh_atomic_lock();

    ret = *target;
    *target &= value;

    bt_mesh_atomic_unlock();

    return ret;
}

/**
 *
 * @brief Atomic decrement primitive
 *
 * @param target memory location to decrement
 *
 * This routine provides the atomic decrement operator. The value at <target>
 * is atomically decremented by 1, and the old value from <target> is returned.
 *
 * @return The value from <target> prior to the decrement
 */
bt_mesh_atomic_val_t bt_mesh_atomic_dec(bt_mesh_atomic_t *target)
{
    bt_mesh_atomic_val_t ret = 0;

    bt_mesh_atomic_lock();

    ret = *target;
    (*target)--;

    bt_mesh_atomic_unlock();

    return ret;
}

/**
 *
 * @brief Atomic increment primitive
 *
 * @param target memory location to increment
 *
 * This routine provides the atomic increment operator. The value at <target>
 * is atomically incremented by 1, and the old value from <target> is returned.
 *
 * @return The value from <target> before the increment
 */
bt_mesh_atomic_val_t bt_mesh_atomic_inc(bt_mesh_atomic_t *target)
{
    bt_mesh_atomic_val_t ret = 0;

    bt_mesh_atomic_lock();

    ret = *target;
    (*target)++;

    bt_mesh_atomic_unlock();

    return ret;
}

#endif /* #ifndef CONFIG_ATOMIC_OPERATIONS_BUILTIN */
