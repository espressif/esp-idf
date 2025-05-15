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

/** CPU_APM_REGION_FILTER_EN_REG register
 *  Region filter enable register
 */
#define CPU_APM_REGION_FILTER_EN_REG (DR_REG_CPU_BASE + 0x0)
/** CPU_APM_REGION_FILTER_EN : R/W; bitpos: [7:0]; default: 1;
 *  Configure bit $n (0-7) to enable region $n.
 *  0: disable
 *  1: enable
 */
#define CPU_APM_REGION_FILTER_EN    0x000000FFU
#define CPU_APM_REGION_FILTER_EN_M  (CPU_APM_REGION_FILTER_EN_V << CPU_APM_REGION_FILTER_EN_S)
#define CPU_APM_REGION_FILTER_EN_V  0x000000FFU
#define CPU_APM_REGION_FILTER_EN_S  0

/** CPU_APM_REGION0_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION0_ADDR_START_REG (DR_REG_CPU_BASE + 0x4)
/** CPU_APM_REGION0_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 0.
 */
#define CPU_APM_REGION0_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION0_ADDR_START_L_M  (CPU_APM_REGION0_ADDR_START_L_V << CPU_APM_REGION0_ADDR_START_L_S)
#define CPU_APM_REGION0_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION0_ADDR_START_L_S  0
/** CPU_APM_REGION0_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 0.
 */
#define CPU_APM_REGION0_ADDR_START    0x0000007FU
#define CPU_APM_REGION0_ADDR_START_M  (CPU_APM_REGION0_ADDR_START_V << CPU_APM_REGION0_ADDR_START_S)
#define CPU_APM_REGION0_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION0_ADDR_START_S  12
/** CPU_APM_REGION0_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 0.
 */
#define CPU_APM_REGION0_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION0_ADDR_START_H_M  (CPU_APM_REGION0_ADDR_START_H_V << CPU_APM_REGION0_ADDR_START_H_S)
#define CPU_APM_REGION0_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION0_ADDR_START_H_S  19

/** CPU_APM_REGION0_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION0_ADDR_END_REG (DR_REG_CPU_BASE + 0x8)
/** CPU_APM_REGION0_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 0.
 */
#define CPU_APM_REGION0_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION0_ADDR_END_L_M  (CPU_APM_REGION0_ADDR_END_L_V << CPU_APM_REGION0_ADDR_END_L_S)
#define CPU_APM_REGION0_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION0_ADDR_END_L_S  0
/** CPU_APM_REGION0_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 0.
 */
#define CPU_APM_REGION0_ADDR_END    0x0000007FU
#define CPU_APM_REGION0_ADDR_END_M  (CPU_APM_REGION0_ADDR_END_V << CPU_APM_REGION0_ADDR_END_S)
#define CPU_APM_REGION0_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION0_ADDR_END_S  12
/** CPU_APM_REGION0_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 0.
 */
#define CPU_APM_REGION0_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION0_ADDR_END_H_M  (CPU_APM_REGION0_ADDR_END_H_V << CPU_APM_REGION0_ADDR_END_H_S)
#define CPU_APM_REGION0_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION0_ADDR_END_H_S  19

/** CPU_APM_REGION0_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION0_ATTR_REG (DR_REG_CPU_BASE + 0xc)
/** CPU_APM_REGION0_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 0.
 */
#define CPU_APM_REGION0_R0_X    (BIT(0))
#define CPU_APM_REGION0_R0_X_M  (CPU_APM_REGION0_R0_X_V << CPU_APM_REGION0_R0_X_S)
#define CPU_APM_REGION0_R0_X_V  0x00000001U
#define CPU_APM_REGION0_R0_X_S  0
/** CPU_APM_REGION0_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 0.
 */
#define CPU_APM_REGION0_R0_W    (BIT(1))
#define CPU_APM_REGION0_R0_W_M  (CPU_APM_REGION0_R0_W_V << CPU_APM_REGION0_R0_W_S)
#define CPU_APM_REGION0_R0_W_V  0x00000001U
#define CPU_APM_REGION0_R0_W_S  1
/** CPU_APM_REGION0_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 0.
 */
#define CPU_APM_REGION0_R0_R    (BIT(2))
#define CPU_APM_REGION0_R0_R_M  (CPU_APM_REGION0_R0_R_V << CPU_APM_REGION0_R0_R_S)
#define CPU_APM_REGION0_R0_R_V  0x00000001U
#define CPU_APM_REGION0_R0_R_S  2
/** CPU_APM_REGION0_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 0.
 */
#define CPU_APM_REGION0_R1_X    (BIT(4))
#define CPU_APM_REGION0_R1_X_M  (CPU_APM_REGION0_R1_X_V << CPU_APM_REGION0_R1_X_S)
#define CPU_APM_REGION0_R1_X_V  0x00000001U
#define CPU_APM_REGION0_R1_X_S  4
/** CPU_APM_REGION0_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 0.
 */
#define CPU_APM_REGION0_R1_W    (BIT(5))
#define CPU_APM_REGION0_R1_W_M  (CPU_APM_REGION0_R1_W_V << CPU_APM_REGION0_R1_W_S)
#define CPU_APM_REGION0_R1_W_V  0x00000001U
#define CPU_APM_REGION0_R1_W_S  5
/** CPU_APM_REGION0_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 0.
 */
#define CPU_APM_REGION0_R1_R    (BIT(6))
#define CPU_APM_REGION0_R1_R_M  (CPU_APM_REGION0_R1_R_V << CPU_APM_REGION0_R1_R_S)
#define CPU_APM_REGION0_R1_R_V  0x00000001U
#define CPU_APM_REGION0_R1_R_S  6
/** CPU_APM_REGION0_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 0.
 */
#define CPU_APM_REGION0_R2_X    (BIT(8))
#define CPU_APM_REGION0_R2_X_M  (CPU_APM_REGION0_R2_X_V << CPU_APM_REGION0_R2_X_S)
#define CPU_APM_REGION0_R2_X_V  0x00000001U
#define CPU_APM_REGION0_R2_X_S  8
/** CPU_APM_REGION0_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 0.
 */
#define CPU_APM_REGION0_R2_W    (BIT(9))
#define CPU_APM_REGION0_R2_W_M  (CPU_APM_REGION0_R2_W_V << CPU_APM_REGION0_R2_W_S)
#define CPU_APM_REGION0_R2_W_V  0x00000001U
#define CPU_APM_REGION0_R2_W_S  9
/** CPU_APM_REGION0_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 0.
 */
#define CPU_APM_REGION0_R2_R    (BIT(10))
#define CPU_APM_REGION0_R2_R_M  (CPU_APM_REGION0_R2_R_V << CPU_APM_REGION0_R2_R_S)
#define CPU_APM_REGION0_R2_R_V  0x00000001U
#define CPU_APM_REGION0_R2_R_S  10
/** CPU_APM_REGION0_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION0_LOCK    (BIT(11))
#define CPU_APM_REGION0_LOCK_M  (CPU_APM_REGION0_LOCK_V << CPU_APM_REGION0_LOCK_S)
#define CPU_APM_REGION0_LOCK_V  0x00000001U
#define CPU_APM_REGION0_LOCK_S  11

/** CPU_APM_REGION1_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION1_ADDR_START_REG (DR_REG_CPU_BASE + 0x10)
/** CPU_APM_REGION1_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 1.
 */
#define CPU_APM_REGION1_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION1_ADDR_START_L_M  (CPU_APM_REGION1_ADDR_START_L_V << CPU_APM_REGION1_ADDR_START_L_S)
#define CPU_APM_REGION1_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION1_ADDR_START_L_S  0
/** CPU_APM_REGION1_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 1.
 */
#define CPU_APM_REGION1_ADDR_START    0x0000007FU
#define CPU_APM_REGION1_ADDR_START_M  (CPU_APM_REGION1_ADDR_START_V << CPU_APM_REGION1_ADDR_START_S)
#define CPU_APM_REGION1_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION1_ADDR_START_S  12
/** CPU_APM_REGION1_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 1.
 */
