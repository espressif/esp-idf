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

/** HP_APM_REGION_FILTER_EN_REG register
 *  Region enable register
 */
#define HP_APM_REGION_FILTER_EN_REG (DR_REG_HP_APM_BASE + 0x0)
/** HP_APM_REGION_FILTER_EN : R/W; bitpos: [15:0]; default: 1;
 *  Configure bit $n (0-15) to enable region $n (0-15).
 *  0: Disable
 *  1: Enable
 */
#define HP_APM_REGION_FILTER_EN    0x0000FFFFU
#define HP_APM_REGION_FILTER_EN_M  (HP_APM_REGION_FILTER_EN_V << HP_APM_REGION_FILTER_EN_S)
#define HP_APM_REGION_FILTER_EN_V  0x0000FFFFU
#define HP_APM_REGION_FILTER_EN_S  0

/** HP_APM_REGION0_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION0_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x4)
/** HP_APM_REGION0_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 0.
 */
#define HP_APM_REGION0_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION0_ADDR_START_M  (HP_APM_REGION0_ADDR_START_V << HP_APM_REGION0_ADDR_START_S)
#define HP_APM_REGION0_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION0_ADDR_START_S  0

/** HP_APM_REGION0_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION0_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x8)
/** HP_APM_REGION0_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 0.
 */
#define HP_APM_REGION0_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION0_ADDR_END_M  (HP_APM_REGION0_ADDR_END_V << HP_APM_REGION0_ADDR_END_S)
#define HP_APM_REGION0_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION0_ADDR_END_S  0

/** HP_APM_REGION0_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION0_ATTR_REG (DR_REG_HP_APM_BASE + 0xc)
/** HP_APM_REGION0_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 0 in REE0 mode.
 */
#define HP_APM_REGION0_R0_X    (BIT(0))
#define HP_APM_REGION0_R0_X_M  (HP_APM_REGION0_R0_X_V << HP_APM_REGION0_R0_X_S)
#define HP_APM_REGION0_R0_X_V  0x00000001U
#define HP_APM_REGION0_R0_X_S  0
/** HP_APM_REGION0_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 0 in REE0 mode.
 */
#define HP_APM_REGION0_R0_W    (BIT(1))
#define HP_APM_REGION0_R0_W_M  (HP_APM_REGION0_R0_W_V << HP_APM_REGION0_R0_W_S)
#define HP_APM_REGION0_R0_W_V  0x00000001U
#define HP_APM_REGION0_R0_W_S  1
/** HP_APM_REGION0_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 0 in REE0 mode.
 */
#define HP_APM_REGION0_R0_R    (BIT(2))
#define HP_APM_REGION0_R0_R_M  (HP_APM_REGION0_R0_R_V << HP_APM_REGION0_R0_R_S)
#define HP_APM_REGION0_R0_R_V  0x00000001U
#define HP_APM_REGION0_R0_R_S  2
/** HP_APM_REGION0_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 0 in REE1 mode.
 */
#define HP_APM_REGION0_R1_X    (BIT(4))
#define HP_APM_REGION0_R1_X_M  (HP_APM_REGION0_R1_X_V << HP_APM_REGION0_R1_X_S)
#define HP_APM_REGION0_R1_X_V  0x00000001U
#define HP_APM_REGION0_R1_X_S  4
/** HP_APM_REGION0_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 0 in REE1 mode.
 */
#define HP_APM_REGION0_R1_W    (BIT(5))
#define HP_APM_REGION0_R1_W_M  (HP_APM_REGION0_R1_W_V << HP_APM_REGION0_R1_W_S)
#define HP_APM_REGION0_R1_W_V  0x00000001U
#define HP_APM_REGION0_R1_W_S  5
/** HP_APM_REGION0_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 0 in REE1 mode.
 */
#define HP_APM_REGION0_R1_R    (BIT(6))
#define HP_APM_REGION0_R1_R_M  (HP_APM_REGION0_R1_R_V << HP_APM_REGION0_R1_R_S)
#define HP_APM_REGION0_R1_R_V  0x00000001U
#define HP_APM_REGION0_R1_R_S  6
/** HP_APM_REGION0_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 0 in REE2 mode.
 */
#define HP_APM_REGION0_R2_X    (BIT(8))
#define HP_APM_REGION0_R2_X_M  (HP_APM_REGION0_R2_X_V << HP_APM_REGION0_R2_X_S)
#define HP_APM_REGION0_R2_X_V  0x00000001U
#define HP_APM_REGION0_R2_X_S  8
/** HP_APM_REGION0_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 0 in REE2 mode.
 */
#define HP_APM_REGION0_R2_W    (BIT(9))
#define HP_APM_REGION0_R2_W_M  (HP_APM_REGION0_R2_W_V << HP_APM_REGION0_R2_W_S)
#define HP_APM_REGION0_R2_W_V  0x00000001U
#define HP_APM_REGION0_R2_W_S  9
/** HP_APM_REGION0_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 0 in REE2 mode.
 */
#define HP_APM_REGION0_R2_R    (BIT(10))
#define HP_APM_REGION0_R2_R_M  (HP_APM_REGION0_R2_R_V << HP_APM_REGION0_R2_R_S)
#define HP_APM_REGION0_R2_R_V  0x00000001U
#define HP_APM_REGION0_R2_R_S  10
/** HP_APM_REGION0_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 0 configuration registers
 *  (HP_APM_REGION0_ADDR_START_REG, HP_APM_REGION0_ADDR_END_REG and
 *  HP_APM_REGION0_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION0_LOCK    (BIT(11))
#define HP_APM_REGION0_LOCK_M  (HP_APM_REGION0_LOCK_V << HP_APM_REGION0_LOCK_S)
#define HP_APM_REGION0_LOCK_V  0x00000001U
#define HP_APM_REGION0_LOCK_S  11

/** HP_APM_REGION1_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION1_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x10)
/** HP_APM_REGION1_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 1.
 */
#define HP_APM_REGION1_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION1_ADDR_START_M  (HP_APM_REGION1_ADDR_START_V << HP_APM_REGION1_ADDR_START_S)
#define HP_APM_REGION1_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION1_ADDR_START_S  0

/** HP_APM_REGION1_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION1_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x14)
/** HP_APM_REGION1_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 1.
 */
#define HP_APM_REGION1_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION1_ADDR_END_M  (HP_APM_REGION1_ADDR_END_V << HP_APM_REGION1_ADDR_END_S)
#define HP_APM_REGION1_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION1_ADDR_END_S  0

/** HP_APM_REGION1_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION1_ATTR_REG (DR_REG_HP_APM_BASE + 0x18)
/** HP_APM_REGION1_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 1 in REE0 mode.
 */
#define HP_APM_REGION1_R0_X    (BIT(0))
#define HP_APM_REGION1_R0_X_M  (HP_APM_REGION1_R0_X_V << HP_APM_REGION1_R0_X_S)
#define HP_APM_REGION1_R0_X_V  0x00000001U
#define HP_APM_REGION1_R0_X_S  0
/** HP_APM_REGION1_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 1 in REE0 mode.
 */
#define HP_APM_REGION1_R0_W    (BIT(1))
#define HP_APM_REGION1_R0_W_M  (HP_APM_REGION1_R0_W_V << HP_APM_REGION1_R0_W_S)
#define HP_APM_REGION1_R0_W_V  0x00000001U
#define HP_APM_REGION1_R0_W_S  1
/** HP_APM_REGION1_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 1 in REE0 mode.
 */
#define HP_APM_REGION1_R0_R    (BIT(2))
#define HP_APM_REGION1_R0_R_M  (HP_APM_REGION1_R0_R_V << HP_APM_REGION1_R0_R_S)
#define HP_APM_REGION1_R0_R_V  0x00000001U
#define HP_APM_REGION1_R0_R_S  2
/** HP_APM_REGION1_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 1 in REE1 mode.
 */
#define HP_APM_REGION1_R1_X    (BIT(4))
#define HP_APM_REGION1_R1_X_M  (HP_APM_REGION1_R1_X_V << HP_APM_REGION1_R1_X_S)
#define HP_APM_REGION1_R1_X_V  0x00000001U
#define HP_APM_REGION1_R1_X_S  4
/** HP_APM_REGION1_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 1 in REE1 mode.
 */
#define HP_APM_REGION1_R1_W    (BIT(5))
#define HP_APM_REGION1_R1_W_M  (HP_APM_REGION1_R1_W_V << HP_APM_REGION1_R1_W_S)
#define HP_APM_REGION1_R1_W_V  0x00000001U
#define HP_APM_REGION1_R1_W_S  5
/** HP_APM_REGION1_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 1 in REE1 mode.
 */
#define HP_APM_REGION1_R1_R    (BIT(6))
#define HP_APM_REGION1_R1_R_M  (HP_APM_REGION1_R1_R_V << HP_APM_REGION1_R1_R_S)
#define HP_APM_REGION1_R1_R_V  0x00000001U
#define HP_APM_REGION1_R1_R_S  6
/** HP_APM_REGION1_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 1 in REE2 mode.
 */
#define HP_APM_REGION1_R2_X    (BIT(8))
#define HP_APM_REGION1_R2_X_M  (HP_APM_REGION1_R2_X_V << HP_APM_REGION1_R2_X_S)
#define HP_APM_REGION1_R2_X_V  0x00000001U
#define HP_APM_REGION1_R2_X_S  8
/** HP_APM_REGION1_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 1 in REE2 mode.
 */
#define HP_APM_REGION1_R2_W    (BIT(9))
#define HP_APM_REGION1_R2_W_M  (HP_APM_REGION1_R2_W_V << HP_APM_REGION1_R2_W_S)
#define HP_APM_REGION1_R2_W_V  0x00000001U
#define HP_APM_REGION1_R2_W_S  9
/** HP_APM_REGION1_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 1 in REE2 mode.
 */
#define HP_APM_REGION1_R2_R    (BIT(10))
#define HP_APM_REGION1_R2_R_M  (HP_APM_REGION1_R2_R_V << HP_APM_REGION1_R2_R_S)
#define HP_APM_REGION1_R2_R_V  0x00000001U
#define HP_APM_REGION1_R2_R_S  10
/** HP_APM_REGION1_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 1 configuration registers
 *  (HP_APM_REGION1_ADDR_START_REG, HP_APM_REGION1_ADDR_END_REG and
 *  HP_APM_REGION1_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION1_LOCK    (BIT(11))
#define HP_APM_REGION1_LOCK_M  (HP_APM_REGION1_LOCK_V << HP_APM_REGION1_LOCK_S)
#define HP_APM_REGION1_LOCK_V  0x00000001U
#define HP_APM_REGION1_LOCK_S  11

/** HP_APM_REGION2_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION2_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x1c)
/** HP_APM_REGION2_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 2.
 */
#define HP_APM_REGION2_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION2_ADDR_START_M  (HP_APM_REGION2_ADDR_START_V << HP_APM_REGION2_ADDR_START_S)
#define HP_APM_REGION2_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION2_ADDR_START_S  0

/** HP_APM_REGION2_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION2_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x20)
/** HP_APM_REGION2_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 2.
 */
#define HP_APM_REGION2_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION2_ADDR_END_M  (HP_APM_REGION2_ADDR_END_V << HP_APM_REGION2_ADDR_END_S)
#define HP_APM_REGION2_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION2_ADDR_END_S  0

/** HP_APM_REGION2_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION2_ATTR_REG (DR_REG_HP_APM_BASE + 0x24)
/** HP_APM_REGION2_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 2 in REE0 mode.
 */
#define HP_APM_REGION2_R0_X    (BIT(0))
#define HP_APM_REGION2_R0_X_M  (HP_APM_REGION2_R0_X_V << HP_APM_REGION2_R0_X_S)
#define HP_APM_REGION2_R0_X_V  0x00000001U
#define HP_APM_REGION2_R0_X_S  0
/** HP_APM_REGION2_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 2 in REE0 mode.
 */
#define HP_APM_REGION2_R0_W    (BIT(1))
#define HP_APM_REGION2_R0_W_M  (HP_APM_REGION2_R0_W_V << HP_APM_REGION2_R0_W_S)
#define HP_APM_REGION2_R0_W_V  0x00000001U
#define HP_APM_REGION2_R0_W_S  1
/** HP_APM_REGION2_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 2 in REE0 mode.
 */
#define HP_APM_REGION2_R0_R    (BIT(2))
#define HP_APM_REGION2_R0_R_M  (HP_APM_REGION2_R0_R_V << HP_APM_REGION2_R0_R_S)
#define HP_APM_REGION2_R0_R_V  0x00000001U
#define HP_APM_REGION2_R0_R_S  2
/** HP_APM_REGION2_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 2 in REE1 mode.
 */
