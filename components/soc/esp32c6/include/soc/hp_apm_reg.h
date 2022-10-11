/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
 *  Region filter enable register
 */
#define HP_APM_REGION_FILTER_EN_REG (DR_REG_HP_APM_BASE + 0x0)
/** HP_APM_REGION_FILTER_EN : R/W; bitpos: [15:0]; default: 1;
 *  Region filter enable
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
 *  Start address of region0
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
 *  End address of region0
 */
#define HP_APM_REGION0_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION0_ADDR_END_M  (HP_APM_REGION0_ADDR_END_V << HP_APM_REGION0_ADDR_END_S)
#define HP_APM_REGION0_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION0_ADDR_END_S  0

/** HP_APM_REGION0_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION0_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0xc)
/** HP_APM_REGION0_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION0_R0_PMS_X    (BIT(0))
#define HP_APM_REGION0_R0_PMS_X_M  (HP_APM_REGION0_R0_PMS_X_V << HP_APM_REGION0_R0_PMS_X_S)
#define HP_APM_REGION0_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION0_R0_PMS_X_S  0
/** HP_APM_REGION0_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION0_R0_PMS_W    (BIT(1))
#define HP_APM_REGION0_R0_PMS_W_M  (HP_APM_REGION0_R0_PMS_W_V << HP_APM_REGION0_R0_PMS_W_S)
#define HP_APM_REGION0_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION0_R0_PMS_W_S  1
/** HP_APM_REGION0_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION0_R0_PMS_R    (BIT(2))
#define HP_APM_REGION0_R0_PMS_R_M  (HP_APM_REGION0_R0_PMS_R_V << HP_APM_REGION0_R0_PMS_R_S)
#define HP_APM_REGION0_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION0_R0_PMS_R_S  2
/** HP_APM_REGION0_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION0_R1_PMS_X    (BIT(4))
#define HP_APM_REGION0_R1_PMS_X_M  (HP_APM_REGION0_R1_PMS_X_V << HP_APM_REGION0_R1_PMS_X_S)
#define HP_APM_REGION0_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION0_R1_PMS_X_S  4
/** HP_APM_REGION0_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION0_R1_PMS_W    (BIT(5))
#define HP_APM_REGION0_R1_PMS_W_M  (HP_APM_REGION0_R1_PMS_W_V << HP_APM_REGION0_R1_PMS_W_S)
#define HP_APM_REGION0_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION0_R1_PMS_W_S  5
/** HP_APM_REGION0_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION0_R1_PMS_R    (BIT(6))
#define HP_APM_REGION0_R1_PMS_R_M  (HP_APM_REGION0_R1_PMS_R_V << HP_APM_REGION0_R1_PMS_R_S)
#define HP_APM_REGION0_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION0_R1_PMS_R_S  6
/** HP_APM_REGION0_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION0_R2_PMS_X    (BIT(8))
#define HP_APM_REGION0_R2_PMS_X_M  (HP_APM_REGION0_R2_PMS_X_V << HP_APM_REGION0_R2_PMS_X_S)
#define HP_APM_REGION0_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION0_R2_PMS_X_S  8
/** HP_APM_REGION0_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION0_R2_PMS_W    (BIT(9))
#define HP_APM_REGION0_R2_PMS_W_M  (HP_APM_REGION0_R2_PMS_W_V << HP_APM_REGION0_R2_PMS_W_S)
#define HP_APM_REGION0_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION0_R2_PMS_W_S  9
/** HP_APM_REGION0_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION0_R2_PMS_R    (BIT(10))
#define HP_APM_REGION0_R2_PMS_R_M  (HP_APM_REGION0_R2_PMS_R_V << HP_APM_REGION0_R2_PMS_R_S)
#define HP_APM_REGION0_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION0_R2_PMS_R_S  10

/** HP_APM_REGION1_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION1_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x10)
/** HP_APM_REGION1_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region1
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
 *  End address of region1
 */
#define HP_APM_REGION1_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION1_ADDR_END_M  (HP_APM_REGION1_ADDR_END_V << HP_APM_REGION1_ADDR_END_S)
#define HP_APM_REGION1_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION1_ADDR_END_S  0

/** HP_APM_REGION1_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION1_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x18)
/** HP_APM_REGION1_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION1_R0_PMS_X    (BIT(0))
#define HP_APM_REGION1_R0_PMS_X_M  (HP_APM_REGION1_R0_PMS_X_V << HP_APM_REGION1_R0_PMS_X_S)
#define HP_APM_REGION1_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION1_R0_PMS_X_S  0
/** HP_APM_REGION1_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION1_R0_PMS_W    (BIT(1))
#define HP_APM_REGION1_R0_PMS_W_M  (HP_APM_REGION1_R0_PMS_W_V << HP_APM_REGION1_R0_PMS_W_S)
#define HP_APM_REGION1_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION1_R0_PMS_W_S  1
/** HP_APM_REGION1_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION1_R0_PMS_R    (BIT(2))
#define HP_APM_REGION1_R0_PMS_R_M  (HP_APM_REGION1_R0_PMS_R_V << HP_APM_REGION1_R0_PMS_R_S)
#define HP_APM_REGION1_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION1_R0_PMS_R_S  2
/** HP_APM_REGION1_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION1_R1_PMS_X    (BIT(4))
#define HP_APM_REGION1_R1_PMS_X_M  (HP_APM_REGION1_R1_PMS_X_V << HP_APM_REGION1_R1_PMS_X_S)
#define HP_APM_REGION1_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION1_R1_PMS_X_S  4
/** HP_APM_REGION1_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION1_R1_PMS_W    (BIT(5))
#define HP_APM_REGION1_R1_PMS_W_M  (HP_APM_REGION1_R1_PMS_W_V << HP_APM_REGION1_R1_PMS_W_S)
#define HP_APM_REGION1_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION1_R1_PMS_W_S  5
/** HP_APM_REGION1_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION1_R1_PMS_R    (BIT(6))
#define HP_APM_REGION1_R1_PMS_R_M  (HP_APM_REGION1_R1_PMS_R_V << HP_APM_REGION1_R1_PMS_R_S)
#define HP_APM_REGION1_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION1_R1_PMS_R_S  6
/** HP_APM_REGION1_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION1_R2_PMS_X    (BIT(8))
#define HP_APM_REGION1_R2_PMS_X_M  (HP_APM_REGION1_R2_PMS_X_V << HP_APM_REGION1_R2_PMS_X_S)
#define HP_APM_REGION1_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION1_R2_PMS_X_S  8
/** HP_APM_REGION1_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION1_R2_PMS_W    (BIT(9))
#define HP_APM_REGION1_R2_PMS_W_M  (HP_APM_REGION1_R2_PMS_W_V << HP_APM_REGION1_R2_PMS_W_S)
#define HP_APM_REGION1_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION1_R2_PMS_W_S  9
/** HP_APM_REGION1_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION1_R2_PMS_R    (BIT(10))
#define HP_APM_REGION1_R2_PMS_R_M  (HP_APM_REGION1_R2_PMS_R_V << HP_APM_REGION1_R2_PMS_R_S)
#define HP_APM_REGION1_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION1_R2_PMS_R_S  10

/** HP_APM_REGION2_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION2_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x1c)
/** HP_APM_REGION2_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region2
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
 *  End address of region2
 */
#define HP_APM_REGION2_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION2_ADDR_END_M  (HP_APM_REGION2_ADDR_END_V << HP_APM_REGION2_ADDR_END_S)
#define HP_APM_REGION2_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION2_ADDR_END_S  0

/** HP_APM_REGION2_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION2_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x24)
/** HP_APM_REGION2_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION2_R0_PMS_X    (BIT(0))
#define HP_APM_REGION2_R0_PMS_X_M  (HP_APM_REGION2_R0_PMS_X_V << HP_APM_REGION2_R0_PMS_X_S)
#define HP_APM_REGION2_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION2_R0_PMS_X_S  0
/** HP_APM_REGION2_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION2_R0_PMS_W    (BIT(1))
#define HP_APM_REGION2_R0_PMS_W_M  (HP_APM_REGION2_R0_PMS_W_V << HP_APM_REGION2_R0_PMS_W_S)
#define HP_APM_REGION2_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION2_R0_PMS_W_S  1
/** HP_APM_REGION2_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION2_R0_PMS_R    (BIT(2))
#define HP_APM_REGION2_R0_PMS_R_M  (HP_APM_REGION2_R0_PMS_R_V << HP_APM_REGION2_R0_PMS_R_S)
#define HP_APM_REGION2_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION2_R0_PMS_R_S  2
/** HP_APM_REGION2_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION2_R1_PMS_X    (BIT(4))
#define HP_APM_REGION2_R1_PMS_X_M  (HP_APM_REGION2_R1_PMS_X_V << HP_APM_REGION2_R1_PMS_X_S)
#define HP_APM_REGION2_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION2_R1_PMS_X_S  4
/** HP_APM_REGION2_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION2_R1_PMS_W    (BIT(5))
#define HP_APM_REGION2_R1_PMS_W_M  (HP_APM_REGION2_R1_PMS_W_V << HP_APM_REGION2_R1_PMS_W_S)
#define HP_APM_REGION2_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION2_R1_PMS_W_S  5
/** HP_APM_REGION2_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION2_R1_PMS_R    (BIT(6))
#define HP_APM_REGION2_R1_PMS_R_M  (HP_APM_REGION2_R1_PMS_R_V << HP_APM_REGION2_R1_PMS_R_S)
#define HP_APM_REGION2_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION2_R1_PMS_R_S  6
/** HP_APM_REGION2_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION2_R2_PMS_X    (BIT(8))
#define HP_APM_REGION2_R2_PMS_X_M  (HP_APM_REGION2_R2_PMS_X_V << HP_APM_REGION2_R2_PMS_X_S)
#define HP_APM_REGION2_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION2_R2_PMS_X_S  8
/** HP_APM_REGION2_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION2_R2_PMS_W    (BIT(9))
#define HP_APM_REGION2_R2_PMS_W_M  (HP_APM_REGION2_R2_PMS_W_V << HP_APM_REGION2_R2_PMS_W_S)
#define HP_APM_REGION2_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION2_R2_PMS_W_S  9
/** HP_APM_REGION2_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION2_R2_PMS_R    (BIT(10))
#define HP_APM_REGION2_R2_PMS_R_M  (HP_APM_REGION2_R2_PMS_R_V << HP_APM_REGION2_R2_PMS_R_S)
#define HP_APM_REGION2_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION2_R2_PMS_R_S  10

/** HP_APM_REGION3_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION3_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x28)
/** HP_APM_REGION3_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region3
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
 *  End address of region3
 */