#define CPU_APM_REGION1_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION1_ADDR_START_H_M  (CPU_APM_REGION1_ADDR_START_H_V << CPU_APM_REGION1_ADDR_START_H_S)
#define CPU_APM_REGION1_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION1_ADDR_START_H_S  19

/** CPU_APM_REGION1_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION1_ADDR_END_REG (DR_REG_CPU_BASE + 0x14)
/** CPU_APM_REGION1_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 1.
 */
#define CPU_APM_REGION1_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION1_ADDR_END_L_M  (CPU_APM_REGION1_ADDR_END_L_V << CPU_APM_REGION1_ADDR_END_L_S)
#define CPU_APM_REGION1_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION1_ADDR_END_L_S  0
/** CPU_APM_REGION1_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 1.
 */
#define CPU_APM_REGION1_ADDR_END    0x0000007FU
#define CPU_APM_REGION1_ADDR_END_M  (CPU_APM_REGION1_ADDR_END_V << CPU_APM_REGION1_ADDR_END_S)
#define CPU_APM_REGION1_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION1_ADDR_END_S  12
/** CPU_APM_REGION1_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 1.
 */
#define CPU_APM_REGION1_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION1_ADDR_END_H_M  (CPU_APM_REGION1_ADDR_END_H_V << CPU_APM_REGION1_ADDR_END_H_S)
#define CPU_APM_REGION1_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION1_ADDR_END_H_S  19

/** CPU_APM_REGION1_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION1_ATTR_REG (DR_REG_CPU_BASE + 0x18)
/** CPU_APM_REGION1_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 1.
 */
#define CPU_APM_REGION1_R0_X    (BIT(0))
#define CPU_APM_REGION1_R0_X_M  (CPU_APM_REGION1_R0_X_V << CPU_APM_REGION1_R0_X_S)
#define CPU_APM_REGION1_R0_X_V  0x00000001U
#define CPU_APM_REGION1_R0_X_S  0
/** CPU_APM_REGION1_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 1.
 */
#define CPU_APM_REGION1_R0_W    (BIT(1))
#define CPU_APM_REGION1_R0_W_M  (CPU_APM_REGION1_R0_W_V << CPU_APM_REGION1_R0_W_S)
#define CPU_APM_REGION1_R0_W_V  0x00000001U
#define CPU_APM_REGION1_R0_W_S  1
/** CPU_APM_REGION1_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 1.
 */
#define CPU_APM_REGION1_R0_R    (BIT(2))
#define CPU_APM_REGION1_R0_R_M  (CPU_APM_REGION1_R0_R_V << CPU_APM_REGION1_R0_R_S)
#define CPU_APM_REGION1_R0_R_V  0x00000001U
#define CPU_APM_REGION1_R0_R_S  2
/** CPU_APM_REGION1_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 1.
 */
#define CPU_APM_REGION1_R1_X    (BIT(4))
#define CPU_APM_REGION1_R1_X_M  (CPU_APM_REGION1_R1_X_V << CPU_APM_REGION1_R1_X_S)
#define CPU_APM_REGION1_R1_X_V  0x00000001U
#define CPU_APM_REGION1_R1_X_S  4
/** CPU_APM_REGION1_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 1.
 */
#define CPU_APM_REGION1_R1_W    (BIT(5))
#define CPU_APM_REGION1_R1_W_M  (CPU_APM_REGION1_R1_W_V << CPU_APM_REGION1_R1_W_S)
#define CPU_APM_REGION1_R1_W_V  0x00000001U
#define CPU_APM_REGION1_R1_W_S  5
/** CPU_APM_REGION1_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 1.
 */
#define CPU_APM_REGION1_R1_R    (BIT(6))
#define CPU_APM_REGION1_R1_R_M  (CPU_APM_REGION1_R1_R_V << CPU_APM_REGION1_R1_R_S)
#define CPU_APM_REGION1_R1_R_V  0x00000001U
#define CPU_APM_REGION1_R1_R_S  6
/** CPU_APM_REGION1_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 1.
 */
#define CPU_APM_REGION1_R2_X    (BIT(8))
#define CPU_APM_REGION1_R2_X_M  (CPU_APM_REGION1_R2_X_V << CPU_APM_REGION1_R2_X_S)
#define CPU_APM_REGION1_R2_X_V  0x00000001U
#define CPU_APM_REGION1_R2_X_S  8
/** CPU_APM_REGION1_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 1.
 */
#define CPU_APM_REGION1_R2_W    (BIT(9))
#define CPU_APM_REGION1_R2_W_M  (CPU_APM_REGION1_R2_W_V << CPU_APM_REGION1_R2_W_S)
#define CPU_APM_REGION1_R2_W_V  0x00000001U
#define CPU_APM_REGION1_R2_W_S  9
/** CPU_APM_REGION1_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 1.
 */
#define CPU_APM_REGION1_R2_R    (BIT(10))
#define CPU_APM_REGION1_R2_R_M  (CPU_APM_REGION1_R2_R_V << CPU_APM_REGION1_R2_R_S)
#define CPU_APM_REGION1_R2_R_V  0x00000001U
#define CPU_APM_REGION1_R2_R_S  10
/** CPU_APM_REGION1_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION1_LOCK    (BIT(11))
#define CPU_APM_REGION1_LOCK_M  (CPU_APM_REGION1_LOCK_V << CPU_APM_REGION1_LOCK_S)
#define CPU_APM_REGION1_LOCK_V  0x00000001U
#define CPU_APM_REGION1_LOCK_S  11

/** CPU_APM_REGION2_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION2_ADDR_START_REG (DR_REG_CPU_BASE + 0x1c)
/** CPU_APM_REGION2_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 2.
 */
#define CPU_APM_REGION2_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION2_ADDR_START_L_M  (CPU_APM_REGION2_ADDR_START_L_V << CPU_APM_REGION2_ADDR_START_L_S)
#define CPU_APM_REGION2_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION2_ADDR_START_L_S  0
/** CPU_APM_REGION2_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 2.
 */
#define CPU_APM_REGION2_ADDR_START    0x0000007FU
#define CPU_APM_REGION2_ADDR_START_M  (CPU_APM_REGION2_ADDR_START_V << CPU_APM_REGION2_ADDR_START_S)
#define CPU_APM_REGION2_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION2_ADDR_START_S  12
/** CPU_APM_REGION2_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 2.
 */
#define CPU_APM_REGION2_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION2_ADDR_START_H_M  (CPU_APM_REGION2_ADDR_START_H_V << CPU_APM_REGION2_ADDR_START_H_S)
#define CPU_APM_REGION2_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION2_ADDR_START_H_S  19

/** CPU_APM_REGION2_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION2_ADDR_END_REG (DR_REG_CPU_BASE + 0x20)
/** CPU_APM_REGION2_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 2.
 */
#define CPU_APM_REGION2_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION2_ADDR_END_L_M  (CPU_APM_REGION2_ADDR_END_L_V << CPU_APM_REGION2_ADDR_END_L_S)
#define CPU_APM_REGION2_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION2_ADDR_END_L_S  0
/** CPU_APM_REGION2_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 2.
 */
#define CPU_APM_REGION2_ADDR_END    0x0000007FU
#define CPU_APM_REGION2_ADDR_END_M  (CPU_APM_REGION2_ADDR_END_V << CPU_APM_REGION2_ADDR_END_S)
#define CPU_APM_REGION2_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION2_ADDR_END_S  12
/** CPU_APM_REGION2_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 2.
 */
#define CPU_APM_REGION2_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION2_ADDR_END_H_M  (CPU_APM_REGION2_ADDR_END_H_V << CPU_APM_REGION2_ADDR_END_H_S)
#define CPU_APM_REGION2_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION2_ADDR_END_H_S  19

/** CPU_APM_REGION2_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION2_ATTR_REG (DR_REG_CPU_BASE + 0x24)
/** CPU_APM_REGION2_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 2.
 */