#define HP_APM_REGION2_R1_X    (BIT(4))
#define HP_APM_REGION2_R1_X_M  (HP_APM_REGION2_R1_X_V << HP_APM_REGION2_R1_X_S)
#define HP_APM_REGION2_R1_X_V  0x00000001U
#define HP_APM_REGION2_R1_X_S  4
/** HP_APM_REGION2_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 2 in REE1 mode.
 */
#define HP_APM_REGION2_R1_W    (BIT(5))
#define HP_APM_REGION2_R1_W_M  (HP_APM_REGION2_R1_W_V << HP_APM_REGION2_R1_W_S)
#define HP_APM_REGION2_R1_W_V  0x00000001U
#define HP_APM_REGION2_R1_W_S  5
/** HP_APM_REGION2_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 2 in REE1 mode.
 */
#define HP_APM_REGION2_R1_R    (BIT(6))
#define HP_APM_REGION2_R1_R_M  (HP_APM_REGION2_R1_R_V << HP_APM_REGION2_R1_R_S)
#define HP_APM_REGION2_R1_R_V  0x00000001U
#define HP_APM_REGION2_R1_R_S  6
/** HP_APM_REGION2_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 2 in REE2 mode.
 */
#define HP_APM_REGION2_R2_X    (BIT(8))
#define HP_APM_REGION2_R2_X_M  (HP_APM_REGION2_R2_X_V << HP_APM_REGION2_R2_X_S)
#define HP_APM_REGION2_R2_X_V  0x00000001U
#define HP_APM_REGION2_R2_X_S  8
/** HP_APM_REGION2_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 2 in REE2 mode.
 */
#define HP_APM_REGION2_R2_W    (BIT(9))
#define HP_APM_REGION2_R2_W_M  (HP_APM_REGION2_R2_W_V << HP_APM_REGION2_R2_W_S)
#define HP_APM_REGION2_R2_W_V  0x00000001U
#define HP_APM_REGION2_R2_W_S  9
/** HP_APM_REGION2_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 2 in REE2 mode.
 */
#define HP_APM_REGION2_R2_R    (BIT(10))
#define HP_APM_REGION2_R2_R_M  (HP_APM_REGION2_R2_R_V << HP_APM_REGION2_R2_R_S)
#define HP_APM_REGION2_R2_R_V  0x00000001U
#define HP_APM_REGION2_R2_R_S  10
/** HP_APM_REGION2_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 2 configuration registers
 *  (HP_APM_REGION2_ADDR_START_REG, HP_APM_REGION2_ADDR_END_REG and
 *  HP_APM_REGION2_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION2_LOCK    (BIT(11))
#define HP_APM_REGION2_LOCK_M  (HP_APM_REGION2_LOCK_V << HP_APM_REGION2_LOCK_S)
#define HP_APM_REGION2_LOCK_V  0x00000001U
#define HP_APM_REGION2_LOCK_S  11

/** HP_APM_REGION3_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION3_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x28)
/** HP_APM_REGION3_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 3.
 */
#define HP_APM_REGION3_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION3_ADDR_START_M  (HP_APM_REGION3_ADDR_START_V << HP_APM_REGION3_ADDR_START_S)
#define HP_APM_REGION3_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION3_ADDR_START_S  0

/** HP_APM_REGION3_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION3_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x2c)
/** HP_APM_REGION3_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 3.
 */
#define HP_APM_REGION3_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION3_ADDR_END_M  (HP_APM_REGION3_ADDR_END_V << HP_APM_REGION3_ADDR_END_S)
#define HP_APM_REGION3_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION3_ADDR_END_S  0

/** HP_APM_REGION3_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION3_ATTR_REG (DR_REG_HP_APM_BASE + 0x30)
/** HP_APM_REGION3_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 3 in REE0 mode.
 */
#define HP_APM_REGION3_R0_X    (BIT(0))
#define HP_APM_REGION3_R0_X_M  (HP_APM_REGION3_R0_X_V << HP_APM_REGION3_R0_X_S)
#define HP_APM_REGION3_R0_X_V  0x00000001U
#define HP_APM_REGION3_R0_X_S  0
/** HP_APM_REGION3_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 3 in REE0 mode.
 */
#define HP_APM_REGION3_R0_W    (BIT(1))
#define HP_APM_REGION3_R0_W_M  (HP_APM_REGION3_R0_W_V << HP_APM_REGION3_R0_W_S)
#define HP_APM_REGION3_R0_W_V  0x00000001U
#define HP_APM_REGION3_R0_W_S  1
/** HP_APM_REGION3_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 3 in REE0 mode.
 */
#define HP_APM_REGION3_R0_R    (BIT(2))
#define HP_APM_REGION3_R0_R_M  (HP_APM_REGION3_R0_R_V << HP_APM_REGION3_R0_R_S)
#define HP_APM_REGION3_R0_R_V  0x00000001U
#define HP_APM_REGION3_R0_R_S  2
/** HP_APM_REGION3_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 3 in REE1 mode.
 */
#define HP_APM_REGION3_R1_X    (BIT(4))
#define HP_APM_REGION3_R1_X_M  (HP_APM_REGION3_R1_X_V << HP_APM_REGION3_R1_X_S)
#define HP_APM_REGION3_R1_X_V  0x00000001U
#define HP_APM_REGION3_R1_X_S  4
/** HP_APM_REGION3_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 3 in REE1 mode.
 */
#define HP_APM_REGION3_R1_W    (BIT(5))
#define HP_APM_REGION3_R1_W_M  (HP_APM_REGION3_R1_W_V << HP_APM_REGION3_R1_W_S)
#define HP_APM_REGION3_R1_W_V  0x00000001U
#define HP_APM_REGION3_R1_W_S  5
/** HP_APM_REGION3_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 3 in REE1 mode.
 */
#define HP_APM_REGION3_R1_R    (BIT(6))
#define HP_APM_REGION3_R1_R_M  (HP_APM_REGION3_R1_R_V << HP_APM_REGION3_R1_R_S)
#define HP_APM_REGION3_R1_R_V  0x00000001U
#define HP_APM_REGION3_R1_R_S  6
/** HP_APM_REGION3_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 3 in REE2 mode.
 */
#define HP_APM_REGION3_R2_X    (BIT(8))
#define HP_APM_REGION3_R2_X_M  (HP_APM_REGION3_R2_X_V << HP_APM_REGION3_R2_X_S)
#define HP_APM_REGION3_R2_X_V  0x00000001U
#define HP_APM_REGION3_R2_X_S  8
/** HP_APM_REGION3_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 3 in REE2 mode.
 */
#define HP_APM_REGION3_R2_W    (BIT(9))
#define HP_APM_REGION3_R2_W_M  (HP_APM_REGION3_R2_W_V << HP_APM_REGION3_R2_W_S)
#define HP_APM_REGION3_R2_W_V  0x00000001U
#define HP_APM_REGION3_R2_W_S  9
/** HP_APM_REGION3_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 3 in REE2 mode.
 */
#define HP_APM_REGION3_R2_R    (BIT(10))
#define HP_APM_REGION3_R2_R_M  (HP_APM_REGION3_R2_R_V << HP_APM_REGION3_R2_R_S)
#define HP_APM_REGION3_R2_R_V  0x00000001U
#define HP_APM_REGION3_R2_R_S  10
/** HP_APM_REGION3_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 3 configuration registers
 *  (HP_APM_REGION3_ADDR_START_REG, HP_APM_REGION3_ADDR_END_REG and
 *  HP_APM_REGION3_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION3_LOCK    (BIT(11))
#define HP_APM_REGION3_LOCK_M  (HP_APM_REGION3_LOCK_V << HP_APM_REGION3_LOCK_S)
#define HP_APM_REGION3_LOCK_V  0x00000001U
#define HP_APM_REGION3_LOCK_S  11

/** HP_APM_REGION4_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION4_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x34)
/** HP_APM_REGION4_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 4.
 */
#define HP_APM_REGION4_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION4_ADDR_START_M  (HP_APM_REGION4_ADDR_START_V << HP_APM_REGION4_ADDR_START_S)
#define HP_APM_REGION4_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION4_ADDR_START_S  0

/** HP_APM_REGION4_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION4_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x38)
/** HP_APM_REGION4_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 4.
 */
#define HP_APM_REGION4_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION4_ADDR_END_M  (HP_APM_REGION4_ADDR_END_V << HP_APM_REGION4_ADDR_END_S)
#define HP_APM_REGION4_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION4_ADDR_END_S  0

/** HP_APM_REGION4_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION4_ATTR_REG (DR_REG_HP_APM_BASE + 0x3c)
/** HP_APM_REGION4_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 4 in REE0 mode.
 */
#define HP_APM_REGION4_R0_X    (BIT(0))
#define HP_APM_REGION4_R0_X_M  (HP_APM_REGION4_R0_X_V << HP_APM_REGION4_R0_X_S)
#define HP_APM_REGION4_R0_X_V  0x00000001U
#define HP_APM_REGION4_R0_X_S  0
/** HP_APM_REGION4_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 4 in REE0 mode.
 */
#define HP_APM_REGION4_R0_W    (BIT(1))
#define HP_APM_REGION4_R0_W_M  (HP_APM_REGION4_R0_W_V << HP_APM_REGION4_R0_W_S)
#define HP_APM_REGION4_R0_W_V  0x00000001U
#define HP_APM_REGION4_R0_W_S  1
/** HP_APM_REGION4_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 4 in REE0 mode.
 */
#define HP_APM_REGION4_R0_R    (BIT(2))
#define HP_APM_REGION4_R0_R_M  (HP_APM_REGION4_R0_R_V << HP_APM_REGION4_R0_R_S)
#define HP_APM_REGION4_R0_R_V  0x00000001U
#define HP_APM_REGION4_R0_R_S  2
/** HP_APM_REGION4_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 4 in REE1 mode.
 */
#define HP_APM_REGION4_R1_X    (BIT(4))
#define HP_APM_REGION4_R1_X_M  (HP_APM_REGION4_R1_X_V << HP_APM_REGION4_R1_X_S)
#define HP_APM_REGION4_R1_X_V  0x00000001U
#define HP_APM_REGION4_R1_X_S  4
/** HP_APM_REGION4_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 4 in REE1 mode.
 */
#define HP_APM_REGION4_R1_W    (BIT(5))
#define HP_APM_REGION4_R1_W_M  (HP_APM_REGION4_R1_W_V << HP_APM_REGION4_R1_W_S)
#define HP_APM_REGION4_R1_W_V  0x00000001U
#define HP_APM_REGION4_R1_W_S  5
/** HP_APM_REGION4_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 4 in REE1 mode.
 */
#define HP_APM_REGION4_R1_R    (BIT(6))
#define HP_APM_REGION4_R1_R_M  (HP_APM_REGION4_R1_R_V << HP_APM_REGION4_R1_R_S)
#define HP_APM_REGION4_R1_R_V  0x00000001U
#define HP_APM_REGION4_R1_R_S  6
/** HP_APM_REGION4_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 4 in REE2 mode.
 */
#define HP_APM_REGION4_R2_X    (BIT(8))
#define HP_APM_REGION4_R2_X_M  (HP_APM_REGION4_R2_X_V << HP_APM_REGION4_R2_X_S)
#define HP_APM_REGION4_R2_X_V  0x00000001U
#define HP_APM_REGION4_R2_X_S  8
/** HP_APM_REGION4_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 4 in REE2 mode.
 */
#define HP_APM_REGION4_R2_W    (BIT(9))
#define HP_APM_REGION4_R2_W_M  (HP_APM_REGION4_R2_W_V << HP_APM_REGION4_R2_W_S)
#define HP_APM_REGION4_R2_W_V  0x00000001U
#define HP_APM_REGION4_R2_W_S  9
/** HP_APM_REGION4_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 4 in REE2 mode.
 */
#define HP_APM_REGION4_R2_R    (BIT(10))
#define HP_APM_REGION4_R2_R_M  (HP_APM_REGION4_R2_R_V << HP_APM_REGION4_R2_R_S)
#define HP_APM_REGION4_R2_R_V  0x00000001U
#define HP_APM_REGION4_R2_R_S  10
/** HP_APM_REGION4_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 4 configuration registers
 *  (HP_APM_REGION4_ADDR_START_REG, HP_APM_REGION4_ADDR_END_REG and
 *  HP_APM_REGION4_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION4_LOCK    (BIT(11))
#define HP_APM_REGION4_LOCK_M  (HP_APM_REGION4_LOCK_V << HP_APM_REGION4_LOCK_S)
#define HP_APM_REGION4_LOCK_V  0x00000001U
#define HP_APM_REGION4_LOCK_S  11

/** HP_APM_REGION5_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION5_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x40)
/** HP_APM_REGION5_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 5.
 */