#define HP_APM_REGION3_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION3_ADDR_END_M  (HP_APM_REGION3_ADDR_END_V << HP_APM_REGION3_ADDR_END_S)
#define HP_APM_REGION3_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION3_ADDR_END_S  0

/** HP_APM_REGION3_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION3_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x30)
/** HP_APM_REGION3_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION3_R0_PMS_X    (BIT(0))
#define HP_APM_REGION3_R0_PMS_X_M  (HP_APM_REGION3_R0_PMS_X_V << HP_APM_REGION3_R0_PMS_X_S)
#define HP_APM_REGION3_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION3_R0_PMS_X_S  0
/** HP_APM_REGION3_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION3_R0_PMS_W    (BIT(1))
#define HP_APM_REGION3_R0_PMS_W_M  (HP_APM_REGION3_R0_PMS_W_V << HP_APM_REGION3_R0_PMS_W_S)
#define HP_APM_REGION3_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION3_R0_PMS_W_S  1
/** HP_APM_REGION3_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION3_R0_PMS_R    (BIT(2))
#define HP_APM_REGION3_R0_PMS_R_M  (HP_APM_REGION3_R0_PMS_R_V << HP_APM_REGION3_R0_PMS_R_S)
#define HP_APM_REGION3_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION3_R0_PMS_R_S  2
/** HP_APM_REGION3_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION3_R1_PMS_X    (BIT(4))
#define HP_APM_REGION3_R1_PMS_X_M  (HP_APM_REGION3_R1_PMS_X_V << HP_APM_REGION3_R1_PMS_X_S)
#define HP_APM_REGION3_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION3_R1_PMS_X_S  4
/** HP_APM_REGION3_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION3_R1_PMS_W    (BIT(5))
#define HP_APM_REGION3_R1_PMS_W_M  (HP_APM_REGION3_R1_PMS_W_V << HP_APM_REGION3_R1_PMS_W_S)
#define HP_APM_REGION3_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION3_R1_PMS_W_S  5
/** HP_APM_REGION3_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION3_R1_PMS_R    (BIT(6))
#define HP_APM_REGION3_R1_PMS_R_M  (HP_APM_REGION3_R1_PMS_R_V << HP_APM_REGION3_R1_PMS_R_S)
#define HP_APM_REGION3_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION3_R1_PMS_R_S  6
/** HP_APM_REGION3_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION3_R2_PMS_X    (BIT(8))
#define HP_APM_REGION3_R2_PMS_X_M  (HP_APM_REGION3_R2_PMS_X_V << HP_APM_REGION3_R2_PMS_X_S)
#define HP_APM_REGION3_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION3_R2_PMS_X_S  8
/** HP_APM_REGION3_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION3_R2_PMS_W    (BIT(9))
#define HP_APM_REGION3_R2_PMS_W_M  (HP_APM_REGION3_R2_PMS_W_V << HP_APM_REGION3_R2_PMS_W_S)
#define HP_APM_REGION3_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION3_R2_PMS_W_S  9
/** HP_APM_REGION3_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION3_R2_PMS_R    (BIT(10))
#define HP_APM_REGION3_R2_PMS_R_M  (HP_APM_REGION3_R2_PMS_R_V << HP_APM_REGION3_R2_PMS_R_S)
#define HP_APM_REGION3_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION3_R2_PMS_R_S  10

/** HP_APM_REGION4_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION4_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x34)
/** HP_APM_REGION4_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region4
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
 *  End address of region4
 */
#define HP_APM_REGION4_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION4_ADDR_END_M  (HP_APM_REGION4_ADDR_END_V << HP_APM_REGION4_ADDR_END_S)
#define HP_APM_REGION4_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION4_ADDR_END_S  0

/** HP_APM_REGION4_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION4_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x3c)
/** HP_APM_REGION4_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION4_R0_PMS_X    (BIT(0))
#define HP_APM_REGION4_R0_PMS_X_M  (HP_APM_REGION4_R0_PMS_X_V << HP_APM_REGION4_R0_PMS_X_S)
#define HP_APM_REGION4_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION4_R0_PMS_X_S  0
/** HP_APM_REGION4_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION4_R0_PMS_W    (BIT(1))
#define HP_APM_REGION4_R0_PMS_W_M  (HP_APM_REGION4_R0_PMS_W_V << HP_APM_REGION4_R0_PMS_W_S)
#define HP_APM_REGION4_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION4_R0_PMS_W_S  1
/** HP_APM_REGION4_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION4_R0_PMS_R    (BIT(2))
#define HP_APM_REGION4_R0_PMS_R_M  (HP_APM_REGION4_R0_PMS_R_V << HP_APM_REGION4_R0_PMS_R_S)
#define HP_APM_REGION4_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION4_R0_PMS_R_S  2
/** HP_APM_REGION4_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION4_R1_PMS_X    (BIT(4))
#define HP_APM_REGION4_R1_PMS_X_M  (HP_APM_REGION4_R1_PMS_X_V << HP_APM_REGION4_R1_PMS_X_S)
#define HP_APM_REGION4_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION4_R1_PMS_X_S  4
/** HP_APM_REGION4_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION4_R1_PMS_W    (BIT(5))
#define HP_APM_REGION4_R1_PMS_W_M  (HP_APM_REGION4_R1_PMS_W_V << HP_APM_REGION4_R1_PMS_W_S)
#define HP_APM_REGION4_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION4_R1_PMS_W_S  5
/** HP_APM_REGION4_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION4_R1_PMS_R    (BIT(6))
#define HP_APM_REGION4_R1_PMS_R_M  (HP_APM_REGION4_R1_PMS_R_V << HP_APM_REGION4_R1_PMS_R_S)
#define HP_APM_REGION4_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION4_R1_PMS_R_S  6
/** HP_APM_REGION4_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION4_R2_PMS_X    (BIT(8))
#define HP_APM_REGION4_R2_PMS_X_M  (HP_APM_REGION4_R2_PMS_X_V << HP_APM_REGION4_R2_PMS_X_S)
#define HP_APM_REGION4_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION4_R2_PMS_X_S  8
/** HP_APM_REGION4_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION4_R2_PMS_W    (BIT(9))
#define HP_APM_REGION4_R2_PMS_W_M  (HP_APM_REGION4_R2_PMS_W_V << HP_APM_REGION4_R2_PMS_W_S)
#define HP_APM_REGION4_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION4_R2_PMS_W_S  9
/** HP_APM_REGION4_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION4_R2_PMS_R    (BIT(10))
#define HP_APM_REGION4_R2_PMS_R_M  (HP_APM_REGION4_R2_PMS_R_V << HP_APM_REGION4_R2_PMS_R_S)
#define HP_APM_REGION4_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION4_R2_PMS_R_S  10

/** HP_APM_REGION5_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION5_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x40)
/** HP_APM_REGION5_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region5
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
 *  End address of region5
 */
#define HP_APM_REGION5_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION5_ADDR_END_M  (HP_APM_REGION5_ADDR_END_V << HP_APM_REGION5_ADDR_END_S)
#define HP_APM_REGION5_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION5_ADDR_END_S  0

