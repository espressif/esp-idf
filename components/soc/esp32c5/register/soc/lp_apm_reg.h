/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** LP_APM_REGION_FILTER_EN_REG register
 *  Region enable register
 */
#define LP_APM_REGION_FILTER_EN_REG (DR_REG_LP_APM_BASE + 0x0)
/** LP_APM_REGION_FILTER_EN : R/W; bitpos: [7:0]; default: 1;
 *  Configure bit $n (0-7) to enable region $n (0-7).
 *  0: Disable
 *  1: Enable
 */
#define LP_APM_REGION_FILTER_EN    0x000000FFU
#define LP_APM_REGION_FILTER_EN_M  (LP_APM_REGION_FILTER_EN_V << LP_APM_REGION_FILTER_EN_S)
#define LP_APM_REGION_FILTER_EN_V  0x000000FFU
#define LP_APM_REGION_FILTER_EN_S  0

/** LP_APM_REGION0_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION0_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x4)
/** LP_APM_REGION0_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 0.
 */
#define LP_APM_REGION0_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION0_ADDR_START_M  (LP_APM_REGION0_ADDR_START_V << LP_APM_REGION0_ADDR_START_S)
#define LP_APM_REGION0_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION0_ADDR_START_S  0

/** LP_APM_REGION0_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION0_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x8)
/** LP_APM_REGION0_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 0.
 */
#define LP_APM_REGION0_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION0_ADDR_END_M  (LP_APM_REGION0_ADDR_END_V << LP_APM_REGION0_ADDR_END_S)
#define LP_APM_REGION0_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION0_ADDR_END_S  0

/** LP_APM_REGION0_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION0_ATTR_REG (DR_REG_LP_APM_BASE + 0xc)
/** LP_APM_REGION0_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 0 in REE0 mode.
 */
#define LP_APM_REGION0_R0_X    (BIT(0))
#define LP_APM_REGION0_R0_X_M  (LP_APM_REGION0_R0_X_V << LP_APM_REGION0_R0_X_S)
#define LP_APM_REGION0_R0_X_V  0x00000001U
#define LP_APM_REGION0_R0_X_S  0
/** LP_APM_REGION0_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 0 in REE0 mode.
 */
#define LP_APM_REGION0_R0_W    (BIT(1))
#define LP_APM_REGION0_R0_W_M  (LP_APM_REGION0_R0_W_V << LP_APM_REGION0_R0_W_S)
#define LP_APM_REGION0_R0_W_V  0x00000001U
#define LP_APM_REGION0_R0_W_S  1
/** LP_APM_REGION0_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 0 in REE0 mode.
 */
#define LP_APM_REGION0_R0_R    (BIT(2))
#define LP_APM_REGION0_R0_R_M  (LP_APM_REGION0_R0_R_V << LP_APM_REGION0_R0_R_S)
#define LP_APM_REGION0_R0_R_V  0x00000001U
#define LP_APM_REGION0_R0_R_S  2
/** LP_APM_REGION0_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 0 in REE1 mode.
 */
#define LP_APM_REGION0_R1_X    (BIT(4))
#define LP_APM_REGION0_R1_X_M  (LP_APM_REGION0_R1_X_V << LP_APM_REGION0_R1_X_S)
#define LP_APM_REGION0_R1_X_V  0x00000001U
#define LP_APM_REGION0_R1_X_S  4
/** LP_APM_REGION0_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 0 in REE1 mode.
 */
#define LP_APM_REGION0_R1_W    (BIT(5))
#define LP_APM_REGION0_R1_W_M  (LP_APM_REGION0_R1_W_V << LP_APM_REGION0_R1_W_S)
#define LP_APM_REGION0_R1_W_V  0x00000001U
#define LP_APM_REGION0_R1_W_S  5
/** LP_APM_REGION0_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 0 in REE1 mode.
 */
#define LP_APM_REGION0_R1_R    (BIT(6))
#define LP_APM_REGION0_R1_R_M  (LP_APM_REGION0_R1_R_V << LP_APM_REGION0_R1_R_S)
#define LP_APM_REGION0_R1_R_V  0x00000001U
#define LP_APM_REGION0_R1_R_S  6
/** LP_APM_REGION0_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 0 in REE1 mode.
 */
#define LP_APM_REGION0_R2_X    (BIT(8))
#define LP_APM_REGION0_R2_X_M  (LP_APM_REGION0_R2_X_V << LP_APM_REGION0_R2_X_S)
#define LP_APM_REGION0_R2_X_V  0x00000001U
#define LP_APM_REGION0_R2_X_S  8
/** LP_APM_REGION0_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 0 in REE2 mode.
 */
#define LP_APM_REGION0_R2_W    (BIT(9))
#define LP_APM_REGION0_R2_W_M  (LP_APM_REGION0_R2_W_V << LP_APM_REGION0_R2_W_S)
#define LP_APM_REGION0_R2_W_V  0x00000001U
#define LP_APM_REGION0_R2_W_S  9
/** LP_APM_REGION0_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 0 in REE2 mode.
 */
#define LP_APM_REGION0_R2_R    (BIT(10))
#define LP_APM_REGION0_R2_R_M  (LP_APM_REGION0_R2_R_V << LP_APM_REGION0_R2_R_S)
#define LP_APM_REGION0_R2_R_V  0x00000001U
#define LP_APM_REGION0_R2_R_S  10
/** LP_APM_REGION0_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 0 configuration registers
 *  (LP_APM_REGION0_ADDR_START_REG, LP_APM_REGION0_ADDR_END_REG and
 *  LP_APM_REGION0_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION0_LOCK    (BIT(11))
#define LP_APM_REGION0_LOCK_M  (LP_APM_REGION0_LOCK_V << LP_APM_REGION0_LOCK_S)
#define LP_APM_REGION0_LOCK_V  0x00000001U
#define LP_APM_REGION0_LOCK_S  11

/** LP_APM_REGION1_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION1_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x10)
/** LP_APM_REGION1_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 1.
 */
#define LP_APM_REGION1_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION1_ADDR_START_M  (LP_APM_REGION1_ADDR_START_V << LP_APM_REGION1_ADDR_START_S)
#define LP_APM_REGION1_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION1_ADDR_START_S  0

/** LP_APM_REGION1_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION1_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x14)
/** LP_APM_REGION1_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 1.
 */
#define LP_APM_REGION1_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION1_ADDR_END_M  (LP_APM_REGION1_ADDR_END_V << LP_APM_REGION1_ADDR_END_S)
#define LP_APM_REGION1_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION1_ADDR_END_S  0