#define HP_APM_REGION5_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION5_ADDR_START_M  (HP_APM_REGION5_ADDR_START_V << HP_APM_REGION5_ADDR_START_S)
#define HP_APM_REGION5_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION5_ADDR_START_S  0

/** HP_APM_REGION5_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION5_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x44)
/** HP_APM_REGION5_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 5.
 */
#define HP_APM_REGION5_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION5_ADDR_END_M  (HP_APM_REGION5_ADDR_END_V << HP_APM_REGION5_ADDR_END_S)
#define HP_APM_REGION5_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION5_ADDR_END_S  0

/** HP_APM_REGION5_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION5_ATTR_REG (DR_REG_HP_APM_BASE + 0x48)
/** HP_APM_REGION5_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 5 in REE0 mode.
 */
#define HP_APM_REGION5_R0_X    (BIT(0))
#define HP_APM_REGION5_R0_X_M  (HP_APM_REGION5_R0_X_V << HP_APM_REGION5_R0_X_S)
#define HP_APM_REGION5_R0_X_V  0x00000001U
#define HP_APM_REGION5_R0_X_S  0
/** HP_APM_REGION5_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 5 in REE0 mode.
 */
#define HP_APM_REGION5_R0_W    (BIT(1))
#define HP_APM_REGION5_R0_W_M  (HP_APM_REGION5_R0_W_V << HP_APM_REGION5_R0_W_S)
#define HP_APM_REGION5_R0_W_V  0x00000001U
#define HP_APM_REGION5_R0_W_S  1
/** HP_APM_REGION5_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 5 in REE0 mode.
 */
#define HP_APM_REGION5_R0_R    (BIT(2))
#define HP_APM_REGION5_R0_R_M  (HP_APM_REGION5_R0_R_V << HP_APM_REGION5_R0_R_S)
#define HP_APM_REGION5_R0_R_V  0x00000001U
#define HP_APM_REGION5_R0_R_S  2
/** HP_APM_REGION5_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 5 in REE1 mode.
 */
#define HP_APM_REGION5_R1_X    (BIT(4))
#define HP_APM_REGION5_R1_X_M  (HP_APM_REGION5_R1_X_V << HP_APM_REGION5_R1_X_S)
#define HP_APM_REGION5_R1_X_V  0x00000001U
#define HP_APM_REGION5_R1_X_S  4
/** HP_APM_REGION5_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 5 in REE1 mode.
 */
#define HP_APM_REGION5_R1_W    (BIT(5))
#define HP_APM_REGION5_R1_W_M  (HP_APM_REGION5_R1_W_V << HP_APM_REGION5_R1_W_S)
#define HP_APM_REGION5_R1_W_V  0x00000001U
#define HP_APM_REGION5_R1_W_S  5
/** HP_APM_REGION5_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 5 in REE1 mode.
 */
#define HP_APM_REGION5_R1_R    (BIT(6))
#define HP_APM_REGION5_R1_R_M  (HP_APM_REGION5_R1_R_V << HP_APM_REGION5_R1_R_S)
#define HP_APM_REGION5_R1_R_V  0x00000001U
#define HP_APM_REGION5_R1_R_S  6
/** HP_APM_REGION5_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 5 in REE2 mode.
 */
#define HP_APM_REGION5_R2_X    (BIT(8))
#define HP_APM_REGION5_R2_X_M  (HP_APM_REGION5_R2_X_V << HP_APM_REGION5_R2_X_S)
#define HP_APM_REGION5_R2_X_V  0x00000001U
#define HP_APM_REGION5_R2_X_S  8
/** HP_APM_REGION5_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 5 in REE2 mode.
 */
#define HP_APM_REGION5_R2_W    (BIT(9))
#define HP_APM_REGION5_R2_W_M  (HP_APM_REGION5_R2_W_V << HP_APM_REGION5_R2_W_S)
#define HP_APM_REGION5_R2_W_V  0x00000001U
#define HP_APM_REGION5_R2_W_S  9
/** HP_APM_REGION5_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 5 in REE2 mode.
 */
#define HP_APM_REGION5_R2_R    (BIT(10))
#define HP_APM_REGION5_R2_R_M  (HP_APM_REGION5_R2_R_V << HP_APM_REGION5_R2_R_S)
#define HP_APM_REGION5_R2_R_V  0x00000001U
#define HP_APM_REGION5_R2_R_S  10
/** HP_APM_REGION5_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 5 configuration registers
 *  (HP_APM_REGION5_ADDR_START_REG, HP_APM_REGION5_ADDR_END_REG and
 *  HP_APM_REGION5_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION5_LOCK    (BIT(11))
#define HP_APM_REGION5_LOCK_M  (HP_APM_REGION5_LOCK_V << HP_APM_REGION5_LOCK_S)
#define HP_APM_REGION5_LOCK_V  0x00000001U
#define HP_APM_REGION5_LOCK_S  11

/** HP_APM_REGION6_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION6_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x4c)
/** HP_APM_REGION6_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 6.
 */
#define HP_APM_REGION6_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION6_ADDR_START_M  (HP_APM_REGION6_ADDR_START_V << HP_APM_REGION6_ADDR_START_S)
#define HP_APM_REGION6_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION6_ADDR_START_S  0

/** HP_APM_REGION6_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION6_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x50)
/** HP_APM_REGION6_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 6.
 */
#define HP_APM_REGION6_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION6_ADDR_END_M  (HP_APM_REGION6_ADDR_END_V << HP_APM_REGION6_ADDR_END_S)
#define HP_APM_REGION6_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION6_ADDR_END_S  0

/** HP_APM_REGION6_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION6_ATTR_REG (DR_REG_HP_APM_BASE + 0x54)
/** HP_APM_REGION6_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 6 in REE0 mode.
 */
#define HP_APM_REGION6_R0_X    (BIT(0))
#define HP_APM_REGION6_R0_X_M  (HP_APM_REGION6_R0_X_V << HP_APM_REGION6_R0_X_S)
#define HP_APM_REGION6_R0_X_V  0x00000001U
#define HP_APM_REGION6_R0_X_S  0
/** HP_APM_REGION6_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 6 in REE0 mode.
 */
#define HP_APM_REGION6_R0_W    (BIT(1))
#define HP_APM_REGION6_R0_W_M  (HP_APM_REGION6_R0_W_V << HP_APM_REGION6_R0_W_S)
#define HP_APM_REGION6_R0_W_V  0x00000001U
#define HP_APM_REGION6_R0_W_S  1
/** HP_APM_REGION6_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 6 in REE0 mode.
 */
#define HP_APM_REGION6_R0_R    (BIT(2))
#define HP_APM_REGION6_R0_R_M  (HP_APM_REGION6_R0_R_V << HP_APM_REGION6_R0_R_S)
#define HP_APM_REGION6_R0_R_V  0x00000001U
#define HP_APM_REGION6_R0_R_S  2
/** HP_APM_REGION6_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 6 in REE1 mode.
 */
#define HP_APM_REGION6_R1_X    (BIT(4))
#define HP_APM_REGION6_R1_X_M  (HP_APM_REGION6_R1_X_V << HP_APM_REGION6_R1_X_S)
#define HP_APM_REGION6_R1_X_V  0x00000001U
#define HP_APM_REGION6_R1_X_S  4
/** HP_APM_REGION6_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 6 in REE1 mode.
 */
#define HP_APM_REGION6_R1_W    (BIT(5))
#define HP_APM_REGION6_R1_W_M  (HP_APM_REGION6_R1_W_V << HP_APM_REGION6_R1_W_S)
#define HP_APM_REGION6_R1_W_V  0x00000001U
#define HP_APM_REGION6_R1_W_S  5
/** HP_APM_REGION6_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 6 in REE1 mode.
 */
#define HP_APM_REGION6_R1_R    (BIT(6))
#define HP_APM_REGION6_R1_R_M  (HP_APM_REGION6_R1_R_V << HP_APM_REGION6_R1_R_S)
#define HP_APM_REGION6_R1_R_V  0x00000001U
#define HP_APM_REGION6_R1_R_S  6
/** HP_APM_REGION6_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 6 in REE2 mode.
 */
#define HP_APM_REGION6_R2_X    (BIT(8))
#define HP_APM_REGION6_R2_X_M  (HP_APM_REGION6_R2_X_V << HP_APM_REGION6_R2_X_S)
#define HP_APM_REGION6_R2_X_V  0x00000001U
#define HP_APM_REGION6_R2_X_S  8
/** HP_APM_REGION6_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 6 in REE2 mode.
 */
#define HP_APM_REGION6_R2_W    (BIT(9))
#define HP_APM_REGION6_R2_W_M  (HP_APM_REGION6_R2_W_V << HP_APM_REGION6_R2_W_S)
#define HP_APM_REGION6_R2_W_V  0x00000001U
#define HP_APM_REGION6_R2_W_S  9
/** HP_APM_REGION6_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 6 in REE2 mode.
 */
#define HP_APM_REGION6_R2_R    (BIT(10))
#define HP_APM_REGION6_R2_R_M  (HP_APM_REGION6_R2_R_V << HP_APM_REGION6_R2_R_S)
#define HP_APM_REGION6_R2_R_V  0x00000001U
#define HP_APM_REGION6_R2_R_S  10
/** HP_APM_REGION6_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 6 configuration registers
 *  (HP_APM_REGION6_ADDR_START_REG, HP_APM_REGION6_ADDR_END_REG and
 *  HP_APM_REGION6_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION6_LOCK    (BIT(11))
#define HP_APM_REGION6_LOCK_M  (HP_APM_REGION6_LOCK_V << HP_APM_REGION6_LOCK_S)
#define HP_APM_REGION6_LOCK_V  0x00000001U
#define HP_APM_REGION6_LOCK_S  11

/** HP_APM_REGION7_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION7_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x58)
/** HP_APM_REGION7_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 7.
 */
#define HP_APM_REGION7_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION7_ADDR_START_M  (HP_APM_REGION7_ADDR_START_V << HP_APM_REGION7_ADDR_START_S)
#define HP_APM_REGION7_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION7_ADDR_START_S  0

/** HP_APM_REGION7_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION7_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x5c)
/** HP_APM_REGION7_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 7.
 */
#define HP_APM_REGION7_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION7_ADDR_END_M  (HP_APM_REGION7_ADDR_END_V << HP_APM_REGION7_ADDR_END_S)
#define HP_APM_REGION7_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION7_ADDR_END_S  0

/** HP_APM_REGION7_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION7_ATTR_REG (DR_REG_HP_APM_BASE + 0x60)
/** HP_APM_REGION7_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 7 in REE0 mode.
 */
#define HP_APM_REGION7_R0_X    (BIT(0))
#define HP_APM_REGION7_R0_X_M  (HP_APM_REGION7_R0_X_V << HP_APM_REGION7_R0_X_S)
#define HP_APM_REGION7_R0_X_V  0x00000001U
#define HP_APM_REGION7_R0_X_S  0
/** HP_APM_REGION7_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 7 in REE0 mode.
 */
#define HP_APM_REGION7_R0_W    (BIT(1))
#define HP_APM_REGION7_R0_W_M  (HP_APM_REGION7_R0_W_V << HP_APM_REGION7_R0_W_S)
#define HP_APM_REGION7_R0_W_V  0x00000001U
#define HP_APM_REGION7_R0_W_S  1
/** HP_APM_REGION7_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 7 in REE0 mode.
 */
#define HP_APM_REGION7_R0_R    (BIT(2))
#define HP_APM_REGION7_R0_R_M  (HP_APM_REGION7_R0_R_V << HP_APM_REGION7_R0_R_S)
#define HP_APM_REGION7_R0_R_V  0x00000001U
#define HP_APM_REGION7_R0_R_S  2
/** HP_APM_REGION7_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 7 in REE1 mode.
 */
#define HP_APM_REGION7_R1_X    (BIT(4))
#define HP_APM_REGION7_R1_X_M  (HP_APM_REGION7_R1_X_V << HP_APM_REGION7_R1_X_S)
#define HP_APM_REGION7_R1_X_V  0x00000001U
#define HP_APM_REGION7_R1_X_S  4
/** HP_APM_REGION7_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 7 in REE1 mode.
 */
#define HP_APM_REGION7_R1_W    (BIT(5))
#define HP_APM_REGION7_R1_W_M  (HP_APM_REGION7_R1_W_V << HP_APM_REGION7_R1_W_S)
#define HP_APM_REGION7_R1_W_V  0x00000001U
#define HP_APM_REGION7_R1_W_S  5
/** HP_APM_REGION7_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 7 in REE1 mode.
 */
