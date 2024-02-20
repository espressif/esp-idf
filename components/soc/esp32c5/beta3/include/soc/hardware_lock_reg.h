/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** ATOMIC_ADDR_LOCK_REG register
 *  hardware lock regsiter
 */
#define ATOMIC_ADDR_LOCK_REG (DR_REG_ATOMIC_BASE + 0x0)
/** ATOMIC_LOCK : R/W; bitpos: [1:0]; default: 0;
 *  read to acquire hardware lock, write to release hardware lock
 */
#define ATOMIC_LOCK    0x00000003U
#define ATOMIC_LOCK_M  (ATOMIC_LOCK_V << ATOMIC_LOCK_S)
#define ATOMIC_LOCK_V  0x00000003U
#define ATOMIC_LOCK_S  0

/** ATOMIC_LR_ADDR_REG register
 *  gloable lr address regsiter
 */
#define ATOMIC_LR_ADDR_REG (DR_REG_ATOMIC_BASE + 0x4)
/** ATOMIC_GLOABLE_LR_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  backup gloable address
 */
#define ATOMIC_GLOABLE_LR_ADDR    0xFFFFFFFFU
#define ATOMIC_GLOABLE_LR_ADDR_M  (ATOMIC_GLOABLE_LR_ADDR_V << ATOMIC_GLOABLE_LR_ADDR_S)
#define ATOMIC_GLOABLE_LR_ADDR_V  0xFFFFFFFFU
#define ATOMIC_GLOABLE_LR_ADDR_S  0

/** ATOMIC_LR_VALUE_REG register
 *  gloable lr value regsiter
 */
#define ATOMIC_LR_VALUE_REG (DR_REG_ATOMIC_BASE + 0x8)
/** ATOMIC_GLOABLE_LR_VALUE : R/W; bitpos: [31:0]; default: 0;
 *  backup gloable value
 */
#define ATOMIC_GLOABLE_LR_VALUE    0xFFFFFFFFU
#define ATOMIC_GLOABLE_LR_VALUE_M  (ATOMIC_GLOABLE_LR_VALUE_V << ATOMIC_GLOABLE_LR_VALUE_S)
#define ATOMIC_GLOABLE_LR_VALUE_V  0xFFFFFFFFU
#define ATOMIC_GLOABLE_LR_VALUE_S  0

/** ATOMIC_LOCK_STATUS_REG register
 *  lock status regsiter
 */
#define ATOMIC_LOCK_STATUS_REG (DR_REG_ATOMIC_BASE + 0xc)
/** ATOMIC_LOCK_STATUS : RO; bitpos: [1:0]; default: 0;
 *  read hareware lock status for debug
 */
#define ATOMIC_LOCK_STATUS    0x00000003U
#define ATOMIC_LOCK_STATUS_M  (ATOMIC_LOCK_STATUS_V << ATOMIC_LOCK_STATUS_S)
#define ATOMIC_LOCK_STATUS_V  0x00000003U
#define ATOMIC_LOCK_STATUS_S  0

/** ATOMIC_COUNTER_REG register
 *  wait counter register
 */
#define ATOMIC_COUNTER_REG (DR_REG_ATOMIC_BASE + 0x10)
/** ATOMIC_WAIT_COUNTER : R/W; bitpos: [15:0]; default: 0;
 *  delay counter
 */
#define ATOMIC_WAIT_COUNTER    0x0000FFFFU
#define ATOMIC_WAIT_COUNTER_M  (ATOMIC_WAIT_COUNTER_V << ATOMIC_WAIT_COUNTER_S)
#define ATOMIC_WAIT_COUNTER_V  0x0000FFFFU
#define ATOMIC_WAIT_COUNTER_S  0

#ifdef __cplusplus
}
#endif