/** LP_APM_REGION1_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION1_ATTR_REG (DR_REG_LP_APM_BASE + 0x18)
/** LP_APM_REGION1_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 1 in REE0 mode.
 */
#define LP_APM_REGION1_R0_X    (BIT(0))
#define LP_APM_REGION1_R0_X_M  (LP_APM_REGION1_R0_X_V << LP_APM_REGION1_R0_X_S)
#define LP_APM_REGION1_R0_X_V  0x00000001U
#define LP_APM_REGION1_R0_X_S  0
/** LP_APM_REGION1_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 1 in REE0 mode.
 */
#define LP_APM_REGION1_R0_W    (BIT(1))
#define LP_APM_REGION1_R0_W_M  (LP_APM_REGION1_R0_W_V << LP_APM_REGION1_R0_W_S)
#define LP_APM_REGION1_R0_W_V  0x00000001U
#define LP_APM_REGION1_R0_W_S  1
/** LP_APM_REGION1_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 1 in REE0 mode.
 */
#define LP_APM_REGION1_R0_R    (BIT(2))
#define LP_APM_REGION1_R0_R_M  (LP_APM_REGION1_R0_R_V << LP_APM_REGION1_R0_R_S)
#define LP_APM_REGION1_R0_R_V  0x00000001U
#define LP_APM_REGION1_R0_R_S  2
/** LP_APM_REGION1_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 1 in REE1 mode.
 */
#define LP_APM_REGION1_R1_X    (BIT(4))
#define LP_APM_REGION1_R1_X_M  (LP_APM_REGION1_R1_X_V << LP_APM_REGION1_R1_X_S)
#define LP_APM_REGION1_R1_X_V  0x00000001U
#define LP_APM_REGION1_R1_X_S  4
/** LP_APM_REGION1_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 1 in REE1 mode.
 */
#define LP_APM_REGION1_R1_W    (BIT(5))
#define LP_APM_REGION1_R1_W_M  (LP_APM_REGION1_R1_W_V << LP_APM_REGION1_R1_W_S)
#define LP_APM_REGION1_R1_W_V  0x00000001U
#define LP_APM_REGION1_R1_W_S  5
/** LP_APM_REGION1_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 1 in REE1 mode.
 */
#define LP_APM_REGION1_R1_R    (BIT(6))
#define LP_APM_REGION1_R1_R_M  (LP_APM_REGION1_R1_R_V << LP_APM_REGION1_R1_R_S)
#define LP_APM_REGION1_R1_R_V  0x00000001U
#define LP_APM_REGION1_R1_R_S  6
/** LP_APM_REGION1_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 1 in REE1 mode.
 */
#define LP_APM_REGION1_R2_X    (BIT(8))
#define LP_APM_REGION1_R2_X_M  (LP_APM_REGION1_R2_X_V << LP_APM_REGION1_R2_X_S)
#define LP_APM_REGION1_R2_X_V  0x00000001U
#define LP_APM_REGION1_R2_X_S  8
/** LP_APM_REGION1_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 1 in REE2 mode.
 */
#define LP_APM_REGION1_R2_W    (BIT(9))
#define LP_APM_REGION1_R2_W_M  (LP_APM_REGION1_R2_W_V << LP_APM_REGION1_R2_W_S)
#define LP_APM_REGION1_R2_W_V  0x00000001U
#define LP_APM_REGION1_R2_W_S  9
/** LP_APM_REGION1_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 1 in REE2 mode.
 */
#define LP_APM_REGION1_R2_R    (BIT(10))
#define LP_APM_REGION1_R2_R_M  (LP_APM_REGION1_R2_R_V << LP_APM_REGION1_R2_R_S)
#define LP_APM_REGION1_R2_R_V  0x00000001U
#define LP_APM_REGION1_R2_R_S  10
/** LP_APM_REGION1_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 1 configuration registers
 *  (LP_APM_REGION1_ADDR_START_REG, LP_APM_REGION1_ADDR_END_REG and
 *  LP_APM_REGION1_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION1_LOCK    (BIT(11))
#define LP_APM_REGION1_LOCK_M  (LP_APM_REGION1_LOCK_V << LP_APM_REGION1_LOCK_S)
#define LP_APM_REGION1_LOCK_V  0x00000001U
#define LP_APM_REGION1_LOCK_S  11

/** LP_APM_REGION2_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION2_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x1c)
/** LP_APM_REGION2_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 2.
 */
#define LP_APM_REGION2_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION2_ADDR_START_M  (LP_APM_REGION2_ADDR_START_V << LP_APM_REGION2_ADDR_START_S)
#define LP_APM_REGION2_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION2_ADDR_START_S  0

/** LP_APM_REGION2_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION2_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x20)
/** LP_APM_REGION2_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 2.
 */
#define LP_APM_REGION2_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION2_ADDR_END_M  (LP_APM_REGION2_ADDR_END_V << LP_APM_REGION2_ADDR_END_S)
#define LP_APM_REGION2_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION2_ADDR_END_S  0

/** LP_APM_REGION2_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION2_ATTR_REG (DR_REG_LP_APM_BASE + 0x24)
/** LP_APM_REGION2_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 2 in REE0 mode.
 */
#define LP_APM_REGION2_R0_X    (BIT(0))
#define LP_APM_REGION2_R0_X_M  (LP_APM_REGION2_R0_X_V << LP_APM_REGION2_R0_X_S)
#define LP_APM_REGION2_R0_X_V  0x00000001U
#define LP_APM_REGION2_R0_X_S  0
/** LP_APM_REGION2_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 2 in REE0 mode.
 */
#define LP_APM_REGION2_R0_W    (BIT(1))
#define LP_APM_REGION2_R0_W_M  (LP_APM_REGION2_R0_W_V << LP_APM_REGION2_R0_W_S)
#define LP_APM_REGION2_R0_W_V  0x00000001U
#define LP_APM_REGION2_R0_W_S  1
/** LP_APM_REGION2_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 2 in REE0 mode.
 */
#define LP_APM_REGION2_R0_R    (BIT(2))
#define LP_APM_REGION2_R0_R_M  (LP_APM_REGION2_R0_R_V << LP_APM_REGION2_R0_R_S)
#define LP_APM_REGION2_R0_R_V  0x00000001U
#define LP_APM_REGION2_R0_R_S  2
/** LP_APM_REGION2_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 2 in REE1 mode.
 */