#define HP_APM_REGION7_R1_R    (BIT(6))
#define HP_APM_REGION7_R1_R_M  (HP_APM_REGION7_R1_R_V << HP_APM_REGION7_R1_R_S)
#define HP_APM_REGION7_R1_R_V  0x00000001U
#define HP_APM_REGION7_R1_R_S  6
/** HP_APM_REGION7_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 7 in REE2 mode.
 */
#define HP_APM_REGION7_R2_X    (BIT(8))
#define HP_APM_REGION7_R2_X_M  (HP_APM_REGION7_R2_X_V << HP_APM_REGION7_R2_X_S)
#define HP_APM_REGION7_R2_X_V  0x00000001U
#define HP_APM_REGION7_R2_X_S  8
/** HP_APM_REGION7_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 7 in REE2 mode.
 */
#define HP_APM_REGION7_R2_W    (BIT(9))
#define HP_APM_REGION7_R2_W_M  (HP_APM_REGION7_R2_W_V << HP_APM_REGION7_R2_W_S)
#define HP_APM_REGION7_R2_W_V  0x00000001U
#define HP_APM_REGION7_R2_W_S  9
/** HP_APM_REGION7_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 7 in REE2 mode.
 */
#define HP_APM_REGION7_R2_R    (BIT(10))
#define HP_APM_REGION7_R2_R_M  (HP_APM_REGION7_R2_R_V << HP_APM_REGION7_R2_R_S)
#define HP_APM_REGION7_R2_R_V  0x00000001U
#define HP_APM_REGION7_R2_R_S  10
/** HP_APM_REGION7_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 7 configuration registers
 *  (HP_APM_REGION7_ADDR_START_REG, HP_APM_REGION7_ADDR_END_REG and
 *  HP_APM_REGION7_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION7_LOCK    (BIT(11))
#define HP_APM_REGION7_LOCK_M  (HP_APM_REGION7_LOCK_V << HP_APM_REGION7_LOCK_S)
#define HP_APM_REGION7_LOCK_V  0x00000001U
#define HP_APM_REGION7_LOCK_S  11

/** HP_APM_REGION8_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION8_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x64)
/** HP_APM_REGION8_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 8.
 */
#define HP_APM_REGION8_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION8_ADDR_START_M  (HP_APM_REGION8_ADDR_START_V << HP_APM_REGION8_ADDR_START_S)
#define HP_APM_REGION8_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION8_ADDR_START_S  0

/** HP_APM_REGION8_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION8_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x68)
/** HP_APM_REGION8_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 8.
 */
#define HP_APM_REGION8_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION8_ADDR_END_M  (HP_APM_REGION8_ADDR_END_V << HP_APM_REGION8_ADDR_END_S)
#define HP_APM_REGION8_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION8_ADDR_END_S  0

/** HP_APM_REGION8_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION8_ATTR_REG (DR_REG_HP_APM_BASE + 0x6c)
/** HP_APM_REGION8_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 8 in REE0 mode.
 */
#define HP_APM_REGION8_R0_X    (BIT(0))
#define HP_APM_REGION8_R0_X_M  (HP_APM_REGION8_R0_X_V << HP_APM_REGION8_R0_X_S)
#define HP_APM_REGION8_R0_X_V  0x00000001U
#define HP_APM_REGION8_R0_X_S  0
/** HP_APM_REGION8_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 8 in REE0 mode.
 */
#define HP_APM_REGION8_R0_W    (BIT(1))
#define HP_APM_REGION8_R0_W_M  (HP_APM_REGION8_R0_W_V << HP_APM_REGION8_R0_W_S)
#define HP_APM_REGION8_R0_W_V  0x00000001U
#define HP_APM_REGION8_R0_W_S  1
/** HP_APM_REGION8_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 8 in REE0 mode.
 */
#define HP_APM_REGION8_R0_R    (BIT(2))
#define HP_APM_REGION8_R0_R_M  (HP_APM_REGION8_R0_R_V << HP_APM_REGION8_R0_R_S)
#define HP_APM_REGION8_R0_R_V  0x00000001U
#define HP_APM_REGION8_R0_R_S  2
/** HP_APM_REGION8_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 8 in REE1 mode.
 */
#define HP_APM_REGION8_R1_X    (BIT(4))
#define HP_APM_REGION8_R1_X_M  (HP_APM_REGION8_R1_X_V << HP_APM_REGION8_R1_X_S)
#define HP_APM_REGION8_R1_X_V  0x00000001U
#define HP_APM_REGION8_R1_X_S  4
/** HP_APM_REGION8_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 8 in REE1 mode.
 */
#define HP_APM_REGION8_R1_W    (BIT(5))
#define HP_APM_REGION8_R1_W_M  (HP_APM_REGION8_R1_W_V << HP_APM_REGION8_R1_W_S)
#define HP_APM_REGION8_R1_W_V  0x00000001U
#define HP_APM_REGION8_R1_W_S  5
/** HP_APM_REGION8_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 8 in REE1 mode.
 */
#define HP_APM_REGION8_R1_R    (BIT(6))
#define HP_APM_REGION8_R1_R_M  (HP_APM_REGION8_R1_R_V << HP_APM_REGION8_R1_R_S)
#define HP_APM_REGION8_R1_R_V  0x00000001U
#define HP_APM_REGION8_R1_R_S  6
/** HP_APM_REGION8_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 8 in REE2 mode.
 */
#define HP_APM_REGION8_R2_X    (BIT(8))
#define HP_APM_REGION8_R2_X_M  (HP_APM_REGION8_R2_X_V << HP_APM_REGION8_R2_X_S)
#define HP_APM_REGION8_R2_X_V  0x00000001U
#define HP_APM_REGION8_R2_X_S  8
/** HP_APM_REGION8_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 8 in REE2 mode.
 */
#define HP_APM_REGION8_R2_W    (BIT(9))
#define HP_APM_REGION8_R2_W_M  (HP_APM_REGION8_R2_W_V << HP_APM_REGION8_R2_W_S)
#define HP_APM_REGION8_R2_W_V  0x00000001U
#define HP_APM_REGION8_R2_W_S  9
/** HP_APM_REGION8_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 8 in REE2 mode.
 */
#define HP_APM_REGION8_R2_R    (BIT(10))
#define HP_APM_REGION8_R2_R_M  (HP_APM_REGION8_R2_R_V << HP_APM_REGION8_R2_R_S)
#define HP_APM_REGION8_R2_R_V  0x00000001U
#define HP_APM_REGION8_R2_R_S  10
/** HP_APM_REGION8_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 8 configuration registers
 *  (HP_APM_REGION8_ADDR_START_REG, HP_APM_REGION8_ADDR_END_REG and
 *  HP_APM_REGION8_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION8_LOCK    (BIT(11))
#define HP_APM_REGION8_LOCK_M  (HP_APM_REGION8_LOCK_V << HP_APM_REGION8_LOCK_S)
#define HP_APM_REGION8_LOCK_V  0x00000001U
#define HP_APM_REGION8_LOCK_S  11

/** HP_APM_REGION9_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION9_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x70)
/** HP_APM_REGION9_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 9.
 */
#define HP_APM_REGION9_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION9_ADDR_START_M  (HP_APM_REGION9_ADDR_START_V << HP_APM_REGION9_ADDR_START_S)
#define HP_APM_REGION9_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION9_ADDR_START_S  0

/** HP_APM_REGION9_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION9_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x74)
/** HP_APM_REGION9_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 9.
 */
#define HP_APM_REGION9_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION9_ADDR_END_M  (HP_APM_REGION9_ADDR_END_V << HP_APM_REGION9_ADDR_END_S)
#define HP_APM_REGION9_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION9_ADDR_END_S  0

/** HP_APM_REGION9_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION9_ATTR_REG (DR_REG_HP_APM_BASE + 0x78)
/** HP_APM_REGION9_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 9 in REE0 mode.
 */
#define HP_APM_REGION9_R0_X    (BIT(0))
#define HP_APM_REGION9_R0_X_M  (HP_APM_REGION9_R0_X_V << HP_APM_REGION9_R0_X_S)
#define HP_APM_REGION9_R0_X_V  0x00000001U
#define HP_APM_REGION9_R0_X_S  0
/** HP_APM_REGION9_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 9 in REE0 mode.
 */
#define HP_APM_REGION9_R0_W    (BIT(1))
#define HP_APM_REGION9_R0_W_M  (HP_APM_REGION9_R0_W_V << HP_APM_REGION9_R0_W_S)
#define HP_APM_REGION9_R0_W_V  0x00000001U
#define HP_APM_REGION9_R0_W_S  1
/** HP_APM_REGION9_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 9 in REE0 mode.
 */
#define HP_APM_REGION9_R0_R    (BIT(2))
#define HP_APM_REGION9_R0_R_M  (HP_APM_REGION9_R0_R_V << HP_APM_REGION9_R0_R_S)
#define HP_APM_REGION9_R0_R_V  0x00000001U
#define HP_APM_REGION9_R0_R_S  2
/** HP_APM_REGION9_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 9 in REE1 mode.
 */
#define HP_APM_REGION9_R1_X    (BIT(4))
#define HP_APM_REGION9_R1_X_M  (HP_APM_REGION9_R1_X_V << HP_APM_REGION9_R1_X_S)
#define HP_APM_REGION9_R1_X_V  0x00000001U
#define HP_APM_REGION9_R1_X_S  4
/** HP_APM_REGION9_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 9 in REE1 mode.
 */
#define HP_APM_REGION9_R1_W    (BIT(5))
#define HP_APM_REGION9_R1_W_M  (HP_APM_REGION9_R1_W_V << HP_APM_REGION9_R1_W_S)
#define HP_APM_REGION9_R1_W_V  0x00000001U
#define HP_APM_REGION9_R1_W_S  5
/** HP_APM_REGION9_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 9 in REE1 mode.
 */
#define HP_APM_REGION9_R1_R    (BIT(6))
#define HP_APM_REGION9_R1_R_M  (HP_APM_REGION9_R1_R_V << HP_APM_REGION9_R1_R_S)
#define HP_APM_REGION9_R1_R_V  0x00000001U
#define HP_APM_REGION9_R1_R_S  6
/** HP_APM_REGION9_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 9 in REE2 mode.
 */
#define HP_APM_REGION9_R2_X    (BIT(8))
#define HP_APM_REGION9_R2_X_M  (HP_APM_REGION9_R2_X_V << HP_APM_REGION9_R2_X_S)
#define HP_APM_REGION9_R2_X_V  0x00000001U
#define HP_APM_REGION9_R2_X_S  8
/** HP_APM_REGION9_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 9 in REE2 mode.
 */
#define HP_APM_REGION9_R2_W    (BIT(9))
#define HP_APM_REGION9_R2_W_M  (HP_APM_REGION9_R2_W_V << HP_APM_REGION9_R2_W_S)
#define HP_APM_REGION9_R2_W_V  0x00000001U
#define HP_APM_REGION9_R2_W_S  9
/** HP_APM_REGION9_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 9 in REE2 mode.
 */
#define HP_APM_REGION9_R2_R    (BIT(10))
#define HP_APM_REGION9_R2_R_M  (HP_APM_REGION9_R2_R_V << HP_APM_REGION9_R2_R_S)
#define HP_APM_REGION9_R2_R_V  0x00000001U
#define HP_APM_REGION9_R2_R_S  10
/** HP_APM_REGION9_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 9 configuration registers
 *  (HP_APM_REGION9_ADDR_START_REG, HP_APM_REGION9_ADDR_END_REG and
 *  HP_APM_REGION9_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION9_LOCK    (BIT(11))
#define HP_APM_REGION9_LOCK_M  (HP_APM_REGION9_LOCK_V << HP_APM_REGION9_LOCK_S)
#define HP_APM_REGION9_LOCK_V  0x00000001U
#define HP_APM_REGION9_LOCK_S  11

/** HP_APM_REGION10_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION10_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x7c)
/** HP_APM_REGION10_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 10.
 */
#define HP_APM_REGION10_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION10_ADDR_START_M  (HP_APM_REGION10_ADDR_START_V << HP_APM_REGION10_ADDR_START_S)
#define HP_APM_REGION10_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION10_ADDR_START_S  0

/** HP_APM_REGION10_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION10_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x80)
/** HP_APM_REGION10_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 10.
 */
#define HP_APM_REGION10_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION10_ADDR_END_M  (HP_APM_REGION10_ADDR_END_V << HP_APM_REGION10_ADDR_END_S)
#define HP_APM_REGION10_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION10_ADDR_END_S  0

/** HP_APM_REGION10_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION10_ATTR_REG (DR_REG_HP_APM_BASE + 0x84)
/** HP_APM_REGION10_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 10 in REE0 mode.
 */