#define CPU_APM_REGION2_R0_X    (BIT(0))
#define CPU_APM_REGION2_R0_X_M  (CPU_APM_REGION2_R0_X_V << CPU_APM_REGION2_R0_X_S)
#define CPU_APM_REGION2_R0_X_V  0x00000001U
#define CPU_APM_REGION2_R0_X_S  0
/** CPU_APM_REGION2_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 2.
 */
#define CPU_APM_REGION2_R0_W    (BIT(1))
#define CPU_APM_REGION2_R0_W_M  (CPU_APM_REGION2_R0_W_V << CPU_APM_REGION2_R0_W_S)
#define CPU_APM_REGION2_R0_W_V  0x00000001U
#define CPU_APM_REGION2_R0_W_S  1
/** CPU_APM_REGION2_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 2.
 */
#define CPU_APM_REGION2_R0_R    (BIT(2))
#define CPU_APM_REGION2_R0_R_M  (CPU_APM_REGION2_R0_R_V << CPU_APM_REGION2_R0_R_S)
#define CPU_APM_REGION2_R0_R_V  0x00000001U
#define CPU_APM_REGION2_R0_R_S  2
/** CPU_APM_REGION2_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 2.
 */
#define CPU_APM_REGION2_R1_X    (BIT(4))
#define CPU_APM_REGION2_R1_X_M  (CPU_APM_REGION2_R1_X_V << CPU_APM_REGION2_R1_X_S)
#define CPU_APM_REGION2_R1_X_V  0x00000001U
#define CPU_APM_REGION2_R1_X_S  4
/** CPU_APM_REGION2_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 2.
 */
#define CPU_APM_REGION2_R1_W    (BIT(5))
#define CPU_APM_REGION2_R1_W_M  (CPU_APM_REGION2_R1_W_V << CPU_APM_REGION2_R1_W_S)
#define CPU_APM_REGION2_R1_W_V  0x00000001U
#define CPU_APM_REGION2_R1_W_S  5
/** CPU_APM_REGION2_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 2.
 */
#define CPU_APM_REGION2_R1_R    (BIT(6))
#define CPU_APM_REGION2_R1_R_M  (CPU_APM_REGION2_R1_R_V << CPU_APM_REGION2_R1_R_S)
#define CPU_APM_REGION2_R1_R_V  0x00000001U
#define CPU_APM_REGION2_R1_R_S  6
/** CPU_APM_REGION2_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 2.
 */
#define CPU_APM_REGION2_R2_X    (BIT(8))
#define CPU_APM_REGION2_R2_X_M  (CPU_APM_REGION2_R2_X_V << CPU_APM_REGION2_R2_X_S)
#define CPU_APM_REGION2_R2_X_V  0x00000001U
#define CPU_APM_REGION2_R2_X_S  8
/** CPU_APM_REGION2_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 2.
 */
#define CPU_APM_REGION2_R2_W    (BIT(9))
#define CPU_APM_REGION2_R2_W_M  (CPU_APM_REGION2_R2_W_V << CPU_APM_REGION2_R2_W_S)
#define CPU_APM_REGION2_R2_W_V  0x00000001U
#define CPU_APM_REGION2_R2_W_S  9
/** CPU_APM_REGION2_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 2.
 */
#define CPU_APM_REGION2_R2_R    (BIT(10))
#define CPU_APM_REGION2_R2_R_M  (CPU_APM_REGION2_R2_R_V << CPU_APM_REGION2_R2_R_S)
#define CPU_APM_REGION2_R2_R_V  0x00000001U
#define CPU_APM_REGION2_R2_R_S  10
/** CPU_APM_REGION2_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION2_LOCK    (BIT(11))
#define CPU_APM_REGION2_LOCK_M  (CPU_APM_REGION2_LOCK_V << CPU_APM_REGION2_LOCK_S)
#define CPU_APM_REGION2_LOCK_V  0x00000001U
#define CPU_APM_REGION2_LOCK_S  11

/** CPU_APM_REGION3_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION3_ADDR_START_REG (DR_REG_CPU_BASE + 0x28)
/** CPU_APM_REGION3_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 3.
 */
#define CPU_APM_REGION3_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION3_ADDR_START_L_M  (CPU_APM_REGION3_ADDR_START_L_V << CPU_APM_REGION3_ADDR_START_L_S)
#define CPU_APM_REGION3_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION3_ADDR_START_L_S  0
/** CPU_APM_REGION3_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 3.
 */
#define CPU_APM_REGION3_ADDR_START    0x0000007FU
#define CPU_APM_REGION3_ADDR_START_M  (CPU_APM_REGION3_ADDR_START_V << CPU_APM_REGION3_ADDR_START_S)
#define CPU_APM_REGION3_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION3_ADDR_START_S  12
/** CPU_APM_REGION3_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 3.
 */
#define CPU_APM_REGION3_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION3_ADDR_START_H_M  (CPU_APM_REGION3_ADDR_START_H_V << CPU_APM_REGION3_ADDR_START_H_S)
#define CPU_APM_REGION3_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION3_ADDR_START_H_S  19

/** CPU_APM_REGION3_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION3_ADDR_END_REG (DR_REG_CPU_BASE + 0x2c)
/** CPU_APM_REGION3_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 3.
 */
#define CPU_APM_REGION3_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION3_ADDR_END_L_M  (CPU_APM_REGION3_ADDR_END_L_V << CPU_APM_REGION3_ADDR_END_L_S)
#define CPU_APM_REGION3_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION3_ADDR_END_L_S  0
/** CPU_APM_REGION3_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 3.
 */
#define CPU_APM_REGION3_ADDR_END    0x0000007FU
#define CPU_APM_REGION3_ADDR_END_M  (CPU_APM_REGION3_ADDR_END_V << CPU_APM_REGION3_ADDR_END_S)
#define CPU_APM_REGION3_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION3_ADDR_END_S  12
/** CPU_APM_REGION3_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 3.
 */
#define CPU_APM_REGION3_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION3_ADDR_END_H_M  (CPU_APM_REGION3_ADDR_END_H_V << CPU_APM_REGION3_ADDR_END_H_S)
#define CPU_APM_REGION3_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION3_ADDR_END_H_S  19

/** CPU_APM_REGION3_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION3_ATTR_REG (DR_REG_CPU_BASE + 0x30)
/** CPU_APM_REGION3_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 3.
 */
#define CPU_APM_REGION3_R0_X    (BIT(0))
#define CPU_APM_REGION3_R0_X_M  (CPU_APM_REGION3_R0_X_V << CPU_APM_REGION3_R0_X_S)
#define CPU_APM_REGION3_R0_X_V  0x00000001U
#define CPU_APM_REGION3_R0_X_S  0
/** CPU_APM_REGION3_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 3.
 */
#define CPU_APM_REGION3_R0_W    (BIT(1))
#define CPU_APM_REGION3_R0_W_M  (CPU_APM_REGION3_R0_W_V << CPU_APM_REGION3_R0_W_S)
#define CPU_APM_REGION3_R0_W_V  0x00000001U
#define CPU_APM_REGION3_R0_W_S  1
/** CPU_APM_REGION3_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 3.
 */
#define CPU_APM_REGION3_R0_R    (BIT(2))
#define CPU_APM_REGION3_R0_R_M  (CPU_APM_REGION3_R0_R_V << CPU_APM_REGION3_R0_R_S)
#define CPU_APM_REGION3_R0_R_V  0x00000001U
#define CPU_APM_REGION3_R0_R_S  2
/** CPU_APM_REGION3_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 3.
 */
#define CPU_APM_REGION3_R1_X    (BIT(4))
#define CPU_APM_REGION3_R1_X_M  (CPU_APM_REGION3_R1_X_V << CPU_APM_REGION3_R1_X_S)
#define CPU_APM_REGION3_R1_X_V  0x00000001U
#define CPU_APM_REGION3_R1_X_S  4
/** CPU_APM_REGION3_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 3.
 */