#define LP_APM_REGION2_R1_X    (BIT(4))
#define LP_APM_REGION2_R1_X_M  (LP_APM_REGION2_R1_X_V << LP_APM_REGION2_R1_X_S)
#define LP_APM_REGION2_R1_X_V  0x00000001U
#define LP_APM_REGION2_R1_X_S  4
/** LP_APM_REGION2_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 2 in REE1 mode.
 */
#define LP_APM_REGION2_R1_W    (BIT(5))
#define LP_APM_REGION2_R1_W_M  (LP_APM_REGION2_R1_W_V << LP_APM_REGION2_R1_W_S)
#define LP_APM_REGION2_R1_W_V  0x00000001U
#define LP_APM_REGION2_R1_W_S  5
/** LP_APM_REGION2_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 2 in REE1 mode.
 */
#define LP_APM_REGION2_R1_R    (BIT(6))
#define LP_APM_REGION2_R1_R_M  (LP_APM_REGION2_R1_R_V << LP_APM_REGION2_R1_R_S)
#define LP_APM_REGION2_R1_R_V  0x00000001U
#define LP_APM_REGION2_R1_R_S  6
/** LP_APM_REGION2_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 2 in REE1 mode.
 */
#define LP_APM_REGION2_R2_X    (BIT(8))
#define LP_APM_REGION2_R2_X_M  (LP_APM_REGION2_R2_X_V << LP_APM_REGION2_R2_X_S)
#define LP_APM_REGION2_R2_X_V  0x00000001U
#define LP_APM_REGION2_R2_X_S  8
/** LP_APM_REGION2_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 2 in REE2 mode.
 */
#define LP_APM_REGION2_R2_W    (BIT(9))
#define LP_APM_REGION2_R2_W_M  (LP_APM_REGION2_R2_W_V << LP_APM_REGION2_R2_W_S)
#define LP_APM_REGION2_R2_W_V  0x00000001U
#define LP_APM_REGION2_R2_W_S  9
/** LP_APM_REGION2_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 2 in REE2 mode.
 */
#define LP_APM_REGION2_R2_R    (BIT(10))
#define LP_APM_REGION2_R2_R_M  (LP_APM_REGION2_R2_R_V << LP_APM_REGION2_R2_R_S)
#define LP_APM_REGION2_R2_R_V  0x00000001U
#define LP_APM_REGION2_R2_R_S  10
/** LP_APM_REGION2_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 2 configuration registers
 *  (LP_APM_REGION2_ADDR_START_REG, LP_APM_REGION2_ADDR_END_REG and
 *  LP_APM_REGION2_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION2_LOCK    (BIT(11))
#define LP_APM_REGION2_LOCK_M  (LP_APM_REGION2_LOCK_V << LP_APM_REGION2_LOCK_S)
#define LP_APM_REGION2_LOCK_V  0x00000001U
#define LP_APM_REGION2_LOCK_S  11

/** LP_APM_REGION3_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION3_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x28)
/** LP_APM_REGION3_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 3.
 */
#define LP_APM_REGION3_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION3_ADDR_START_M  (LP_APM_REGION3_ADDR_START_V << LP_APM_REGION3_ADDR_START_S)
#define LP_APM_REGION3_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION3_ADDR_START_S  0

/** LP_APM_REGION3_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION3_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x2c)
/** LP_APM_REGION3_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 3.
 */
#define LP_APM_REGION3_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION3_ADDR_END_M  (LP_APM_REGION3_ADDR_END_V << LP_APM_REGION3_ADDR_END_S)
#define LP_APM_REGION3_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION3_ADDR_END_S  0

/** LP_APM_REGION3_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION3_ATTR_REG (DR_REG_LP_APM_BASE + 0x30)
/** LP_APM_REGION3_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 3 in REE0 mode.
 */
#define LP_APM_REGION3_R0_X    (BIT(0))
#define LP_APM_REGION3_R0_X_M  (LP_APM_REGION3_R0_X_V << LP_APM_REGION3_R0_X_S)
#define LP_APM_REGION3_R0_X_V  0x00000001U
#define LP_APM_REGION3_R0_X_S  0
/** LP_APM_REGION3_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 3 in REE0 mode.
 */
#define LP_APM_REGION3_R0_W    (BIT(1))
#define LP_APM_REGION3_R0_W_M  (LP_APM_REGION3_R0_W_V << LP_APM_REGION3_R0_W_S)
#define LP_APM_REGION3_R0_W_V  0x00000001U
#define LP_APM_REGION3_R0_W_S  1
/** LP_APM_REGION3_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 3 in REE0 mode.
 */
#define LP_APM_REGION3_R0_R    (BIT(2))
#define LP_APM_REGION3_R0_R_M  (LP_APM_REGION3_R0_R_V << LP_APM_REGION3_R0_R_S)
#define LP_APM_REGION3_R0_R_V  0x00000001U
#define LP_APM_REGION3_R0_R_S  2
/** LP_APM_REGION3_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 3 in REE1 mode.
 */
#define LP_APM_REGION3_R1_X    (BIT(4))
#define LP_APM_REGION3_R1_X_M  (LP_APM_REGION3_R1_X_V << LP_APM_REGION3_R1_X_S)
#define LP_APM_REGION3_R1_X_V  0x00000001U
#define LP_APM_REGION3_R1_X_S  4
/** LP_APM_REGION3_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 3 in REE1 mode.
 */
#define LP_APM_REGION3_R1_W    (BIT(5))
#define LP_APM_REGION3_R1_W_M  (LP_APM_REGION3_R1_W_V << LP_APM_REGION3_R1_W_S)
#define LP_APM_REGION3_R1_W_V  0x00000001U
#define LP_APM_REGION3_R1_W_S  5
/** LP_APM_REGION3_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 3 in REE1 mode.
 */
#define LP_APM_REGION3_R1_R    (BIT(6))
#define LP_APM_REGION3_R1_R_M  (LP_APM_REGION3_R1_R_V << LP_APM_REGION3_R1_R_S)
#define LP_APM_REGION3_R1_R_V  0x00000001U
#define LP_APM_REGION3_R1_R_S  6
/** LP_APM_REGION3_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 3 in REE1 mode.
 */
#define LP_APM_REGION3_R2_X    (BIT(8))
#define LP_APM_REGION3_R2_X_M  (LP_APM_REGION3_R2_X_V << LP_APM_REGION3_R2_X_S)
#define LP_APM_REGION3_R2_X_V  0x00000001U
#define LP_APM_REGION3_R2_X_S  8
/** LP_APM_REGION3_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 3 in REE2 mode.
 */