#define HP_APM_REGION10_R0_X    (BIT(0))
#define HP_APM_REGION10_R0_X_M  (HP_APM_REGION10_R0_X_V << HP_APM_REGION10_R0_X_S)
#define HP_APM_REGION10_R0_X_V  0x00000001U
#define HP_APM_REGION10_R0_X_S  0
/** HP_APM_REGION10_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 10 in REE0 mode.
 */
#define HP_APM_REGION10_R0_W    (BIT(1))
#define HP_APM_REGION10_R0_W_M  (HP_APM_REGION10_R0_W_V << HP_APM_REGION10_R0_W_S)
#define HP_APM_REGION10_R0_W_V  0x00000001U
#define HP_APM_REGION10_R0_W_S  1
/** HP_APM_REGION10_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 10 in REE0 mode.
 */
#define HP_APM_REGION10_R0_R    (BIT(2))
#define HP_APM_REGION10_R0_R_M  (HP_APM_REGION10_R0_R_V << HP_APM_REGION10_R0_R_S)
#define HP_APM_REGION10_R0_R_V  0x00000001U
#define HP_APM_REGION10_R0_R_S  2
/** HP_APM_REGION10_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 10 in REE1 mode.
 */
#define HP_APM_REGION10_R1_X    (BIT(4))
#define HP_APM_REGION10_R1_X_M  (HP_APM_REGION10_R1_X_V << HP_APM_REGION10_R1_X_S)
#define HP_APM_REGION10_R1_X_V  0x00000001U
#define HP_APM_REGION10_R1_X_S  4
/** HP_APM_REGION10_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 10 in REE1 mode.
 */
#define HP_APM_REGION10_R1_W    (BIT(5))
#define HP_APM_REGION10_R1_W_M  (HP_APM_REGION10_R1_W_V << HP_APM_REGION10_R1_W_S)
#define HP_APM_REGION10_R1_W_V  0x00000001U
#define HP_APM_REGION10_R1_W_S  5
/** HP_APM_REGION10_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 10 in REE1 mode.
 */
#define HP_APM_REGION10_R1_R    (BIT(6))
#define HP_APM_REGION10_R1_R_M  (HP_APM_REGION10_R1_R_V << HP_APM_REGION10_R1_R_S)
#define HP_APM_REGION10_R1_R_V  0x00000001U
#define HP_APM_REGION10_R1_R_S  6
/** HP_APM_REGION10_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 10 in REE2 mode.
 */
#define HP_APM_REGION10_R2_X    (BIT(8))
#define HP_APM_REGION10_R2_X_M  (HP_APM_REGION10_R2_X_V << HP_APM_REGION10_R2_X_S)
#define HP_APM_REGION10_R2_X_V  0x00000001U
#define HP_APM_REGION10_R2_X_S  8
/** HP_APM_REGION10_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 10 in REE2 mode.
 */
#define HP_APM_REGION10_R2_W    (BIT(9))
#define HP_APM_REGION10_R2_W_M  (HP_APM_REGION10_R2_W_V << HP_APM_REGION10_R2_W_S)
#define HP_APM_REGION10_R2_W_V  0x00000001U
#define HP_APM_REGION10_R2_W_S  9
/** HP_APM_REGION10_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 10 in REE2 mode.
 */
#define HP_APM_REGION10_R2_R    (BIT(10))
#define HP_APM_REGION10_R2_R_M  (HP_APM_REGION10_R2_R_V << HP_APM_REGION10_R2_R_S)
#define HP_APM_REGION10_R2_R_V  0x00000001U
#define HP_APM_REGION10_R2_R_S  10
/** HP_APM_REGION10_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 10 configuration registers
 *  (HP_APM_REGION10_ADDR_START_REG, HP_APM_REGION10_ADDR_END_REG and
 *  HP_APM_REGION10_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION10_LOCK    (BIT(11))
#define HP_APM_REGION10_LOCK_M  (HP_APM_REGION10_LOCK_V << HP_APM_REGION10_LOCK_S)
#define HP_APM_REGION10_LOCK_V  0x00000001U
#define HP_APM_REGION10_LOCK_S  11

/** HP_APM_REGION11_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION11_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x88)
/** HP_APM_REGION11_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 11.
 */
#define HP_APM_REGION11_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION11_ADDR_START_M  (HP_APM_REGION11_ADDR_START_V << HP_APM_REGION11_ADDR_START_S)
#define HP_APM_REGION11_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION11_ADDR_START_S  0

/** HP_APM_REGION11_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION11_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x8c)
/** HP_APM_REGION11_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 11.
 */
#define HP_APM_REGION11_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION11_ADDR_END_M  (HP_APM_REGION11_ADDR_END_V << HP_APM_REGION11_ADDR_END_S)
#define HP_APM_REGION11_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION11_ADDR_END_S  0

/** HP_APM_REGION11_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION11_ATTR_REG (DR_REG_HP_APM_BASE + 0x90)
/** HP_APM_REGION11_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 11 in REE0 mode.
 */
#define HP_APM_REGION11_R0_X    (BIT(0))
#define HP_APM_REGION11_R0_X_M  (HP_APM_REGION11_R0_X_V << HP_APM_REGION11_R0_X_S)
#define HP_APM_REGION11_R0_X_V  0x00000001U
#define HP_APM_REGION11_R0_X_S  0
/** HP_APM_REGION11_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 11 in REE0 mode.
 */
#define HP_APM_REGION11_R0_W    (BIT(1))
#define HP_APM_REGION11_R0_W_M  (HP_APM_REGION11_R0_W_V << HP_APM_REGION11_R0_W_S)
#define HP_APM_REGION11_R0_W_V  0x00000001U
#define HP_APM_REGION11_R0_W_S  1
/** HP_APM_REGION11_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 11 in REE0 mode.
 */
#define HP_APM_REGION11_R0_R    (BIT(2))
#define HP_APM_REGION11_R0_R_M  (HP_APM_REGION11_R0_R_V << HP_APM_REGION11_R0_R_S)
#define HP_APM_REGION11_R0_R_V  0x00000001U
#define HP_APM_REGION11_R0_R_S  2
/** HP_APM_REGION11_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 11 in REE1 mode.
 */
#define HP_APM_REGION11_R1_X    (BIT(4))
#define HP_APM_REGION11_R1_X_M  (HP_APM_REGION11_R1_X_V << HP_APM_REGION11_R1_X_S)
#define HP_APM_REGION11_R1_X_V  0x00000001U
#define HP_APM_REGION11_R1_X_S  4
/** HP_APM_REGION11_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 11 in REE1 mode.
 */
#define HP_APM_REGION11_R1_W    (BIT(5))
#define HP_APM_REGION11_R1_W_M  (HP_APM_REGION11_R1_W_V << HP_APM_REGION11_R1_W_S)
#define HP_APM_REGION11_R1_W_V  0x00000001U
#define HP_APM_REGION11_R1_W_S  5
/** HP_APM_REGION11_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 11 in REE1 mode.
 */
#define HP_APM_REGION11_R1_R    (BIT(6))
#define HP_APM_REGION11_R1_R_M  (HP_APM_REGION11_R1_R_V << HP_APM_REGION11_R1_R_S)
#define HP_APM_REGION11_R1_R_V  0x00000001U
#define HP_APM_REGION11_R1_R_S  6
/** HP_APM_REGION11_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 11 in REE2 mode.
 */
#define HP_APM_REGION11_R2_X    (BIT(8))
#define HP_APM_REGION11_R2_X_M  (HP_APM_REGION11_R2_X_V << HP_APM_REGION11_R2_X_S)
#define HP_APM_REGION11_R2_X_V  0x00000001U
#define HP_APM_REGION11_R2_X_S  8
/** HP_APM_REGION11_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 11 in REE2 mode.
 */
#define HP_APM_REGION11_R2_W    (BIT(9))
#define HP_APM_REGION11_R2_W_M  (HP_APM_REGION11_R2_W_V << HP_APM_REGION11_R2_W_S)
#define HP_APM_REGION11_R2_W_V  0x00000001U
#define HP_APM_REGION11_R2_W_S  9
/** HP_APM_REGION11_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 11 in REE2 mode.
 */
#define HP_APM_REGION11_R2_R    (BIT(10))
#define HP_APM_REGION11_R2_R_M  (HP_APM_REGION11_R2_R_V << HP_APM_REGION11_R2_R_S)
#define HP_APM_REGION11_R2_R_V  0x00000001U
#define HP_APM_REGION11_R2_R_S  10
/** HP_APM_REGION11_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 11 configuration registers
 *  (HP_APM_REGION11_ADDR_START_REG, HP_APM_REGION11_ADDR_END_REG and
 *  HP_APM_REGION11_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION11_LOCK    (BIT(11))
#define HP_APM_REGION11_LOCK_M  (HP_APM_REGION11_LOCK_V << HP_APM_REGION11_LOCK_S)
#define HP_APM_REGION11_LOCK_V  0x00000001U
#define HP_APM_REGION11_LOCK_S  11

/** HP_APM_REGION12_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION12_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x94)
/** HP_APM_REGION12_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 12.
 */
#define HP_APM_REGION12_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION12_ADDR_START_M  (HP_APM_REGION12_ADDR_START_V << HP_APM_REGION12_ADDR_START_S)
#define HP_APM_REGION12_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION12_ADDR_START_S  0

/** HP_APM_REGION12_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION12_ADDR_END_REG (DR_REG_HP_APM_BASE + 0x98)
/** HP_APM_REGION12_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 12.
 */
#define HP_APM_REGION12_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION12_ADDR_END_M  (HP_APM_REGION12_ADDR_END_V << HP_APM_REGION12_ADDR_END_S)
#define HP_APM_REGION12_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION12_ADDR_END_S  0

/** HP_APM_REGION12_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION12_ATTR_REG (DR_REG_HP_APM_BASE + 0x9c)
/** HP_APM_REGION12_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 12 in REE0 mode.
 */
#define HP_APM_REGION12_R0_X    (BIT(0))
#define HP_APM_REGION12_R0_X_M  (HP_APM_REGION12_R0_X_V << HP_APM_REGION12_R0_X_S)
#define HP_APM_REGION12_R0_X_V  0x00000001U
#define HP_APM_REGION12_R0_X_S  0
/** HP_APM_REGION12_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 12 in REE0 mode.
 */
#define HP_APM_REGION12_R0_W    (BIT(1))
#define HP_APM_REGION12_R0_W_M  (HP_APM_REGION12_R0_W_V << HP_APM_REGION12_R0_W_S)
#define HP_APM_REGION12_R0_W_V  0x00000001U
#define HP_APM_REGION12_R0_W_S  1
/** HP_APM_REGION12_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 12 in REE0 mode.
 */
#define HP_APM_REGION12_R0_R    (BIT(2))
#define HP_APM_REGION12_R0_R_M  (HP_APM_REGION12_R0_R_V << HP_APM_REGION12_R0_R_S)
#define HP_APM_REGION12_R0_R_V  0x00000001U
#define HP_APM_REGION12_R0_R_S  2
/** HP_APM_REGION12_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 12 in REE1 mode.
 */
#define HP_APM_REGION12_R1_X    (BIT(4))
#define HP_APM_REGION12_R1_X_M  (HP_APM_REGION12_R1_X_V << HP_APM_REGION12_R1_X_S)
#define HP_APM_REGION12_R1_X_V  0x00000001U
#define HP_APM_REGION12_R1_X_S  4
/** HP_APM_REGION12_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 12 in REE1 mode.
 */
#define HP_APM_REGION12_R1_W    (BIT(5))
#define HP_APM_REGION12_R1_W_M  (HP_APM_REGION12_R1_W_V << HP_APM_REGION12_R1_W_S)
#define HP_APM_REGION12_R1_W_V  0x00000001U
#define HP_APM_REGION12_R1_W_S  5
/** HP_APM_REGION12_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 12 in REE1 mode.
 */
#define HP_APM_REGION12_R1_R    (BIT(6))
#define HP_APM_REGION12_R1_R_M  (HP_APM_REGION12_R1_R_V << HP_APM_REGION12_R1_R_S)
#define HP_APM_REGION12_R1_R_V  0x00000001U
#define HP_APM_REGION12_R1_R_S  6
/** HP_APM_REGION12_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 12 in REE2 mode.
 */
#define HP_APM_REGION12_R2_X    (BIT(8))
#define HP_APM_REGION12_R2_X_M  (HP_APM_REGION12_R2_X_V << HP_APM_REGION12_R2_X_S)
#define HP_APM_REGION12_R2_X_V  0x00000001U
#define HP_APM_REGION12_R2_X_S  8
/** HP_APM_REGION12_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 12 in REE2 mode.
 */
#define HP_APM_REGION12_R2_W    (BIT(9))
#define HP_APM_REGION12_R2_W_M  (HP_APM_REGION12_R2_W_V << HP_APM_REGION12_R2_W_S)
#define HP_APM_REGION12_R2_W_V  0x00000001U
#define HP_APM_REGION12_R2_W_S  9
/** HP_APM_REGION12_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 12 in REE2 mode.
 */