#define CPU_APM_REGION3_R1_W    (BIT(5))
#define CPU_APM_REGION3_R1_W_M  (CPU_APM_REGION3_R1_W_V << CPU_APM_REGION3_R1_W_S)
#define CPU_APM_REGION3_R1_W_V  0x00000001U
#define CPU_APM_REGION3_R1_W_S  5
/** CPU_APM_REGION3_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 3.
 */
#define CPU_APM_REGION3_R1_R    (BIT(6))
#define CPU_APM_REGION3_R1_R_M  (CPU_APM_REGION3_R1_R_V << CPU_APM_REGION3_R1_R_S)
#define CPU_APM_REGION3_R1_R_V  0x00000001U
#define CPU_APM_REGION3_R1_R_S  6
/** CPU_APM_REGION3_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 3.
 */
#define CPU_APM_REGION3_R2_X    (BIT(8))
#define CPU_APM_REGION3_R2_X_M  (CPU_APM_REGION3_R2_X_V << CPU_APM_REGION3_R2_X_S)
#define CPU_APM_REGION3_R2_X_V  0x00000001U
#define CPU_APM_REGION3_R2_X_S  8
/** CPU_APM_REGION3_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 3.
 */
#define CPU_APM_REGION3_R2_W    (BIT(9))
#define CPU_APM_REGION3_R2_W_M  (CPU_APM_REGION3_R2_W_V << CPU_APM_REGION3_R2_W_S)
#define CPU_APM_REGION3_R2_W_V  0x00000001U
#define CPU_APM_REGION3_R2_W_S  9
/** CPU_APM_REGION3_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 3.
 */
#define CPU_APM_REGION3_R2_R    (BIT(10))
#define CPU_APM_REGION3_R2_R_M  (CPU_APM_REGION3_R2_R_V << CPU_APM_REGION3_R2_R_S)
#define CPU_APM_REGION3_R2_R_V  0x00000001U
#define CPU_APM_REGION3_R2_R_S  10
/** CPU_APM_REGION3_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION3_LOCK    (BIT(11))
#define CPU_APM_REGION3_LOCK_M  (CPU_APM_REGION3_LOCK_V << CPU_APM_REGION3_LOCK_S)
#define CPU_APM_REGION3_LOCK_V  0x00000001U
#define CPU_APM_REGION3_LOCK_S  11

/** CPU_APM_REGION4_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION4_ADDR_START_REG (DR_REG_CPU_BASE + 0x34)
/** CPU_APM_REGION4_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 4.
 */
#define CPU_APM_REGION4_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION4_ADDR_START_L_M  (CPU_APM_REGION4_ADDR_START_L_V << CPU_APM_REGION4_ADDR_START_L_S)
#define CPU_APM_REGION4_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION4_ADDR_START_L_S  0
/** CPU_APM_REGION4_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 4.
 */
#define CPU_APM_REGION4_ADDR_START    0x0000007FU
#define CPU_APM_REGION4_ADDR_START_M  (CPU_APM_REGION4_ADDR_START_V << CPU_APM_REGION4_ADDR_START_S)
#define CPU_APM_REGION4_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION4_ADDR_START_S  12
/** CPU_APM_REGION4_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 4.
 */
#define CPU_APM_REGION4_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION4_ADDR_START_H_M  (CPU_APM_REGION4_ADDR_START_H_V << CPU_APM_REGION4_ADDR_START_H_S)
#define CPU_APM_REGION4_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION4_ADDR_START_H_S  19

/** CPU_APM_REGION4_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION4_ADDR_END_REG (DR_REG_CPU_BASE + 0x38)
/** CPU_APM_REGION4_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 4.
 */
#define CPU_APM_REGION4_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION4_ADDR_END_L_M  (CPU_APM_REGION4_ADDR_END_L_V << CPU_APM_REGION4_ADDR_END_L_S)
#define CPU_APM_REGION4_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION4_ADDR_END_L_S  0
/** CPU_APM_REGION4_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 4.
 */
#define CPU_APM_REGION4_ADDR_END    0x0000007FU
#define CPU_APM_REGION4_ADDR_END_M  (CPU_APM_REGION4_ADDR_END_V << CPU_APM_REGION4_ADDR_END_S)
#define CPU_APM_REGION4_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION4_ADDR_END_S  12
/** CPU_APM_REGION4_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 4.
 */
#define CPU_APM_REGION4_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION4_ADDR_END_H_M  (CPU_APM_REGION4_ADDR_END_H_V << CPU_APM_REGION4_ADDR_END_H_S)
#define CPU_APM_REGION4_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION4_ADDR_END_H_S  19

/** CPU_APM_REGION4_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION4_ATTR_REG (DR_REG_CPU_BASE + 0x3c)
/** CPU_APM_REGION4_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 4.
 */
#define CPU_APM_REGION4_R0_X    (BIT(0))
#define CPU_APM_REGION4_R0_X_M  (CPU_APM_REGION4_R0_X_V << CPU_APM_REGION4_R0_X_S)
#define CPU_APM_REGION4_R0_X_V  0x00000001U
#define CPU_APM_REGION4_R0_X_S  0
/** CPU_APM_REGION4_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 4.
 */
#define CPU_APM_REGION4_R0_W    (BIT(1))
#define CPU_APM_REGION4_R0_W_M  (CPU_APM_REGION4_R0_W_V << CPU_APM_REGION4_R0_W_S)
#define CPU_APM_REGION4_R0_W_V  0x00000001U
#define CPU_APM_REGION4_R0_W_S  1
/** CPU_APM_REGION4_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 4.
 */
#define CPU_APM_REGION4_R0_R    (BIT(2))
#define CPU_APM_REGION4_R0_R_M  (CPU_APM_REGION4_R0_R_V << CPU_APM_REGION4_R0_R_S)
#define CPU_APM_REGION4_R0_R_V  0x00000001U
#define CPU_APM_REGION4_R0_R_S  2
/** CPU_APM_REGION4_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 4.
 */
#define CPU_APM_REGION4_R1_X    (BIT(4))
#define CPU_APM_REGION4_R1_X_M  (CPU_APM_REGION4_R1_X_V << CPU_APM_REGION4_R1_X_S)
#define CPU_APM_REGION4_R1_X_V  0x00000001U
#define CPU_APM_REGION4_R1_X_S  4
/** CPU_APM_REGION4_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 4.
 */
#define CPU_APM_REGION4_R1_W    (BIT(5))
#define CPU_APM_REGION4_R1_W_M  (CPU_APM_REGION4_R1_W_V << CPU_APM_REGION4_R1_W_S)
#define CPU_APM_REGION4_R1_W_V  0x00000001U
#define CPU_APM_REGION4_R1_W_S  5
/** CPU_APM_REGION4_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 4.
 */
#define CPU_APM_REGION4_R1_R    (BIT(6))
#define CPU_APM_REGION4_R1_R_M  (CPU_APM_REGION4_R1_R_V << CPU_APM_REGION4_R1_R_S)
#define CPU_APM_REGION4_R1_R_V  0x00000001U
#define CPU_APM_REGION4_R1_R_S  6
/** CPU_APM_REGION4_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 4.
 */
#define CPU_APM_REGION4_R2_X    (BIT(8))
#define CPU_APM_REGION4_R2_X_M  (CPU_APM_REGION4_R2_X_V << CPU_APM_REGION4_R2_X_S)
#define CPU_APM_REGION4_R2_X_V  0x00000001U
#define CPU_APM_REGION4_R2_X_S  8
/** CPU_APM_REGION4_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 4.
 */
#define CPU_APM_REGION4_R2_W    (BIT(9))
#define CPU_APM_REGION4_R2_W_M  (CPU_APM_REGION4_R2_W_V << CPU_APM_REGION4_R2_W_S)
#define CPU_APM_REGION4_R2_W_V  0x00000001U
#define CPU_APM_REGION4_R2_W_S  9
/** CPU_APM_REGION4_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 4.
 */