#define LP_APM_REGION3_R2_W    (BIT(9))
#define LP_APM_REGION3_R2_W_M  (LP_APM_REGION3_R2_W_V << LP_APM_REGION3_R2_W_S)
#define LP_APM_REGION3_R2_W_V  0x00000001U
#define LP_APM_REGION3_R2_W_S  9
/** LP_APM_REGION3_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 3 in REE2 mode.
 */
#define LP_APM_REGION3_R2_R    (BIT(10))
#define LP_APM_REGION3_R2_R_M  (LP_APM_REGION3_R2_R_V << LP_APM_REGION3_R2_R_S)
#define LP_APM_REGION3_R2_R_V  0x00000001U
#define LP_APM_REGION3_R2_R_S  10
/** LP_APM_REGION3_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 3 configuration registers
 *  (LP_APM_REGION3_ADDR_START_REG, LP_APM_REGION3_ADDR_END_REG and
 *  LP_APM_REGION3_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION3_LOCK    (BIT(11))
#define LP_APM_REGION3_LOCK_M  (LP_APM_REGION3_LOCK_V << LP_APM_REGION3_LOCK_S)
#define LP_APM_REGION3_LOCK_V  0x00000001U
#define LP_APM_REGION3_LOCK_S  11

/** LP_APM_REGION4_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION4_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x34)
/** LP_APM_REGION4_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 4.
 */
#define LP_APM_REGION4_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION4_ADDR_START_M  (LP_APM_REGION4_ADDR_START_V << LP_APM_REGION4_ADDR_START_S)
#define LP_APM_REGION4_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION4_ADDR_START_S  0

/** LP_APM_REGION4_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION4_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x38)
/** LP_APM_REGION4_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 4.
 */
#define LP_APM_REGION4_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION4_ADDR_END_M  (LP_APM_REGION4_ADDR_END_V << LP_APM_REGION4_ADDR_END_S)
#define LP_APM_REGION4_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION4_ADDR_END_S  0

/** LP_APM_REGION4_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION4_ATTR_REG (DR_REG_LP_APM_BASE + 0x3c)
/** LP_APM_REGION4_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 4 in REE0 mode.
 */
#define LP_APM_REGION4_R0_X    (BIT(0))
#define LP_APM_REGION4_R0_X_M  (LP_APM_REGION4_R0_X_V << LP_APM_REGION4_R0_X_S)
#define LP_APM_REGION4_R0_X_V  0x00000001U
#define LP_APM_REGION4_R0_X_S  0
/** LP_APM_REGION4_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 4 in REE0 mode.
 */
#define LP_APM_REGION4_R0_W    (BIT(1))
#define LP_APM_REGION4_R0_W_M  (LP_APM_REGION4_R0_W_V << LP_APM_REGION4_R0_W_S)
#define LP_APM_REGION4_R0_W_V  0x00000001U
#define LP_APM_REGION4_R0_W_S  1
/** LP_APM_REGION4_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 4 in REE0 mode.
 */
#define LP_APM_REGION4_R0_R    (BIT(2))
#define LP_APM_REGION4_R0_R_M  (LP_APM_REGION4_R0_R_V << LP_APM_REGION4_R0_R_S)
#define LP_APM_REGION4_R0_R_V  0x00000001U
#define LP_APM_REGION4_R0_R_S  2
/** LP_APM_REGION4_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 4 in REE1 mode.
 */
#define LP_APM_REGION4_R1_X    (BIT(4))
#define LP_APM_REGION4_R1_X_M  (LP_APM_REGION4_R1_X_V << LP_APM_REGION4_R1_X_S)
#define LP_APM_REGION4_R1_X_V  0x00000001U
#define LP_APM_REGION4_R1_X_S  4
/** LP_APM_REGION4_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 4 in REE1 mode.
 */
#define LP_APM_REGION4_R1_W    (BIT(5))
#define LP_APM_REGION4_R1_W_M  (LP_APM_REGION4_R1_W_V << LP_APM_REGION4_R1_W_S)
#define LP_APM_REGION4_R1_W_V  0x00000001U
#define LP_APM_REGION4_R1_W_S  5
/** LP_APM_REGION4_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 4 in REE1 mode.
 */
#define LP_APM_REGION4_R1_R    (BIT(6))
#define LP_APM_REGION4_R1_R_M  (LP_APM_REGION4_R1_R_V << LP_APM_REGION4_R1_R_S)
#define LP_APM_REGION4_R1_R_V  0x00000001U
#define LP_APM_REGION4_R1_R_S  6
/** LP_APM_REGION4_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 4 in REE1 mode.
 */
#define LP_APM_REGION4_R2_X    (BIT(8))
#define LP_APM_REGION4_R2_X_M  (LP_APM_REGION4_R2_X_V << LP_APM_REGION4_R2_X_S)
#define LP_APM_REGION4_R2_X_V  0x00000001U
#define LP_APM_REGION4_R2_X_S  8
/** LP_APM_REGION4_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 4 in REE2 mode.
 */
#define LP_APM_REGION4_R2_W    (BIT(9))
#define LP_APM_REGION4_R2_W_M  (LP_APM_REGION4_R2_W_V << LP_APM_REGION4_R2_W_S)
#define LP_APM_REGION4_R2_W_V  0x00000001U
#define LP_APM_REGION4_R2_W_S  9
/** LP_APM_REGION4_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 4 in REE2 mode.
 */
#define LP_APM_REGION4_R2_R    (BIT(10))
#define LP_APM_REGION4_R2_R_M  (LP_APM_REGION4_R2_R_V << LP_APM_REGION4_R2_R_S)
#define LP_APM_REGION4_R2_R_V  0x00000001U
#define LP_APM_REGION4_R2_R_S  10
/** LP_APM_REGION4_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 4 configuration registers
 *  (LP_APM_REGION4_ADDR_START_REG, LP_APM_REGION4_ADDR_END_REG and
 *  LP_APM_REGION4_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION4_LOCK    (BIT(11))
#define LP_APM_REGION4_LOCK_M  (LP_APM_REGION4_LOCK_V << LP_APM_REGION4_LOCK_S)
#define LP_APM_REGION4_LOCK_V  0x00000001U
#define LP_APM_REGION4_LOCK_S  11

/** LP_APM_REGION5_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION5_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x40)
/** LP_APM_REGION5_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 5.
 */
#define LP_APM_REGION5_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION5_ADDR_START_M  (LP_APM_REGION5_ADDR_START_V << LP_APM_REGION5_ADDR_START_S)
#define LP_APM_REGION5_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION5_ADDR_START_S  0

/** LP_APM_REGION5_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION5_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x44)
/** LP_APM_REGION5_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 5.
 */