#define HP_APM_REGION12_R2_R    (BIT(10))
#define HP_APM_REGION12_R2_R_M  (HP_APM_REGION12_R2_R_V << HP_APM_REGION12_R2_R_S)
#define HP_APM_REGION12_R2_R_V  0x00000001U
#define HP_APM_REGION12_R2_R_S  10
/** HP_APM_REGION12_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 12 configuration registers
 *  (HP_APM_REGION12_ADDR_START_REG, HP_APM_REGION12_ADDR_END_REG and
 *  HP_APM_REGION12_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION12_LOCK    (BIT(11))
#define HP_APM_REGION12_LOCK_M  (HP_APM_REGION12_LOCK_V << HP_APM_REGION12_LOCK_S)
#define HP_APM_REGION12_LOCK_V  0x00000001U
#define HP_APM_REGION12_LOCK_S  11

/** HP_APM_REGION13_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION13_ADDR_START_REG (DR_REG_HP_APM_BASE + 0xa0)
/** HP_APM_REGION13_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 13.
 */
#define HP_APM_REGION13_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION13_ADDR_START_M  (HP_APM_REGION13_ADDR_START_V << HP_APM_REGION13_ADDR_START_S)
#define HP_APM_REGION13_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION13_ADDR_START_S  0

/** HP_APM_REGION13_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION13_ADDR_END_REG (DR_REG_HP_APM_BASE + 0xa4)
/** HP_APM_REGION13_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 13.
 */
#define HP_APM_REGION13_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION13_ADDR_END_M  (HP_APM_REGION13_ADDR_END_V << HP_APM_REGION13_ADDR_END_S)
#define HP_APM_REGION13_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION13_ADDR_END_S  0

/** HP_APM_REGION13_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION13_ATTR_REG (DR_REG_HP_APM_BASE + 0xa8)
/** HP_APM_REGION13_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 13 in REE0 mode.
 */
#define HP_APM_REGION13_R0_X    (BIT(0))
#define HP_APM_REGION13_R0_X_M  (HP_APM_REGION13_R0_X_V << HP_APM_REGION13_R0_X_S)
#define HP_APM_REGION13_R0_X_V  0x00000001U
#define HP_APM_REGION13_R0_X_S  0
/** HP_APM_REGION13_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 13 in REE0 mode.
 */
#define HP_APM_REGION13_R0_W    (BIT(1))
#define HP_APM_REGION13_R0_W_M  (HP_APM_REGION13_R0_W_V << HP_APM_REGION13_R0_W_S)
#define HP_APM_REGION13_R0_W_V  0x00000001U
#define HP_APM_REGION13_R0_W_S  1
/** HP_APM_REGION13_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 13 in REE0 mode.
 */
#define HP_APM_REGION13_R0_R    (BIT(2))
#define HP_APM_REGION13_R0_R_M  (HP_APM_REGION13_R0_R_V << HP_APM_REGION13_R0_R_S)
#define HP_APM_REGION13_R0_R_V  0x00000001U
#define HP_APM_REGION13_R0_R_S  2
/** HP_APM_REGION13_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 13 in REE1 mode.
 */
#define HP_APM_REGION13_R1_X    (BIT(4))
#define HP_APM_REGION13_R1_X_M  (HP_APM_REGION13_R1_X_V << HP_APM_REGION13_R1_X_S)
#define HP_APM_REGION13_R1_X_V  0x00000001U
#define HP_APM_REGION13_R1_X_S  4
/** HP_APM_REGION13_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 13 in REE1 mode.
 */
#define HP_APM_REGION13_R1_W    (BIT(5))
#define HP_APM_REGION13_R1_W_M  (HP_APM_REGION13_R1_W_V << HP_APM_REGION13_R1_W_S)
#define HP_APM_REGION13_R1_W_V  0x00000001U
#define HP_APM_REGION13_R1_W_S  5
/** HP_APM_REGION13_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 13 in REE1 mode.
 */
#define HP_APM_REGION13_R1_R    (BIT(6))
#define HP_APM_REGION13_R1_R_M  (HP_APM_REGION13_R1_R_V << HP_APM_REGION13_R1_R_S)
#define HP_APM_REGION13_R1_R_V  0x00000001U
#define HP_APM_REGION13_R1_R_S  6
/** HP_APM_REGION13_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 13 in REE2 mode.
 */
#define HP_APM_REGION13_R2_X    (BIT(8))
#define HP_APM_REGION13_R2_X_M  (HP_APM_REGION13_R2_X_V << HP_APM_REGION13_R2_X_S)
#define HP_APM_REGION13_R2_X_V  0x00000001U
#define HP_APM_REGION13_R2_X_S  8
/** HP_APM_REGION13_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 13 in REE2 mode.
 */
#define HP_APM_REGION13_R2_W    (BIT(9))
#define HP_APM_REGION13_R2_W_M  (HP_APM_REGION13_R2_W_V << HP_APM_REGION13_R2_W_S)
#define HP_APM_REGION13_R2_W_V  0x00000001U
#define HP_APM_REGION13_R2_W_S  9
/** HP_APM_REGION13_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 13 in REE2 mode.
 */
#define HP_APM_REGION13_R2_R    (BIT(10))
#define HP_APM_REGION13_R2_R_M  (HP_APM_REGION13_R2_R_V << HP_APM_REGION13_R2_R_S)
#define HP_APM_REGION13_R2_R_V  0x00000001U
#define HP_APM_REGION13_R2_R_S  10
/** HP_APM_REGION13_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 13 configuration registers
 *  (HP_APM_REGION13_ADDR_START_REG, HP_APM_REGION13_ADDR_END_REG and
 *  HP_APM_REGION13_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION13_LOCK    (BIT(11))
#define HP_APM_REGION13_LOCK_M  (HP_APM_REGION13_LOCK_V << HP_APM_REGION13_LOCK_S)
#define HP_APM_REGION13_LOCK_V  0x00000001U
#define HP_APM_REGION13_LOCK_S  11

/** HP_APM_REGION14_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION14_ADDR_START_REG (DR_REG_HP_APM_BASE + 0xac)
/** HP_APM_REGION14_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 14.
 */
#define HP_APM_REGION14_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION14_ADDR_START_M  (HP_APM_REGION14_ADDR_START_V << HP_APM_REGION14_ADDR_START_S)
#define HP_APM_REGION14_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION14_ADDR_START_S  0

/** HP_APM_REGION14_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION14_ADDR_END_REG (DR_REG_HP_APM_BASE + 0xb0)
/** HP_APM_REGION14_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 14.
 */
#define HP_APM_REGION14_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION14_ADDR_END_M  (HP_APM_REGION14_ADDR_END_V << HP_APM_REGION14_ADDR_END_S)
#define HP_APM_REGION14_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION14_ADDR_END_S  0

/** HP_APM_REGION14_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION14_ATTR_REG (DR_REG_HP_APM_BASE + 0xb4)
/** HP_APM_REGION14_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 14 in REE0 mode.
 */
#define HP_APM_REGION14_R0_X    (BIT(0))
#define HP_APM_REGION14_R0_X_M  (HP_APM_REGION14_R0_X_V << HP_APM_REGION14_R0_X_S)
#define HP_APM_REGION14_R0_X_V  0x00000001U
#define HP_APM_REGION14_R0_X_S  0
/** HP_APM_REGION14_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 14 in REE0 mode.
 */
#define HP_APM_REGION14_R0_W    (BIT(1))
#define HP_APM_REGION14_R0_W_M  (HP_APM_REGION14_R0_W_V << HP_APM_REGION14_R0_W_S)
#define HP_APM_REGION14_R0_W_V  0x00000001U
#define HP_APM_REGION14_R0_W_S  1
/** HP_APM_REGION14_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 14 in REE0 mode.
 */
#define HP_APM_REGION14_R0_R    (BIT(2))
#define HP_APM_REGION14_R0_R_M  (HP_APM_REGION14_R0_R_V << HP_APM_REGION14_R0_R_S)
#define HP_APM_REGION14_R0_R_V  0x00000001U
#define HP_APM_REGION14_R0_R_S  2
/** HP_APM_REGION14_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 14 in REE1 mode.
 */
#define HP_APM_REGION14_R1_X    (BIT(4))
#define HP_APM_REGION14_R1_X_M  (HP_APM_REGION14_R1_X_V << HP_APM_REGION14_R1_X_S)
#define HP_APM_REGION14_R1_X_V  0x00000001U
#define HP_APM_REGION14_R1_X_S  4
/** HP_APM_REGION14_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 14 in REE1 mode.
 */
#define HP_APM_REGION14_R1_W    (BIT(5))
#define HP_APM_REGION14_R1_W_M  (HP_APM_REGION14_R1_W_V << HP_APM_REGION14_R1_W_S)
#define HP_APM_REGION14_R1_W_V  0x00000001U
#define HP_APM_REGION14_R1_W_S  5
/** HP_APM_REGION14_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 14 in REE1 mode.
 */
#define HP_APM_REGION14_R1_R    (BIT(6))
#define HP_APM_REGION14_R1_R_M  (HP_APM_REGION14_R1_R_V << HP_APM_REGION14_R1_R_S)
#define HP_APM_REGION14_R1_R_V  0x00000001U
#define HP_APM_REGION14_R1_R_S  6
/** HP_APM_REGION14_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 14 in REE2 mode.
 */
#define HP_APM_REGION14_R2_X    (BIT(8))
#define HP_APM_REGION14_R2_X_M  (HP_APM_REGION14_R2_X_V << HP_APM_REGION14_R2_X_S)
#define HP_APM_REGION14_R2_X_V  0x00000001U
#define HP_APM_REGION14_R2_X_S  8
/** HP_APM_REGION14_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 14 in REE2 mode.
 */
#define HP_APM_REGION14_R2_W    (BIT(9))
#define HP_APM_REGION14_R2_W_M  (HP_APM_REGION14_R2_W_V << HP_APM_REGION14_R2_W_S)
#define HP_APM_REGION14_R2_W_V  0x00000001U
#define HP_APM_REGION14_R2_W_S  9
/** HP_APM_REGION14_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 14 in REE2 mode.
 */
#define HP_APM_REGION14_R2_R    (BIT(10))
#define HP_APM_REGION14_R2_R_M  (HP_APM_REGION14_R2_R_V << HP_APM_REGION14_R2_R_S)
#define HP_APM_REGION14_R2_R_V  0x00000001U
#define HP_APM_REGION14_R2_R_S  10
/** HP_APM_REGION14_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 14 configuration registers
 *  (HP_APM_REGION14_ADDR_START_REG, HP_APM_REGION14_ADDR_END_REG and
 *  HP_APM_REGION14_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION14_LOCK    (BIT(11))
#define HP_APM_REGION14_LOCK_M  (HP_APM_REGION14_LOCK_V << HP_APM_REGION14_LOCK_S)
#define HP_APM_REGION14_LOCK_V  0x00000001U
#define HP_APM_REGION14_LOCK_S  11

/** HP_APM_REGION15_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION15_ADDR_START_REG (DR_REG_HP_APM_BASE + 0xb8)
/** HP_APM_REGION15_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures the start address of region 15.
 */
#define HP_APM_REGION15_ADDR_START    0xFFFFFFFFU
#define HP_APM_REGION15_ADDR_START_M  (HP_APM_REGION15_ADDR_START_V << HP_APM_REGION15_ADDR_START_S)
#define HP_APM_REGION15_ADDR_START_V  0xFFFFFFFFU
#define HP_APM_REGION15_ADDR_START_S  0

/** HP_APM_REGION15_ADDR_END_REG register
 *  Region address register
 */
#define HP_APM_REGION15_ADDR_END_REG (DR_REG_HP_APM_BASE + 0xbc)
/** HP_APM_REGION15_ADDR_END : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the end address of region 15.
 */
#define HP_APM_REGION15_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION15_ADDR_END_M  (HP_APM_REGION15_ADDR_END_V << HP_APM_REGION15_ADDR_END_S)
#define HP_APM_REGION15_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION15_ADDR_END_S  0

/** HP_APM_REGION15_ATTR_REG register
 *  Region access permissions configuration register
 */
#define HP_APM_REGION15_ATTR_REG (DR_REG_HP_APM_BASE + 0xc0)
/** HP_APM_REGION15_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution permission in region 15 in REE0 mode.
 */
#define HP_APM_REGION15_R0_X    (BIT(0))
#define HP_APM_REGION15_R0_X_M  (HP_APM_REGION15_R0_X_V << HP_APM_REGION15_R0_X_S)
#define HP_APM_REGION15_R0_X_V  0x00000001U
#define HP_APM_REGION15_R0_X_S  0
/** HP_APM_REGION15_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write permission in region 15 in REE0 mode.
 */
