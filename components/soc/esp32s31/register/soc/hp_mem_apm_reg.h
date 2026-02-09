/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HP_MEM_APM_REGION_FILTER_EN_REG register
 *  Region filter enable register
 */
#define HP_MEM_APM_REGION_FILTER_EN_REG (DR_REG_HP_MEM_APM_BASE + 0x0)
/** HP_MEM_APM_REGION_FILTER_EN : R/W; bitpos: [7:0]; default: 1;
 *  Configure bit $n (0-7) to enable region $n.
 *  0: disable
 *  1: enable
 */
#define HP_MEM_APM_REGION_FILTER_EN    0x000000FFU
#define HP_MEM_APM_REGION_FILTER_EN_M  (HP_MEM_APM_REGION_FILTER_EN_V << HP_MEM_APM_REGION_FILTER_EN_S)
#define HP_MEM_APM_REGION_FILTER_EN_V  0x000000FFU
#define HP_MEM_APM_REGION_FILTER_EN_S  0

/** HP_MEM_APM_REGION0_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION0_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x4)
/** HP_MEM_APM_REGION0_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 0.
 */
#define HP_MEM_APM_REGION0_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION0_ADDR_START_L_M  (HP_MEM_APM_REGION0_ADDR_START_L_V << HP_MEM_APM_REGION0_ADDR_START_L_S)
#define HP_MEM_APM_REGION0_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION0_ADDR_START_L_S  0
/** HP_MEM_APM_REGION0_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 0.
 */
#define HP_MEM_APM_REGION0_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION0_ADDR_START_M  (HP_MEM_APM_REGION0_ADDR_START_V << HP_MEM_APM_REGION0_ADDR_START_S)
#define HP_MEM_APM_REGION0_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION0_ADDR_START_S  7
/** HP_MEM_APM_REGION0_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 0.
 */
#define HP_MEM_APM_REGION0_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION0_ADDR_START_H_M  (HP_MEM_APM_REGION0_ADDR_START_H_V << HP_MEM_APM_REGION0_ADDR_START_H_S)
#define HP_MEM_APM_REGION0_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION0_ADDR_START_H_S  19

/** HP_MEM_APM_REGION0_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION0_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x8)
/** HP_MEM_APM_REGION0_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 0.
 */
#define HP_MEM_APM_REGION0_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION0_ADDR_END_L_M  (HP_MEM_APM_REGION0_ADDR_END_L_V << HP_MEM_APM_REGION0_ADDR_END_L_S)
#define HP_MEM_APM_REGION0_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION0_ADDR_END_L_S  0
/** HP_MEM_APM_REGION0_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 0.
 */
#define HP_MEM_APM_REGION0_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION0_ADDR_END_M  (HP_MEM_APM_REGION0_ADDR_END_V << HP_MEM_APM_REGION0_ADDR_END_S)
#define HP_MEM_APM_REGION0_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION0_ADDR_END_S  7
/** HP_MEM_APM_REGION0_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 0.
 */
#define HP_MEM_APM_REGION0_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION0_ADDR_END_H_M  (HP_MEM_APM_REGION0_ADDR_END_H_V << HP_MEM_APM_REGION0_ADDR_END_H_S)
#define HP_MEM_APM_REGION0_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION0_ADDR_END_H_S  19

/** HP_MEM_APM_REGION0_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION0_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0xc)
/** HP_MEM_APM_REGION0_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 0.
 */
#define HP_MEM_APM_REGION0_R0_X    (BIT(0))
#define HP_MEM_APM_REGION0_R0_X_M  (HP_MEM_APM_REGION0_R0_X_V << HP_MEM_APM_REGION0_R0_X_S)
#define HP_MEM_APM_REGION0_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION0_R0_X_S  0
/** HP_MEM_APM_REGION0_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 0.
 */
#define HP_MEM_APM_REGION0_R0_W    (BIT(1))
#define HP_MEM_APM_REGION0_R0_W_M  (HP_MEM_APM_REGION0_R0_W_V << HP_MEM_APM_REGION0_R0_W_S)
#define HP_MEM_APM_REGION0_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION0_R0_W_S  1
/** HP_MEM_APM_REGION0_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 0.
 */
#define HP_MEM_APM_REGION0_R0_R    (BIT(2))
#define HP_MEM_APM_REGION0_R0_R_M  (HP_MEM_APM_REGION0_R0_R_V << HP_MEM_APM_REGION0_R0_R_S)
#define HP_MEM_APM_REGION0_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION0_R0_R_S  2
/** HP_MEM_APM_REGION0_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 0.
 */
#define HP_MEM_APM_REGION0_R1_X    (BIT(4))
#define HP_MEM_APM_REGION0_R1_X_M  (HP_MEM_APM_REGION0_R1_X_V << HP_MEM_APM_REGION0_R1_X_S)
#define HP_MEM_APM_REGION0_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION0_R1_X_S  4
/** HP_MEM_APM_REGION0_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 0.
 */
#define HP_MEM_APM_REGION0_R1_W    (BIT(5))
#define HP_MEM_APM_REGION0_R1_W_M  (HP_MEM_APM_REGION0_R1_W_V << HP_MEM_APM_REGION0_R1_W_S)
#define HP_MEM_APM_REGION0_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION0_R1_W_S  5
/** HP_MEM_APM_REGION0_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 0.
 */
#define HP_MEM_APM_REGION0_R1_R    (BIT(6))
#define HP_MEM_APM_REGION0_R1_R_M  (HP_MEM_APM_REGION0_R1_R_V << HP_MEM_APM_REGION0_R1_R_S)
#define HP_MEM_APM_REGION0_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION0_R1_R_S  6
/** HP_MEM_APM_REGION0_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 0.
 */
#define HP_MEM_APM_REGION0_R2_X    (BIT(8))
#define HP_MEM_APM_REGION0_R2_X_M  (HP_MEM_APM_REGION0_R2_X_V << HP_MEM_APM_REGION0_R2_X_S)
#define HP_MEM_APM_REGION0_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION0_R2_X_S  8
/** HP_MEM_APM_REGION0_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 0.
 */
#define HP_MEM_APM_REGION0_R2_W    (BIT(9))
#define HP_MEM_APM_REGION0_R2_W_M  (HP_MEM_APM_REGION0_R2_W_V << HP_MEM_APM_REGION0_R2_W_S)
#define HP_MEM_APM_REGION0_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION0_R2_W_S  9
/** HP_MEM_APM_REGION0_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 0.
 */
#define HP_MEM_APM_REGION0_R2_R    (BIT(10))
#define HP_MEM_APM_REGION0_R2_R_M  (HP_MEM_APM_REGION0_R2_R_V << HP_MEM_APM_REGION0_R2_R_S)
#define HP_MEM_APM_REGION0_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION0_R2_R_S  10
/** HP_MEM_APM_REGION0_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 0.
 */
#define HP_MEM_APM_REGION0_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION0_TEE_X_M  (HP_MEM_APM_REGION0_TEE_X_V << HP_MEM_APM_REGION0_TEE_X_S)
#define HP_MEM_APM_REGION0_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION0_TEE_X_S  12
/** HP_MEM_APM_REGION0_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 0.
 */
#define HP_MEM_APM_REGION0_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION0_TEE_W_M  (HP_MEM_APM_REGION0_TEE_W_V << HP_MEM_APM_REGION0_TEE_W_S)
#define HP_MEM_APM_REGION0_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION0_TEE_W_S  13
/** HP_MEM_APM_REGION0_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 0.
 */
#define HP_MEM_APM_REGION0_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION0_TEE_R_M  (HP_MEM_APM_REGION0_TEE_R_V << HP_MEM_APM_REGION0_TEE_R_S)
#define HP_MEM_APM_REGION0_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION0_TEE_R_S  14
/** HP_MEM_APM_REGION0_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION0_LOCK    (BIT(16))
#define HP_MEM_APM_REGION0_LOCK_M  (HP_MEM_APM_REGION0_LOCK_V << HP_MEM_APM_REGION0_LOCK_S)
#define HP_MEM_APM_REGION0_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION0_LOCK_S  16

/** HP_MEM_APM_REGION1_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION1_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x10)
/** HP_MEM_APM_REGION1_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 1.
 */
#define HP_MEM_APM_REGION1_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION1_ADDR_START_L_M  (HP_MEM_APM_REGION1_ADDR_START_L_V << HP_MEM_APM_REGION1_ADDR_START_L_S)
#define HP_MEM_APM_REGION1_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION1_ADDR_START_L_S  0
/** HP_MEM_APM_REGION1_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 1.
 */
#define HP_MEM_APM_REGION1_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION1_ADDR_START_M  (HP_MEM_APM_REGION1_ADDR_START_V << HP_MEM_APM_REGION1_ADDR_START_S)
#define HP_MEM_APM_REGION1_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION1_ADDR_START_S  7
/** HP_MEM_APM_REGION1_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 1.
 */
#define HP_MEM_APM_REGION1_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION1_ADDR_START_H_M  (HP_MEM_APM_REGION1_ADDR_START_H_V << HP_MEM_APM_REGION1_ADDR_START_H_S)
#define HP_MEM_APM_REGION1_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION1_ADDR_START_H_S  19

/** HP_MEM_APM_REGION1_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION1_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x14)
/** HP_MEM_APM_REGION1_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 1.
 */
#define HP_MEM_APM_REGION1_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION1_ADDR_END_L_M  (HP_MEM_APM_REGION1_ADDR_END_L_V << HP_MEM_APM_REGION1_ADDR_END_L_S)
#define HP_MEM_APM_REGION1_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION1_ADDR_END_L_S  0
/** HP_MEM_APM_REGION1_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 1.
 */
#define HP_MEM_APM_REGION1_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION1_ADDR_END_M  (HP_MEM_APM_REGION1_ADDR_END_V << HP_MEM_APM_REGION1_ADDR_END_S)
#define HP_MEM_APM_REGION1_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION1_ADDR_END_S  7
/** HP_MEM_APM_REGION1_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 1.
 */
#define HP_MEM_APM_REGION1_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION1_ADDR_END_H_M  (HP_MEM_APM_REGION1_ADDR_END_H_V << HP_MEM_APM_REGION1_ADDR_END_H_S)
#define HP_MEM_APM_REGION1_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION1_ADDR_END_H_S  19

/** HP_MEM_APM_REGION1_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION1_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0x18)
/** HP_MEM_APM_REGION1_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 1.
 */
#define HP_MEM_APM_REGION1_R0_X    (BIT(0))
#define HP_MEM_APM_REGION1_R0_X_M  (HP_MEM_APM_REGION1_R0_X_V << HP_MEM_APM_REGION1_R0_X_S)
#define HP_MEM_APM_REGION1_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION1_R0_X_S  0
/** HP_MEM_APM_REGION1_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 1.
 */
#define HP_MEM_APM_REGION1_R0_W    (BIT(1))
#define HP_MEM_APM_REGION1_R0_W_M  (HP_MEM_APM_REGION1_R0_W_V << HP_MEM_APM_REGION1_R0_W_S)
#define HP_MEM_APM_REGION1_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION1_R0_W_S  1
/** HP_MEM_APM_REGION1_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 1.
 */
#define HP_MEM_APM_REGION1_R0_R    (BIT(2))
#define HP_MEM_APM_REGION1_R0_R_M  (HP_MEM_APM_REGION1_R0_R_V << HP_MEM_APM_REGION1_R0_R_S)
#define HP_MEM_APM_REGION1_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION1_R0_R_S  2
/** HP_MEM_APM_REGION1_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 1.
 */