#define CPU_APM_REGION4_R2_R    (BIT(10))
#define CPU_APM_REGION4_R2_R_M  (CPU_APM_REGION4_R2_R_V << CPU_APM_REGION4_R2_R_S)
#define CPU_APM_REGION4_R2_R_V  0x00000001U
#define CPU_APM_REGION4_R2_R_S  10
/** CPU_APM_REGION4_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION4_LOCK    (BIT(11))
#define CPU_APM_REGION4_LOCK_M  (CPU_APM_REGION4_LOCK_V << CPU_APM_REGION4_LOCK_S)
#define CPU_APM_REGION4_LOCK_V  0x00000001U
#define CPU_APM_REGION4_LOCK_S  11

/** CPU_APM_REGION5_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION5_ADDR_START_REG (DR_REG_CPU_BASE + 0x40)
/** CPU_APM_REGION5_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 5.
 */
#define CPU_APM_REGION5_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION5_ADDR_START_L_M  (CPU_APM_REGION5_ADDR_START_L_V << CPU_APM_REGION5_ADDR_START_L_S)
#define CPU_APM_REGION5_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION5_ADDR_START_L_S  0
/** CPU_APM_REGION5_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 5.
 */
#define CPU_APM_REGION5_ADDR_START    0x0000007FU
#define CPU_APM_REGION5_ADDR_START_M  (CPU_APM_REGION5_ADDR_START_V << CPU_APM_REGION5_ADDR_START_S)
#define CPU_APM_REGION5_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION5_ADDR_START_S  12
/** CPU_APM_REGION5_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 5.
 */
#define CPU_APM_REGION5_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION5_ADDR_START_H_M  (CPU_APM_REGION5_ADDR_START_H_V << CPU_APM_REGION5_ADDR_START_H_S)
#define CPU_APM_REGION5_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION5_ADDR_START_H_S  19

/** CPU_APM_REGION5_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION5_ADDR_END_REG (DR_REG_CPU_BASE + 0x44)
/** CPU_APM_REGION5_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 5.
 */
#define CPU_APM_REGION5_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION5_ADDR_END_L_M  (CPU_APM_REGION5_ADDR_END_L_V << CPU_APM_REGION5_ADDR_END_L_S)
#define CPU_APM_REGION5_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION5_ADDR_END_L_S  0
/** CPU_APM_REGION5_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 5.
 */
#define CPU_APM_REGION5_ADDR_END    0x0000007FU
#define CPU_APM_REGION5_ADDR_END_M  (CPU_APM_REGION5_ADDR_END_V << CPU_APM_REGION5_ADDR_END_S)
#define CPU_APM_REGION5_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION5_ADDR_END_S  12
/** CPU_APM_REGION5_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 5.
 */
#define CPU_APM_REGION5_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION5_ADDR_END_H_M  (CPU_APM_REGION5_ADDR_END_H_V << CPU_APM_REGION5_ADDR_END_H_S)
#define CPU_APM_REGION5_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION5_ADDR_END_H_S  19

/** CPU_APM_REGION5_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION5_ATTR_REG (DR_REG_CPU_BASE + 0x48)
/** CPU_APM_REGION5_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 5.
 */
#define CPU_APM_REGION5_R0_X    (BIT(0))
#define CPU_APM_REGION5_R0_X_M  (CPU_APM_REGION5_R0_X_V << CPU_APM_REGION5_R0_X_S)
#define CPU_APM_REGION5_R0_X_V  0x00000001U
#define CPU_APM_REGION5_R0_X_S  0
/** CPU_APM_REGION5_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 5.
 */
#define CPU_APM_REGION5_R0_W    (BIT(1))
#define CPU_APM_REGION5_R0_W_M  (CPU_APM_REGION5_R0_W_V << CPU_APM_REGION5_R0_W_S)
#define CPU_APM_REGION5_R0_W_V  0x00000001U
#define CPU_APM_REGION5_R0_W_S  1
/** CPU_APM_REGION5_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 5.
 */
#define CPU_APM_REGION5_R0_R    (BIT(2))
#define CPU_APM_REGION5_R0_R_M  (CPU_APM_REGION5_R0_R_V << CPU_APM_REGION5_R0_R_S)
#define CPU_APM_REGION5_R0_R_V  0x00000001U
#define CPU_APM_REGION5_R0_R_S  2
/** CPU_APM_REGION5_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 5.
 */
#define CPU_APM_REGION5_R1_X    (BIT(4))
#define CPU_APM_REGION5_R1_X_M  (CPU_APM_REGION5_R1_X_V << CPU_APM_REGION5_R1_X_S)
#define CPU_APM_REGION5_R1_X_V  0x00000001U
#define CPU_APM_REGION5_R1_X_S  4
/** CPU_APM_REGION5_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 5.
 */
#define CPU_APM_REGION5_R1_W    (BIT(5))
#define CPU_APM_REGION5_R1_W_M  (CPU_APM_REGION5_R1_W_V << CPU_APM_REGION5_R1_W_S)
#define CPU_APM_REGION5_R1_W_V  0x00000001U
#define CPU_APM_REGION5_R1_W_S  5
/** CPU_APM_REGION5_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 5.
 */
#define CPU_APM_REGION5_R1_R    (BIT(6))
#define CPU_APM_REGION5_R1_R_M  (CPU_APM_REGION5_R1_R_V << CPU_APM_REGION5_R1_R_S)
#define CPU_APM_REGION5_R1_R_V  0x00000001U
#define CPU_APM_REGION5_R1_R_S  6
/** CPU_APM_REGION5_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 5.
 */
#define CPU_APM_REGION5_R2_X    (BIT(8))
#define CPU_APM_REGION5_R2_X_M  (CPU_APM_REGION5_R2_X_V << CPU_APM_REGION5_R2_X_S)
#define CPU_APM_REGION5_R2_X_V  0x00000001U
#define CPU_APM_REGION5_R2_X_S  8
/** CPU_APM_REGION5_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 5.
 */
#define CPU_APM_REGION5_R2_W    (BIT(9))
#define CPU_APM_REGION5_R2_W_M  (CPU_APM_REGION5_R2_W_V << CPU_APM_REGION5_R2_W_S)
#define CPU_APM_REGION5_R2_W_V  0x00000001U
#define CPU_APM_REGION5_R2_W_S  9
/** CPU_APM_REGION5_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 5.
 */
#define CPU_APM_REGION5_R2_R    (BIT(10))
#define CPU_APM_REGION5_R2_R_M  (CPU_APM_REGION5_R2_R_V << CPU_APM_REGION5_R2_R_S)
#define CPU_APM_REGION5_R2_R_V  0x00000001U
#define CPU_APM_REGION5_R2_R_S  10
/** CPU_APM_REGION5_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION5_LOCK    (BIT(11))
#define CPU_APM_REGION5_LOCK_M  (CPU_APM_REGION5_LOCK_V << CPU_APM_REGION5_LOCK_S)
#define CPU_APM_REGION5_LOCK_V  0x00000001U
#define CPU_APM_REGION5_LOCK_S  11

/** CPU_APM_REGION6_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION6_ADDR_START_REG (DR_REG_CPU_BASE + 0x4c)
/** CPU_APM_REGION6_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 6.
 */
#define CPU_APM_REGION6_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION6_ADDR_START_L_M  (CPU_APM_REGION6_ADDR_START_L_V << CPU_APM_REGION6_ADDR_START_L_S)
#define CPU_APM_REGION6_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION6_ADDR_START_L_S  0
/** CPU_APM_REGION6_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 6.
 */
#define CPU_APM_REGION6_ADDR_START    0x0000007FU
#define CPU_APM_REGION6_ADDR_START_M  (CPU_APM_REGION6_ADDR_START_V << CPU_APM_REGION6_ADDR_START_S)
#define CPU_APM_REGION6_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION6_ADDR_START_S  12
/** CPU_APM_REGION6_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 6.
 */
#define CPU_APM_REGION6_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION6_ADDR_START_H_M  (CPU_APM_REGION6_ADDR_START_H_V << CPU_APM_REGION6_ADDR_START_H_S)
#define CPU_APM_REGION6_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION6_ADDR_START_H_S  19