/** HP_APM_REGION5_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION5_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x48)
/** HP_APM_REGION5_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION5_R0_PMS_X    (BIT(0))
#define HP_APM_REGION5_R0_PMS_X_M  (HP_APM_REGION5_R0_PMS_X_V << HP_APM_REGION5_R0_PMS_X_S)
#define HP_APM_REGION5_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION5_R0_PMS_X_S  0
/** HP_APM_REGION5_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION5_R0_PMS_W    (BIT(1))
#define HP_APM_REGION5_R0_PMS_W_M  (HP_APM_REGION5_R0_PMS_W_V << HP_APM_REGION5_R0_PMS_W_S)
#define HP_APM_REGION5_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION5_R0_PMS_W_S  1
/** HP_APM_REGION5_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION5_R0_PMS_R    (BIT(2))
#define HP_APM_REGION5_R0_PMS_R_M  (HP_APM_REGION5_R0_PMS_R_V << HP_APM_REGION5_R0_PMS_R_S)
#define HP_APM_REGION5_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION5_R0_PMS_R_S  2
/** HP_APM_REGION5_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION5_R1_PMS_X    (BIT(4))
#define HP_APM_REGION5_R1_PMS_X_M  (HP_APM_REGION5_R1_PMS_X_V << HP_APM_REGION5_R1_PMS_X_S)
#define HP_APM_REGION5_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION5_R1_PMS_X_S  4
/** HP_APM_REGION5_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION5_R1_PMS_W    (BIT(5))
#define HP_APM_REGION5_R1_PMS_W_M  (HP_APM_REGION5_R1_PMS_W_V << HP_APM_REGION5_R1_PMS_W_S)
#define HP_APM_REGION5_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION5_R1_PMS_W_S  5
/** HP_APM_REGION5_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION5_R1_PMS_R    (BIT(6))
#define HP_APM_REGION5_R1_PMS_R_M  (HP_APM_REGION5_R1_PMS_R_V << HP_APM_REGION5_R1_PMS_R_S)
#define HP_APM_REGION5_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION5_R1_PMS_R_S  6
/** HP_APM_REGION5_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION5_R2_PMS_X    (BIT(8))
#define HP_APM_REGION5_R2_PMS_X_M  (HP_APM_REGION5_R2_PMS_X_V << HP_APM_REGION5_R2_PMS_X_S)
#define HP_APM_REGION5_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION5_R2_PMS_X_S  8
/** HP_APM_REGION5_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION5_R2_PMS_W    (BIT(9))
#define HP_APM_REGION5_R2_PMS_W_M  (HP_APM_REGION5_R2_PMS_W_V << HP_APM_REGION5_R2_PMS_W_S)
#define HP_APM_REGION5_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION5_R2_PMS_W_S  9
/** HP_APM_REGION5_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION5_R2_PMS_R    (BIT(10))
#define HP_APM_REGION5_R2_PMS_R_M  (HP_APM_REGION5_R2_PMS_R_V << HP_APM_REGION5_R2_PMS_R_S)
#define HP_APM_REGION5_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION5_R2_PMS_R_S  10

/** HP_APM_REGION6_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION6_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x4c)
/** HP_APM_REGION6_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region6
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
 *  End address of region6
 */
#define HP_APM_REGION6_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION6_ADDR_END_M  (HP_APM_REGION6_ADDR_END_V << HP_APM_REGION6_ADDR_END_S)
#define HP_APM_REGION6_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION6_ADDR_END_S  0

/** HP_APM_REGION6_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION6_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x54)
/** HP_APM_REGION6_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION6_R0_PMS_X    (BIT(0))
#define HP_APM_REGION6_R0_PMS_X_M  (HP_APM_REGION6_R0_PMS_X_V << HP_APM_REGION6_R0_PMS_X_S)
#define HP_APM_REGION6_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION6_R0_PMS_X_S  0
/** HP_APM_REGION6_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION6_R0_PMS_W    (BIT(1))
#define HP_APM_REGION6_R0_PMS_W_M  (HP_APM_REGION6_R0_PMS_W_V << HP_APM_REGION6_R0_PMS_W_S)
#define HP_APM_REGION6_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION6_R0_PMS_W_S  1
/** HP_APM_REGION6_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION6_R0_PMS_R    (BIT(2))
#define HP_APM_REGION6_R0_PMS_R_M  (HP_APM_REGION6_R0_PMS_R_V << HP_APM_REGION6_R0_PMS_R_S)
#define HP_APM_REGION6_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION6_R0_PMS_R_S  2
/** HP_APM_REGION6_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION6_R1_PMS_X    (BIT(4))
#define HP_APM_REGION6_R1_PMS_X_M  (HP_APM_REGION6_R1_PMS_X_V << HP_APM_REGION6_R1_PMS_X_S)
#define HP_APM_REGION6_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION6_R1_PMS_X_S  4
/** HP_APM_REGION6_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION6_R1_PMS_W    (BIT(5))
#define HP_APM_REGION6_R1_PMS_W_M  (HP_APM_REGION6_R1_PMS_W_V << HP_APM_REGION6_R1_PMS_W_S)
#define HP_APM_REGION6_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION6_R1_PMS_W_S  5
/** HP_APM_REGION6_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION6_R1_PMS_R    (BIT(6))
#define HP_APM_REGION6_R1_PMS_R_M  (HP_APM_REGION6_R1_PMS_R_V << HP_APM_REGION6_R1_PMS_R_S)
#define HP_APM_REGION6_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION6_R1_PMS_R_S  6
/** HP_APM_REGION6_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION6_R2_PMS_X    (BIT(8))
#define HP_APM_REGION6_R2_PMS_X_M  (HP_APM_REGION6_R2_PMS_X_V << HP_APM_REGION6_R2_PMS_X_S)
#define HP_APM_REGION6_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION6_R2_PMS_X_S  8
/** HP_APM_REGION6_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION6_R2_PMS_W    (BIT(9))
#define HP_APM_REGION6_R2_PMS_W_M  (HP_APM_REGION6_R2_PMS_W_V << HP_APM_REGION6_R2_PMS_W_S)
#define HP_APM_REGION6_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION6_R2_PMS_W_S  9
/** HP_APM_REGION6_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION6_R2_PMS_R    (BIT(10))
#define HP_APM_REGION6_R2_PMS_R_M  (HP_APM_REGION6_R2_PMS_R_V << HP_APM_REGION6_R2_PMS_R_S)
#define HP_APM_REGION6_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION6_R2_PMS_R_S  10

/** HP_APM_REGION7_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION7_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x58)
/** HP_APM_REGION7_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region7
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
 *  End address of region7
 */
#define HP_APM_REGION7_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION7_ADDR_END_M  (HP_APM_REGION7_ADDR_END_V << HP_APM_REGION7_ADDR_END_S)
#define HP_APM_REGION7_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION7_ADDR_END_S  0

/** HP_APM_REGION7_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION7_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x60)
/** HP_APM_REGION7_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION7_R0_PMS_X    (BIT(0))
#define HP_APM_REGION7_R0_PMS_X_M  (HP_APM_REGION7_R0_PMS_X_V << HP_APM_REGION7_R0_PMS_X_S)
#define HP_APM_REGION7_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION7_R0_PMS_X_S  0
/** HP_APM_REGION7_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION7_R0_PMS_W    (BIT(1))
#define HP_APM_REGION7_R0_PMS_W_M  (HP_APM_REGION7_R0_PMS_W_V << HP_APM_REGION7_R0_PMS_W_S)
#define HP_APM_REGION7_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION7_R0_PMS_W_S  1
/** HP_APM_REGION7_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION7_R0_PMS_R    (BIT(2))
#define HP_APM_REGION7_R0_PMS_R_M  (HP_APM_REGION7_R0_PMS_R_V << HP_APM_REGION7_R0_PMS_R_S)
#define HP_APM_REGION7_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION7_R0_PMS_R_S  2
/** HP_APM_REGION7_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION7_R1_PMS_X    (BIT(4))
#define HP_APM_REGION7_R1_PMS_X_M  (HP_APM_REGION7_R1_PMS_X_V << HP_APM_REGION7_R1_PMS_X_S)
#define HP_APM_REGION7_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION7_R1_PMS_X_S  4
/** HP_APM_REGION7_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION7_R1_PMS_W    (BIT(5))
#define HP_APM_REGION7_R1_PMS_W_M  (HP_APM_REGION7_R1_PMS_W_V << HP_APM_REGION7_R1_PMS_W_S)
#define HP_APM_REGION7_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION7_R1_PMS_W_S  5
/** HP_APM_REGION7_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION7_R1_PMS_R    (BIT(6))
#define HP_APM_REGION7_R1_PMS_R_M  (HP_APM_REGION7_R1_PMS_R_V << HP_APM_REGION7_R1_PMS_R_S)
#define HP_APM_REGION7_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION7_R1_PMS_R_S  6
/** HP_APM_REGION7_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION7_R2_PMS_X    (BIT(8))
#define HP_APM_REGION7_R2_PMS_X_M  (HP_APM_REGION7_R2_PMS_X_V << HP_APM_REGION7_R2_PMS_X_S)
#define HP_APM_REGION7_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION7_R2_PMS_X_S  8
/** HP_APM_REGION7_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION7_R2_PMS_W    (BIT(9))
#define HP_APM_REGION7_R2_PMS_W_M  (HP_APM_REGION7_R2_PMS_W_V << HP_APM_REGION7_R2_PMS_W_S)
#define HP_APM_REGION7_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION7_R2_PMS_W_S  9
/** HP_APM_REGION7_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION7_R2_PMS_R    (BIT(10))
#define HP_APM_REGION7_R2_PMS_R_M  (HP_APM_REGION7_R2_PMS_R_V << HP_APM_REGION7_R2_PMS_R_S)
#define HP_APM_REGION7_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION7_R2_PMS_R_S  10

/** HP_APM_REGION8_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION8_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x64)
/** HP_APM_REGION8_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region8
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
 *  End address of region8
 */
#define HP_APM_REGION8_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION8_ADDR_END_M  (HP_APM_REGION8_ADDR_END_V << HP_APM_REGION8_ADDR_END_S)
#define HP_APM_REGION8_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION8_ADDR_END_S  0