#define HP_MEM_APM_REGION1_R1_X    (BIT(4))
#define HP_MEM_APM_REGION1_R1_X_M  (HP_MEM_APM_REGION1_R1_X_V << HP_MEM_APM_REGION1_R1_X_S)
#define HP_MEM_APM_REGION1_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION1_R1_X_S  4
/** HP_MEM_APM_REGION1_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 1.
 */
#define HP_MEM_APM_REGION1_R1_W    (BIT(5))
#define HP_MEM_APM_REGION1_R1_W_M  (HP_MEM_APM_REGION1_R1_W_V << HP_MEM_APM_REGION1_R1_W_S)
#define HP_MEM_APM_REGION1_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION1_R1_W_S  5
/** HP_MEM_APM_REGION1_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 1.
 */
#define HP_MEM_APM_REGION1_R1_R    (BIT(6))
#define HP_MEM_APM_REGION1_R1_R_M  (HP_MEM_APM_REGION1_R1_R_V << HP_MEM_APM_REGION1_R1_R_S)
#define HP_MEM_APM_REGION1_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION1_R1_R_S  6
/** HP_MEM_APM_REGION1_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 1.
 */
#define HP_MEM_APM_REGION1_R2_X    (BIT(8))
#define HP_MEM_APM_REGION1_R2_X_M  (HP_MEM_APM_REGION1_R2_X_V << HP_MEM_APM_REGION1_R2_X_S)
#define HP_MEM_APM_REGION1_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION1_R2_X_S  8
/** HP_MEM_APM_REGION1_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 1.
 */
#define HP_MEM_APM_REGION1_R2_W    (BIT(9))
#define HP_MEM_APM_REGION1_R2_W_M  (HP_MEM_APM_REGION1_R2_W_V << HP_MEM_APM_REGION1_R2_W_S)
#define HP_MEM_APM_REGION1_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION1_R2_W_S  9
/** HP_MEM_APM_REGION1_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 1.
 */
#define HP_MEM_APM_REGION1_R2_R    (BIT(10))
#define HP_MEM_APM_REGION1_R2_R_M  (HP_MEM_APM_REGION1_R2_R_V << HP_MEM_APM_REGION1_R2_R_S)
#define HP_MEM_APM_REGION1_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION1_R2_R_S  10
/** HP_MEM_APM_REGION1_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 1.
 */
#define HP_MEM_APM_REGION1_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION1_TEE_X_M  (HP_MEM_APM_REGION1_TEE_X_V << HP_MEM_APM_REGION1_TEE_X_S)
#define HP_MEM_APM_REGION1_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION1_TEE_X_S  12
/** HP_MEM_APM_REGION1_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 1.
 */
#define HP_MEM_APM_REGION1_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION1_TEE_W_M  (HP_MEM_APM_REGION1_TEE_W_V << HP_MEM_APM_REGION1_TEE_W_S)
#define HP_MEM_APM_REGION1_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION1_TEE_W_S  13
/** HP_MEM_APM_REGION1_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 1.
 */
#define HP_MEM_APM_REGION1_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION1_TEE_R_M  (HP_MEM_APM_REGION1_TEE_R_V << HP_MEM_APM_REGION1_TEE_R_S)
#define HP_MEM_APM_REGION1_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION1_TEE_R_S  14
/** HP_MEM_APM_REGION1_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION1_LOCK    (BIT(16))
#define HP_MEM_APM_REGION1_LOCK_M  (HP_MEM_APM_REGION1_LOCK_V << HP_MEM_APM_REGION1_LOCK_S)
#define HP_MEM_APM_REGION1_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION1_LOCK_S  16

/** HP_MEM_APM_REGION2_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION2_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x1c)
/** HP_MEM_APM_REGION2_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 2.
 */
#define HP_MEM_APM_REGION2_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION2_ADDR_START_L_M  (HP_MEM_APM_REGION2_ADDR_START_L_V << HP_MEM_APM_REGION2_ADDR_START_L_S)
#define HP_MEM_APM_REGION2_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION2_ADDR_START_L_S  0
/** HP_MEM_APM_REGION2_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 2.
 */
#define HP_MEM_APM_REGION2_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION2_ADDR_START_M  (HP_MEM_APM_REGION2_ADDR_START_V << HP_MEM_APM_REGION2_ADDR_START_S)
#define HP_MEM_APM_REGION2_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION2_ADDR_START_S  7
/** HP_MEM_APM_REGION2_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 2.
 */
#define HP_MEM_APM_REGION2_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION2_ADDR_START_H_M  (HP_MEM_APM_REGION2_ADDR_START_H_V << HP_MEM_APM_REGION2_ADDR_START_H_S)
#define HP_MEM_APM_REGION2_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION2_ADDR_START_H_S  19

/** HP_MEM_APM_REGION2_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION2_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x20)
/** HP_MEM_APM_REGION2_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 2.
 */
#define HP_MEM_APM_REGION2_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION2_ADDR_END_L_M  (HP_MEM_APM_REGION2_ADDR_END_L_V << HP_MEM_APM_REGION2_ADDR_END_L_S)
#define HP_MEM_APM_REGION2_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION2_ADDR_END_L_S  0
/** HP_MEM_APM_REGION2_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 2.
 */
#define HP_MEM_APM_REGION2_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION2_ADDR_END_M  (HP_MEM_APM_REGION2_ADDR_END_V << HP_MEM_APM_REGION2_ADDR_END_S)
#define HP_MEM_APM_REGION2_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION2_ADDR_END_S  7
/** HP_MEM_APM_REGION2_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 2.
 */
#define HP_MEM_APM_REGION2_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION2_ADDR_END_H_M  (HP_MEM_APM_REGION2_ADDR_END_H_V << HP_MEM_APM_REGION2_ADDR_END_H_S)
#define HP_MEM_APM_REGION2_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION2_ADDR_END_H_S  19

/** HP_MEM_APM_REGION2_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION2_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0x24)
/** HP_MEM_APM_REGION2_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 2.
 */
#define HP_MEM_APM_REGION2_R0_X    (BIT(0))
#define HP_MEM_APM_REGION2_R0_X_M  (HP_MEM_APM_REGION2_R0_X_V << HP_MEM_APM_REGION2_R0_X_S)
#define HP_MEM_APM_REGION2_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION2_R0_X_S  0
/** HP_MEM_APM_REGION2_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 2.
 */
#define HP_MEM_APM_REGION2_R0_W    (BIT(1))
#define HP_MEM_APM_REGION2_R0_W_M  (HP_MEM_APM_REGION2_R0_W_V << HP_MEM_APM_REGION2_R0_W_S)
#define HP_MEM_APM_REGION2_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION2_R0_W_S  1
/** HP_MEM_APM_REGION2_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 2.
 */
#define HP_MEM_APM_REGION2_R0_R    (BIT(2))
#define HP_MEM_APM_REGION2_R0_R_M  (HP_MEM_APM_REGION2_R0_R_V << HP_MEM_APM_REGION2_R0_R_S)
#define HP_MEM_APM_REGION2_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION2_R0_R_S  2
/** HP_MEM_APM_REGION2_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 2.
 */
#define HP_MEM_APM_REGION2_R1_X    (BIT(4))
#define HP_MEM_APM_REGION2_R1_X_M  (HP_MEM_APM_REGION2_R1_X_V << HP_MEM_APM_REGION2_R1_X_S)
#define HP_MEM_APM_REGION2_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION2_R1_X_S  4
/** HP_MEM_APM_REGION2_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 2.
 */
#define HP_MEM_APM_REGION2_R1_W    (BIT(5))
#define HP_MEM_APM_REGION2_R1_W_M  (HP_MEM_APM_REGION2_R1_W_V << HP_MEM_APM_REGION2_R1_W_S)
#define HP_MEM_APM_REGION2_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION2_R1_W_S  5
/** HP_MEM_APM_REGION2_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 2.
 */
#define HP_MEM_APM_REGION2_R1_R    (BIT(6))
#define HP_MEM_APM_REGION2_R1_R_M  (HP_MEM_APM_REGION2_R1_R_V << HP_MEM_APM_REGION2_R1_R_S)
#define HP_MEM_APM_REGION2_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION2_R1_R_S  6
/** HP_MEM_APM_REGION2_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 2.
 */
#define HP_MEM_APM_REGION2_R2_X    (BIT(8))
#define HP_MEM_APM_REGION2_R2_X_M  (HP_MEM_APM_REGION2_R2_X_V << HP_MEM_APM_REGION2_R2_X_S)
#define HP_MEM_APM_REGION2_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION2_R2_X_S  8
/** HP_MEM_APM_REGION2_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 2.
 */
#define HP_MEM_APM_REGION2_R2_W    (BIT(9))
#define HP_MEM_APM_REGION2_R2_W_M  (HP_MEM_APM_REGION2_R2_W_V << HP_MEM_APM_REGION2_R2_W_S)
#define HP_MEM_APM_REGION2_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION2_R2_W_S  9
/** HP_MEM_APM_REGION2_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 2.
 */
#define HP_MEM_APM_REGION2_R2_R    (BIT(10))
#define HP_MEM_APM_REGION2_R2_R_M  (HP_MEM_APM_REGION2_R2_R_V << HP_MEM_APM_REGION2_R2_R_S)
#define HP_MEM_APM_REGION2_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION2_R2_R_S  10
/** HP_MEM_APM_REGION2_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 2.
 */
#define HP_MEM_APM_REGION2_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION2_TEE_X_M  (HP_MEM_APM_REGION2_TEE_X_V << HP_MEM_APM_REGION2_TEE_X_S)
#define HP_MEM_APM_REGION2_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION2_TEE_X_S  12
/** HP_MEM_APM_REGION2_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 2.
 */
#define HP_MEM_APM_REGION2_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION2_TEE_W_M  (HP_MEM_APM_REGION2_TEE_W_V << HP_MEM_APM_REGION2_TEE_W_S)
#define HP_MEM_APM_REGION2_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION2_TEE_W_S  13
/** HP_MEM_APM_REGION2_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 2.
 */
#define HP_MEM_APM_REGION2_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION2_TEE_R_M  (HP_MEM_APM_REGION2_TEE_R_V << HP_MEM_APM_REGION2_TEE_R_S)
#define HP_MEM_APM_REGION2_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION2_TEE_R_S  14
/** HP_MEM_APM_REGION2_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION2_LOCK    (BIT(16))
#define HP_MEM_APM_REGION2_LOCK_M  (HP_MEM_APM_REGION2_LOCK_V << HP_MEM_APM_REGION2_LOCK_S)
#define HP_MEM_APM_REGION2_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION2_LOCK_S  16

/** HP_MEM_APM_REGION3_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION3_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x28)
/** HP_MEM_APM_REGION3_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 3.
 */
#define HP_MEM_APM_REGION3_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION3_ADDR_START_L_M  (HP_MEM_APM_REGION3_ADDR_START_L_V << HP_MEM_APM_REGION3_ADDR_START_L_S)
#define HP_MEM_APM_REGION3_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION3_ADDR_START_L_S  0
/** HP_MEM_APM_REGION3_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 3.
 */
#define HP_MEM_APM_REGION3_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION3_ADDR_START_M  (HP_MEM_APM_REGION3_ADDR_START_V << HP_MEM_APM_REGION3_ADDR_START_S)
#define HP_MEM_APM_REGION3_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION3_ADDR_START_S  7
/** HP_MEM_APM_REGION3_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 3.
 */
#define HP_MEM_APM_REGION3_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION3_ADDR_START_H_M  (HP_MEM_APM_REGION3_ADDR_START_H_V << HP_MEM_APM_REGION3_ADDR_START_H_S)
#define HP_MEM_APM_REGION3_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION3_ADDR_START_H_S  19