/** CPU_APM_REGION6_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION6_ADDR_END_REG (DR_REG_CPU_BASE + 0x50)
/** CPU_APM_REGION6_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 6.
 */
#define CPU_APM_REGION6_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION6_ADDR_END_L_M  (CPU_APM_REGION6_ADDR_END_L_V << CPU_APM_REGION6_ADDR_END_L_S)
#define CPU_APM_REGION6_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION6_ADDR_END_L_S  0
/** CPU_APM_REGION6_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 6.
 */
#define CPU_APM_REGION6_ADDR_END    0x0000007FU
#define CPU_APM_REGION6_ADDR_END_M  (CPU_APM_REGION6_ADDR_END_V << CPU_APM_REGION6_ADDR_END_S)
#define CPU_APM_REGION6_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION6_ADDR_END_S  12
/** CPU_APM_REGION6_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 6.
 */
#define CPU_APM_REGION6_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION6_ADDR_END_H_M  (CPU_APM_REGION6_ADDR_END_H_V << CPU_APM_REGION6_ADDR_END_H_S)
#define CPU_APM_REGION6_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION6_ADDR_END_H_S  19

/** CPU_APM_REGION6_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION6_ATTR_REG (DR_REG_CPU_BASE + 0x54)
/** CPU_APM_REGION6_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 6.
 */
#define CPU_APM_REGION6_R0_X    (BIT(0))
#define CPU_APM_REGION6_R0_X_M  (CPU_APM_REGION6_R0_X_V << CPU_APM_REGION6_R0_X_S)
#define CPU_APM_REGION6_R0_X_V  0x00000001U
#define CPU_APM_REGION6_R0_X_S  0
/** CPU_APM_REGION6_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 6.
 */
#define CPU_APM_REGION6_R0_W    (BIT(1))
#define CPU_APM_REGION6_R0_W_M  (CPU_APM_REGION6_R0_W_V << CPU_APM_REGION6_R0_W_S)
#define CPU_APM_REGION6_R0_W_V  0x00000001U
#define CPU_APM_REGION6_R0_W_S  1
/** CPU_APM_REGION6_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 6.
 */
#define CPU_APM_REGION6_R0_R    (BIT(2))
#define CPU_APM_REGION6_R0_R_M  (CPU_APM_REGION6_R0_R_V << CPU_APM_REGION6_R0_R_S)
#define CPU_APM_REGION6_R0_R_V  0x00000001U
#define CPU_APM_REGION6_R0_R_S  2
/** CPU_APM_REGION6_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 6.
 */
#define CPU_APM_REGION6_R1_X    (BIT(4))
#define CPU_APM_REGION6_R1_X_M  (CPU_APM_REGION6_R1_X_V << CPU_APM_REGION6_R1_X_S)
#define CPU_APM_REGION6_R1_X_V  0x00000001U
#define CPU_APM_REGION6_R1_X_S  4
/** CPU_APM_REGION6_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 6.
 */
#define CPU_APM_REGION6_R1_W    (BIT(5))
#define CPU_APM_REGION6_R1_W_M  (CPU_APM_REGION6_R1_W_V << CPU_APM_REGION6_R1_W_S)
#define CPU_APM_REGION6_R1_W_V  0x00000001U
#define CPU_APM_REGION6_R1_W_S  5
/** CPU_APM_REGION6_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 6.
 */
#define CPU_APM_REGION6_R1_R    (BIT(6))
#define CPU_APM_REGION6_R1_R_M  (CPU_APM_REGION6_R1_R_V << CPU_APM_REGION6_R1_R_S)
#define CPU_APM_REGION6_R1_R_V  0x00000001U
#define CPU_APM_REGION6_R1_R_S  6
/** CPU_APM_REGION6_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 6.
 */
#define CPU_APM_REGION6_R2_X    (BIT(8))
#define CPU_APM_REGION6_R2_X_M  (CPU_APM_REGION6_R2_X_V << CPU_APM_REGION6_R2_X_S)
#define CPU_APM_REGION6_R2_X_V  0x00000001U
#define CPU_APM_REGION6_R2_X_S  8
/** CPU_APM_REGION6_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 6.
 */
#define CPU_APM_REGION6_R2_W    (BIT(9))
#define CPU_APM_REGION6_R2_W_M  (CPU_APM_REGION6_R2_W_V << CPU_APM_REGION6_R2_W_S)
#define CPU_APM_REGION6_R2_W_V  0x00000001U
#define CPU_APM_REGION6_R2_W_S  9
/** CPU_APM_REGION6_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 6.
 */
#define CPU_APM_REGION6_R2_R    (BIT(10))
#define CPU_APM_REGION6_R2_R_M  (CPU_APM_REGION6_R2_R_V << CPU_APM_REGION6_R2_R_S)
#define CPU_APM_REGION6_R2_R_V  0x00000001U
#define CPU_APM_REGION6_R2_R_S  10
/** CPU_APM_REGION6_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION6_LOCK    (BIT(11))
#define CPU_APM_REGION6_LOCK_M  (CPU_APM_REGION6_LOCK_V << CPU_APM_REGION6_LOCK_S)
#define CPU_APM_REGION6_LOCK_V  0x00000001U
#define CPU_APM_REGION6_LOCK_S  11

/** CPU_APM_REGION7_ADDR_START_REG register
 *  Region address register
 */
#define CPU_APM_REGION7_ADDR_START_REG (DR_REG_CPU_BASE + 0x58)
/** CPU_APM_REGION7_ADDR_START_L : HRO; bitpos: [11:0]; default: 0;
 *  Low 12 bit, start address of region 7.
 */
#define CPU_APM_REGION7_ADDR_START_L    0x00000FFFU
#define CPU_APM_REGION7_ADDR_START_L_M  (CPU_APM_REGION7_ADDR_START_L_V << CPU_APM_REGION7_ADDR_START_L_S)
#define CPU_APM_REGION7_ADDR_START_L_V  0x00000FFFU
#define CPU_APM_REGION7_ADDR_START_L_S  0
/** CPU_APM_REGION7_ADDR_START : R/W; bitpos: [18:12]; default: 0;
 *  Configures start address of region 7.
 */
#define CPU_APM_REGION7_ADDR_START    0x0000007FU
#define CPU_APM_REGION7_ADDR_START_M  (CPU_APM_REGION7_ADDR_START_V << CPU_APM_REGION7_ADDR_START_S)
#define CPU_APM_REGION7_ADDR_START_V  0x0000007FU
#define CPU_APM_REGION7_ADDR_START_S  12
/** CPU_APM_REGION7_ADDR_START_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  start address of region 7.
 */
#define CPU_APM_REGION7_ADDR_START_H    0x00001FFFU
#define CPU_APM_REGION7_ADDR_START_H_M  (CPU_APM_REGION7_ADDR_START_H_V << CPU_APM_REGION7_ADDR_START_H_S)
#define CPU_APM_REGION7_ADDR_START_H_V  0x00001FFFU
#define CPU_APM_REGION7_ADDR_START_H_S  19

/** CPU_APM_REGION7_ADDR_END_REG register
 *  Region address register
 */
#define CPU_APM_REGION7_ADDR_END_REG (DR_REG_CPU_BASE + 0x5c)
/** CPU_APM_REGION7_ADDR_END_L : HRO; bitpos: [11:0]; default: 4095;
 *  Low 12 bit, end address of region 7.
 */
#define CPU_APM_REGION7_ADDR_END_L    0x00000FFFU
#define CPU_APM_REGION7_ADDR_END_L_M  (CPU_APM_REGION7_ADDR_END_L_V << CPU_APM_REGION7_ADDR_END_L_S)
#define CPU_APM_REGION7_ADDR_END_L_V  0x00000FFFU
#define CPU_APM_REGION7_ADDR_END_L_S  0
/** CPU_APM_REGION7_ADDR_END : R/W; bitpos: [18:12]; default: 127;
 *  Configures end address of region 7.
 */