/** HP_APM_REGION8_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION8_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x6c)
/** HP_APM_REGION8_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION8_R0_PMS_X    (BIT(0))
#define HP_APM_REGION8_R0_PMS_X_M  (HP_APM_REGION8_R0_PMS_X_V << HP_APM_REGION8_R0_PMS_X_S)
#define HP_APM_REGION8_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION8_R0_PMS_X_S  0
/** HP_APM_REGION8_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION8_R0_PMS_W    (BIT(1))
#define HP_APM_REGION8_R0_PMS_W_M  (HP_APM_REGION8_R0_PMS_W_V << HP_APM_REGION8_R0_PMS_W_S)
#define HP_APM_REGION8_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION8_R0_PMS_W_S  1
/** HP_APM_REGION8_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION8_R0_PMS_R    (BIT(2))
#define HP_APM_REGION8_R0_PMS_R_M  (HP_APM_REGION8_R0_PMS_R_V << HP_APM_REGION8_R0_PMS_R_S)
#define HP_APM_REGION8_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION8_R0_PMS_R_S  2
/** HP_APM_REGION8_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION8_R1_PMS_X    (BIT(4))
#define HP_APM_REGION8_R1_PMS_X_M  (HP_APM_REGION8_R1_PMS_X_V << HP_APM_REGION8_R1_PMS_X_S)
#define HP_APM_REGION8_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION8_R1_PMS_X_S  4
/** HP_APM_REGION8_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION8_R1_PMS_W    (BIT(5))
#define HP_APM_REGION8_R1_PMS_W_M  (HP_APM_REGION8_R1_PMS_W_V << HP_APM_REGION8_R1_PMS_W_S)
#define HP_APM_REGION8_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION8_R1_PMS_W_S  5
/** HP_APM_REGION8_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION8_R1_PMS_R    (BIT(6))
#define HP_APM_REGION8_R1_PMS_R_M  (HP_APM_REGION8_R1_PMS_R_V << HP_APM_REGION8_R1_PMS_R_S)
#define HP_APM_REGION8_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION8_R1_PMS_R_S  6
/** HP_APM_REGION8_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION8_R2_PMS_X    (BIT(8))
#define HP_APM_REGION8_R2_PMS_X_M  (HP_APM_REGION8_R2_PMS_X_V << HP_APM_REGION8_R2_PMS_X_S)
#define HP_APM_REGION8_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION8_R2_PMS_X_S  8
/** HP_APM_REGION8_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION8_R2_PMS_W    (BIT(9))
#define HP_APM_REGION8_R2_PMS_W_M  (HP_APM_REGION8_R2_PMS_W_V << HP_APM_REGION8_R2_PMS_W_S)
#define HP_APM_REGION8_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION8_R2_PMS_W_S  9
/** HP_APM_REGION8_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION8_R2_PMS_R    (BIT(10))
#define HP_APM_REGION8_R2_PMS_R_M  (HP_APM_REGION8_R2_PMS_R_V << HP_APM_REGION8_R2_PMS_R_S)
#define HP_APM_REGION8_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION8_R2_PMS_R_S  10

/** HP_APM_REGION9_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION9_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x70)
/** HP_APM_REGION9_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region9
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
 *  End address of region9
 */
#define HP_APM_REGION9_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION9_ADDR_END_M  (HP_APM_REGION9_ADDR_END_V << HP_APM_REGION9_ADDR_END_S)
#define HP_APM_REGION9_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION9_ADDR_END_S  0

/** HP_APM_REGION9_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION9_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x78)
/** HP_APM_REGION9_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION9_R0_PMS_X    (BIT(0))
#define HP_APM_REGION9_R0_PMS_X_M  (HP_APM_REGION9_R0_PMS_X_V << HP_APM_REGION9_R0_PMS_X_S)
#define HP_APM_REGION9_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION9_R0_PMS_X_S  0
/** HP_APM_REGION9_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION9_R0_PMS_W    (BIT(1))
#define HP_APM_REGION9_R0_PMS_W_M  (HP_APM_REGION9_R0_PMS_W_V << HP_APM_REGION9_R0_PMS_W_S)
#define HP_APM_REGION9_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION9_R0_PMS_W_S  1
/** HP_APM_REGION9_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION9_R0_PMS_R    (BIT(2))
#define HP_APM_REGION9_R0_PMS_R_M  (HP_APM_REGION9_R0_PMS_R_V << HP_APM_REGION9_R0_PMS_R_S)
#define HP_APM_REGION9_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION9_R0_PMS_R_S  2
/** HP_APM_REGION9_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION9_R1_PMS_X    (BIT(4))
#define HP_APM_REGION9_R1_PMS_X_M  (HP_APM_REGION9_R1_PMS_X_V << HP_APM_REGION9_R1_PMS_X_S)
#define HP_APM_REGION9_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION9_R1_PMS_X_S  4
/** HP_APM_REGION9_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION9_R1_PMS_W    (BIT(5))
#define HP_APM_REGION9_R1_PMS_W_M  (HP_APM_REGION9_R1_PMS_W_V << HP_APM_REGION9_R1_PMS_W_S)
#define HP_APM_REGION9_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION9_R1_PMS_W_S  5
/** HP_APM_REGION9_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION9_R1_PMS_R    (BIT(6))
#define HP_APM_REGION9_R1_PMS_R_M  (HP_APM_REGION9_R1_PMS_R_V << HP_APM_REGION9_R1_PMS_R_S)
#define HP_APM_REGION9_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION9_R1_PMS_R_S  6
/** HP_APM_REGION9_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION9_R2_PMS_X    (BIT(8))
#define HP_APM_REGION9_R2_PMS_X_M  (HP_APM_REGION9_R2_PMS_X_V << HP_APM_REGION9_R2_PMS_X_S)
#define HP_APM_REGION9_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION9_R2_PMS_X_S  8
/** HP_APM_REGION9_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION9_R2_PMS_W    (BIT(9))
#define HP_APM_REGION9_R2_PMS_W_M  (HP_APM_REGION9_R2_PMS_W_V << HP_APM_REGION9_R2_PMS_W_S)
#define HP_APM_REGION9_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION9_R2_PMS_W_S  9
/** HP_APM_REGION9_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION9_R2_PMS_R    (BIT(10))
#define HP_APM_REGION9_R2_PMS_R_M  (HP_APM_REGION9_R2_PMS_R_V << HP_APM_REGION9_R2_PMS_R_S)
#define HP_APM_REGION9_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION9_R2_PMS_R_S  10

/** HP_APM_REGION10_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION10_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x7c)
/** HP_APM_REGION10_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region10
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
 *  End address of region10
 */
#define HP_APM_REGION10_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION10_ADDR_END_M  (HP_APM_REGION10_ADDR_END_V << HP_APM_REGION10_ADDR_END_S)
#define HP_APM_REGION10_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION10_ADDR_END_S  0

/** HP_APM_REGION10_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION10_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x84)
/** HP_APM_REGION10_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION10_R0_PMS_X    (BIT(0))
#define HP_APM_REGION10_R0_PMS_X_M  (HP_APM_REGION10_R0_PMS_X_V << HP_APM_REGION10_R0_PMS_X_S)
#define HP_APM_REGION10_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION10_R0_PMS_X_S  0
/** HP_APM_REGION10_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION10_R0_PMS_W    (BIT(1))
#define HP_APM_REGION10_R0_PMS_W_M  (HP_APM_REGION10_R0_PMS_W_V << HP_APM_REGION10_R0_PMS_W_S)
#define HP_APM_REGION10_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION10_R0_PMS_W_S  1
/** HP_APM_REGION10_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION10_R0_PMS_R    (BIT(2))
#define HP_APM_REGION10_R0_PMS_R_M  (HP_APM_REGION10_R0_PMS_R_V << HP_APM_REGION10_R0_PMS_R_S)
#define HP_APM_REGION10_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION10_R0_PMS_R_S  2
/** HP_APM_REGION10_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION10_R1_PMS_X    (BIT(4))
#define HP_APM_REGION10_R1_PMS_X_M  (HP_APM_REGION10_R1_PMS_X_V << HP_APM_REGION10_R1_PMS_X_S)
#define HP_APM_REGION10_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION10_R1_PMS_X_S  4
/** HP_APM_REGION10_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION10_R1_PMS_W    (BIT(5))
#define HP_APM_REGION10_R1_PMS_W_M  (HP_APM_REGION10_R1_PMS_W_V << HP_APM_REGION10_R1_PMS_W_S)
#define HP_APM_REGION10_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION10_R1_PMS_W_S  5
/** HP_APM_REGION10_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION10_R1_PMS_R    (BIT(6))
#define HP_APM_REGION10_R1_PMS_R_M  (HP_APM_REGION10_R1_PMS_R_V << HP_APM_REGION10_R1_PMS_R_S)
#define HP_APM_REGION10_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION10_R1_PMS_R_S  6
/** HP_APM_REGION10_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION10_R2_PMS_X    (BIT(8))
#define HP_APM_REGION10_R2_PMS_X_M  (HP_APM_REGION10_R2_PMS_X_V << HP_APM_REGION10_R2_PMS_X_S)
#define HP_APM_REGION10_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION10_R2_PMS_X_S  8
/** HP_APM_REGION10_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION10_R2_PMS_W    (BIT(9))
#define HP_APM_REGION10_R2_PMS_W_M  (HP_APM_REGION10_R2_PMS_W_V << HP_APM_REGION10_R2_PMS_W_S)
#define HP_APM_REGION10_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION10_R2_PMS_W_S  9
/** HP_APM_REGION10_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION10_R2_PMS_R    (BIT(10))
#define HP_APM_REGION10_R2_PMS_R_M  (HP_APM_REGION10_R2_PMS_R_V << HP_APM_REGION10_R2_PMS_R_S)
#define HP_APM_REGION10_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION10_R2_PMS_R_S  10

/** HP_APM_REGION11_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION11_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x88)
/** HP_APM_REGION11_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region11
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
 *  End address of region11
 */