#define LP_APM_REGION5_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION5_ADDR_END_M  (LP_APM_REGION5_ADDR_END_V << LP_APM_REGION5_ADDR_END_S)
#define LP_APM_REGION5_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION5_ADDR_END_S  0

/** LP_APM_REGION5_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION5_ATTR_REG (DR_REG_LP_APM_BASE + 0x48)
/** LP_APM_REGION5_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 5 in REE0 mode.
 */
#define LP_APM_REGION5_R0_X    (BIT(0))
#define LP_APM_REGION5_R0_X_M  (LP_APM_REGION5_R0_X_V << LP_APM_REGION5_R0_X_S)
#define LP_APM_REGION5_R0_X_V  0x00000001U
#define LP_APM_REGION5_R0_X_S  0
/** LP_APM_REGION5_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 5 in REE0 mode.
 */
#define LP_APM_REGION5_R0_W    (BIT(1))
#define LP_APM_REGION5_R0_W_M  (LP_APM_REGION5_R0_W_V << LP_APM_REGION5_R0_W_S)
#define LP_APM_REGION5_R0_W_V  0x00000001U
#define LP_APM_REGION5_R0_W_S  1
/** LP_APM_REGION5_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 5 in REE0 mode.
 */
#define LP_APM_REGION5_R0_R    (BIT(2))
#define LP_APM_REGION5_R0_R_M  (LP_APM_REGION5_R0_R_V << LP_APM_REGION5_R0_R_S)
#define LP_APM_REGION5_R0_R_V  0x00000001U
#define LP_APM_REGION5_R0_R_S  2
/** LP_APM_REGION5_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 5 in REE1 mode.
 */
#define LP_APM_REGION5_R1_X    (BIT(4))
#define LP_APM_REGION5_R1_X_M  (LP_APM_REGION5_R1_X_V << LP_APM_REGION5_R1_X_S)
#define LP_APM_REGION5_R1_X_V  0x00000001U
#define LP_APM_REGION5_R1_X_S  4
/** LP_APM_REGION5_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 5 in REE1 mode.
 */
#define LP_APM_REGION5_R1_W    (BIT(5))
#define LP_APM_REGION5_R1_W_M  (LP_APM_REGION5_R1_W_V << LP_APM_REGION5_R1_W_S)
#define LP_APM_REGION5_R1_W_V  0x00000001U
#define LP_APM_REGION5_R1_W_S  5
/** LP_APM_REGION5_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 5 in REE1 mode.
 */
#define LP_APM_REGION5_R1_R    (BIT(6))
#define LP_APM_REGION5_R1_R_M  (LP_APM_REGION5_R1_R_V << LP_APM_REGION5_R1_R_S)
#define LP_APM_REGION5_R1_R_V  0x00000001U
#define LP_APM_REGION5_R1_R_S  6
/** LP_APM_REGION5_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 5 in REE1 mode.
 */
#define LP_APM_REGION5_R2_X    (BIT(8))
#define LP_APM_REGION5_R2_X_M  (LP_APM_REGION5_R2_X_V << LP_APM_REGION5_R2_X_S)
#define LP_APM_REGION5_R2_X_V  0x00000001U
#define LP_APM_REGION5_R2_X_S  8
/** LP_APM_REGION5_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 5 in REE2 mode.
 */
#define LP_APM_REGION5_R2_W    (BIT(9))
#define LP_APM_REGION5_R2_W_M  (LP_APM_REGION5_R2_W_V << LP_APM_REGION5_R2_W_S)
#define LP_APM_REGION5_R2_W_V  0x00000001U
#define LP_APM_REGION5_R2_W_S  9
/** LP_APM_REGION5_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 5 in REE2 mode.
 */
#define LP_APM_REGION5_R2_R    (BIT(10))
#define LP_APM_REGION5_R2_R_M  (LP_APM_REGION5_R2_R_V << LP_APM_REGION5_R2_R_S)
#define LP_APM_REGION5_R2_R_V  0x00000001U
#define LP_APM_REGION5_R2_R_S  10
/** LP_APM_REGION5_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 5 configuration registers
 *  (LP_APM_REGION5_ADDR_START_REG, LP_APM_REGION5_ADDR_END_REG and
 *  LP_APM_REGION5_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION5_LOCK    (BIT(11))
#define LP_APM_REGION5_LOCK_M  (LP_APM_REGION5_LOCK_V << LP_APM_REGION5_LOCK_S)
#define LP_APM_REGION5_LOCK_V  0x00000001U
#define LP_APM_REGION5_LOCK_S  11

/** LP_APM_REGION6_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION6_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x4c)
/** LP_APM_REGION6_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 6.
 */
#define LP_APM_REGION6_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION6_ADDR_START_M  (LP_APM_REGION6_ADDR_START_V << LP_APM_REGION6_ADDR_START_S)
#define LP_APM_REGION6_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION6_ADDR_START_S  0

/** LP_APM_REGION6_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION6_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x50)
/** LP_APM_REGION6_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 6.
 */
#define LP_APM_REGION6_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION6_ADDR_END_M  (LP_APM_REGION6_ADDR_END_V << LP_APM_REGION6_ADDR_END_S)
#define LP_APM_REGION6_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION6_ADDR_END_S  0

/** LP_APM_REGION6_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION6_ATTR_REG (DR_REG_LP_APM_BASE + 0x54)
/** LP_APM_REGION6_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 6 in REE0 mode.
 */
#define LP_APM_REGION6_R0_X    (BIT(0))
#define LP_APM_REGION6_R0_X_M  (LP_APM_REGION6_R0_X_V << LP_APM_REGION6_R0_X_S)
#define LP_APM_REGION6_R0_X_V  0x00000001U
#define LP_APM_REGION6_R0_X_S  0
/** LP_APM_REGION6_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 6 in REE0 mode.
 */
#define LP_APM_REGION6_R0_W    (BIT(1))
#define LP_APM_REGION6_R0_W_M  (LP_APM_REGION6_R0_W_V << LP_APM_REGION6_R0_W_S)
#define LP_APM_REGION6_R0_W_V  0x00000001U
#define LP_APM_REGION6_R0_W_S  1
/** LP_APM_REGION6_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 6 in REE0 mode.
 */
#define LP_APM_REGION6_R0_R    (BIT(2))
#define LP_APM_REGION6_R0_R_M  (LP_APM_REGION6_R0_R_V << LP_APM_REGION6_R0_R_S)
#define LP_APM_REGION6_R0_R_V  0x00000001U
#define LP_APM_REGION6_R0_R_S  2
/** LP_APM_REGION6_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 6 in REE1 mode.
 */