/** HP_MEM_APM_REGION3_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION3_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x2c)
/** HP_MEM_APM_REGION3_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 3.
 */
#define HP_MEM_APM_REGION3_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION3_ADDR_END_L_M  (HP_MEM_APM_REGION3_ADDR_END_L_V << HP_MEM_APM_REGION3_ADDR_END_L_S)
#define HP_MEM_APM_REGION3_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION3_ADDR_END_L_S  0
/** HP_MEM_APM_REGION3_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 3.
 */
#define HP_MEM_APM_REGION3_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION3_ADDR_END_M  (HP_MEM_APM_REGION3_ADDR_END_V << HP_MEM_APM_REGION3_ADDR_END_S)
#define HP_MEM_APM_REGION3_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION3_ADDR_END_S  7
/** HP_MEM_APM_REGION3_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 3.
 */
#define HP_MEM_APM_REGION3_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION3_ADDR_END_H_M  (HP_MEM_APM_REGION3_ADDR_END_H_V << HP_MEM_APM_REGION3_ADDR_END_H_S)
#define HP_MEM_APM_REGION3_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION3_ADDR_END_H_S  19

/** HP_MEM_APM_REGION3_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION3_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0x30)
/** HP_MEM_APM_REGION3_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 3.
 */
#define HP_MEM_APM_REGION3_R0_X    (BIT(0))
#define HP_MEM_APM_REGION3_R0_X_M  (HP_MEM_APM_REGION3_R0_X_V << HP_MEM_APM_REGION3_R0_X_S)
#define HP_MEM_APM_REGION3_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION3_R0_X_S  0
/** HP_MEM_APM_REGION3_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 3.
 */
#define HP_MEM_APM_REGION3_R0_W    (BIT(1))
#define HP_MEM_APM_REGION3_R0_W_M  (HP_MEM_APM_REGION3_R0_W_V << HP_MEM_APM_REGION3_R0_W_S)
#define HP_MEM_APM_REGION3_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION3_R0_W_S  1
/** HP_MEM_APM_REGION3_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 3.
 */
#define HP_MEM_APM_REGION3_R0_R    (BIT(2))
#define HP_MEM_APM_REGION3_R0_R_M  (HP_MEM_APM_REGION3_R0_R_V << HP_MEM_APM_REGION3_R0_R_S)
#define HP_MEM_APM_REGION3_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION3_R0_R_S  2
/** HP_MEM_APM_REGION3_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 3.
 */
#define HP_MEM_APM_REGION3_R1_X    (BIT(4))
#define HP_MEM_APM_REGION3_R1_X_M  (HP_MEM_APM_REGION3_R1_X_V << HP_MEM_APM_REGION3_R1_X_S)
#define HP_MEM_APM_REGION3_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION3_R1_X_S  4
/** HP_MEM_APM_REGION3_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 3.
 */
#define HP_MEM_APM_REGION3_R1_W    (BIT(5))
#define HP_MEM_APM_REGION3_R1_W_M  (HP_MEM_APM_REGION3_R1_W_V << HP_MEM_APM_REGION3_R1_W_S)
#define HP_MEM_APM_REGION3_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION3_R1_W_S  5
/** HP_MEM_APM_REGION3_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 3.
 */
#define HP_MEM_APM_REGION3_R1_R    (BIT(6))
#define HP_MEM_APM_REGION3_R1_R_M  (HP_MEM_APM_REGION3_R1_R_V << HP_MEM_APM_REGION3_R1_R_S)
#define HP_MEM_APM_REGION3_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION3_R1_R_S  6
/** HP_MEM_APM_REGION3_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 3.
 */
#define HP_MEM_APM_REGION3_R2_X    (BIT(8))
#define HP_MEM_APM_REGION3_R2_X_M  (HP_MEM_APM_REGION3_R2_X_V << HP_MEM_APM_REGION3_R2_X_S)
#define HP_MEM_APM_REGION3_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION3_R2_X_S  8
/** HP_MEM_APM_REGION3_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 3.
 */
#define HP_MEM_APM_REGION3_R2_W    (BIT(9))
#define HP_MEM_APM_REGION3_R2_W_M  (HP_MEM_APM_REGION3_R2_W_V << HP_MEM_APM_REGION3_R2_W_S)
#define HP_MEM_APM_REGION3_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION3_R2_W_S  9
/** HP_MEM_APM_REGION3_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 3.
 */
#define HP_MEM_APM_REGION3_R2_R    (BIT(10))
#define HP_MEM_APM_REGION3_R2_R_M  (HP_MEM_APM_REGION3_R2_R_V << HP_MEM_APM_REGION3_R2_R_S)
#define HP_MEM_APM_REGION3_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION3_R2_R_S  10
/** HP_MEM_APM_REGION3_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 3.
 */
#define HP_MEM_APM_REGION3_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION3_TEE_X_M  (HP_MEM_APM_REGION3_TEE_X_V << HP_MEM_APM_REGION3_TEE_X_S)
#define HP_MEM_APM_REGION3_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION3_TEE_X_S  12
/** HP_MEM_APM_REGION3_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 3.
 */
#define HP_MEM_APM_REGION3_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION3_TEE_W_M  (HP_MEM_APM_REGION3_TEE_W_V << HP_MEM_APM_REGION3_TEE_W_S)
#define HP_MEM_APM_REGION3_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION3_TEE_W_S  13
/** HP_MEM_APM_REGION3_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 3.
 */
#define HP_MEM_APM_REGION3_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION3_TEE_R_M  (HP_MEM_APM_REGION3_TEE_R_V << HP_MEM_APM_REGION3_TEE_R_S)
#define HP_MEM_APM_REGION3_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION3_TEE_R_S  14
/** HP_MEM_APM_REGION3_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION3_LOCK    (BIT(16))
#define HP_MEM_APM_REGION3_LOCK_M  (HP_MEM_APM_REGION3_LOCK_V << HP_MEM_APM_REGION3_LOCK_S)
#define HP_MEM_APM_REGION3_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION3_LOCK_S  16

/** HP_MEM_APM_REGION4_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION4_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x34)
/** HP_MEM_APM_REGION4_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 4.
 */
#define HP_MEM_APM_REGION4_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION4_ADDR_START_L_M  (HP_MEM_APM_REGION4_ADDR_START_L_V << HP_MEM_APM_REGION4_ADDR_START_L_S)
#define HP_MEM_APM_REGION4_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION4_ADDR_START_L_S  0
/** HP_MEM_APM_REGION4_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 4.
 */
#define HP_MEM_APM_REGION4_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION4_ADDR_START_M  (HP_MEM_APM_REGION4_ADDR_START_V << HP_MEM_APM_REGION4_ADDR_START_S)
#define HP_MEM_APM_REGION4_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION4_ADDR_START_S  7
/** HP_MEM_APM_REGION4_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 4.
 */
#define HP_MEM_APM_REGION4_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION4_ADDR_START_H_M  (HP_MEM_APM_REGION4_ADDR_START_H_V << HP_MEM_APM_REGION4_ADDR_START_H_S)
#define HP_MEM_APM_REGION4_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION4_ADDR_START_H_S  19

/** HP_MEM_APM_REGION4_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION4_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x38)
/** HP_MEM_APM_REGION4_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 4.
 */
#define HP_MEM_APM_REGION4_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION4_ADDR_END_L_M  (HP_MEM_APM_REGION4_ADDR_END_L_V << HP_MEM_APM_REGION4_ADDR_END_L_S)
#define HP_MEM_APM_REGION4_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION4_ADDR_END_L_S  0
/** HP_MEM_APM_REGION4_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 4.
 */
#define HP_MEM_APM_REGION4_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION4_ADDR_END_M  (HP_MEM_APM_REGION4_ADDR_END_V << HP_MEM_APM_REGION4_ADDR_END_S)
#define HP_MEM_APM_REGION4_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION4_ADDR_END_S  7
/** HP_MEM_APM_REGION4_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 4.
 */
#define HP_MEM_APM_REGION4_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION4_ADDR_END_H_M  (HP_MEM_APM_REGION4_ADDR_END_H_V << HP_MEM_APM_REGION4_ADDR_END_H_S)
#define HP_MEM_APM_REGION4_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION4_ADDR_END_H_S  19

/** HP_MEM_APM_REGION4_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION4_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0x3c)
/** HP_MEM_APM_REGION4_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 4.
 */
#define HP_MEM_APM_REGION4_R0_X    (BIT(0))
#define HP_MEM_APM_REGION4_R0_X_M  (HP_MEM_APM_REGION4_R0_X_V << HP_MEM_APM_REGION4_R0_X_S)
#define HP_MEM_APM_REGION4_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION4_R0_X_S  0
/** HP_MEM_APM_REGION4_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 4.
 */
#define HP_MEM_APM_REGION4_R0_W    (BIT(1))
#define HP_MEM_APM_REGION4_R0_W_M  (HP_MEM_APM_REGION4_R0_W_V << HP_MEM_APM_REGION4_R0_W_S)
#define HP_MEM_APM_REGION4_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION4_R0_W_S  1
/** HP_MEM_APM_REGION4_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 4.
 */
#define HP_MEM_APM_REGION4_R0_R    (BIT(2))
#define HP_MEM_APM_REGION4_R0_R_M  (HP_MEM_APM_REGION4_R0_R_V << HP_MEM_APM_REGION4_R0_R_S)
#define HP_MEM_APM_REGION4_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION4_R0_R_S  2
/** HP_MEM_APM_REGION4_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 4.
 */
#define HP_MEM_APM_REGION4_R1_X    (BIT(4))
#define HP_MEM_APM_REGION4_R1_X_M  (HP_MEM_APM_REGION4_R1_X_V << HP_MEM_APM_REGION4_R1_X_S)
#define HP_MEM_APM_REGION4_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION4_R1_X_S  4
/** HP_MEM_APM_REGION4_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 4.
 */
#define HP_MEM_APM_REGION4_R1_W    (BIT(5))
#define HP_MEM_APM_REGION4_R1_W_M  (HP_MEM_APM_REGION4_R1_W_V << HP_MEM_APM_REGION4_R1_W_S)
#define HP_MEM_APM_REGION4_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION4_R1_W_S  5
/** HP_MEM_APM_REGION4_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 4.
 */
#define HP_MEM_APM_REGION4_R1_R    (BIT(6))
#define HP_MEM_APM_REGION4_R1_R_M  (HP_MEM_APM_REGION4_R1_R_V << HP_MEM_APM_REGION4_R1_R_S)
#define HP_MEM_APM_REGION4_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION4_R1_R_S  6
/** HP_MEM_APM_REGION4_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 4.
 */
#define HP_MEM_APM_REGION4_R2_X    (BIT(8))
#define HP_MEM_APM_REGION4_R2_X_M  (HP_MEM_APM_REGION4_R2_X_V << HP_MEM_APM_REGION4_R2_X_S)
#define HP_MEM_APM_REGION4_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION4_R2_X_S  8
/** HP_MEM_APM_REGION4_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 4.
 */
#define HP_MEM_APM_REGION4_R2_W    (BIT(9))
#define HP_MEM_APM_REGION4_R2_W_M  (HP_MEM_APM_REGION4_R2_W_V << HP_MEM_APM_REGION4_R2_W_S)
#define HP_MEM_APM_REGION4_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION4_R2_W_S  9
/** HP_MEM_APM_REGION4_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 4.
 */