#define HP_APM_REGION11_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION11_ADDR_END_M  (HP_APM_REGION11_ADDR_END_V << HP_APM_REGION11_ADDR_END_S)
#define HP_APM_REGION11_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION11_ADDR_END_S  0

/** HP_APM_REGION11_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION11_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x90)
/** HP_APM_REGION11_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION11_R0_PMS_X    (BIT(0))
#define HP_APM_REGION11_R0_PMS_X_M  (HP_APM_REGION11_R0_PMS_X_V << HP_APM_REGION11_R0_PMS_X_S)
#define HP_APM_REGION11_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION11_R0_PMS_X_S  0
/** HP_APM_REGION11_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION11_R0_PMS_W    (BIT(1))
#define HP_APM_REGION11_R0_PMS_W_M  (HP_APM_REGION11_R0_PMS_W_V << HP_APM_REGION11_R0_PMS_W_S)
#define HP_APM_REGION11_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION11_R0_PMS_W_S  1
/** HP_APM_REGION11_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION11_R0_PMS_R    (BIT(2))
#define HP_APM_REGION11_R0_PMS_R_M  (HP_APM_REGION11_R0_PMS_R_V << HP_APM_REGION11_R0_PMS_R_S)
#define HP_APM_REGION11_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION11_R0_PMS_R_S  2
/** HP_APM_REGION11_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION11_R1_PMS_X    (BIT(4))
#define HP_APM_REGION11_R1_PMS_X_M  (HP_APM_REGION11_R1_PMS_X_V << HP_APM_REGION11_R1_PMS_X_S)
#define HP_APM_REGION11_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION11_R1_PMS_X_S  4
/** HP_APM_REGION11_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION11_R1_PMS_W    (BIT(5))
#define HP_APM_REGION11_R1_PMS_W_M  (HP_APM_REGION11_R1_PMS_W_V << HP_APM_REGION11_R1_PMS_W_S)
#define HP_APM_REGION11_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION11_R1_PMS_W_S  5
/** HP_APM_REGION11_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION11_R1_PMS_R    (BIT(6))
#define HP_APM_REGION11_R1_PMS_R_M  (HP_APM_REGION11_R1_PMS_R_V << HP_APM_REGION11_R1_PMS_R_S)
#define HP_APM_REGION11_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION11_R1_PMS_R_S  6
/** HP_APM_REGION11_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION11_R2_PMS_X    (BIT(8))
#define HP_APM_REGION11_R2_PMS_X_M  (HP_APM_REGION11_R2_PMS_X_V << HP_APM_REGION11_R2_PMS_X_S)
#define HP_APM_REGION11_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION11_R2_PMS_X_S  8
/** HP_APM_REGION11_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION11_R2_PMS_W    (BIT(9))
#define HP_APM_REGION11_R2_PMS_W_M  (HP_APM_REGION11_R2_PMS_W_V << HP_APM_REGION11_R2_PMS_W_S)
#define HP_APM_REGION11_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION11_R2_PMS_W_S  9
/** HP_APM_REGION11_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION11_R2_PMS_R    (BIT(10))
#define HP_APM_REGION11_R2_PMS_R_M  (HP_APM_REGION11_R2_PMS_R_V << HP_APM_REGION11_R2_PMS_R_S)
#define HP_APM_REGION11_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION11_R2_PMS_R_S  10

/** HP_APM_REGION12_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION12_ADDR_START_REG (DR_REG_HP_APM_BASE + 0x94)
/** HP_APM_REGION12_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region12
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
 *  End address of region12
 */
#define HP_APM_REGION12_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION12_ADDR_END_M  (HP_APM_REGION12_ADDR_END_V << HP_APM_REGION12_ADDR_END_S)
#define HP_APM_REGION12_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION12_ADDR_END_S  0

/** HP_APM_REGION12_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION12_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0x9c)
/** HP_APM_REGION12_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION12_R0_PMS_X    (BIT(0))
#define HP_APM_REGION12_R0_PMS_X_M  (HP_APM_REGION12_R0_PMS_X_V << HP_APM_REGION12_R0_PMS_X_S)
#define HP_APM_REGION12_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION12_R0_PMS_X_S  0
/** HP_APM_REGION12_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION12_R0_PMS_W    (BIT(1))
#define HP_APM_REGION12_R0_PMS_W_M  (HP_APM_REGION12_R0_PMS_W_V << HP_APM_REGION12_R0_PMS_W_S)
#define HP_APM_REGION12_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION12_R0_PMS_W_S  1
/** HP_APM_REGION12_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION12_R0_PMS_R    (BIT(2))
#define HP_APM_REGION12_R0_PMS_R_M  (HP_APM_REGION12_R0_PMS_R_V << HP_APM_REGION12_R0_PMS_R_S)
#define HP_APM_REGION12_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION12_R0_PMS_R_S  2
/** HP_APM_REGION12_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION12_R1_PMS_X    (BIT(4))
#define HP_APM_REGION12_R1_PMS_X_M  (HP_APM_REGION12_R1_PMS_X_V << HP_APM_REGION12_R1_PMS_X_S)
#define HP_APM_REGION12_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION12_R1_PMS_X_S  4
/** HP_APM_REGION12_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION12_R1_PMS_W    (BIT(5))
#define HP_APM_REGION12_R1_PMS_W_M  (HP_APM_REGION12_R1_PMS_W_V << HP_APM_REGION12_R1_PMS_W_S)
#define HP_APM_REGION12_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION12_R1_PMS_W_S  5
/** HP_APM_REGION12_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION12_R1_PMS_R    (BIT(6))
#define HP_APM_REGION12_R1_PMS_R_M  (HP_APM_REGION12_R1_PMS_R_V << HP_APM_REGION12_R1_PMS_R_S)
#define HP_APM_REGION12_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION12_R1_PMS_R_S  6
/** HP_APM_REGION12_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION12_R2_PMS_X    (BIT(8))
#define HP_APM_REGION12_R2_PMS_X_M  (HP_APM_REGION12_R2_PMS_X_V << HP_APM_REGION12_R2_PMS_X_S)
#define HP_APM_REGION12_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION12_R2_PMS_X_S  8
/** HP_APM_REGION12_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION12_R2_PMS_W    (BIT(9))
#define HP_APM_REGION12_R2_PMS_W_M  (HP_APM_REGION12_R2_PMS_W_V << HP_APM_REGION12_R2_PMS_W_S)
#define HP_APM_REGION12_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION12_R2_PMS_W_S  9
/** HP_APM_REGION12_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION12_R2_PMS_R    (BIT(10))
#define HP_APM_REGION12_R2_PMS_R_M  (HP_APM_REGION12_R2_PMS_R_V << HP_APM_REGION12_R2_PMS_R_S)
#define HP_APM_REGION12_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION12_R2_PMS_R_S  10

/** HP_APM_REGION13_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION13_ADDR_START_REG (DR_REG_HP_APM_BASE + 0xa0)
/** HP_APM_REGION13_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region13
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
 *  End address of region13
 */
#define HP_APM_REGION13_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION13_ADDR_END_M  (HP_APM_REGION13_ADDR_END_V << HP_APM_REGION13_ADDR_END_S)
#define HP_APM_REGION13_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION13_ADDR_END_S  0