#define LP_APM_REGION6_R1_X    (BIT(4))
#define LP_APM_REGION6_R1_X_M  (LP_APM_REGION6_R1_X_V << LP_APM_REGION6_R1_X_S)
#define LP_APM_REGION6_R1_X_V  0x00000001U
#define LP_APM_REGION6_R1_X_S  4
/** LP_APM_REGION6_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 6 in REE1 mode.
 */
#define LP_APM_REGION6_R1_W    (BIT(5))
#define LP_APM_REGION6_R1_W_M  (LP_APM_REGION6_R1_W_V << LP_APM_REGION6_R1_W_S)
#define LP_APM_REGION6_R1_W_V  0x00000001U
#define LP_APM_REGION6_R1_W_S  5
/** LP_APM_REGION6_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 6 in REE1 mode.
 */
#define LP_APM_REGION6_R1_R    (BIT(6))
#define LP_APM_REGION6_R1_R_M  (LP_APM_REGION6_R1_R_V << LP_APM_REGION6_R1_R_S)
#define LP_APM_REGION6_R1_R_V  0x00000001U
#define LP_APM_REGION6_R1_R_S  6
/** LP_APM_REGION6_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 6 in REE1 mode.
 */
#define LP_APM_REGION6_R2_X    (BIT(8))
#define LP_APM_REGION6_R2_X_M  (LP_APM_REGION6_R2_X_V << LP_APM_REGION6_R2_X_S)
#define LP_APM_REGION6_R2_X_V  0x00000001U
#define LP_APM_REGION6_R2_X_S  8
/** LP_APM_REGION6_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 6 in REE2 mode.
 */
#define LP_APM_REGION6_R2_W    (BIT(9))
#define LP_APM_REGION6_R2_W_M  (LP_APM_REGION6_R2_W_V << LP_APM_REGION6_R2_W_S)
#define LP_APM_REGION6_R2_W_V  0x00000001U
#define LP_APM_REGION6_R2_W_S  9
/** LP_APM_REGION6_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 6 in REE2 mode.
 */
#define LP_APM_REGION6_R2_R    (BIT(10))
#define LP_APM_REGION6_R2_R_M  (LP_APM_REGION6_R2_R_V << LP_APM_REGION6_R2_R_S)
#define LP_APM_REGION6_R2_R_V  0x00000001U
#define LP_APM_REGION6_R2_R_S  10
/** LP_APM_REGION6_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 6 configuration registers
 *  (LP_APM_REGION6_ADDR_START_REG, LP_APM_REGION6_ADDR_END_REG and
 *  LP_APM_REGION6_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION6_LOCK    (BIT(11))
#define LP_APM_REGION6_LOCK_M  (LP_APM_REGION6_LOCK_V << LP_APM_REGION6_LOCK_S)
#define LP_APM_REGION6_LOCK_V  0x00000001U
#define LP_APM_REGION6_LOCK_S  11

/** LP_APM_REGION7_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION7_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x58)
/** LP_APM_REGION7_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 7.
 */
#define LP_APM_REGION7_ADDR_START    0xFFFFFFFFU
#define LP_APM_REGION7_ADDR_START_M  (LP_APM_REGION7_ADDR_START_V << LP_APM_REGION7_ADDR_START_S)
#define LP_APM_REGION7_ADDR_START_V  0xFFFFFFFFU
#define LP_APM_REGION7_ADDR_START_S  0

/** LP_APM_REGION7_ADDR_END_REG register
 *  Region address register
 */
#define LP_APM_REGION7_ADDR_END_REG (DR_REG_LP_APM_BASE + 0x5c)
/** LP_APM_REGION7_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 7.
 */
#define LP_APM_REGION7_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION7_ADDR_END_M  (LP_APM_REGION7_ADDR_END_V << LP_APM_REGION7_ADDR_END_S)
#define LP_APM_REGION7_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION7_ADDR_END_S  0

/** LP_APM_REGION7_ATTR_REG register
 *  Region access permissions configuration register
 */
#define LP_APM_REGION7_ATTR_REG (DR_REG_LP_APM_BASE + 0x60)
/** LP_APM_REGION7_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 7 in REE0 mode.
 */
#define LP_APM_REGION7_R0_X    (BIT(0))
#define LP_APM_REGION7_R0_X_M  (LP_APM_REGION7_R0_X_V << LP_APM_REGION7_R0_X_S)
#define LP_APM_REGION7_R0_X_V  0x00000001U
#define LP_APM_REGION7_R0_X_S  0
/** LP_APM_REGION7_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 7 in REE0 mode.
 */
#define LP_APM_REGION7_R0_W    (BIT(1))
#define LP_APM_REGION7_R0_W_M  (LP_APM_REGION7_R0_W_V << LP_APM_REGION7_R0_W_S)
#define LP_APM_REGION7_R0_W_V  0x00000001U
#define LP_APM_REGION7_R0_W_S  1
/** LP_APM_REGION7_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 7 in REE0 mode.
 */
#define LP_APM_REGION7_R0_R    (BIT(2))
#define LP_APM_REGION7_R0_R_M  (LP_APM_REGION7_R0_R_V << LP_APM_REGION7_R0_R_S)
#define LP_APM_REGION7_R0_R_V  0x00000001U
#define LP_APM_REGION7_R0_R_S  2
/** LP_APM_REGION7_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 7 in REE1 mode.
 */
#define LP_APM_REGION7_R1_X    (BIT(4))
#define LP_APM_REGION7_R1_X_M  (LP_APM_REGION7_R1_X_V << LP_APM_REGION7_R1_X_S)
#define LP_APM_REGION7_R1_X_V  0x00000001U
#define LP_APM_REGION7_R1_X_S  4
/** LP_APM_REGION7_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 7 in REE1 mode.
 */
#define LP_APM_REGION7_R1_W    (BIT(5))
#define LP_APM_REGION7_R1_W_M  (LP_APM_REGION7_R1_W_V << LP_APM_REGION7_R1_W_S)
#define LP_APM_REGION7_R1_W_V  0x00000001U
#define LP_APM_REGION7_R1_W_S  5
/** LP_APM_REGION7_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 7 in REE1 mode.
 */
#define LP_APM_REGION7_R1_R    (BIT(6))
#define LP_APM_REGION7_R1_R_M  (LP_APM_REGION7_R1_R_V << LP_APM_REGION7_R1_R_S)
#define LP_APM_REGION7_R1_R_V  0x00000001U
#define LP_APM_REGION7_R1_R_S  6
/** LP_APM_REGION7_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 7 in REE1 mode.
 */