#define CPU_APM_REGION7_ADDR_END    0x0000007FU
#define CPU_APM_REGION7_ADDR_END_M  (CPU_APM_REGION7_ADDR_END_V << CPU_APM_REGION7_ADDR_END_S)
#define CPU_APM_REGION7_ADDR_END_V  0x0000007FU
#define CPU_APM_REGION7_ADDR_END_S  12
/** CPU_APM_REGION7_ADDR_END_H : HRO; bitpos: [31:19]; default: 2064;
 *  High 13 bit,  end address of region 7.
 */
#define CPU_APM_REGION7_ADDR_END_H    0x00001FFFU
#define CPU_APM_REGION7_ADDR_END_H_M  (CPU_APM_REGION7_ADDR_END_H_V << CPU_APM_REGION7_ADDR_END_H_S)
#define CPU_APM_REGION7_ADDR_END_H_V  0x00001FFFU
#define CPU_APM_REGION7_ADDR_END_H_S  19

/** CPU_APM_REGION7_ATTR_REG register
 *  Region access authority attribute register
 */
#define CPU_APM_REGION7_ATTR_REG (DR_REG_CPU_BASE + 0x60)
/** CPU_APM_REGION7_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 7.
 */
#define CPU_APM_REGION7_R0_X    (BIT(0))
#define CPU_APM_REGION7_R0_X_M  (CPU_APM_REGION7_R0_X_V << CPU_APM_REGION7_R0_X_S)
#define CPU_APM_REGION7_R0_X_V  0x00000001U
#define CPU_APM_REGION7_R0_X_S  0
/** CPU_APM_REGION7_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 7.
 */
#define CPU_APM_REGION7_R0_W    (BIT(1))
#define CPU_APM_REGION7_R0_W_M  (CPU_APM_REGION7_R0_W_V << CPU_APM_REGION7_R0_W_S)
#define CPU_APM_REGION7_R0_W_V  0x00000001U
#define CPU_APM_REGION7_R0_W_S  1
/** CPU_APM_REGION7_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 7.
 */
#define CPU_APM_REGION7_R0_R    (BIT(2))
#define CPU_APM_REGION7_R0_R_M  (CPU_APM_REGION7_R0_R_V << CPU_APM_REGION7_R0_R_S)
#define CPU_APM_REGION7_R0_R_V  0x00000001U
#define CPU_APM_REGION7_R0_R_S  2
/** CPU_APM_REGION7_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 7.
 */
#define CPU_APM_REGION7_R1_X    (BIT(4))
#define CPU_APM_REGION7_R1_X_M  (CPU_APM_REGION7_R1_X_V << CPU_APM_REGION7_R1_X_S)
#define CPU_APM_REGION7_R1_X_V  0x00000001U
#define CPU_APM_REGION7_R1_X_S  4
/** CPU_APM_REGION7_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 7.
 */
#define CPU_APM_REGION7_R1_W    (BIT(5))
#define CPU_APM_REGION7_R1_W_M  (CPU_APM_REGION7_R1_W_V << CPU_APM_REGION7_R1_W_S)
#define CPU_APM_REGION7_R1_W_V  0x00000001U
#define CPU_APM_REGION7_R1_W_S  5
/** CPU_APM_REGION7_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 7.
 */
#define CPU_APM_REGION7_R1_R    (BIT(6))
#define CPU_APM_REGION7_R1_R_M  (CPU_APM_REGION7_R1_R_V << CPU_APM_REGION7_R1_R_S)
#define CPU_APM_REGION7_R1_R_V  0x00000001U
#define CPU_APM_REGION7_R1_R_S  6
/** CPU_APM_REGION7_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 7.
 */
#define CPU_APM_REGION7_R2_X    (BIT(8))
#define CPU_APM_REGION7_R2_X_M  (CPU_APM_REGION7_R2_X_V << CPU_APM_REGION7_R2_X_S)
#define CPU_APM_REGION7_R2_X_V  0x00000001U
#define CPU_APM_REGION7_R2_X_S  8
/** CPU_APM_REGION7_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 7.
 */
#define CPU_APM_REGION7_R2_W    (BIT(9))
#define CPU_APM_REGION7_R2_W_M  (CPU_APM_REGION7_R2_W_V << CPU_APM_REGION7_R2_W_S)
#define CPU_APM_REGION7_R2_W_V  0x00000001U
#define CPU_APM_REGION7_R2_W_S  9
/** CPU_APM_REGION7_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 7.
 */
#define CPU_APM_REGION7_R2_R    (BIT(10))
#define CPU_APM_REGION7_R2_R_M  (CPU_APM_REGION7_R2_R_V << CPU_APM_REGION7_R2_R_S)
#define CPU_APM_REGION7_R2_R_V  0x00000001U
#define CPU_APM_REGION7_R2_R_S  10
/** CPU_APM_REGION7_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define CPU_APM_REGION7_LOCK    (BIT(11))
#define CPU_APM_REGION7_LOCK_M  (CPU_APM_REGION7_LOCK_V << CPU_APM_REGION7_LOCK_S)
#define CPU_APM_REGION7_LOCK_V  0x00000001U
#define CPU_APM_REGION7_LOCK_S  11

/** CPU_APM_FUNC_CTRL_REG register
 *  APM function control register
 */
#define CPU_APM_FUNC_CTRL_REG (DR_REG_CPU_BASE + 0xc4)
/** CPU_APM_M0_FUNC_EN : R/W; bitpos: [0]; default: 1;
 *  PMS M0 function enable
 */
#define CPU_APM_M0_FUNC_EN    (BIT(0))
#define CPU_APM_M0_FUNC_EN_M  (CPU_APM_M0_FUNC_EN_V << CPU_APM_M0_FUNC_EN_S)
#define CPU_APM_M0_FUNC_EN_V  0x00000001U
#define CPU_APM_M0_FUNC_EN_S  0
/** CPU_APM_M1_FUNC_EN : R/W; bitpos: [1]; default: 1;
 *  PMS M1 function enable
 */
#define CPU_APM_M1_FUNC_EN    (BIT(1))
#define CPU_APM_M1_FUNC_EN_M  (CPU_APM_M1_FUNC_EN_V << CPU_APM_M1_FUNC_EN_S)
#define CPU_APM_M1_FUNC_EN_V  0x00000001U
#define CPU_APM_M1_FUNC_EN_S  1

/** CPU_APM_M0_STATUS_REG register
 *  M0 status register
 */
#define CPU_APM_M0_STATUS_REG (DR_REG_CPU_BASE + 0xc8)
/** CPU_APM_M0_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define CPU_APM_M0_EXCEPTION_STATUS    0x00000003U
#define CPU_APM_M0_EXCEPTION_STATUS_M  (CPU_APM_M0_EXCEPTION_STATUS_V << CPU_APM_M0_EXCEPTION_STATUS_S)
#define CPU_APM_M0_EXCEPTION_STATUS_V  0x00000003U
#define CPU_APM_M0_EXCEPTION_STATUS_S  0

/** CPU_APM_M0_STATUS_CLR_REG register
 *  M0 status clear register
 */
#define CPU_APM_M0_STATUS_CLR_REG (DR_REG_CPU_BASE + 0xcc)
/** CPU_APM_M0_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define CPU_APM_M0_EXCEPTION_STATUS_CLR    (BIT(0))
#define CPU_APM_M0_EXCEPTION_STATUS_CLR_M  (CPU_APM_M0_EXCEPTION_STATUS_CLR_V << CPU_APM_M0_EXCEPTION_STATUS_CLR_S)
#define CPU_APM_M0_EXCEPTION_STATUS_CLR_V  0x00000001U
#define CPU_APM_M0_EXCEPTION_STATUS_CLR_S  0

/** CPU_APM_M0_EXCEPTION_INFO0_REG register
 *  M0 exception_info0 register
 */
#define CPU_APM_M0_EXCEPTION_INFO0_REG (DR_REG_CPU_BASE + 0xd0)
/** CPU_APM_M0_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Represents exception region.
 */