#define HP_MEM_APM_REGION4_R2_R    (BIT(10))
#define HP_MEM_APM_REGION4_R2_R_M  (HP_MEM_APM_REGION4_R2_R_V << HP_MEM_APM_REGION4_R2_R_S)
#define HP_MEM_APM_REGION4_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION4_R2_R_S  10
/** HP_MEM_APM_REGION4_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 4.
 */
#define HP_MEM_APM_REGION4_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION4_TEE_X_M  (HP_MEM_APM_REGION4_TEE_X_V << HP_MEM_APM_REGION4_TEE_X_S)
#define HP_MEM_APM_REGION4_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION4_TEE_X_S  12
/** HP_MEM_APM_REGION4_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 4.
 */
#define HP_MEM_APM_REGION4_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION4_TEE_W_M  (HP_MEM_APM_REGION4_TEE_W_V << HP_MEM_APM_REGION4_TEE_W_S)
#define HP_MEM_APM_REGION4_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION4_TEE_W_S  13
/** HP_MEM_APM_REGION4_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 4.
 */
#define HP_MEM_APM_REGION4_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION4_TEE_R_M  (HP_MEM_APM_REGION4_TEE_R_V << HP_MEM_APM_REGION4_TEE_R_S)
#define HP_MEM_APM_REGION4_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION4_TEE_R_S  14
/** HP_MEM_APM_REGION4_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION4_LOCK    (BIT(16))
#define HP_MEM_APM_REGION4_LOCK_M  (HP_MEM_APM_REGION4_LOCK_V << HP_MEM_APM_REGION4_LOCK_S)
#define HP_MEM_APM_REGION4_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION4_LOCK_S  16

/** HP_MEM_APM_REGION5_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION5_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x40)
/** HP_MEM_APM_REGION5_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 5.
 */
#define HP_MEM_APM_REGION5_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION5_ADDR_START_L_M  (HP_MEM_APM_REGION5_ADDR_START_L_V << HP_MEM_APM_REGION5_ADDR_START_L_S)
#define HP_MEM_APM_REGION5_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION5_ADDR_START_L_S  0
/** HP_MEM_APM_REGION5_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 5.
 */
#define HP_MEM_APM_REGION5_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION5_ADDR_START_M  (HP_MEM_APM_REGION5_ADDR_START_V << HP_MEM_APM_REGION5_ADDR_START_S)
#define HP_MEM_APM_REGION5_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION5_ADDR_START_S  7
/** HP_MEM_APM_REGION5_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 5.
 */
#define HP_MEM_APM_REGION5_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION5_ADDR_START_H_M  (HP_MEM_APM_REGION5_ADDR_START_H_V << HP_MEM_APM_REGION5_ADDR_START_H_S)
#define HP_MEM_APM_REGION5_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION5_ADDR_START_H_S  19

/** HP_MEM_APM_REGION5_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION5_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x44)
/** HP_MEM_APM_REGION5_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 5.
 */
#define HP_MEM_APM_REGION5_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION5_ADDR_END_L_M  (HP_MEM_APM_REGION5_ADDR_END_L_V << HP_MEM_APM_REGION5_ADDR_END_L_S)
#define HP_MEM_APM_REGION5_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION5_ADDR_END_L_S  0
/** HP_MEM_APM_REGION5_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 5.
 */
#define HP_MEM_APM_REGION5_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION5_ADDR_END_M  (HP_MEM_APM_REGION5_ADDR_END_V << HP_MEM_APM_REGION5_ADDR_END_S)
#define HP_MEM_APM_REGION5_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION5_ADDR_END_S  7
/** HP_MEM_APM_REGION5_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 5.
 */
#define HP_MEM_APM_REGION5_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION5_ADDR_END_H_M  (HP_MEM_APM_REGION5_ADDR_END_H_V << HP_MEM_APM_REGION5_ADDR_END_H_S)
#define HP_MEM_APM_REGION5_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION5_ADDR_END_H_S  19

/** HP_MEM_APM_REGION5_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION5_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0x48)
/** HP_MEM_APM_REGION5_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 5.
 */
#define HP_MEM_APM_REGION5_R0_X    (BIT(0))
#define HP_MEM_APM_REGION5_R0_X_M  (HP_MEM_APM_REGION5_R0_X_V << HP_MEM_APM_REGION5_R0_X_S)
#define HP_MEM_APM_REGION5_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION5_R0_X_S  0
/** HP_MEM_APM_REGION5_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 5.
 */
#define HP_MEM_APM_REGION5_R0_W    (BIT(1))
#define HP_MEM_APM_REGION5_R0_W_M  (HP_MEM_APM_REGION5_R0_W_V << HP_MEM_APM_REGION5_R0_W_S)
#define HP_MEM_APM_REGION5_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION5_R0_W_S  1
/** HP_MEM_APM_REGION5_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 5.
 */
#define HP_MEM_APM_REGION5_R0_R    (BIT(2))
#define HP_MEM_APM_REGION5_R0_R_M  (HP_MEM_APM_REGION5_R0_R_V << HP_MEM_APM_REGION5_R0_R_S)
#define HP_MEM_APM_REGION5_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION5_R0_R_S  2
/** HP_MEM_APM_REGION5_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 5.
 */
#define HP_MEM_APM_REGION5_R1_X    (BIT(4))
#define HP_MEM_APM_REGION5_R1_X_M  (HP_MEM_APM_REGION5_R1_X_V << HP_MEM_APM_REGION5_R1_X_S)
#define HP_MEM_APM_REGION5_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION5_R1_X_S  4
/** HP_MEM_APM_REGION5_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 5.
 */
#define HP_MEM_APM_REGION5_R1_W    (BIT(5))
#define HP_MEM_APM_REGION5_R1_W_M  (HP_MEM_APM_REGION5_R1_W_V << HP_MEM_APM_REGION5_R1_W_S)
#define HP_MEM_APM_REGION5_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION5_R1_W_S  5
/** HP_MEM_APM_REGION5_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 5.
 */
#define HP_MEM_APM_REGION5_R1_R    (BIT(6))
#define HP_MEM_APM_REGION5_R1_R_M  (HP_MEM_APM_REGION5_R1_R_V << HP_MEM_APM_REGION5_R1_R_S)
#define HP_MEM_APM_REGION5_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION5_R1_R_S  6
/** HP_MEM_APM_REGION5_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 5.
 */
#define HP_MEM_APM_REGION5_R2_X    (BIT(8))
#define HP_MEM_APM_REGION5_R2_X_M  (HP_MEM_APM_REGION5_R2_X_V << HP_MEM_APM_REGION5_R2_X_S)
#define HP_MEM_APM_REGION5_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION5_R2_X_S  8
/** HP_MEM_APM_REGION5_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 5.
 */
#define HP_MEM_APM_REGION5_R2_W    (BIT(9))
#define HP_MEM_APM_REGION5_R2_W_M  (HP_MEM_APM_REGION5_R2_W_V << HP_MEM_APM_REGION5_R2_W_S)
#define HP_MEM_APM_REGION5_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION5_R2_W_S  9
/** HP_MEM_APM_REGION5_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 5.
 */
#define HP_MEM_APM_REGION5_R2_R    (BIT(10))
#define HP_MEM_APM_REGION5_R2_R_M  (HP_MEM_APM_REGION5_R2_R_V << HP_MEM_APM_REGION5_R2_R_S)
#define HP_MEM_APM_REGION5_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION5_R2_R_S  10
/** HP_MEM_APM_REGION5_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 5.
 */
#define HP_MEM_APM_REGION5_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION5_TEE_X_M  (HP_MEM_APM_REGION5_TEE_X_V << HP_MEM_APM_REGION5_TEE_X_S)
#define HP_MEM_APM_REGION5_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION5_TEE_X_S  12
/** HP_MEM_APM_REGION5_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 5.
 */
#define HP_MEM_APM_REGION5_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION5_TEE_W_M  (HP_MEM_APM_REGION5_TEE_W_V << HP_MEM_APM_REGION5_TEE_W_S)
#define HP_MEM_APM_REGION5_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION5_TEE_W_S  13
/** HP_MEM_APM_REGION5_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 5.
 */
#define HP_MEM_APM_REGION5_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION5_TEE_R_M  (HP_MEM_APM_REGION5_TEE_R_V << HP_MEM_APM_REGION5_TEE_R_S)
#define HP_MEM_APM_REGION5_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION5_TEE_R_S  14
/** HP_MEM_APM_REGION5_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION5_LOCK    (BIT(16))
#define HP_MEM_APM_REGION5_LOCK_M  (HP_MEM_APM_REGION5_LOCK_V << HP_MEM_APM_REGION5_LOCK_S)
#define HP_MEM_APM_REGION5_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION5_LOCK_S  16

/** HP_MEM_APM_REGION6_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION6_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x4c)
/** HP_MEM_APM_REGION6_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 6.
 */
#define HP_MEM_APM_REGION6_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION6_ADDR_START_L_M  (HP_MEM_APM_REGION6_ADDR_START_L_V << HP_MEM_APM_REGION6_ADDR_START_L_S)
#define HP_MEM_APM_REGION6_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION6_ADDR_START_L_S  0
/** HP_MEM_APM_REGION6_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 6.
 */
#define HP_MEM_APM_REGION6_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION6_ADDR_START_M  (HP_MEM_APM_REGION6_ADDR_START_V << HP_MEM_APM_REGION6_ADDR_START_S)
#define HP_MEM_APM_REGION6_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION6_ADDR_START_S  7
/** HP_MEM_APM_REGION6_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 6.
 */
#define HP_MEM_APM_REGION6_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION6_ADDR_START_H_M  (HP_MEM_APM_REGION6_ADDR_START_H_V << HP_MEM_APM_REGION6_ADDR_START_H_S)
#define HP_MEM_APM_REGION6_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION6_ADDR_START_H_S  19

/** HP_MEM_APM_REGION6_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION6_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x50)
/** HP_MEM_APM_REGION6_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 6.
 */
#define HP_MEM_APM_REGION6_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION6_ADDR_END_L_M  (HP_MEM_APM_REGION6_ADDR_END_L_V << HP_MEM_APM_REGION6_ADDR_END_L_S)
#define HP_MEM_APM_REGION6_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION6_ADDR_END_L_S  0
/** HP_MEM_APM_REGION6_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 6.
 */
#define HP_MEM_APM_REGION6_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION6_ADDR_END_M  (HP_MEM_APM_REGION6_ADDR_END_V << HP_MEM_APM_REGION6_ADDR_END_S)
#define HP_MEM_APM_REGION6_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION6_ADDR_END_S  7
/** HP_MEM_APM_REGION6_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 6.
 */
#define HP_MEM_APM_REGION6_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION6_ADDR_END_H_M  (HP_MEM_APM_REGION6_ADDR_END_H_V << HP_MEM_APM_REGION6_ADDR_END_H_S)
#define HP_MEM_APM_REGION6_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION6_ADDR_END_H_S  19

/** HP_MEM_APM_REGION6_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION6_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0x54)
/** HP_MEM_APM_REGION6_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 6.
 */
#define HP_MEM_APM_REGION6_R0_X    (BIT(0))
#define HP_MEM_APM_REGION6_R0_X_M  (HP_MEM_APM_REGION6_R0_X_V << HP_MEM_APM_REGION6_R0_X_S)
#define HP_MEM_APM_REGION6_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION6_R0_X_S  0
/** HP_MEM_APM_REGION6_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 6.
 */
#define HP_MEM_APM_REGION6_R0_W    (BIT(1))
#define HP_MEM_APM_REGION6_R0_W_M  (HP_MEM_APM_REGION6_R0_W_V << HP_MEM_APM_REGION6_R0_W_S)
#define HP_MEM_APM_REGION6_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION6_R0_W_S  1
/** HP_MEM_APM_REGION6_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 6.
 */