#define LP_APM_REGION7_R2_X    (BIT(8))
#define LP_APM_REGION7_R2_X_M  (LP_APM_REGION7_R2_X_V << LP_APM_REGION7_R2_X_S)
#define LP_APM_REGION7_R2_X_V  0x00000001U
#define LP_APM_REGION7_R2_X_S  8
/** LP_APM_REGION7_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 7 in REE2 mode.
 */
#define LP_APM_REGION7_R2_W    (BIT(9))
#define LP_APM_REGION7_R2_W_M  (LP_APM_REGION7_R2_W_V << LP_APM_REGION7_R2_W_S)
#define LP_APM_REGION7_R2_W_V  0x00000001U
#define LP_APM_REGION7_R2_W_S  9
/** LP_APM_REGION7_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 7 in REE2 mode.
 */
#define LP_APM_REGION7_R2_R    (BIT(10))
#define LP_APM_REGION7_R2_R_M  (LP_APM_REGION7_R2_R_V << LP_APM_REGION7_R2_R_S)
#define LP_APM_REGION7_R2_R_V  0x00000001U
#define LP_APM_REGION7_R2_R_S  10
/** LP_APM_REGION7_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 7 configuration registers
 *  (LP_APM_REGION7_ADDR_START_REG, LP_APM_REGION7_ADDR_END_REG and
 *  LP_APM_REGION7_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define LP_APM_REGION7_LOCK    (BIT(11))
#define LP_APM_REGION7_LOCK_M  (LP_APM_REGION7_LOCK_V << LP_APM_REGION7_LOCK_S)
#define LP_APM_REGION7_LOCK_V  0x00000001U
#define LP_APM_REGION7_LOCK_S  11

/** LP_APM_FUNC_CTRL_REG register
 *  APM access path permission management register
 */
#define LP_APM_FUNC_CTRL_REG (DR_REG_LP_APM_BASE + 0xc4)
/** LP_APM_M0_FUNC_EN : R/W; bitpos: [0]; default: 1;
 *  Configures to enable permission management for LP_APM_CTRL M0.
 */
#define LP_APM_M0_FUNC_EN    (BIT(0))
#define LP_APM_M0_FUNC_EN_M  (LP_APM_M0_FUNC_EN_V << LP_APM_M0_FUNC_EN_S)
#define LP_APM_M0_FUNC_EN_V  0x00000001U
#define LP_APM_M0_FUNC_EN_S  0
/** LP_APM_M1_FUNC_EN : R/W; bitpos: [1]; default: 1;
 *  Configures to enable permission management for LP_APM_CTRL M1.
 */
#define LP_APM_M1_FUNC_EN    (BIT(1))
#define LP_APM_M1_FUNC_EN_M  (LP_APM_M1_FUNC_EN_V << LP_APM_M1_FUNC_EN_S)
#define LP_APM_M1_FUNC_EN_V  0x00000001U
#define LP_APM_M1_FUNC_EN_S  1

/** LP_APM_M0_STATUS_REG register
 *  LP_APM_CTRL M0 status register
 */
#define LP_APM_M0_STATUS_REG (DR_REG_LP_APM_BASE + 0xc8)
/** LP_APM_M0_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents permission restrictions
 *  bit1: 1 represents address out of bounds
 */
#define LP_APM_M0_EXCEPTION_STATUS    0x00000003U
#define LP_APM_M0_EXCEPTION_STATUS_M  (LP_APM_M0_EXCEPTION_STATUS_V << LP_APM_M0_EXCEPTION_STATUS_S)
#define LP_APM_M0_EXCEPTION_STATUS_V  0x00000003U
#define LP_APM_M0_EXCEPTION_STATUS_S  0

/** LP_APM_M0_STATUS_CLR_REG register
 *  LP_APM_CTRL M0 status clear register
 */
#define LP_APM_M0_STATUS_CLR_REG (DR_REG_LP_APM_BASE + 0xcc)
/** LP_APM_M0_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define LP_APM_M0_EXCEPTION_STATUS_CLR    (BIT(0))
#define LP_APM_M0_EXCEPTION_STATUS_CLR_M  (LP_APM_M0_EXCEPTION_STATUS_CLR_V << LP_APM_M0_EXCEPTION_STATUS_CLR_S)
#define LP_APM_M0_EXCEPTION_STATUS_CLR_V  0x00000001U
#define LP_APM_M0_EXCEPTION_STATUS_CLR_S  0

/** LP_APM_M0_EXCEPTION_INFO0_REG register
 *  LP_APM_CTRL M0 exception information register
 */
#define LP_APM_M0_EXCEPTION_INFO0_REG (DR_REG_LP_APM_BASE + 0xd0)
/** LP_APM_M0_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents the region where an exception occurs.
 */
#define LP_APM_M0_EXCEPTION_REGION    0x000000FFU
#define LP_APM_M0_EXCEPTION_REGION_M  (LP_APM_M0_EXCEPTION_REGION_V << LP_APM_M0_EXCEPTION_REGION_S)
#define LP_APM_M0_EXCEPTION_REGION_V  0x000000FFU
#define LP_APM_M0_EXCEPTION_REGION_S  0
/** LP_APM_M0_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents the master's security mode when an exception occurs.
 */
#define LP_APM_M0_EXCEPTION_MODE    0x00000003U
#define LP_APM_M0_EXCEPTION_MODE_M  (LP_APM_M0_EXCEPTION_MODE_V << LP_APM_M0_EXCEPTION_MODE_S)
#define LP_APM_M0_EXCEPTION_MODE_V  0x00000003U
#define LP_APM_M0_EXCEPTION_MODE_S  16
/** LP_APM_M0_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents master ID when an exception occurs.
 */
#define LP_APM_M0_EXCEPTION_ID    0x0000001FU
#define LP_APM_M0_EXCEPTION_ID_M  (LP_APM_M0_EXCEPTION_ID_V << LP_APM_M0_EXCEPTION_ID_S)
#define LP_APM_M0_EXCEPTION_ID_V  0x0000001FU
#define LP_APM_M0_EXCEPTION_ID_S  18

/** LP_APM_M0_EXCEPTION_INFO1_REG register
 *  LP_APM_CTRL M0 exception information register
 */
#define LP_APM_M0_EXCEPTION_INFO1_REG (DR_REG_LP_APM_BASE + 0xd4)
/** LP_APM_M0_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the access address when an exception occurs.
 */