/** HP_APM_REGION13_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION13_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0xa8)
/** HP_APM_REGION13_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION13_R0_PMS_X    (BIT(0))
#define HP_APM_REGION13_R0_PMS_X_M  (HP_APM_REGION13_R0_PMS_X_V << HP_APM_REGION13_R0_PMS_X_S)
#define HP_APM_REGION13_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION13_R0_PMS_X_S  0
/** HP_APM_REGION13_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION13_R0_PMS_W    (BIT(1))
#define HP_APM_REGION13_R0_PMS_W_M  (HP_APM_REGION13_R0_PMS_W_V << HP_APM_REGION13_R0_PMS_W_S)
#define HP_APM_REGION13_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION13_R0_PMS_W_S  1
/** HP_APM_REGION13_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION13_R0_PMS_R    (BIT(2))
#define HP_APM_REGION13_R0_PMS_R_M  (HP_APM_REGION13_R0_PMS_R_V << HP_APM_REGION13_R0_PMS_R_S)
#define HP_APM_REGION13_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION13_R0_PMS_R_S  2
/** HP_APM_REGION13_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION13_R1_PMS_X    (BIT(4))
#define HP_APM_REGION13_R1_PMS_X_M  (HP_APM_REGION13_R1_PMS_X_V << HP_APM_REGION13_R1_PMS_X_S)
#define HP_APM_REGION13_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION13_R1_PMS_X_S  4
/** HP_APM_REGION13_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION13_R1_PMS_W    (BIT(5))
#define HP_APM_REGION13_R1_PMS_W_M  (HP_APM_REGION13_R1_PMS_W_V << HP_APM_REGION13_R1_PMS_W_S)
#define HP_APM_REGION13_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION13_R1_PMS_W_S  5
/** HP_APM_REGION13_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION13_R1_PMS_R    (BIT(6))
#define HP_APM_REGION13_R1_PMS_R_M  (HP_APM_REGION13_R1_PMS_R_V << HP_APM_REGION13_R1_PMS_R_S)
#define HP_APM_REGION13_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION13_R1_PMS_R_S  6
/** HP_APM_REGION13_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION13_R2_PMS_X    (BIT(8))
#define HP_APM_REGION13_R2_PMS_X_M  (HP_APM_REGION13_R2_PMS_X_V << HP_APM_REGION13_R2_PMS_X_S)
#define HP_APM_REGION13_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION13_R2_PMS_X_S  8
/** HP_APM_REGION13_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION13_R2_PMS_W    (BIT(9))
#define HP_APM_REGION13_R2_PMS_W_M  (HP_APM_REGION13_R2_PMS_W_V << HP_APM_REGION13_R2_PMS_W_S)
#define HP_APM_REGION13_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION13_R2_PMS_W_S  9
/** HP_APM_REGION13_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION13_R2_PMS_R    (BIT(10))
#define HP_APM_REGION13_R2_PMS_R_M  (HP_APM_REGION13_R2_PMS_R_V << HP_APM_REGION13_R2_PMS_R_S)
#define HP_APM_REGION13_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION13_R2_PMS_R_S  10

/** HP_APM_REGION14_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION14_ADDR_START_REG (DR_REG_HP_APM_BASE + 0xac)
/** HP_APM_REGION14_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region14
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
 *  End address of region14
 */
#define HP_APM_REGION14_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION14_ADDR_END_M  (HP_APM_REGION14_ADDR_END_V << HP_APM_REGION14_ADDR_END_S)
#define HP_APM_REGION14_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION14_ADDR_END_S  0

/** HP_APM_REGION14_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION14_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0xb4)
/** HP_APM_REGION14_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION14_R0_PMS_X    (BIT(0))
#define HP_APM_REGION14_R0_PMS_X_M  (HP_APM_REGION14_R0_PMS_X_V << HP_APM_REGION14_R0_PMS_X_S)
#define HP_APM_REGION14_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION14_R0_PMS_X_S  0
/** HP_APM_REGION14_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION14_R0_PMS_W    (BIT(1))
#define HP_APM_REGION14_R0_PMS_W_M  (HP_APM_REGION14_R0_PMS_W_V << HP_APM_REGION14_R0_PMS_W_S)
#define HP_APM_REGION14_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION14_R0_PMS_W_S  1
/** HP_APM_REGION14_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION14_R0_PMS_R    (BIT(2))
#define HP_APM_REGION14_R0_PMS_R_M  (HP_APM_REGION14_R0_PMS_R_V << HP_APM_REGION14_R0_PMS_R_S)
#define HP_APM_REGION14_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION14_R0_PMS_R_S  2
/** HP_APM_REGION14_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION14_R1_PMS_X    (BIT(4))
#define HP_APM_REGION14_R1_PMS_X_M  (HP_APM_REGION14_R1_PMS_X_V << HP_APM_REGION14_R1_PMS_X_S)
#define HP_APM_REGION14_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION14_R1_PMS_X_S  4
/** HP_APM_REGION14_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION14_R1_PMS_W    (BIT(5))
#define HP_APM_REGION14_R1_PMS_W_M  (HP_APM_REGION14_R1_PMS_W_V << HP_APM_REGION14_R1_PMS_W_S)
#define HP_APM_REGION14_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION14_R1_PMS_W_S  5
/** HP_APM_REGION14_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION14_R1_PMS_R    (BIT(6))
#define HP_APM_REGION14_R1_PMS_R_M  (HP_APM_REGION14_R1_PMS_R_V << HP_APM_REGION14_R1_PMS_R_S)
#define HP_APM_REGION14_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION14_R1_PMS_R_S  6
/** HP_APM_REGION14_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION14_R2_PMS_X    (BIT(8))
#define HP_APM_REGION14_R2_PMS_X_M  (HP_APM_REGION14_R2_PMS_X_V << HP_APM_REGION14_R2_PMS_X_S)
#define HP_APM_REGION14_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION14_R2_PMS_X_S  8
/** HP_APM_REGION14_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION14_R2_PMS_W    (BIT(9))
#define HP_APM_REGION14_R2_PMS_W_M  (HP_APM_REGION14_R2_PMS_W_V << HP_APM_REGION14_R2_PMS_W_S)
#define HP_APM_REGION14_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION14_R2_PMS_W_S  9
/** HP_APM_REGION14_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION14_R2_PMS_R    (BIT(10))
#define HP_APM_REGION14_R2_PMS_R_M  (HP_APM_REGION14_R2_PMS_R_V << HP_APM_REGION14_R2_PMS_R_S)
#define HP_APM_REGION14_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION14_R2_PMS_R_S  10

/** HP_APM_REGION15_ADDR_START_REG register
 *  Region address register
 */
#define HP_APM_REGION15_ADDR_START_REG (DR_REG_HP_APM_BASE + 0xb8)
/** HP_APM_REGION15_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Start address of region15
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
 *  End address of region15
 */
#define HP_APM_REGION15_ADDR_END    0xFFFFFFFFU
#define HP_APM_REGION15_ADDR_END_M  (HP_APM_REGION15_ADDR_END_V << HP_APM_REGION15_ADDR_END_S)
#define HP_APM_REGION15_ADDR_END_V  0xFFFFFFFFU
#define HP_APM_REGION15_ADDR_END_S  0

/** HP_APM_REGION15_PMS_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_APM_REGION15_PMS_ATTR_REG (DR_REG_HP_APM_BASE + 0xc0)
/** HP_APM_REGION15_R0_PMS_X : R/W; bitpos: [0]; default: 0;
 *  Region execute authority in REE_MODE0
 */
#define HP_APM_REGION15_R0_PMS_X    (BIT(0))
#define HP_APM_REGION15_R0_PMS_X_M  (HP_APM_REGION15_R0_PMS_X_V << HP_APM_REGION15_R0_PMS_X_S)
#define HP_APM_REGION15_R0_PMS_X_V  0x00000001U
#define HP_APM_REGION15_R0_PMS_X_S  0
/** HP_APM_REGION15_R0_PMS_W : R/W; bitpos: [1]; default: 0;
 *  Region write authority in REE_MODE0
 */
#define HP_APM_REGION15_R0_PMS_W    (BIT(1))
#define HP_APM_REGION15_R0_PMS_W_M  (HP_APM_REGION15_R0_PMS_W_V << HP_APM_REGION15_R0_PMS_W_S)
#define HP_APM_REGION15_R0_PMS_W_V  0x00000001U
#define HP_APM_REGION15_R0_PMS_W_S  1
/** HP_APM_REGION15_R0_PMS_R : R/W; bitpos: [2]; default: 0;
 *  Region read authority in REE_MODE0
 */
#define HP_APM_REGION15_R0_PMS_R    (BIT(2))
#define HP_APM_REGION15_R0_PMS_R_M  (HP_APM_REGION15_R0_PMS_R_V << HP_APM_REGION15_R0_PMS_R_S)
#define HP_APM_REGION15_R0_PMS_R_V  0x00000001U
#define HP_APM_REGION15_R0_PMS_R_S  2
/** HP_APM_REGION15_R1_PMS_X : R/W; bitpos: [4]; default: 0;
 *  Region execute authority in REE_MODE1
 */
#define HP_APM_REGION15_R1_PMS_X    (BIT(4))
#define HP_APM_REGION15_R1_PMS_X_M  (HP_APM_REGION15_R1_PMS_X_V << HP_APM_REGION15_R1_PMS_X_S)
#define HP_APM_REGION15_R1_PMS_X_V  0x00000001U
#define HP_APM_REGION15_R1_PMS_X_S  4
/** HP_APM_REGION15_R1_PMS_W : R/W; bitpos: [5]; default: 0;
 *  Region write authority in REE_MODE1
 */
#define HP_APM_REGION15_R1_PMS_W    (BIT(5))
#define HP_APM_REGION15_R1_PMS_W_M  (HP_APM_REGION15_R1_PMS_W_V << HP_APM_REGION15_R1_PMS_W_S)
#define HP_APM_REGION15_R1_PMS_W_V  0x00000001U
#define HP_APM_REGION15_R1_PMS_W_S  5
/** HP_APM_REGION15_R1_PMS_R : R/W; bitpos: [6]; default: 0;
 *  Region read authority in REE_MODE1
 */
#define HP_APM_REGION15_R1_PMS_R    (BIT(6))
#define HP_APM_REGION15_R1_PMS_R_M  (HP_APM_REGION15_R1_PMS_R_V << HP_APM_REGION15_R1_PMS_R_S)
#define HP_APM_REGION15_R1_PMS_R_V  0x00000001U
#define HP_APM_REGION15_R1_PMS_R_S  6
/** HP_APM_REGION15_R2_PMS_X : R/W; bitpos: [8]; default: 0;
 *  Region execute authority in REE_MODE2
 */