#define HP_MEM_APM_REGION6_R0_R    (BIT(2))
#define HP_MEM_APM_REGION6_R0_R_M  (HP_MEM_APM_REGION6_R0_R_V << HP_MEM_APM_REGION6_R0_R_S)
#define HP_MEM_APM_REGION6_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION6_R0_R_S  2
/** HP_MEM_APM_REGION6_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 6.
 */
#define HP_MEM_APM_REGION6_R1_X    (BIT(4))
#define HP_MEM_APM_REGION6_R1_X_M  (HP_MEM_APM_REGION6_R1_X_V << HP_MEM_APM_REGION6_R1_X_S)
#define HP_MEM_APM_REGION6_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION6_R1_X_S  4
/** HP_MEM_APM_REGION6_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 6.
 */
#define HP_MEM_APM_REGION6_R1_W    (BIT(5))
#define HP_MEM_APM_REGION6_R1_W_M  (HP_MEM_APM_REGION6_R1_W_V << HP_MEM_APM_REGION6_R1_W_S)
#define HP_MEM_APM_REGION6_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION6_R1_W_S  5
/** HP_MEM_APM_REGION6_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 6.
 */
#define HP_MEM_APM_REGION6_R1_R    (BIT(6))
#define HP_MEM_APM_REGION6_R1_R_M  (HP_MEM_APM_REGION6_R1_R_V << HP_MEM_APM_REGION6_R1_R_S)
#define HP_MEM_APM_REGION6_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION6_R1_R_S  6
/** HP_MEM_APM_REGION6_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 6.
 */
#define HP_MEM_APM_REGION6_R2_X    (BIT(8))
#define HP_MEM_APM_REGION6_R2_X_M  (HP_MEM_APM_REGION6_R2_X_V << HP_MEM_APM_REGION6_R2_X_S)
#define HP_MEM_APM_REGION6_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION6_R2_X_S  8
/** HP_MEM_APM_REGION6_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 6.
 */
#define HP_MEM_APM_REGION6_R2_W    (BIT(9))
#define HP_MEM_APM_REGION6_R2_W_M  (HP_MEM_APM_REGION6_R2_W_V << HP_MEM_APM_REGION6_R2_W_S)
#define HP_MEM_APM_REGION6_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION6_R2_W_S  9
/** HP_MEM_APM_REGION6_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 6.
 */
#define HP_MEM_APM_REGION6_R2_R    (BIT(10))
#define HP_MEM_APM_REGION6_R2_R_M  (HP_MEM_APM_REGION6_R2_R_V << HP_MEM_APM_REGION6_R2_R_S)
#define HP_MEM_APM_REGION6_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION6_R2_R_S  10
/** HP_MEM_APM_REGION6_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 6.
 */
#define HP_MEM_APM_REGION6_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION6_TEE_X_M  (HP_MEM_APM_REGION6_TEE_X_V << HP_MEM_APM_REGION6_TEE_X_S)
#define HP_MEM_APM_REGION6_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION6_TEE_X_S  12
/** HP_MEM_APM_REGION6_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 6.
 */
#define HP_MEM_APM_REGION6_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION6_TEE_W_M  (HP_MEM_APM_REGION6_TEE_W_V << HP_MEM_APM_REGION6_TEE_W_S)
#define HP_MEM_APM_REGION6_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION6_TEE_W_S  13
/** HP_MEM_APM_REGION6_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 6.
 */
#define HP_MEM_APM_REGION6_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION6_TEE_R_M  (HP_MEM_APM_REGION6_TEE_R_V << HP_MEM_APM_REGION6_TEE_R_S)
#define HP_MEM_APM_REGION6_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION6_TEE_R_S  14
/** HP_MEM_APM_REGION6_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION6_LOCK    (BIT(16))
#define HP_MEM_APM_REGION6_LOCK_M  (HP_MEM_APM_REGION6_LOCK_V << HP_MEM_APM_REGION6_LOCK_S)
#define HP_MEM_APM_REGION6_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION6_LOCK_S  16

/** HP_MEM_APM_REGION7_ADDR_START_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION7_ADDR_START_REG (DR_REG_HP_MEM_APM_BASE + 0x58)
/** HP_MEM_APM_REGION7_ADDR_START_L : HRO; bitpos: [6:0]; default: 0;
 *  Low 12 bit, start address of region 7.
 */
#define HP_MEM_APM_REGION7_ADDR_START_L    0x0000007FU
#define HP_MEM_APM_REGION7_ADDR_START_L_M  (HP_MEM_APM_REGION7_ADDR_START_L_V << HP_MEM_APM_REGION7_ADDR_START_L_S)
#define HP_MEM_APM_REGION7_ADDR_START_L_V  0x0000007FU
#define HP_MEM_APM_REGION7_ADDR_START_L_S  0
/** HP_MEM_APM_REGION7_ADDR_START : R/W; bitpos: [18:7]; default: 0;
 *  Configures start address of region 7.
 */
#define HP_MEM_APM_REGION7_ADDR_START    0x00000FFFU
#define HP_MEM_APM_REGION7_ADDR_START_M  (HP_MEM_APM_REGION7_ADDR_START_V << HP_MEM_APM_REGION7_ADDR_START_S)
#define HP_MEM_APM_REGION7_ADDR_START_V  0x00000FFFU
#define HP_MEM_APM_REGION7_ADDR_START_S  7
/** HP_MEM_APM_REGION7_ADDR_START_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  start address of region 7.
 */
#define HP_MEM_APM_REGION7_ADDR_START_H    0x00001FFFU
#define HP_MEM_APM_REGION7_ADDR_START_H_M  (HP_MEM_APM_REGION7_ADDR_START_H_V << HP_MEM_APM_REGION7_ADDR_START_H_S)
#define HP_MEM_APM_REGION7_ADDR_START_H_V  0x00001FFFU
#define HP_MEM_APM_REGION7_ADDR_START_H_S  19

/** HP_MEM_APM_REGION7_ADDR_END_REG register
 *  Region address register
 */
#define HP_MEM_APM_REGION7_ADDR_END_REG (DR_REG_HP_MEM_APM_BASE + 0x5c)
/** HP_MEM_APM_REGION7_ADDR_END_L : HRO; bitpos: [6:0]; default: 127;
 *  Low 12 bit, end address of region 7.
 */
#define HP_MEM_APM_REGION7_ADDR_END_L    0x0000007FU
#define HP_MEM_APM_REGION7_ADDR_END_L_M  (HP_MEM_APM_REGION7_ADDR_END_L_V << HP_MEM_APM_REGION7_ADDR_END_L_S)
#define HP_MEM_APM_REGION7_ADDR_END_L_V  0x0000007FU
#define HP_MEM_APM_REGION7_ADDR_END_L_S  0
/** HP_MEM_APM_REGION7_ADDR_END : R/W; bitpos: [18:7]; default: 4095;
 *  Configures end address of region 7.
 */
#define HP_MEM_APM_REGION7_ADDR_END    0x00000FFFU
#define HP_MEM_APM_REGION7_ADDR_END_M  (HP_MEM_APM_REGION7_ADDR_END_V << HP_MEM_APM_REGION7_ADDR_END_S)
#define HP_MEM_APM_REGION7_ADDR_END_V  0x00000FFFU
#define HP_MEM_APM_REGION7_ADDR_END_S  7
/** HP_MEM_APM_REGION7_ADDR_END_H : HRO; bitpos: [31:19]; default: 1504;
 *  High 13 bit,  end address of region 7.
 */
#define HP_MEM_APM_REGION7_ADDR_END_H    0x00001FFFU
#define HP_MEM_APM_REGION7_ADDR_END_H_M  (HP_MEM_APM_REGION7_ADDR_END_H_V << HP_MEM_APM_REGION7_ADDR_END_H_S)
#define HP_MEM_APM_REGION7_ADDR_END_H_V  0x00001FFFU
#define HP_MEM_APM_REGION7_ADDR_END_H_S  19

/** HP_MEM_APM_REGION7_ATTR_REG register
 *  Region access authority attribute register
 */
#define HP_MEM_APM_REGION7_ATTR_REG (DR_REG_HP_MEM_APM_BASE + 0x60)
/** HP_MEM_APM_REGION7_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 7.
 */
#define HP_MEM_APM_REGION7_R0_X    (BIT(0))
#define HP_MEM_APM_REGION7_R0_X_M  (HP_MEM_APM_REGION7_R0_X_V << HP_MEM_APM_REGION7_R0_X_S)
#define HP_MEM_APM_REGION7_R0_X_V  0x00000001U
#define HP_MEM_APM_REGION7_R0_X_S  0
/** HP_MEM_APM_REGION7_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 7.
 */
#define HP_MEM_APM_REGION7_R0_W    (BIT(1))
#define HP_MEM_APM_REGION7_R0_W_M  (HP_MEM_APM_REGION7_R0_W_V << HP_MEM_APM_REGION7_R0_W_S)
#define HP_MEM_APM_REGION7_R0_W_V  0x00000001U
#define HP_MEM_APM_REGION7_R0_W_S  1
/** HP_MEM_APM_REGION7_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 7.
 */
#define HP_MEM_APM_REGION7_R0_R    (BIT(2))
#define HP_MEM_APM_REGION7_R0_R_M  (HP_MEM_APM_REGION7_R0_R_V << HP_MEM_APM_REGION7_R0_R_S)
#define HP_MEM_APM_REGION7_R0_R_V  0x00000001U
#define HP_MEM_APM_REGION7_R0_R_S  2
/** HP_MEM_APM_REGION7_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 7.
 */
#define HP_MEM_APM_REGION7_R1_X    (BIT(4))
#define HP_MEM_APM_REGION7_R1_X_M  (HP_MEM_APM_REGION7_R1_X_V << HP_MEM_APM_REGION7_R1_X_S)
#define HP_MEM_APM_REGION7_R1_X_V  0x00000001U
#define HP_MEM_APM_REGION7_R1_X_S  4
/** HP_MEM_APM_REGION7_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 7.
 */
#define HP_MEM_APM_REGION7_R1_W    (BIT(5))
#define HP_MEM_APM_REGION7_R1_W_M  (HP_MEM_APM_REGION7_R1_W_V << HP_MEM_APM_REGION7_R1_W_S)
#define HP_MEM_APM_REGION7_R1_W_V  0x00000001U
#define HP_MEM_APM_REGION7_R1_W_S  5
/** HP_MEM_APM_REGION7_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 7.
 */
#define HP_MEM_APM_REGION7_R1_R    (BIT(6))
#define HP_MEM_APM_REGION7_R1_R_M  (HP_MEM_APM_REGION7_R1_R_V << HP_MEM_APM_REGION7_R1_R_S)
#define HP_MEM_APM_REGION7_R1_R_V  0x00000001U
#define HP_MEM_APM_REGION7_R1_R_S  6
/** HP_MEM_APM_REGION7_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 7.
 */
#define HP_MEM_APM_REGION7_R2_X    (BIT(8))
#define HP_MEM_APM_REGION7_R2_X_M  (HP_MEM_APM_REGION7_R2_X_V << HP_MEM_APM_REGION7_R2_X_S)
#define HP_MEM_APM_REGION7_R2_X_V  0x00000001U
#define HP_MEM_APM_REGION7_R2_X_S  8
/** HP_MEM_APM_REGION7_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 7.
 */
#define HP_MEM_APM_REGION7_R2_W    (BIT(9))
#define HP_MEM_APM_REGION7_R2_W_M  (HP_MEM_APM_REGION7_R2_W_V << HP_MEM_APM_REGION7_R2_W_S)
#define HP_MEM_APM_REGION7_R2_W_V  0x00000001U
#define HP_MEM_APM_REGION7_R2_W_S  9
/** HP_MEM_APM_REGION7_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 7.
 */
