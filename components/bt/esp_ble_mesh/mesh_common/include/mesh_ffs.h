/*
 * Copyright (c) 2015, Wind River Systems, Inc.
 * Copyright (c) 2017, Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_FFS_H_
#define _BLE_MESH_FFS_H_

#include "mesh_types.h"
#include "mesh_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

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

static ALWAYS_INLINE unsigned int find_msb_set(uint32_t op)
{
    if (op == 0) {
        return 0;
    }

    return 32 - __builtin_clz(op);
}

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

static ALWAYS_INLINE unsigned int find_lsb_set(uint32_t op)
{
    return __builtin_ffs(op);
}

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_FFS_H_ */