#define CPU_APM_M0_EXCEPTION_REGION    0x0000FFFFU
#define CPU_APM_M0_EXCEPTION_REGION_M  (CPU_APM_M0_EXCEPTION_REGION_V << CPU_APM_M0_EXCEPTION_REGION_S)
#define CPU_APM_M0_EXCEPTION_REGION_V  0x0000FFFFU
#define CPU_APM_M0_EXCEPTION_REGION_S  0
/** CPU_APM_M0_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define CPU_APM_M0_EXCEPTION_MODE    0x00000003U
#define CPU_APM_M0_EXCEPTION_MODE_M  (CPU_APM_M0_EXCEPTION_MODE_V << CPU_APM_M0_EXCEPTION_MODE_S)
#define CPU_APM_M0_EXCEPTION_MODE_V  0x00000003U
#define CPU_APM_M0_EXCEPTION_MODE_S  16
/** CPU_APM_M0_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define CPU_APM_M0_EXCEPTION_ID    0x0000001FU
#define CPU_APM_M0_EXCEPTION_ID_M  (CPU_APM_M0_EXCEPTION_ID_V << CPU_APM_M0_EXCEPTION_ID_S)
#define CPU_APM_M0_EXCEPTION_ID_V  0x0000001FU
#define CPU_APM_M0_EXCEPTION_ID_S  18

/** CPU_APM_M0_EXCEPTION_INFO1_REG register
 *  M0 exception_info1 register
 */
#define CPU_APM_M0_EXCEPTION_INFO1_REG (DR_REG_CPU_BASE + 0xd4)
/** CPU_APM_M0_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define CPU_APM_M0_EXCEPTION_ADDR    0xFFFFFFFFU
#define CPU_APM_M0_EXCEPTION_ADDR_M  (CPU_APM_M0_EXCEPTION_ADDR_V << CPU_APM_M0_EXCEPTION_ADDR_S)
#define CPU_APM_M0_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define CPU_APM_M0_EXCEPTION_ADDR_S  0

/** CPU_APM_M1_STATUS_REG register
 *  M1 status register
 */
#define CPU_APM_M1_STATUS_REG (DR_REG_CPU_BASE + 0xd8)
/** CPU_APM_M1_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.
 *  bit0: 1 represents authority_exception
 *  bit1: 1 represents space_exception
 */
#define CPU_APM_M1_EXCEPTION_STATUS    0x00000003U
#define CPU_APM_M1_EXCEPTION_STATUS_M  (CPU_APM_M1_EXCEPTION_STATUS_V << CPU_APM_M1_EXCEPTION_STATUS_S)
#define CPU_APM_M1_EXCEPTION_STATUS_V  0x00000003U
#define CPU_APM_M1_EXCEPTION_STATUS_S  0

/** CPU_APM_M1_STATUS_CLR_REG register
 *  M1 status clear register
 */
#define CPU_APM_M1_STATUS_CLR_REG (DR_REG_CPU_BASE + 0xdc)
/** CPU_APM_M1_EXCEPTION_STATUS_CLR : WT; bitpos: [0]; default: 0;
 *  Configures to clear exception status.
 */
#define CPU_APM_M1_EXCEPTION_STATUS_CLR    (BIT(0))
#define CPU_APM_M1_EXCEPTION_STATUS_CLR_M  (CPU_APM_M1_EXCEPTION_STATUS_CLR_V << CPU_APM_M1_EXCEPTION_STATUS_CLR_S)
#define CPU_APM_M1_EXCEPTION_STATUS_CLR_V  0x00000001U
#define CPU_APM_M1_EXCEPTION_STATUS_CLR_S  0

/** CPU_APM_M1_EXCEPTION_INFO0_REG register
 *  M1 exception_info0 register
 */
#define CPU_APM_M1_EXCEPTION_INFO0_REG (DR_REG_CPU_BASE + 0xe0)
/** CPU_APM_M1_EXCEPTION_REGION : RO; bitpos: [15:0]; default: 0;
 *  Represents exception region.
 */
#define CPU_APM_M1_EXCEPTION_REGION    0x0000FFFFU
#define CPU_APM_M1_EXCEPTION_REGION_M  (CPU_APM_M1_EXCEPTION_REGION_V << CPU_APM_M1_EXCEPTION_REGION_S)
#define CPU_APM_M1_EXCEPTION_REGION_V  0x0000FFFFU
#define CPU_APM_M1_EXCEPTION_REGION_S  0
/** CPU_APM_M1_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define CPU_APM_M1_EXCEPTION_MODE    0x00000003U
#define CPU_APM_M1_EXCEPTION_MODE_M  (CPU_APM_M1_EXCEPTION_MODE_V << CPU_APM_M1_EXCEPTION_MODE_S)
#define CPU_APM_M1_EXCEPTION_MODE_V  0x00000003U
#define CPU_APM_M1_EXCEPTION_MODE_S  16
/** CPU_APM_M1_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define CPU_APM_M1_EXCEPTION_ID    0x0000001FU
#define CPU_APM_M1_EXCEPTION_ID_M  (CPU_APM_M1_EXCEPTION_ID_V << CPU_APM_M1_EXCEPTION_ID_S)
#define CPU_APM_M1_EXCEPTION_ID_V  0x0000001FU
#define CPU_APM_M1_EXCEPTION_ID_S  18

/** CPU_APM_M1_EXCEPTION_INFO1_REG register
 *  M1 exception_info1 register
 */
#define CPU_APM_M1_EXCEPTION_INFO1_REG (DR_REG_CPU_BASE + 0xe4)
/** CPU_APM_M1_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define CPU_APM_M1_EXCEPTION_ADDR    0xFFFFFFFFU
#define CPU_APM_M1_EXCEPTION_ADDR_M  (CPU_APM_M1_EXCEPTION_ADDR_V << CPU_APM_M1_EXCEPTION_ADDR_S)
#define CPU_APM_M1_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define CPU_APM_M1_EXCEPTION_ADDR_S  0

/** CPU_APM_INT_EN_REG register
 *  APM interrupt enable register
 */
#define CPU_APM_INT_EN_REG (DR_REG_CPU_BASE + 0x118)
/** CPU_APM_M0_APM_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable APM M0 interrupt.
 *  0: disable
 *  1: enable
 */
#define CPU_APM_M0_APM_INT_EN    (BIT(0))
#define CPU_APM_M0_APM_INT_EN_M  (CPU_APM_M0_APM_INT_EN_V << CPU_APM_M0_APM_INT_EN_S)
#define CPU_APM_M0_APM_INT_EN_V  0x00000001U
#define CPU_APM_M0_APM_INT_EN_S  0
/** CPU_APM_M1_APM_INT_EN : R/W; bitpos: [1]; default: 0;
 *  Configures to enable APM M1 interrupt.
 *  0: disable
 *  1: enable
 */
#define CPU_APM_M1_APM_INT_EN    (BIT(1))
#define CPU_APM_M1_APM_INT_EN_M  (CPU_APM_M1_APM_INT_EN_V << CPU_APM_M1_APM_INT_EN_S)
#define CPU_APM_M1_APM_INT_EN_V  0x00000001U
#define CPU_APM_M1_APM_INT_EN_S  1

/** CPU_APM_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define CPU_APM_CLOCK_GATE_REG (DR_REG_CPU_BASE + 0x7f8)
/** CPU_APM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: enable automatic clock gating
 *  1: keep the clock always on
 */
#define CPU_APM_CLK_EN    (BIT(0))
#define CPU_APM_CLK_EN_M  (CPU_APM_CLK_EN_V << CPU_APM_CLK_EN_S)
#define CPU_APM_CLK_EN_V  0x00000001U
#define CPU_APM_CLK_EN_S  0

/** CPU_APM_DATE_REG register
 *  Version control register
 */
#define CPU_APM_DATE_REG (DR_REG_CPU_BASE + 0x7fc)
/** CPU_APM_DATE : R/W; bitpos: [27:0]; default: 37823248;
 *  Version control register.
 */
#define CPU_APM_DATE    0x0FFFFFFFU
#define CPU_APM_DATE_M  (CPU_APM_DATE_V << CPU_APM_DATE_S)
#define CPU_APM_DATE_V  0x0FFFFFFFU
#define CPU_APM_DATE_S  0

#ifdef __cplusplus
}
#endif