#define HP_MEM_APM_REGION7_R2_R    (BIT(10))
#define HP_MEM_APM_REGION7_R2_R_M  (HP_MEM_APM_REGION7_R2_R_V << HP_MEM_APM_REGION7_R2_R_S)
#define HP_MEM_APM_REGION7_R2_R_V  0x00000001U
#define HP_MEM_APM_REGION7_R2_R_S  10
/** HP_MEM_APM_REGION7_TEE_X : R/W; bitpos: [12]; default: 1;
 *  Configures the execution authority of TEE_MODE in region 7.
 */
#define HP_MEM_APM_REGION7_TEE_X    (BIT(12))
#define HP_MEM_APM_REGION7_TEE_X_M  (HP_MEM_APM_REGION7_TEE_X_V << HP_MEM_APM_REGION7_TEE_X_S)
#define HP_MEM_APM_REGION7_TEE_X_V  0x00000001U
#define HP_MEM_APM_REGION7_TEE_X_S  12
/** HP_MEM_APM_REGION7_TEE_W : R/W; bitpos: [13]; default: 1;
 *  Configures the write authority of TEE_MODE in region 7.
 */
#define HP_MEM_APM_REGION7_TEE_W    (BIT(13))
#define HP_MEM_APM_REGION7_TEE_W_M  (HP_MEM_APM_REGION7_TEE_W_V << HP_MEM_APM_REGION7_TEE_W_S)
#define HP_MEM_APM_REGION7_TEE_W_V  0x00000001U
#define HP_MEM_APM_REGION7_TEE_W_S  13
/** HP_MEM_APM_REGION7_TEE_R : R/W; bitpos: [14]; default: 1;
 *  Configures the read authority of TEE_MODE in region 7.
 */
#define HP_MEM_APM_REGION7_TEE_R    (BIT(14))
#define HP_MEM_APM_REGION7_TEE_R_M  (HP_MEM_APM_REGION7_TEE_R_V << HP_MEM_APM_REGION7_TEE_R_S)
#define HP_MEM_APM_REGION7_TEE_R_V  0x00000001U
#define HP_MEM_APM_REGION7_TEE_R_S  14
/** HP_MEM_APM_REGION7_LOCK : R/W; bitpos: [16]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define HP_MEM_APM_REGION7_LOCK    (BIT(16))
#define HP_MEM_APM_REGION7_LOCK_M  (HP_MEM_APM_REGION7_LOCK_V << HP_MEM_APM_REGION7_LOCK_S)
#define HP_MEM_APM_REGION7_LOCK_V  0x00000001U
#define HP_MEM_APM_REGION7_LOCK_S  16

/** HP_MEM_APM_FUNC_CTRL_REG register
 *  APM function control register
 */
#define HP_MEM_APM_FUNC_CTRL_REG (DR_REG_HP_MEM_APM_BASE + 0xc4)
/** HP_MEM_APM_M0_FUNC_EN : R/W; bitpos: [0]; default: 1;
 *  PMS M0 function enable
 */
#define HP_MEM_APM_M0_FUNC_EN    (BIT(0))
#define HP_MEM_APM_M0_FUNC_EN_M  (HP_MEM_APM_M0_FUNC_EN_V << HP_MEM_APM_M0_FUNC_EN_S)
#define HP_MEM_APM_M0_FUNC_EN_V  0x00000001U
#define HP_MEM_APM_M0_FUNC_EN_S  0
/** HP_MEM_APM_M1_FUNC_EN : R/W; bitpos: [1]; default: 1;
 *  PMS M1 function enable
 */
#define HP_MEM_APM_M1_FUNC_EN    (BIT(1))
#define HP_MEM_APM_M1_FUNC_EN_M  (HP_MEM_APM_M1_FUNC_EN_V << HP_MEM_APM_M1_FUNC_EN_S)
#define HP_MEM_APM_M1_FUNC_EN_V  0x00000001U
#define HP_MEM_APM_M1_FUNC_EN_S  1
/** HP_MEM_APM_M2_FUNC_EN : R/W; bitpos: [2]; default: 1;
 *  PMS M2 function enable
 */
#define HP_MEM_APM_M2_FUNC_EN    (BIT(2))
#define HP_MEM_APM_M2_FUNC_EN_M  (HP_MEM_APM_M2_FUNC_EN_V << HP_MEM_APM_M2_FUNC_EN_S)
#define HP_MEM_APM_M2_FUNC_EN_V  0x00000001U
#define HP_MEM_APM_M2_FUNC_EN_S  2
/** HP_MEM_APM_M3_FUNC_EN : R/W; bitpos: [3]; default: 1;
 *  PMS M3 function enable
 */
#define HP_MEM_APM_M3_FUNC_EN    (BIT(3))
#define HP_MEM_APM_M3_FUNC_EN_M  (HP_MEM_APM_M3_FUNC_EN_V << HP_MEM_APM_M3_FUNC_EN_S)
#define HP_MEM_APM_M3_FUNC_EN_V  0x00000001U
#define HP_MEM_APM_M3_FUNC_EN_S  3
/** HP_MEM_APM_M4_FUNC_EN : R/W; bitpos: [4]; default: 1;
 *  PMS M4 function enable
 */
#define HP_MEM_APM_M4_FUNC_EN    (BIT(4))
#define HP_MEM_APM_M4_FUNC_EN_M  (HP_MEM_APM_M4_FUNC_EN_V << HP_MEM_APM_M4_FUNC_EN_S)
#define HP_MEM_APM_M4_FUNC_EN_V  0x00000001U
#define HP_MEM_APM_M4_FUNC_EN_S  4
/** HP_MEM_APM_M5_FUNC_EN : R/W; bitpos: [5]; default: 1;
 *  PMS M5 function enable
 */
#define HP_MEM_APM_M5_FUNC_EN    (BIT(5))
#define HP_MEM_APM_M5_FUNC_EN_M  (HP_MEM_APM_M5_FUNC_EN_V << HP_MEM_APM_M5_FUNC_EN_S)
#define HP_MEM_APM_M5_FUNC_EN_V  0x00000001U
#define HP_MEM_APM_M5_FUNC_EN_S  5

/** HP_MEM_APM_M0_STATUS_REG register
 *  M0 status register
 */
#define HP_MEM_APM_M0_STATUS_REG (DR_REG_HP_MEM_APM_BASE + 0xc8)
/** HP_MEM_APM_M0_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define HP_MEM_APM_M0_EXCEPTION_STATUS    0x00000003U
#define HP_MEM_APM_M0_EXCEPTION_STATUS_M  (HP_MEM_APM_M0_EXCEPTION_STATUS_V << HP_MEM_APM_M0_EXCEPTION_STATUS_S)
#define HP_MEM_APM_M0_EXCEPTION_STATUS_V  0x00000003U
#define HP_MEM_APM_M0_EXCEPTION_STATUS_S  0

/** HP_MEM_APM_M0_STATUS_CLR_REG register
 *  M0 status clear register
 */
#define HP_MEM_APM_M0_STATUS_CLR_REG (DR_REG_HP_MEM_APM_BASE + 0xcc)
/** HP_MEM_APM_M0_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_MEM_APM_M0_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_MEM_APM_M0_EXCEPTION_STATUS_CLR_M  (HP_MEM_APM_M0_EXCEPTION_STATUS_CLR_V << HP_MEM_APM_M0_EXCEPTION_STATUS_CLR_S)
#define HP_MEM_APM_M0_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_MEM_APM_M0_EXCEPTION_STATUS_CLR_S  0

/** HP_MEM_APM_M0_EXCEPTION_INFO0_REG register
 *  M0 exception_info0 register
 */
#define HP_MEM_APM_M0_EXCEPTION_INFO0_REG (DR_REG_HP_MEM_APM_BASE + 0xd0)
/** HP_MEM_APM_M0_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents exception region.
 */
#define HP_MEM_APM_M0_EXCEPTION_REGION    0x000000FFU
#define HP_MEM_APM_M0_EXCEPTION_REGION_M  (HP_MEM_APM_M0_EXCEPTION_REGION_V << HP_MEM_APM_M0_EXCEPTION_REGION_S)
#define HP_MEM_APM_M0_EXCEPTION_REGION_V  0x000000FFU
#define HP_MEM_APM_M0_EXCEPTION_REGION_S  0
/** HP_MEM_APM_M0_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define HP_MEM_APM_M0_EXCEPTION_MODE    0x00000003U
#define HP_MEM_APM_M0_EXCEPTION_MODE_M  (HP_MEM_APM_M0_EXCEPTION_MODE_V << HP_MEM_APM_M0_EXCEPTION_MODE_S)
#define HP_MEM_APM_M0_EXCEPTION_MODE_V  0x00000003U
#define HP_MEM_APM_M0_EXCEPTION_MODE_S  16
/** HP_MEM_APM_M0_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define HP_MEM_APM_M0_EXCEPTION_ID    0x0000001FU
#define HP_MEM_APM_M0_EXCEPTION_ID_M  (HP_MEM_APM_M0_EXCEPTION_ID_V << HP_MEM_APM_M0_EXCEPTION_ID_S)
#define HP_MEM_APM_M0_EXCEPTION_ID_V  0x0000001FU
#define HP_MEM_APM_M0_EXCEPTION_ID_S  18

/** HP_MEM_APM_M0_EXCEPTION_INFO1_REG register
 *  M0 exception_info1 register
 */
#define HP_MEM_APM_M0_EXCEPTION_INFO1_REG (DR_REG_HP_MEM_APM_BASE + 0xd4)
/** HP_MEM_APM_M0_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define HP_MEM_APM_M0_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_MEM_APM_M0_EXCEPTION_ADDR_M  (HP_MEM_APM_M0_EXCEPTION_ADDR_V << HP_MEM_APM_M0_EXCEPTION_ADDR_S)
#define HP_MEM_APM_M0_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_MEM_APM_M0_EXCEPTION_ADDR_S  0

/** HP_MEM_APM_M1_STATUS_REG register
 *  M1 status register
 */
#define HP_MEM_APM_M1_STATUS_REG (DR_REG_HP_MEM_APM_BASE + 0xd8)
/** HP_MEM_APM_M1_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define HP_MEM_APM_M1_EXCEPTION_STATUS    0x00000003U
#define HP_MEM_APM_M1_EXCEPTION_STATUS_M  (HP_MEM_APM_M1_EXCEPTION_STATUS_V << HP_MEM_APM_M1_EXCEPTION_STATUS_S)
#define HP_MEM_APM_M1_EXCEPTION_STATUS_V  0x00000003U
#define HP_MEM_APM_M1_EXCEPTION_STATUS_S  0

/** HP_MEM_APM_M1_STATUS_CLR_REG register
 *  M1 status clear register
 */
#define HP_MEM_APM_M1_STATUS_CLR_REG (DR_REG_HP_MEM_APM_BASE + 0xdc)
/** HP_MEM_APM_M1_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_MEM_APM_M1_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_MEM_APM_M1_EXCEPTION_STATUS_CLR_M  (HP_MEM_APM_M1_EXCEPTION_STATUS_CLR_V << HP_MEM_APM_M1_EXCEPTION_STATUS_CLR_S)
#define HP_MEM_APM_M1_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_MEM_APM_M1_EXCEPTION_STATUS_CLR_S  0

/** HP_MEM_APM_M1_EXCEPTION_INFO0_REG register
 *  M1 exception_info0 register
 */
#define HP_MEM_APM_M1_EXCEPTION_INFO0_REG (DR_REG_HP_MEM_APM_BASE + 0xe0)
/** HP_MEM_APM_M1_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents exception region.
 */