#define HP_APM_REGION15_R2_PMS_X    (BIT(8))
#define HP_APM_REGION15_R2_PMS_X_M  (HP_APM_REGION15_R2_PMS_X_V << HP_APM_REGION15_R2_PMS_X_S)
#define HP_APM_REGION15_R2_PMS_X_V  0x00000001U
#define HP_APM_REGION15_R2_PMS_X_S  8
/** HP_APM_REGION15_R2_PMS_W : R/W; bitpos: [9]; default: 0;
 *  Region write authority in REE_MODE2
 */
#define HP_APM_REGION15_R2_PMS_W    (BIT(9))
#define HP_APM_REGION15_R2_PMS_W_M  (HP_APM_REGION15_R2_PMS_W_V << HP_APM_REGION15_R2_PMS_W_S)
#define HP_APM_REGION15_R2_PMS_W_V  0x00000001U
#define HP_APM_REGION15_R2_PMS_W_S  9
/** HP_APM_REGION15_R2_PMS_R : R/W; bitpos: [10]; default: 0;
 *  Region read authority in REE_MODE2
 */
#define HP_APM_REGION15_R2_PMS_R    (BIT(10))
#define HP_APM_REGION15_R2_PMS_R_M  (HP_APM_REGION15_R2_PMS_R_V << HP_APM_REGION15_R2_PMS_R_S)
#define HP_APM_REGION15_R2_PMS_R_V  0x00000001U
#define HP_APM_REGION15_R2_PMS_R_S  10

/** HP_APM_FUNC_CTRL_REG register
 *  PMS function control register
 */
#define HP_APM_FUNC_CTRL_REG (DR_REG_HP_APM_BASE + 0xc4)
/** HP_APM_M0_PMS_FUNC_EN : R/W; bitpos: [0]; default: 1;
 *  PMS M0 function enable
 */
#define HP_APM_M0_PMS_FUNC_EN    (BIT(0))
#define HP_APM_M0_PMS_FUNC_EN_M  (HP_APM_M0_PMS_FUNC_EN_V << HP_APM_M0_PMS_FUNC_EN_S)
#define HP_APM_M0_PMS_FUNC_EN_V  0x00000001U
#define HP_APM_M0_PMS_FUNC_EN_S  0
/** HP_APM_M1_PMS_FUNC_EN : R/W; bitpos: [1]; default: 1;
 *  PMS M1 function enable
 */
#define HP_APM_M1_PMS_FUNC_EN    (BIT(1))
#define HP_APM_M1_PMS_FUNC_EN_M  (HP_APM_M1_PMS_FUNC_EN_V << HP_APM_M1_PMS_FUNC_EN_S)
#define HP_APM_M1_PMS_FUNC_EN_V  0x00000001U
#define HP_APM_M1_PMS_FUNC_EN_S  1
/** HP_APM_M2_PMS_FUNC_EN : R/W; bitpos: [2]; default: 1;
 *  PMS M2 function enable
 */
#define HP_APM_M2_PMS_FUNC_EN    (BIT(2))
#define HP_APM_M2_PMS_FUNC_EN_M  (HP_APM_M2_PMS_FUNC_EN_V << HP_APM_M2_PMS_FUNC_EN_S)
#define HP_APM_M2_PMS_FUNC_EN_V  0x00000001U
#define HP_APM_M2_PMS_FUNC_EN_S  2
/** HP_APM_M3_PMS_FUNC_EN : R/W; bitpos: [3]; default: 1;
 *  PMS M3 function enable
 */
#define HP_APM_M3_PMS_FUNC_EN    (BIT(3))
#define HP_APM_M3_PMS_FUNC_EN_M  (HP_APM_M3_PMS_FUNC_EN_V << HP_APM_M3_PMS_FUNC_EN_S)
#define HP_APM_M3_PMS_FUNC_EN_V  0x00000001U
#define HP_APM_M3_PMS_FUNC_EN_S  3

/** HP_APM_M0_STATUS_REG register
 *  M0 status register
 */
#define HP_APM_M0_STATUS_REG (DR_REG_HP_APM_BASE + 0xc8)
/** HP_APM_M0_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Exception status
 */
#define HP_APM_M0_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M0_EXCEPTION_STATUS_M  (HP_APM_M0_EXCEPTION_STATUS_V << HP_APM_M0_EXCEPTION_STATUS_S)
#define HP_APM_M0_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M0_EXCEPTION_STATUS_S  0

/** HP_APM_M0_STATUS_CLR_REG register
 *  M0 status clear register
 */
#define HP_APM_M0_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xcc)
/** HP_APM_M0_REGION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Clear exception status
 */
#define HP_APM_M0_REGION_STATUS_CLR    (BIT(0))
#define HP_APM_M0_REGION_STATUS_CLR_M  (HP_APM_M0_REGION_STATUS_CLR_V << HP_APM_M0_REGION_STATUS_CLR_S)
#define HP_APM_M0_REGION_STATUS_CLR_V  0x00000001U
#define HP_APM_M0_REGION_STATUS_CLR_S  0

/** HP_APM_M0_EXCEPTION_INFO0_REG register
 *  M0 exception_info0 register
 */
#define HP_APM_M0_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0xd0)
/** HP_APM_M0_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Exception region
 */
#define HP_APM_M0_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M0_EXCEPTION_REGION_M  (HP_APM_M0_EXCEPTION_REGION_V << HP_APM_M0_EXCEPTION_REGION_S)
#define HP_APM_M0_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M0_EXCEPTION_REGION_S  0
/** HP_APM_M0_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Exception mode
 */
#define HP_APM_M0_EXCEPTION_MODE    0x00000003U
#define HP_APM_M0_EXCEPTION_MODE_M  (HP_APM_M0_EXCEPTION_MODE_V << HP_APM_M0_EXCEPTION_MODE_S)
#define HP_APM_M0_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M0_EXCEPTION_MODE_S  16
/** HP_APM_M0_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Exception id information
 */
#define HP_APM_M0_EXCEPTION_ID    0x0000001FU
#define HP_APM_M0_EXCEPTION_ID_M  (HP_APM_M0_EXCEPTION_ID_V << HP_APM_M0_EXCEPTION_ID_S)
#define HP_APM_M0_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M0_EXCEPTION_ID_S  18

/** HP_APM_M0_EXCEPTION_INFO1_REG register
 *  M0 exception_info1 register
 */
#define HP_APM_M0_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0xd4)
/** HP_APM_M0_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Exception addr
 */
#define HP_APM_M0_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M0_EXCEPTION_ADDR_M  (HP_APM_M0_EXCEPTION_ADDR_V << HP_APM_M0_EXCEPTION_ADDR_S)
#define HP_APM_M0_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M0_EXCEPTION_ADDR_S  0

/** HP_APM_M1_STATUS_REG register
 *  M1 status register
 */
#define HP_APM_M1_STATUS_REG (DR_REG_HP_APM_BASE + 0xd8)
/** HP_APM_M1_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Exception status
 */
#define HP_APM_M1_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M1_EXCEPTION_STATUS_M  (HP_APM_M1_EXCEPTION_STATUS_V << HP_APM_M1_EXCEPTION_STATUS_S)
#define HP_APM_M1_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M1_EXCEPTION_STATUS_S  0

/** HP_APM_M1_STATUS_CLR_REG register
 *  M1 status clear register
 */
#define HP_APM_M1_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xdc)
/** HP_APM_M1_REGION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Clear exception status
 */
#define HP_APM_M1_REGION_STATUS_CLR    (BIT(0))
#define HP_APM_M1_REGION_STATUS_CLR_M  (HP_APM_M1_REGION_STATUS_CLR_V << HP_APM_M1_REGION_STATUS_CLR_S)
#define HP_APM_M1_REGION_STATUS_CLR_V  0x00000001U
#define HP_APM_M1_REGION_STATUS_CLR_S  0

/** HP_APM_M1_EXCEPTION_INFO0_REG register
 *  M1 exception_info0 register
 */
#define HP_APM_M1_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0xe0)
/** HP_APM_M1_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Exception region
 */
#define HP_APM_M1_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M1_EXCEPTION_REGION_M  (HP_APM_M1_EXCEPTION_REGION_V << HP_APM_M1_EXCEPTION_REGION_S)
#define HP_APM_M1_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M1_EXCEPTION_REGION_S  0
/** HP_APM_M1_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Exception mode
 */
#define HP_APM_M1_EXCEPTION_MODE    0x00000003U
#define HP_APM_M1_EXCEPTION_MODE_M  (HP_APM_M1_EXCEPTION_MODE_V << HP_APM_M1_EXCEPTION_MODE_S)
#define HP_APM_M1_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M1_EXCEPTION_MODE_S  16
/** HP_APM_M1_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Exception id information
 */
#define HP_APM_M1_EXCEPTION_ID    0x0000001FU
#define HP_APM_M1_EXCEPTION_ID_M  (HP_APM_M1_EXCEPTION_ID_V << HP_APM_M1_EXCEPTION_ID_S)
#define HP_APM_M1_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M1_EXCEPTION_ID_S  18

/** HP_APM_M1_EXCEPTION_INFO1_REG register
 *  M1 exception_info1 register
 */
#define HP_APM_M1_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0xe4)
/** HP_APM_M1_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Exception addr
 */
#define HP_APM_M1_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M1_EXCEPTION_ADDR_M  (HP_APM_M1_EXCEPTION_ADDR_V << HP_APM_M1_EXCEPTION_ADDR_S)
#define HP_APM_M1_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M1_EXCEPTION_ADDR_S  0

/** HP_APM_M2_STATUS_REG register
 *  M2 status register
 */
#define HP_APM_M2_STATUS_REG (DR_REG_HP_APM_BASE + 0xe8)
/** HP_APM_M2_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Exception status
 */