#define LP_APM_M0_EXCEPTION_ADDR    0xFFFFFFFFU
#define LP_APM_M0_EXCEPTION_ADDR_M  (LP_APM_M0_EXCEPTION_ADDR_V << LP_APM_M0_EXCEPTION_ADDR_S)
#define LP_APM_M0_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define LP_APM_M0_EXCEPTION_ADDR_S  0

/** LP_APM_M1_STATUS_REG register
 *  LP_APM_CTRL M1 status register
 */
#define LP_APM_M1_STATUS_REG (DR_REG_LP_APM_BASE + 0xd8)
/** LP_APM_M1_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents permission restrictions
 *  bit1: 1 represents address out of bounds
 */
#define LP_APM_M1_EXCEPTION_STATUS    0x00000003U
#define LP_APM_M1_EXCEPTION_STATUS_M  (LP_APM_M1_EXCEPTION_STATUS_V << LP_APM_M1_EXCEPTION_STATUS_S)
#define LP_APM_M1_EXCEPTION_STATUS_V  0x00000003U
#define LP_APM_M1_EXCEPTION_STATUS_S  0

/** LP_APM_M1_STATUS_CLR_REG register
 *  LP_APM_CTRL M1 status clear register
 */
#define LP_APM_M1_STATUS_CLR_REG (DR_REG_LP_APM_BASE + 0xdc)
/** LP_APM_M1_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define LP_APM_M1_EXCEPTION_STATUS_CLR    (BIT(0))
#define LP_APM_M1_EXCEPTION_STATUS_CLR_M  (LP_APM_M1_EXCEPTION_STATUS_CLR_V << LP_APM_M1_EXCEPTION_STATUS_CLR_S)
#define LP_APM_M1_EXCEPTION_STATUS_CLR_V  0x00000001U
#define LP_APM_M1_EXCEPTION_STATUS_CLR_S  0

/** LP_APM_M1_EXCEPTION_INFO0_REG register
 *  LP_APM_CTRL M1 exception information register
 */
#define LP_APM_M1_EXCEPTION_INFO0_REG (DR_REG_LP_APM_BASE + 0xe0)
/** LP_APM_M1_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents the region where an exception occurs.
 */
#define LP_APM_M1_EXCEPTION_REGION    0x000000FFU
#define LP_APM_M1_EXCEPTION_REGION_M  (LP_APM_M1_EXCEPTION_REGION_V << LP_APM_M1_EXCEPTION_REGION_S)
#define LP_APM_M1_EXCEPTION_REGION_V  0x000000FFU
#define LP_APM_M1_EXCEPTION_REGION_S  0
/** LP_APM_M1_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents the master's security mode when an exception occurs.
 */
#define LP_APM_M1_EXCEPTION_MODE    0x00000003U
#define LP_APM_M1_EXCEPTION_MODE_M  (LP_APM_M1_EXCEPTION_MODE_V << LP_APM_M1_EXCEPTION_MODE_S)
#define LP_APM_M1_EXCEPTION_MODE_V  0x00000003U
#define LP_APM_M1_EXCEPTION_MODE_S  16
/** LP_APM_M1_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents master ID when an exception occurs.
 */
#define LP_APM_M1_EXCEPTION_ID    0x0000001FU
#define LP_APM_M1_EXCEPTION_ID_M  (LP_APM_M1_EXCEPTION_ID_V << LP_APM_M1_EXCEPTION_ID_S)
#define LP_APM_M1_EXCEPTION_ID_V  0x0000001FU
#define LP_APM_M1_EXCEPTION_ID_S  18

/** LP_APM_M1_EXCEPTION_INFO1_REG register
 *  LP_APM_CTRL M1 exception information register
 */
#define LP_APM_M1_EXCEPTION_INFO1_REG (DR_REG_LP_APM_BASE + 0xe4)
/** LP_APM_M1_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the access address when an exception occurs.
 */
#define LP_APM_M1_EXCEPTION_ADDR    0xFFFFFFFFU
#define LP_APM_M1_EXCEPTION_ADDR_M  (LP_APM_M1_EXCEPTION_ADDR_V << LP_APM_M1_EXCEPTION_ADDR_S)
#define LP_APM_M1_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define LP_APM_M1_EXCEPTION_ADDR_S  0

/** LP_APM_INT_EN_REG register
 *  LP_APM_CTRL M0/1 interrupt enable register
 */
#define LP_APM_INT_EN_REG (DR_REG_LP_APM_BASE + 0xe8)
/** LP_APM_M0_APM_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable LP_APM_CTRL M0 interrupt.
 *  0: Disable
 *  1: Enable
 */
#define LP_APM_M0_APM_INT_EN    (BIT(0))
#define LP_APM_M0_APM_INT_EN_M  (LP_APM_M0_APM_INT_EN_V << LP_APM_M0_APM_INT_EN_S)
#define LP_APM_M0_APM_INT_EN_V  0x00000001U
#define LP_APM_M0_APM_INT_EN_S  0
/** LP_APM_M1_APM_INT_EN : R/W; bitpos: [1]; default: 0;
 *  Configures to enable LP_APM_CTRL M1 interrupt.
 *  0: Disable
 *  1: Enable
 */
#define LP_APM_M1_APM_INT_EN    (BIT(1))
#define LP_APM_M1_APM_INT_EN_M  (LP_APM_M1_APM_INT_EN_V << LP_APM_M1_APM_INT_EN_S)
#define LP_APM_M1_APM_INT_EN_V  0x00000001U
#define LP_APM_M1_APM_INT_EN_S  1

/** LP_APM_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define LP_APM_CLOCK_GATE_REG (DR_REG_LP_APM_BASE + 0xec)
/** LP_APM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: Enable automatic clock gating
 *  1: Keep the clock always on
 */
#define LP_APM_CLK_EN    (BIT(0))
#define LP_APM_CLK_EN_M  (LP_APM_CLK_EN_V << LP_APM_CLK_EN_S)
#define LP_APM_CLK_EN_V  0x00000001U
#define LP_APM_CLK_EN_S  0

/** LP_APM_DATE_REG register
 *  Version control register
 */
#define LP_APM_DATE_REG (DR_REG_LP_APM_BASE + 0xfc)
/** LP_APM_DATE : R/W; bitpos: [27:0]; default: 35725664;
 *  Version control register.
 */
#define LP_APM_DATE    0x0FFFFFFFU
#define LP_APM_DATE_M  (LP_APM_DATE_V << LP_APM_DATE_S)
#define LP_APM_DATE_V  0x0FFFFFFFU
#define LP_APM_DATE_S  0

#ifdef __cplusplus
}
#endif