#define HP_MEM_APM_M1_EXCEPTION_REGION    0x000000FFU
#define HP_MEM_APM_M1_EXCEPTION_REGION_M  (HP_MEM_APM_M1_EXCEPTION_REGION_V << HP_MEM_APM_M1_EXCEPTION_REGION_S)
#define HP_MEM_APM_M1_EXCEPTION_REGION_V  0x000000FFU
#define HP_MEM_APM_M1_EXCEPTION_REGION_S  0
/** HP_MEM_APM_M1_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define HP_MEM_APM_M1_EXCEPTION_MODE    0x00000003U
#define HP_MEM_APM_M1_EXCEPTION_MODE_M  (HP_MEM_APM_M1_EXCEPTION_MODE_V << HP_MEM_APM_M1_EXCEPTION_MODE_S)
#define HP_MEM_APM_M1_EXCEPTION_MODE_V  0x00000003U
#define HP_MEM_APM_M1_EXCEPTION_MODE_S  16
/** HP_MEM_APM_M1_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define HP_MEM_APM_M1_EXCEPTION_ID    0x0000001FU
#define HP_MEM_APM_M1_EXCEPTION_ID_M  (HP_MEM_APM_M1_EXCEPTION_ID_V << HP_MEM_APM_M1_EXCEPTION_ID_S)
#define HP_MEM_APM_M1_EXCEPTION_ID_V  0x0000001FU
#define HP_MEM_APM_M1_EXCEPTION_ID_S  18

/** HP_MEM_APM_M1_EXCEPTION_INFO1_REG register
 *  M1 exception_info1 register
 */
#define HP_MEM_APM_M1_EXCEPTION_INFO1_REG (DR_REG_HP_MEM_APM_BASE + 0xe4)
/** HP_MEM_APM_M1_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define HP_MEM_APM_M1_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_MEM_APM_M1_EXCEPTION_ADDR_M  (HP_MEM_APM_M1_EXCEPTION_ADDR_V << HP_MEM_APM_M1_EXCEPTION_ADDR_S)
#define HP_MEM_APM_M1_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_MEM_APM_M1_EXCEPTION_ADDR_S  0

/** HP_MEM_APM_M2_STATUS_REG register
 *  M2 status register
 */
#define HP_MEM_APM_M2_STATUS_REG (DR_REG_HP_MEM_APM_BASE + 0xe8)
/** HP_MEM_APM_M2_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define HP_MEM_APM_M2_EXCEPTION_STATUS    0x00000003U
#define HP_MEM_APM_M2_EXCEPTION_STATUS_M  (HP_MEM_APM_M2_EXCEPTION_STATUS_V << HP_MEM_APM_M2_EXCEPTION_STATUS_S)
#define HP_MEM_APM_M2_EXCEPTION_STATUS_V  0x00000003U
#define HP_MEM_APM_M2_EXCEPTION_STATUS_S  0

/** HP_MEM_APM_M2_STATUS_CLR_REG register
 *  M2 status clear register
 */
#define HP_MEM_APM_M2_STATUS_CLR_REG (DR_REG_HP_MEM_APM_BASE + 0xec)
/** HP_MEM_APM_M2_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_MEM_APM_M2_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_MEM_APM_M2_EXCEPTION_STATUS_CLR_M  (HP_MEM_APM_M2_EXCEPTION_STATUS_CLR_V << HP_MEM_APM_M2_EXCEPTION_STATUS_CLR_S)
#define HP_MEM_APM_M2_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_MEM_APM_M2_EXCEPTION_STATUS_CLR_S  0

/** HP_MEM_APM_M2_EXCEPTION_INFO0_REG register
 *  M2 exception_info0 register
 */
#define HP_MEM_APM_M2_EXCEPTION_INFO0_REG (DR_REG_HP_MEM_APM_BASE + 0xf0)
/** HP_MEM_APM_M2_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents exception region.
 */
#define HP_MEM_APM_M2_EXCEPTION_REGION    0x000000FFU
#define HP_MEM_APM_M2_EXCEPTION_REGION_M  (HP_MEM_APM_M2_EXCEPTION_REGION_V << HP_MEM_APM_M2_EXCEPTION_REGION_S)
#define HP_MEM_APM_M2_EXCEPTION_REGION_V  0x000000FFU
#define HP_MEM_APM_M2_EXCEPTION_REGION_S  0
/** HP_MEM_APM_M2_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define HP_MEM_APM_M2_EXCEPTION_MODE    0x00000003U
#define HP_MEM_APM_M2_EXCEPTION_MODE_M  (HP_MEM_APM_M2_EXCEPTION_MODE_V << HP_MEM_APM_M2_EXCEPTION_MODE_S)
#define HP_MEM_APM_M2_EXCEPTION_MODE_V  0x00000003U
#define HP_MEM_APM_M2_EXCEPTION_MODE_S  16
/** HP_MEM_APM_M2_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define HP_MEM_APM_M2_EXCEPTION_ID    0x0000001FU
#define HP_MEM_APM_M2_EXCEPTION_ID_M  (HP_MEM_APM_M2_EXCEPTION_ID_V << HP_MEM_APM_M2_EXCEPTION_ID_S)
#define HP_MEM_APM_M2_EXCEPTION_ID_V  0x0000001FU
#define HP_MEM_APM_M2_EXCEPTION_ID_S  18

/** HP_MEM_APM_M2_EXCEPTION_INFO1_REG register
 *  M2 exception_info1 register
 */
#define HP_MEM_APM_M2_EXCEPTION_INFO1_REG (DR_REG_HP_MEM_APM_BASE + 0xf4)
/** HP_MEM_APM_M2_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define HP_MEM_APM_M2_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_MEM_APM_M2_EXCEPTION_ADDR_M  (HP_MEM_APM_M2_EXCEPTION_ADDR_V << HP_MEM_APM_M2_EXCEPTION_ADDR_S)
#define HP_MEM_APM_M2_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_MEM_APM_M2_EXCEPTION_ADDR_S  0

/** HP_MEM_APM_M3_STATUS_REG register
 *  M3 status register
 */
#define HP_MEM_APM_M3_STATUS_REG (DR_REG_HP_MEM_APM_BASE + 0xf8)
/** HP_MEM_APM_M3_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define HP_MEM_APM_M3_EXCEPTION_STATUS    0x00000003U
#define HP_MEM_APM_M3_EXCEPTION_STATUS_M  (HP_MEM_APM_M3_EXCEPTION_STATUS_V << HP_MEM_APM_M3_EXCEPTION_STATUS_S)
#define HP_MEM_APM_M3_EXCEPTION_STATUS_V  0x00000003U
#define HP_MEM_APM_M3_EXCEPTION_STATUS_S  0

/** HP_MEM_APM_M3_STATUS_CLR_REG register
 *  M3 status clear register
 */
#define HP_MEM_APM_M3_STATUS_CLR_REG (DR_REG_HP_MEM_APM_BASE + 0xfc)
/** HP_MEM_APM_M3_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_MEM_APM_M3_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_MEM_APM_M3_EXCEPTION_STATUS_CLR_M  (HP_MEM_APM_M3_EXCEPTION_STATUS_CLR_V << HP_MEM_APM_M3_EXCEPTION_STATUS_CLR_S)
#define HP_MEM_APM_M3_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_MEM_APM_M3_EXCEPTION_STATUS_CLR_S  0

/** HP_MEM_APM_M3_EXCEPTION_INFO0_REG register
 *  M3 exception_info0 register
 */
#define HP_MEM_APM_M3_EXCEPTION_INFO0_REG (DR_REG_HP_MEM_APM_BASE + 0x100)
/** HP_MEM_APM_M3_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents exception region.
 */
#define HP_MEM_APM_M3_EXCEPTION_REGION    0x000000FFU
#define HP_MEM_APM_M3_EXCEPTION_REGION_M  (HP_MEM_APM_M3_EXCEPTION_REGION_V << HP_MEM_APM_M3_EXCEPTION_REGION_S)
#define HP_MEM_APM_M3_EXCEPTION_REGION_V  0x000000FFU
#define HP_MEM_APM_M3_EXCEPTION_REGION_S  0
/** HP_MEM_APM_M3_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define HP_MEM_APM_M3_EXCEPTION_MODE    0x00000003U
#define HP_MEM_APM_M3_EXCEPTION_MODE_M  (HP_MEM_APM_M3_EXCEPTION_MODE_V << HP_MEM_APM_M3_EXCEPTION_MODE_S)
#define HP_MEM_APM_M3_EXCEPTION_MODE_V  0x00000003U
#define HP_MEM_APM_M3_EXCEPTION_MODE_S  16
/** HP_MEM_APM_M3_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define HP_MEM_APM_M3_EXCEPTION_ID    0x0000001FU
#define HP_MEM_APM_M3_EXCEPTION_ID_M  (HP_MEM_APM_M3_EXCEPTION_ID_V << HP_MEM_APM_M3_EXCEPTION_ID_S)
#define HP_MEM_APM_M3_EXCEPTION_ID_V  0x0000001FU
#define HP_MEM_APM_M3_EXCEPTION_ID_S  18

/** HP_MEM_APM_M3_EXCEPTION_INFO1_REG register
 *  M3 exception_info1 register
 */
#define HP_MEM_APM_M3_EXCEPTION_INFO1_REG (DR_REG_HP_MEM_APM_BASE + 0x104)
/** HP_MEM_APM_M3_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define HP_MEM_APM_M3_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_MEM_APM_M3_EXCEPTION_ADDR_M  (HP_MEM_APM_M3_EXCEPTION_ADDR_V << HP_MEM_APM_M3_EXCEPTION_ADDR_S)
#define HP_MEM_APM_M3_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_MEM_APM_M3_EXCEPTION_ADDR_S  0

/** HP_MEM_APM_M4_STATUS_REG register
 *  M4 status register
 */
#define HP_MEM_APM_M4_STATUS_REG (DR_REG_HP_MEM_APM_BASE + 0x108)
/** HP_MEM_APM_M4_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define HP_MEM_APM_M4_EXCEPTION_STATUS    0x00000003U
#define HP_MEM_APM_M4_EXCEPTION_STATUS_M  (HP_MEM_APM_M4_EXCEPTION_STATUS_V << HP_MEM_APM_M4_EXCEPTION_STATUS_S)
#define HP_MEM_APM_M4_EXCEPTION_STATUS_V  0x00000003U
#define HP_MEM_APM_M4_EXCEPTION_STATUS_S  0

/** HP_MEM_APM_M4_STATUS_CLR_REG register
 *  M4 status clear register
 */
#define HP_MEM_APM_M4_STATUS_CLR_REG (DR_REG_HP_MEM_APM_BASE + 0x10c)
/** HP_MEM_APM_M4_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_MEM_APM_M4_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_MEM_APM_M4_EXCEPTION_STATUS_CLR_M  (HP_MEM_APM_M4_EXCEPTION_STATUS_CLR_V << HP_MEM_APM_M4_EXCEPTION_STATUS_CLR_S)
#define HP_MEM_APM_M4_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_MEM_APM_M4_EXCEPTION_STATUS_CLR_S  0

/** HP_MEM_APM_M4_EXCEPTION_INFO0_REG register
 *  M4 exception_info0 register
 */
#define HP_MEM_APM_M4_EXCEPTION_INFO0_REG (DR_REG_HP_MEM_APM_BASE + 0x110)
/** HP_MEM_APM_M4_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents exception region.
 */