#define HP_APM_M2_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M2_EXCEPTION_STATUS_M  (HP_APM_M2_EXCEPTION_STATUS_V << HP_APM_M2_EXCEPTION_STATUS_S)
#define HP_APM_M2_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M2_EXCEPTION_STATUS_S  0

/** HP_APM_M2_STATUS_CLR_REG register
 *  M2 status clear register
 */
#define HP_APM_M2_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xec)
/** HP_APM_M2_REGION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Clear exception status
 */
#define HP_APM_M2_REGION_STATUS_CLR    (BIT(0))
#define HP_APM_M2_REGION_STATUS_CLR_M  (HP_APM_M2_REGION_STATUS_CLR_V << HP_APM_M2_REGION_STATUS_CLR_S)
#define HP_APM_M2_REGION_STATUS_CLR_V  0x00000001U
#define HP_APM_M2_REGION_STATUS_CLR_S  0

/** HP_APM_M2_EXCEPTION_INFO0_REG register
 *  M2 exception_info0 register
 */
#define HP_APM_M2_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0xf0)
/** HP_APM_M2_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Exception region
 */
#define HP_APM_M2_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M2_EXCEPTION_REGION_M  (HP_APM_M2_EXCEPTION_REGION_V << HP_APM_M2_EXCEPTION_REGION_S)
#define HP_APM_M2_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M2_EXCEPTION_REGION_S  0
/** HP_APM_M2_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Exception mode
 */
#define HP_APM_M2_EXCEPTION_MODE    0x00000003U
#define HP_APM_M2_EXCEPTION_MODE_M  (HP_APM_M2_EXCEPTION_MODE_V << HP_APM_M2_EXCEPTION_MODE_S)
#define HP_APM_M2_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M2_EXCEPTION_MODE_S  16
/** HP_APM_M2_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Exception id information
 */
#define HP_APM_M2_EXCEPTION_ID    0x0000001FU
#define HP_APM_M2_EXCEPTION_ID_M  (HP_APM_M2_EXCEPTION_ID_V << HP_APM_M2_EXCEPTION_ID_S)
#define HP_APM_M2_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M2_EXCEPTION_ID_S  18

/** HP_APM_M2_EXCEPTION_INFO1_REG register
 *  M2 exception_info1 register
 */
#define HP_APM_M2_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0xf4)
/** HP_APM_M2_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Exception addr
 */
#define HP_APM_M2_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M2_EXCEPTION_ADDR_M  (HP_APM_M2_EXCEPTION_ADDR_V << HP_APM_M2_EXCEPTION_ADDR_S)
#define HP_APM_M2_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M2_EXCEPTION_ADDR_S  0

/** HP_APM_M3_STATUS_REG register
 *  M3 status register
 */
#define HP_APM_M3_STATUS_REG (DR_REG_HP_APM_BASE + 0xf8)
/** HP_APM_M3_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Exception status
 */
#define HP_APM_M3_EXCEPTION_STATUS    0x00000003U
#define HP_APM_M3_EXCEPTION_STATUS_M  (HP_APM_M3_EXCEPTION_STATUS_V << HP_APM_M3_EXCEPTION_STATUS_S)
#define HP_APM_M3_EXCEPTION_STATUS_V  0x00000003U
#define HP_APM_M3_EXCEPTION_STATUS_S  0

/** HP_APM_M3_STATUS_CLR_REG register
 *  M3 status clear register
 */
#define HP_APM_M3_STATUS_CLR_REG (DR_REG_HP_APM_BASE + 0xfc)
/** HP_APM_M3_REGION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Clear exception status
 */
#define HP_APM_M3_REGION_STATUS_CLR    (BIT(0))
#define HP_APM_M3_REGION_STATUS_CLR_M  (HP_APM_M3_REGION_STATUS_CLR_V << HP_APM_M3_REGION_STATUS_CLR_S)
#define HP_APM_M3_REGION_STATUS_CLR_V  0x00000001U
#define HP_APM_M3_REGION_STATUS_CLR_S  0

/** HP_APM_M3_EXCEPTION_INFO0_REG register
 *  M3 exception_info0 register
 */
#define HP_APM_M3_EXCEPTION_INFO0_REG (DR_REG_HP_APM_BASE + 0x100)
/** HP_APM_M3_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Exception region
 */
#define HP_APM_M3_EXCEPTION_REGION    0x0000FFFFU
#define HP_APM_M3_EXCEPTION_REGION_M  (HP_APM_M3_EXCEPTION_REGION_V << HP_APM_M3_EXCEPTION_REGION_S)
#define HP_APM_M3_EXCEPTION_REGION_V  0x0000FFFFU
#define HP_APM_M3_EXCEPTION_REGION_S  0
/** HP_APM_M3_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Exception mode
 */
#define HP_APM_M3_EXCEPTION_MODE    0x00000003U
#define HP_APM_M3_EXCEPTION_MODE_M  (HP_APM_M3_EXCEPTION_MODE_V << HP_APM_M3_EXCEPTION_MODE_S)
#define HP_APM_M3_EXCEPTION_MODE_V  0x00000003U
#define HP_APM_M3_EXCEPTION_MODE_S  16
/** HP_APM_M3_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Exception id information
 */
#define HP_APM_M3_EXCEPTION_ID    0x0000001FU
#define HP_APM_M3_EXCEPTION_ID_M  (HP_APM_M3_EXCEPTION_ID_V << HP_APM_M3_EXCEPTION_ID_S)
#define HP_APM_M3_EXCEPTION_ID_V  0x0000001FU
#define HP_APM_M3_EXCEPTION_ID_S  18

/** HP_APM_M3_EXCEPTION_INFO1_REG register
 *  M3 exception_info1 register
 */
#define HP_APM_M3_EXCEPTION_INFO1_REG (DR_REG_HP_APM_BASE + 0x104)
/** HP_APM_M3_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Exception addr
 */
#define HP_APM_M3_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_APM_M3_EXCEPTION_ADDR_M  (HP_APM_M3_EXCEPTION_ADDR_V << HP_APM_M3_EXCEPTION_ADDR_S)
#define HP_APM_M3_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_APM_M3_EXCEPTION_ADDR_S  0

/** HP_APM_INT_EN_REG register
 *  APM interrupt enable register
 */
#define HP_APM_INT_EN_REG (DR_REG_HP_APM_BASE + 0x108)
/** HP_APM_M0_APM_INT_EN : R/W; bitpos: [0]; default: 0;
 *  APM M0 interrupt enable
 */
#define HP_APM_M0_APM_INT_EN    (BIT(0))
#define HP_APM_M0_APM_INT_EN_M  (HP_APM_M0_APM_INT_EN_V << HP_APM_M0_APM_INT_EN_S)
#define HP_APM_M0_APM_INT_EN_V  0x00000001U
#define HP_APM_M0_APM_INT_EN_S  0
/** HP_APM_M1_APM_INT_EN : R/W; bitpos: [1]; default: 0;
 *  APM M1 interrupt enable
 */
#define HP_APM_M1_APM_INT_EN    (BIT(1))
#define HP_APM_M1_APM_INT_EN_M  (HP_APM_M1_APM_INT_EN_V << HP_APM_M1_APM_INT_EN_S)
#define HP_APM_M1_APM_INT_EN_V  0x00000001U
#define HP_APM_M1_APM_INT_EN_S  1
/** HP_APM_M2_APM_INT_EN : R/W; bitpos: [2]; default: 0;
 *  APM M2 interrupt enable
 */
#define HP_APM_M2_APM_INT_EN    (BIT(2))
#define HP_APM_M2_APM_INT_EN_M  (HP_APM_M2_APM_INT_EN_V << HP_APM_M2_APM_INT_EN_S)
#define HP_APM_M2_APM_INT_EN_V  0x00000001U
#define HP_APM_M2_APM_INT_EN_S  2
/** HP_APM_M3_APM_INT_EN : R/W; bitpos: [3]; default: 0;
 *  APM M3 interrupt enable
 */
#define HP_APM_M3_APM_INT_EN    (BIT(3))
#define HP_APM_M3_APM_INT_EN_M  (HP_APM_M3_APM_INT_EN_V << HP_APM_M3_APM_INT_EN_S)
#define HP_APM_M3_APM_INT_EN_V  0x00000001U
#define HP_APM_M3_APM_INT_EN_S  3

/** HP_APM_CLOCK_GATE_REG register
 *  clock gating register
 */
#define HP_APM_CLOCK_GATE_REG (DR_REG_HP_APM_BASE + 0x10c)
/** HP_APM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  reg_clk_en
 */
#define HP_APM_CLK_EN    (BIT(0))
#define HP_APM_CLK_EN_M  (HP_APM_CLK_EN_V << HP_APM_CLK_EN_S)
#define HP_APM_CLK_EN_V  0x00000001U
#define HP_APM_CLK_EN_S  0

/** HP_APM_DATE_REG register
 *  Version register
 */
#define HP_APM_DATE_REG (DR_REG_HP_APM_BASE + 0x7fc)
/** HP_APM_DATE : R/W; bitpos: [27:0]; default: 35672640;
 *  reg_date
 */
#define HP_APM_DATE    0x0FFFFFFFU
#define HP_APM_DATE_M  (HP_APM_DATE_V << HP_APM_DATE_S)
#define HP_APM_DATE_V  0x0FFFFFFFU
#define HP_APM_DATE_S  0

#ifdef __cplusplus
}
#endif