#define HP_APM_REGION15_R0_W    (BIT(1))
#define HP_APM_REGION15_R0_W_M  (HP_APM_REGION15_R0_W_V << HP_APM_REGION15_R0_W_S)
#define HP_APM_REGION15_R0_W_V  0x00000001U
#define HP_APM_REGION15_R0_W_S  1
/** HP_APM_REGION15_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read permission in region 15 in REE0 mode.
 */
#define HP_APM_REGION15_R0_R    (BIT(2))
#define HP_APM_REGION15_R0_R_M  (HP_APM_REGION15_R0_R_V << HP_APM_REGION15_R0_R_S)
#define HP_APM_REGION15_R0_R_V  0x00000001U
#define HP_APM_REGION15_R0_R_S  2
/** HP_APM_REGION15_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution permission in region 15 in REE1 mode.
 */
#define HP_APM_REGION15_R1_X    (BIT(4))
#define HP_APM_REGION15_R1_X_M  (HP_APM_REGION15_R1_X_V << HP_APM_REGION15_R1_X_S)
#define HP_APM_REGION15_R1_X_V  0x00000001U
#define HP_APM_REGION15_R1_X_S  4
/** HP_APM_REGION15_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write permission in region 15 in REE1 mode.
 */
#define HP_APM_REGION15_R1_W    (BIT(5))
#define HP_APM_REGION15_R1_W_M  (HP_APM_REGION15_R1_W_V << HP_APM_REGION15_R1_W_S)
#define HP_APM_REGION15_R1_W_V  0x00000001U
#define HP_APM_REGION15_R1_W_S  5
/** HP_APM_REGION15_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read permission in region 15 in REE1 mode.
 */
#define HP_APM_REGION15_R1_R    (BIT(6))
#define HP_APM_REGION15_R1_R_M  (HP_APM_REGION15_R1_R_V << HP_APM_REGION15_R1_R_S)
#define HP_APM_REGION15_R1_R_V  0x00000001U
#define HP_APM_REGION15_R1_R_S  6
/** HP_APM_REGION15_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution permission in region 15 in REE2 mode.
 */
#define HP_APM_REGION15_R2_X    (BIT(8))
#define HP_APM_REGION15_R2_X_M  (HP_APM_REGION15_R2_X_V << HP_APM_REGION15_R2_X_S)
#define HP_APM_REGION15_R2_X_V  0x00000001U
#define HP_APM_REGION15_R2_X_S  8
/** HP_APM_REGION15_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write permission in region 15 in REE2 mode.
 */
#define HP_APM_REGION15_R2_W    (BIT(9))
#define HP_APM_REGION15_R2_W_M  (HP_APM_REGION15_R2_W_V << HP_APM_REGION15_R2_W_S)
#define HP_APM_REGION15_R2_W_V  0x00000001U
#define HP_APM_REGION15_R2_W_S  9
/** HP_APM_REGION15_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read permission in region 15 in REE2 mode.
 */
#define HP_APM_REGION15_R2_R    (BIT(10))
#define HP_APM_REGION15_R2_R_M  (HP_APM_REGION15_R2_R_V << HP_APM_REGION15_R2_R_S)
#define HP_APM_REGION15_R2_R_V  0x00000001U
#define HP_APM_REGION15_R2_R_S  10
/** HP_APM_REGION15_LOCK : R/W; bitpos: [11]; default: 0;
 *  Configures to lock the value of region 15 configuration registers
 *  (HP_APM_REGION15_ADDR_START_REG, HP_APM_REGION15_ADDR_END_REG and
 *  HP_APM_REGION15_ATTR_REG).
 *  0: Do not lock
 *  1: Lock
 */
#define HP_APM_REGION15_LOCK    (BIT(11))
#define HP_APM_REGION15_LOCK_M  (HP_APM_REGION15_LOCK_V << HP_APM_REGION15_LOCK_S)
#define HP_APM_REGION15_LOCK_V  0x00000001U
#define HP_APM_REGION15_LOCK_S  11

/** HP_APM_FUNC_CTRL_REG register
 *  APM access path permission management register
 */
#define HP_APM_FUNC_CTRL_REG (DR_REG_HP_APM_BASE + 0xc4)
/** HP_APM_M0_FUNC_EN : R/W; bitpos: [0]; default: 1;
 *  Configures to enable permission management for HP_APM_CTRL M0.
 */
#define HP_APM_M0_FUNC_EN    (BIT(0))
#define HP_APM_M0_FUNC_EN_M  (HP_APM_M0_FUNC_EN_V << HP_APM_M0_FUNC_EN_S)
#define HP_APM_M0_FUNC_EN_V  0x00000001U
#define HP_APM_M0_FUNC_EN_S  0
/** HP_APM_M1_FUNC_EN : R/W; bitpos: [1]; default: 1;
 *  Configures to enable permission management for HP_APM_CTRL M1.
 */
#define HP_APM_M1_FUNC_EN    (BIT(1))
#define HP_APM_M1_FUNC_EN_M  (HP_APM_M1_FUNC_EN_V << HP_APM_M1_FUNC_EN_S)
#define HP_APM_M1_FUNC_EN_V  0x00000001U
#define HP_APM_M1_FUNC_EN_S  1
/** HP_APM_M2_FUNC_EN : R/W; bitpos: [2]; default: 1;
 *  Configures to enable permission management for HP_APM_CTRL M2.
 */
#define HP_APM_M2_FUNC_EN    (BIT(2))
#define HP_APM_M2_FUNC_EN_M  (HP_APM_M2_FUNC_EN_V << HP_APM_M2_FUNC_EN_S)
#define HP_APM_M2_FUNC_EN_V  0x00000001U
#define HP_APM_M2_FUNC_EN_S  2
/** HP_APM_M3_FUNC_EN : R/W; bitpos: [3]; default: 1;
 *  Configures to enable permission management for HP_APM_CTRL M3.
 */
#define HP_APM_M3_FUNC_EN    (BIT(3))
#define HP_APM_M3_FUNC_EN_M  (HP_APM_M3_FUNC_EN_V << HP_APM_M3_FUNC_EN_S)
#define HP_APM_M3_FUNC_EN_V  0x00000001U
#define HP_APM_M3_FUNC_EN_S  3
/** HP_APM_M4_FUNC_EN : R/W; bitpos: [4]; default: 1;
 *  Configures to enable permission management for HP_APM_CTRL M4.
 */
#define HP_APM_M4_FUNC_EN    (BIT(4))
#define HP_APM_M4_FUNC_EN_M  (HP_APM_M4_FUNC_EN_V << HP_APM_M4_FUNC_EN_S)
#define HP_APM_M4_FUNC_EN_V  0x00000001U
#define HP_APM_M4_FUNC_EN_S  4

/** HP_APM_M0_STATUS_REG register
 *  HP_APM_CTRL M0 status register
 */
#define HP_APM_M0_STATUS_REG (DR_REG_HP_APM_BASE + 0xc8)
/** HP_APM_M0_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents permission restrictions
 *  bit1: 1 represents address out of bounds
 */
#define HP_APM_M0_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M0_EXCEPTION_STATUS_M  (HP_APM_M0_EXCEPTION_STATUS_V << HP_APM_M0_EXCEPTION_STATUS_S)
#define HP_APM_M0_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M0_EXCEPTION_STATUS_S  0

/** HP_APM_M0_STATUS_CLR_REG register
 *  HP_APM_CTRL M0 status clear register
 */
#define HP_APM_M0_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xcc)
/** HP_APM_M0_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_APM_M0_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_APM_M0_EXCEPTION_STATUS_CLR_M  (HP_APM_M0_EXCEPTION_STATUS_CLR_V << HP_APM_M0_EXCEPTION_STATUS_CLR_S)
#define HP_APM_M0_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_APM_M0_EXCEPTION_STATUS_CLR_S  0

/** HP_APM_M0_EXCEPTION_INFO0_REG register
 *  HP_APM_CTRL M0 exception information register
 */
#define HP_APM_M0_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0xd0)
/** HP_APM_M0_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Represents the region where an exception occurs.
 */
#define HP_APM_M0_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M0_EXCEPTION_REGION_M  (HP_APM_M0_EXCEPTION_REGION_V << HP_APM_M0_EXCEPTION_REGION_S)
#define HP_APM_M0_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M0_EXCEPTION_REGION_S  0
/** HP_APM_M0_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents the master's security mode when an exception occurs.
 */
#define HP_APM_M0_EXCEPTION_MODE    0x00000003U
#define HP_APM_M0_EXCEPTION_MODE_M  (HP_APM_M0_EXCEPTION_MODE_V << HP_APM_M0_EXCEPTION_MODE_S)
#define HP_APM_M0_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M0_EXCEPTION_MODE_S  16
/** HP_APM_M0_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents master ID when an exception occurs.
 */
#define HP_APM_M0_EXCEPTION_ID    0x0000001FU
#define HP_APM_M0_EXCEPTION_ID_M  (HP_APM_M0_EXCEPTION_ID_V << HP_APM_M0_EXCEPTION_ID_S)
#define HP_APM_M0_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M0_EXCEPTION_ID_S  18

/** HP_APM_M0_EXCEPTION_INFO1_REG register
 *  HP_APM_CTRL M0 exception information register
 */
#define HP_APM_M0_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0xd4)
/** HP_APM_M0_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the access address when an exception occurs.
 */
#define HP_APM_M0_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M0_EXCEPTION_ADDR_M  (HP_APM_M0_EXCEPTION_ADDR_V << HP_APM_M0_EXCEPTION_ADDR_S)
#define HP_APM_M0_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M0_EXCEPTION_ADDR_S  0

/** HP_APM_M1_STATUS_REG register
 *  HP_APM_CTRL M1 status register
 */
#define HP_APM_M1_STATUS_REG (DR_REG_HP_APM_BASE + 0xd8)
/** HP_APM_M1_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents permission restrictions
 *  bit1: 1 represents address out of bounds
 */
#define HP_APM_M1_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M1_EXCEPTION_STATUS_M  (HP_APM_M1_EXCEPTION_STATUS_V << HP_APM_M1_EXCEPTION_STATUS_S)
#define HP_APM_M1_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M1_EXCEPTION_STATUS_S  0

/** HP_APM_M1_STATUS_CLR_REG register
 *  HP_APM_CTRL M1 status clear register
 */
#define HP_APM_M1_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xdc)
/** HP_APM_M1_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_APM_M1_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_APM_M1_EXCEPTION_STATUS_CLR_M  (HP_APM_M1_EXCEPTION_STATUS_CLR_V << HP_APM_M1_EXCEPTION_STATUS_CLR_S)
#define HP_APM_M1_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_APM_M1_EXCEPTION_STATUS_CLR_S  0

/** HP_APM_M1_EXCEPTION_INFO0_REG register
 *  HP_APM_CTRL M1 exception information register
 */
#define HP_APM_M1_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0xe0)
/** HP_APM_M1_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Represents the region where an exception occurs.
 */
#define HP_APM_M1_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M1_EXCEPTION_REGION_M  (HP_APM_M1_EXCEPTION_REGION_V << HP_APM_M1_EXCEPTION_REGION_S)
#define HP_APM_M1_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M1_EXCEPTION_REGION_S  0
/** HP_APM_M1_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents the master's security mode when an exception occurs.
 */
#define HP_APM_M1_EXCEPTION_MODE    0x00000003U
#define HP_APM_M1_EXCEPTION_MODE_M  (HP_APM_M1_EXCEPTION_MODE_V << HP_APM_M1_EXCEPTION_MODE_S)
#define HP_APM_M1_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M1_EXCEPTION_MODE_S  16
/** HP_APM_M1_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents master ID when an exception occurs.
 */
#define HP_APM_M1_EXCEPTION_ID    0x0000001FU
#define HP_APM_M1_EXCEPTION_ID_M  (HP_APM_M1_EXCEPTION_ID_V << HP_APM_M1_EXCEPTION_ID_S)
#define HP_APM_M1_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M1_EXCEPTION_ID_S  18

/** HP_APM_M1_EXCEPTION_INFO1_REG register
 *  HP_APM_CTRL M1 exception information register
 */
#define HP_APM_M1_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0xe4)
/** HP_APM_M1_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the access address when an exception occurs.
 */
#define HP_APM_M1_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M1_EXCEPTION_ADDR_M  (HP_APM_M1_EXCEPTION_ADDR_V << HP_APM_M1_EXCEPTION_ADDR_S)
#define HP_APM_M1_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M1_EXCEPTION_ADDR_S  0

/** HP_APM_M2_STATUS_REG register
 *  HP_APM_CTRL M2 status register
 */