#define HP_MEM_APM_M4_EXCEPTION_REGION    0x000000FFU
#define HP_MEM_APM_M4_EXCEPTION_REGION_M  (HP_MEM_APM_M4_EXCEPTION_REGION_V << HP_MEM_APM_M4_EXCEPTION_REGION_S)
#define HP_MEM_APM_M4_EXCEPTION_REGION_V  0x000000FFU
#define HP_MEM_APM_M4_EXCEPTION_REGION_S  0
/** HP_MEM_APM_M4_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define HP_MEM_APM_M4_EXCEPTION_MODE    0x00000003U
#define HP_MEM_APM_M4_EXCEPTION_MODE_M  (HP_MEM_APM_M4_EXCEPTION_MODE_V << HP_MEM_APM_M4_EXCEPTION_MODE_S)
#define HP_MEM_APM_M4_EXCEPTION_MODE_V  0x00000003U
#define HP_MEM_APM_M4_EXCEPTION_MODE_S  16
/** HP_MEM_APM_M4_EXCEPTION_ID : RO; bitpos: [24:18]; default: 0;
 *  Represents exception id information.
 */
#define HP_MEM_APM_M4_EXCEPTION_ID    0x0000007FU
#define HP_MEM_APM_M4_EXCEPTION_ID_M  (HP_MEM_APM_M4_EXCEPTION_ID_V << HP_MEM_APM_M4_EXCEPTION_ID_S)
#define HP_MEM_APM_M4_EXCEPTION_ID_V  0x0000007FU
#define HP_MEM_APM_M4_EXCEPTION_ID_S  18

/** HP_MEM_APM_M4_EXCEPTION_INFO1_REG register
 *  M4 exception_info1 register
 */
#define HP_MEM_APM_M4_EXCEPTION_INFO1_REG (DR_REG_HP_MEM_APM_BASE + 0x114)
/** HP_MEM_APM_M4_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define HP_MEM_APM_M4_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_MEM_APM_M4_EXCEPTION_ADDR_M  (HP_MEM_APM_M4_EXCEPTION_ADDR_V << HP_MEM_APM_M4_EXCEPTION_ADDR_S)
#define HP_MEM_APM_M4_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_MEM_APM_M4_EXCEPTION_ADDR_S  0

/** HP_MEM_APM_M5_STATUS_REG register
 *  M5 status register
 */
#define HP_MEM_APM_M5_STATUS_REG (DR_REG_HP_MEM_APM_BASE + 0x118)
/** HP_MEM_APM_M5_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define HP_MEM_APM_M5_EXCEPTION_STATUS    0x00000003U
#define HP_MEM_APM_M5_EXCEPTION_STATUS_M  (HP_MEM_APM_M5_EXCEPTION_STATUS_V << HP_MEM_APM_M5_EXCEPTION_STATUS_S)
#define HP_MEM_APM_M5_EXCEPTION_STATUS_V  0x00000003U
#define HP_MEM_APM_M5_EXCEPTION_STATUS_S  0

/** HP_MEM_APM_M5_STATUS_CLR_REG register
 *  M5 status clear register
 */
#define HP_MEM_APM_M5_STATUS_CLR_REG (DR_REG_HP_MEM_APM_BASE + 0x11c)
/** HP_MEM_APM_M5_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define HP_MEM_APM_M5_EXCEPTION_STATUS_CLR    (BIT(0))
#define HP_MEM_APM_M5_EXCEPTION_STATUS_CLR_M  (HP_MEM_APM_M5_EXCEPTION_STATUS_CLR_V << HP_MEM_APM_M5_EXCEPTION_STATUS_CLR_S)
#define HP_MEM_APM_M5_EXCEPTION_STATUS_CLR_V  0x00000001U
#define HP_MEM_APM_M5_EXCEPTION_STATUS_CLR_S  0

/** HP_MEM_APM_M5_EXCEPTION_INFO0_REG register
 *  M5 exception_info0 register
 */
#define HP_MEM_APM_M5_EXCEPTION_INFO0_REG (DR_REG_HP_MEM_APM_BASE + 0x120)
/** HP_MEM_APM_M5_EXCEPTION_REGION : RO; bitpos: [7:0]; default: 0;
 *  Represents exception region.
 */
#define HP_MEM_APM_M5_EXCEPTION_REGION    0x000000FFU
#define HP_MEM_APM_M5_EXCEPTION_REGION_M  (HP_MEM_APM_M5_EXCEPTION_REGION_V << HP_MEM_APM_M5_EXCEPTION_REGION_S)
#define HP_MEM_APM_M5_EXCEPTION_REGION_V  0x000000FFU
#define HP_MEM_APM_M5_EXCEPTION_REGION_S  0
/** HP_MEM_APM_M5_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define HP_MEM_APM_M5_EXCEPTION_MODE    0x00000003U
#define HP_MEM_APM_M5_EXCEPTION_MODE_M  (HP_MEM_APM_M5_EXCEPTION_MODE_V << HP_MEM_APM_M5_EXCEPTION_MODE_S)
#define HP_MEM_APM_M5_EXCEPTION_MODE_V  0x00000003U
#define HP_MEM_APM_M5_EXCEPTION_MODE_S  16
/** HP_MEM_APM_M5_EXCEPTION_ID : RO; bitpos: [23:18]; default: 0;
 *  Represents exception id information.
 */
#define HP_MEM_APM_M5_EXCEPTION_ID    0x0000003FU
#define HP_MEM_APM_M5_EXCEPTION_ID_M  (HP_MEM_APM_M5_EXCEPTION_ID_V << HP_MEM_APM_M5_EXCEPTION_ID_S)
#define HP_MEM_APM_M5_EXCEPTION_ID_V  0x0000003FU
#define HP_MEM_APM_M5_EXCEPTION_ID_S  18

/** HP_MEM_APM_M5_EXCEPTION_INFO1_REG register
 *  M5 exception_info1 register
 */
#define HP_MEM_APM_M5_EXCEPTION_INFO1_REG (DR_REG_HP_MEM_APM_BASE + 0x124)
/** HP_MEM_APM_M5_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define HP_MEM_APM_M5_EXCEPTION_ADDR    0xFFFFFFFFU
#define HP_MEM_APM_M5_EXCEPTION_ADDR_M  (HP_MEM_APM_M5_EXCEPTION_ADDR_V << HP_MEM_APM_M5_EXCEPTION_ADDR_S)
#define HP_MEM_APM_M5_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define HP_MEM_APM_M5_EXCEPTION_ADDR_S  0

/** HP_MEM_APM_INT_EN_REG register
 *  APM interrupt enable register
 */
#define HP_MEM_APM_INT_EN_REG (DR_REG_HP_MEM_APM_BASE + 0x128)
/** HP_MEM_APM_M0_APM_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable APM M0 interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_MEM_APM_M0_APM_INT_EN    (BIT(0))
#define HP_MEM_APM_M0_APM_INT_EN_M  (HP_MEM_APM_M0_APM_INT_EN_V << HP_MEM_APM_M0_APM_INT_EN_S)
#define HP_MEM_APM_M0_APM_INT_EN_V  0x00000001U
#define HP_MEM_APM_M0_APM_INT_EN_S  0
/** HP_MEM_APM_M1_APM_INT_EN : R/W; bitpos: [1]; default: 0;
 *  Configures to enable APM M1 interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_MEM_APM_M1_APM_INT_EN    (BIT(1))
#define HP_MEM_APM_M1_APM_INT_EN_M  (HP_MEM_APM_M1_APM_INT_EN_V << HP_MEM_APM_M1_APM_INT_EN_S)
#define HP_MEM_APM_M1_APM_INT_EN_V  0x00000001U
#define HP_MEM_APM_M1_APM_INT_EN_S  1
/** HP_MEM_APM_M2_APM_INT_EN : R/W; bitpos: [2]; default: 0;
 *  Configures to enable APM M2 interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_MEM_APM_M2_APM_INT_EN    (BIT(2))
#define HP_MEM_APM_M2_APM_INT_EN_M  (HP_MEM_APM_M2_APM_INT_EN_V << HP_MEM_APM_M2_APM_INT_EN_S)
#define HP_MEM_APM_M2_APM_INT_EN_V  0x00000001U
#define HP_MEM_APM_M2_APM_INT_EN_S  2
/** HP_MEM_APM_M3_APM_INT_EN : R/W; bitpos: [3]; default: 0;
 *  Configures to enable APM M3 interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_MEM_APM_M3_APM_INT_EN    (BIT(3))
#define HP_MEM_APM_M3_APM_INT_EN_M  (HP_MEM_APM_M3_APM_INT_EN_V << HP_MEM_APM_M3_APM_INT_EN_S)
#define HP_MEM_APM_M3_APM_INT_EN_V  0x00000001U
#define HP_MEM_APM_M3_APM_INT_EN_S  3
/** HP_MEM_APM_M4_APM_INT_EN : R/W; bitpos: [4]; default: 0;
 *  Configures to enable APM M4 interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_MEM_APM_M4_APM_INT_EN    (BIT(4))
#define HP_MEM_APM_M4_APM_INT_EN_M  (HP_MEM_APM_M4_APM_INT_EN_V << HP_MEM_APM_M4_APM_INT_EN_S)
#define HP_MEM_APM_M4_APM_INT_EN_V  0x00000001U
#define HP_MEM_APM_M4_APM_INT_EN_S  4
/** HP_MEM_APM_M5_APM_INT_EN : R/W; bitpos: [5]; default: 0;
 *  Configures to enable APM M5 interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_MEM_APM_M5_APM_INT_EN    (BIT(5))
#define HP_MEM_APM_M5_APM_INT_EN_M  (HP_MEM_APM_M5_APM_INT_EN_V << HP_MEM_APM_M5_APM_INT_EN_S)
#define HP_MEM_APM_M5_APM_INT_EN_V  0x00000001U
#define HP_MEM_APM_M5_APM_INT_EN_S  5

/** HP_MEM_APM_BUS_ERR_CONF_REG register
 *  APM interrupt enable register
 */
#define HP_MEM_APM_BUS_ERR_CONF_REG (DR_REG_HP_MEM_APM_BASE + 0x12c)
/** HP_MEM_APM_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether return error response to cpu when access blocked
 *  0: disable error response
 *  1: enable error response
 */
#define HP_MEM_APM_BUS_ERR_RESP_EN    (BIT(0))
#define HP_MEM_APM_BUS_ERR_RESP_EN_M  (HP_MEM_APM_BUS_ERR_RESP_EN_V << HP_MEM_APM_BUS_ERR_RESP_EN_S)
#define HP_MEM_APM_BUS_ERR_RESP_EN_V  0x00000001U
#define HP_MEM_APM_BUS_ERR_RESP_EN_S  0

/** HP_MEM_APM_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define HP_MEM_APM_CLOCK_GATE_REG (DR_REG_HP_MEM_APM_BASE + 0x3f8)
/** HP_MEM_APM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: enable automatic clock gating
 *  1: keep the clock always on
 */
#define HP_MEM_APM_CLK_EN    (BIT(0))
#define HP_MEM_APM_CLK_EN_M  (HP_MEM_APM_CLK_EN_V << HP_MEM_APM_CLK_EN_S)
#define HP_MEM_APM_CLK_EN_V  0x00000001U
#define HP_MEM_APM_CLK_EN_S  0

/** HP_MEM_APM_DATE_REG register
 *  Version control register
 */
#define HP_MEM_APM_DATE_REG (DR_REG_HP_MEM_APM_BASE + 0x3fc)
/** HP_MEM_APM_DATE : R/W; bitpos: [27:0]; default: 38822496;
 *  Version control register.
 */
#define HP_MEM_APM_DATE    0x0FFFFFFFU
#define HP_MEM_APM_DATE_M  (HP_MEM_APM_DATE_V << HP_MEM_APM_DATE_S)
#define HP_MEM_APM_DATE_V  0x0FFFFFFFU
#define HP_MEM_APM_DATE_S  0

#ifdef __cplusplus
}
#endif