#define HP_APM_M2_STATUS_REG (DR_REG_HP_APM_BASE + 0xe8)
/** HP_APM_M2_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents permission restrictions
 *  bit1: 1 represents address out of bounds
 */
#define HP_APM_M2_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M2_EXCEPTION_STATUS_M  (HP_APM_M2_EXCEPTION_STATUS_V << HP_APM_M2_EXCEPTION_STATUS_S)
#define HP_APM_M2_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M2_EXCEPTION_STATUS_S  0

/** HP_APM_M2_STATUS_CLR_REG register
 *  HP_APM_CTRL M2 status clear register
 */
#define HP_APM_M2_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xec)
/** HP_APM_M2_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_APM_M2_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_APM_M2_EXCEPTION_STATUS_CLR_M  (HP_APM_M2_EXCEPTION_STATUS_CLR_V << HP_APM_M2_EXCEPTION_STATUS_CLR_S)
#define HP_APM_M2_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_APM_M2_EXCEPTION_STATUS_CLR_S  0

/** HP_APM_M2_EXCEPTION_INFO0_REG register
 *  HP_APM_CTRL M2 exception information register
 */
#define HP_APM_M2_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0xf0)
/** HP_APM_M2_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Represents the region where an exception occurs.
 */
#define HP_APM_M2_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M2_EXCEPTION_REGION_M  (HP_APM_M2_EXCEPTION_REGION_V << HP_APM_M2_EXCEPTION_REGION_S)
#define HP_APM_M2_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M2_EXCEPTION_REGION_S  0
/** HP_APM_M2_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents the master's security mode when an exception occurs.
 */
#define HP_APM_M2_EXCEPTION_MODE    0x00000003U
#define HP_APM_M2_EXCEPTION_MODE_M  (HP_APM_M2_EXCEPTION_MODE_V << HP_APM_M2_EXCEPTION_MODE_S)
#define HP_APM_M2_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M2_EXCEPTION_MODE_S  16
/** HP_APM_M2_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents master ID when an exception occurs.
 */
#define HP_APM_M2_EXCEPTION_ID    0x0000001FU
#define HP_APM_M2_EXCEPTION_ID_M  (HP_APM_M2_EXCEPTION_ID_V << HP_APM_M2_EXCEPTION_ID_S)
#define HP_APM_M2_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M2_EXCEPTION_ID_S  18

/** HP_APM_M2_EXCEPTION_INFO1_REG register
 *  HP_APM_CTRL M2 exception information register
 */
#define HP_APM_M2_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0xf4)
/** HP_APM_M2_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the access address when an exception occurs.
 */
#define HP_APM_M2_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M2_EXCEPTION_ADDR_M  (HP_APM_M2_EXCEPTION_ADDR_V << HP_APM_M2_EXCEPTION_ADDR_S)
#define HP_APM_M2_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M2_EXCEPTION_ADDR_S  0

/** HP_APM_M3_STATUS_REG register
 *  HP_APM_CTRL M3 status register
 */
#define HP_APM_M3_STATUS_REG (DR_REG_HP_APM_BASE + 0xf8)
/** HP_APM_M3_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents permission restrictions
 *  bit1: 1 represents address out of bounds
 */
#define HP_APM_M3_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M3_EXCEPTION_STATUS_M  (HP_APM_M3_EXCEPTION_STATUS_V << HP_APM_M3_EXCEPTION_STATUS_S)
#define HP_APM_M3_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M3_EXCEPTION_STATUS_S  0

/** HP_APM_M3_STATUS_CLR_REG register
 *  HP_APM_CTRL M3 status clear register
 */
#define HP_APM_M3_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xfc)
/** HP_APM_M3_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_APM_M3_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_APM_M3_EXCEPTION_STATUS_CLR_M  (HP_APM_M3_EXCEPTION_STATUS_CLR_V << HP_APM_M3_EXCEPTION_STATUS_CLR_S)
#define HP_APM_M3_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_APM_M3_EXCEPTION_STATUS_CLR_S  0

/** HP_APM_M3_EXCEPTION_INFO0_REG register
 *  HP_APM_CTRL M3 exception information register
 */
#define HP_APM_M3_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0x100)
/** HP_APM_M3_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Represents the region where an exception occurs.
 */
#define HP_APM_M3_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M3_EXCEPTION_REGION_M  (HP_APM_M3_EXCEPTION_REGION_V << HP_APM_M3_EXCEPTION_REGION_S)
#define HP_APM_M3_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M3_EXCEPTION_REGION_S  0
/** HP_APM_M3_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents the master's security mode when an exception occurs.
 */
#define HP_APM_M3_EXCEPTION_MODE    0x00000003U
#define HP_APM_M3_EXCEPTION_MODE_M  (HP_APM_M3_EXCEPTION_MODE_V << HP_APM_M3_EXCEPTION_MODE_S)
#define HP_APM_M3_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M3_EXCEPTION_MODE_S  16
/** HP_APM_M3_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents master ID when an exception occurs.
 */
#define HP_APM_M3_EXCEPTION_ID    0x0000001FU
#define HP_APM_M3_EXCEPTION_ID_M  (HP_APM_M3_EXCEPTION_ID_V << HP_APM_M3_EXCEPTION_ID_S)
#define HP_APM_M3_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M3_EXCEPTION_ID_S  18

/** HP_APM_M3_EXCEPTION_INFO1_REG register
 *  HP_APM_CTRL M3 exception information register
 */
#define HP_APM_M3_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0x104)
/** HP_APM_M3_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the access address when an exception occurs.
 */
#define HP_APM_M3_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M3_EXCEPTION_ADDR_M  (HP_APM_M3_EXCEPTION_ADDR_V << HP_APM_M3_EXCEPTION_ADDR_S)
#define HP_APM_M3_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M3_EXCEPTION_ADDR_S  0

/** HP_APM_M4_STATUS_REG register
 *  HP_APM_CTRL M4 status register
 */
#define HP_APM_M4_STATUS_REG (DR_REG_HP_APM_BASE + 0x108)
/** HP_APM_M4_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents permission restrictions
 *  bit1: 1 represents address out of bounds
 */
#define HP_APM_M4_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M4_EXCEPTION_STATUS_M  (HP_APM_M4_EXCEPTION_STATUS_V << HP_APM_M4_EXCEPTION_STATUS_S)
#define HP_APM_M4_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M4_EXCEPTION_STATUS_S  0

/** HP_APM_M4_STATUS_CLR_REG register
 *  HP_APM_CTRL M4 status clear register
 */
#define HP_APM_M4_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0x10c)
/** HP_APM_M4_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_APM_M4_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_APM_M4_EXCEPTION_STATUS_CLR_M  (HP_APM_M4_EXCEPTION_STATUS_CLR_V << HP_APM_M4_EXCEPTION_STATUS_CLR_S)
#define HP_APM_M4_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_APM_M4_EXCEPTION_STATUS_CLR_S  0

/** HP_APM_M4_EXCEPTION_INFO0_REG register
 *  HP_APM_CTRL M4 exception information register
 */
#define HP_APM_M4_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0x110)
/** HP_APM_M4_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Represents the region where an exception occurs.
 */
#define HP_APM_M4_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M4_EXCEPTION_REGION_M  (HP_APM_M4_EXCEPTION_REGION_V << HP_APM_M4_EXCEPTION_REGION_S)
#define HP_APM_M4_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M4_EXCEPTION_REGION_S  0
/** HP_APM_M4_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents the master's security mode when an exception occurs.
 */
#define HP_APM_M4_EXCEPTION_MODE    0x00000003U
#define HP_APM_M4_EXCEPTION_MODE_M  (HP_APM_M4_EXCEPTION_MODE_V << HP_APM_M4_EXCEPTION_MODE_S)
#define HP_APM_M4_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M4_EXCEPTION_MODE_S  16
/** HP_APM_M4_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents master ID when an exception occurs.
 */
#define HP_APM_M4_EXCEPTION_ID    0x0000001FU
#define HP_APM_M4_EXCEPTION_ID_M  (HP_APM_M4_EXCEPTION_ID_V << HP_APM_M4_EXCEPTION_ID_S)
#define HP_APM_M4_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M4_EXCEPTION_ID_S  18

/** HP_APM_M4_EXCEPTION_INFO1_REG register
 *  HP_APM_CTRL M4 exception information register
 */
#define HP_APM_M4_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0x114)
/** HP_APM_M4_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the access address when an exception occurs.
 */
#define HP_APM_M4_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M4_EXCEPTION_ADDR_M  (HP_APM_M4_EXCEPTION_ADDR_V << HP_APM_M4_EXCEPTION_ADDR_S)
#define HP_APM_M4_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M4_EXCEPTION_ADDR_S  0

/** HP_APM_INT_EN_REG register
 *  HP_APM_CTRL M0/1/2/3/4 interrupt enable register
 */
#define HP_APM_INT_EN_REG (DR_REG_HP_APM_BASE + 0x118)
/** HP_APM_M0_APM_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable HP_APM_CTRL M0 interrupt.
 *  0: Disable
 *  1: Enable
 */
#define HP_APM_M0_APM_INT_EN    (BIT(0))
#define HP_APM_M0_APM_INT_EN_M  (HP_APM_M0_APM_INT_EN_V << HP_APM_M0_APM_INT_EN_S)
#define HP_APM_M0_APM_INT_EN_V  0x00000001U
#define HP_APM_M0_APM_INT_EN_S  0
/** HP_APM_M1_APM_INT_EN : R/W; bitpos: [1]; default: 0;
 *  Configures to enable HP_APM_CTRL M1 interrupt.
 *  0: Disable
 *  1: Enable
 */
#define HP_APM_M1_APM_INT_EN    (BIT(1))
#define HP_APM_M1_APM_INT_EN_M  (HP_APM_M1_APM_INT_EN_V << HP_APM_M1_APM_INT_EN_S)
#define HP_APM_M1_APM_INT_EN_V  0x00000001U
#define HP_APM_M1_APM_INT_EN_S  1
/** HP_APM_M2_APM_INT_EN : R/W; bitpos: [2]; default: 0;
 *  Configures to enable HP_APM_CTRL M2 interrupt.
 *  0: Disable
 *  1: Enable
 */
#define HP_APM_M2_APM_INT_EN    (BIT(2))
#define HP_APM_M2_APM_INT_EN_M  (HP_APM_M2_APM_INT_EN_V << HP_APM_M2_APM_INT_EN_S)
#define HP_APM_M2_APM_INT_EN_V  0x00000001U
#define HP_APM_M2_APM_INT_EN_S  2
/** HP_APM_M3_APM_INT_EN : R/W; bitpos: [3]; default: 0;
 *  Configures to enable HP_APM_CTRL M3 interrupt.
 *  0: Disable
 *  1: Enable
 */
#define HP_APM_M3_APM_INT_EN    (BIT(3))
#define HP_APM_M3_APM_INT_EN_M  (HP_APM_M3_APM_INT_EN_V << HP_APM_M3_APM_INT_EN_S)
#define HP_APM_M3_APM_INT_EN_V  0x00000001U
#define HP_APM_M3_APM_INT_EN_S  3
/** HP_APM_M4_APM_INT_EN : R/W; bitpos: [4]; default: 0;
 *  Configures to enable HP_APM_CTRL M4 interrupt.
 *  0: Disable
 *  1: Enable
 */
#define HP_APM_M4_APM_INT_EN    (BIT(4))
#define HP_APM_M4_APM_INT_EN_M  (HP_APM_M4_APM_INT_EN_V << HP_APM_M4_APM_INT_EN_S)
#define HP_APM_M4_APM_INT_EN_V  0x00000001U
#define HP_APM_M4_APM_INT_EN_S  4

/** HP_APM_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define HP_APM_CLOCK_GATE_REG (DR_REG_HP_APM_BASE + 0x7f8)
/** HP_APM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: Enable automatic clock gating
 *  1: Keep the clock always on
 */
#define HP_APM_CLK_EN    (BIT(0))
#define HP_APM_CLK_EN_M  (HP_APM_CLK_EN_V << HP_APM_CLK_EN_S)
#define HP_APM_CLK_EN_V  0x00000001U
#define HP_APM_CLK_EN_S  0

/** HP_APM_DATE_REG register
 *  Version control register
 */
#define HP_APM_DATE_REG (DR_REG_HP_APM_BASE + 0x7fc)
/** HP_APM_DATE : R/W; bitpos: [27:0]; default: 36773904;
 *  Version control register.
 */
#define HP_APM_DATE    0x0FFFFFFFU
#define HP_APM_DATE_M  (HP_APM_DATE_V << HP_APM_DATE_S)
#define HP_APM_DATE_V  0x0FFFFFFFU
#define HP_APM_DATE_S  0

#ifdef __cplusplus
}
#endif
