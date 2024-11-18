/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 *  Region filter enable register
 */
#define LP_APM_REGION_FILTER_EN_REG (DR_REG_LP_APM_BASE + 0x0)
/** LP_APM_REGION_FILTER_EN : R/W; bitpos: [3:0]; default: 1;
 *  Configure bit $n (0-3) to enable region $n.\\
 *  0: disable \\
 *  1: enable  \\
 */
#define LP_APM_REGION_FILTER_EN    0x0000000FU
#define LP_APM_REGION_FILTER_EN_M  (LP_APM_REGION_FILTER_EN_V << LP_APM_REGION_FILTER_EN_S)
#define LP_APM_REGION_FILTER_EN_V  0x0000000FU
#define LP_APM_REGION_FILTER_EN_S  0

/** LP_APM_REGION0_ADDR_START_REG register
 *  Region address register
 */
#define LP_APM_REGION0_ADDR_START_REG (DR_REG_LP_APM_BASE + 0x4)
/** LP_APM_REGION0_ADDR_START : R/W; bitpos: [31:0]; default: 0;
 *  Configures start address of region 0.
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
 *  Configures end address of region 0.
 */
#define LP_APM_REGION0_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION0_ADDR_END_M  (LP_APM_REGION0_ADDR_END_V << LP_APM_REGION0_ADDR_END_S)
#define LP_APM_REGION0_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION0_ADDR_END_S  0

/** LP_APM_REGION0_ATTR_REG register
 *  Region access authority attribute register
 */
#define LP_APM_REGION0_ATTR_REG (DR_REG_LP_APM_BASE + 0xc)
/** LP_APM_REGION0_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 0.
 */
#define LP_APM_REGION0_R0_X    (BIT(0))
#define LP_APM_REGION0_R0_X_M  (LP_APM_REGION0_R0_X_V << LP_APM_REGION0_R0_X_S)
#define LP_APM_REGION0_R0_X_V  0x00000001U
#define LP_APM_REGION0_R0_X_S  0
/** LP_APM_REGION0_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 0.
 */
#define LP_APM_REGION0_R0_W    (BIT(1))
#define LP_APM_REGION0_R0_W_M  (LP_APM_REGION0_R0_W_V << LP_APM_REGION0_R0_W_S)
#define LP_APM_REGION0_R0_W_V  0x00000001U
#define LP_APM_REGION0_R0_W_S  1
/** LP_APM_REGION0_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 0.
 */
#define LP_APM_REGION0_R0_R    (BIT(2))
#define LP_APM_REGION0_R0_R_M  (LP_APM_REGION0_R0_R_V << LP_APM_REGION0_R0_R_S)
#define LP_APM_REGION0_R0_R_V  0x00000001U
#define LP_APM_REGION0_R0_R_S  2
/** LP_APM_REGION0_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 0.
 */
#define LP_APM_REGION0_R1_X    (BIT(4))
#define LP_APM_REGION0_R1_X_M  (LP_APM_REGION0_R1_X_V << LP_APM_REGION0_R1_X_S)
#define LP_APM_REGION0_R1_X_V  0x00000001U
#define LP_APM_REGION0_R1_X_S  4
/** LP_APM_REGION0_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 0.
 */
#define LP_APM_REGION0_R1_W    (BIT(5))
#define LP_APM_REGION0_R1_W_M  (LP_APM_REGION0_R1_W_V << LP_APM_REGION0_R1_W_S)
#define LP_APM_REGION0_R1_W_V  0x00000001U
#define LP_APM_REGION0_R1_W_S  5
/** LP_APM_REGION0_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 0.
 */
#define LP_APM_REGION0_R1_R    (BIT(6))
#define LP_APM_REGION0_R1_R_M  (LP_APM_REGION0_R1_R_V << LP_APM_REGION0_R1_R_S)
#define LP_APM_REGION0_R1_R_V  0x00000001U
#define LP_APM_REGION0_R1_R_S  6
/** LP_APM_REGION0_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 0.
 */
#define LP_APM_REGION0_R2_X    (BIT(8))
#define LP_APM_REGION0_R2_X_M  (LP_APM_REGION0_R2_X_V << LP_APM_REGION0_R2_X_S)
#define LP_APM_REGION0_R2_X_V  0x00000001U
#define LP_APM_REGION0_R2_X_S  8
/** LP_APM_REGION0_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 0.
 */
#define LP_APM_REGION0_R2_W    (BIT(9))
#define LP_APM_REGION0_R2_W_M  (LP_APM_REGION0_R2_W_V << LP_APM_REGION0_R2_W_S)
#define LP_APM_REGION0_R2_W_V  0x00000001U
#define LP_APM_REGION0_R2_W_S  9
/** LP_APM_REGION0_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 0.
 */
#define LP_APM_REGION0_R2_R    (BIT(10))
#define LP_APM_REGION0_R2_R_M  (LP_APM_REGION0_R2_R_V << LP_APM_REGION0_R2_R_S)
#define LP_APM_REGION0_R2_R_V  0x00000001U
#define LP_APM_REGION0_R2_R_S  10
/** LP_APM_REGION0_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
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
 *  Configures start address of region 1.
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
 *  Configures end address of region 1.
 */
#define LP_APM_REGION1_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION1_ADDR_END_M  (LP_APM_REGION1_ADDR_END_V << LP_APM_REGION1_ADDR_END_S)
#define LP_APM_REGION1_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION1_ADDR_END_S  0

/** LP_APM_REGION1_ATTR_REG register
 *  Region access authority attribute register
 */
#define LP_APM_REGION1_ATTR_REG (DR_REG_LP_APM_BASE + 0x18)
/** LP_APM_REGION1_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 1.
 */
#define LP_APM_REGION1_R0_X    (BIT(0))
#define LP_APM_REGION1_R0_X_M  (LP_APM_REGION1_R0_X_V << LP_APM_REGION1_R0_X_S)
#define LP_APM_REGION1_R0_X_V  0x00000001U
#define LP_APM_REGION1_R0_X_S  0
/** LP_APM_REGION1_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 1.
 */
#define LP_APM_REGION1_R0_W    (BIT(1))
#define LP_APM_REGION1_R0_W_M  (LP_APM_REGION1_R0_W_V << LP_APM_REGION1_R0_W_S)
#define LP_APM_REGION1_R0_W_V  0x00000001U
#define LP_APM_REGION1_R0_W_S  1
/** LP_APM_REGION1_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 1.
 */
#define LP_APM_REGION1_R0_R    (BIT(2))
#define LP_APM_REGION1_R0_R_M  (LP_APM_REGION1_R0_R_V << LP_APM_REGION1_R0_R_S)
#define LP_APM_REGION1_R0_R_V  0x00000001U
#define LP_APM_REGION1_R0_R_S  2
/** LP_APM_REGION1_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 1.
 */
#define LP_APM_REGION1_R1_X    (BIT(4))
#define LP_APM_REGION1_R1_X_M  (LP_APM_REGION1_R1_X_V << LP_APM_REGION1_R1_X_S)
#define LP_APM_REGION1_R1_X_V  0x00000001U
#define LP_APM_REGION1_R1_X_S  4
/** LP_APM_REGION1_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 1.
 */
#define LP_APM_REGION1_R1_W    (BIT(5))
#define LP_APM_REGION1_R1_W_M  (LP_APM_REGION1_R1_W_V << LP_APM_REGION1_R1_W_S)
#define LP_APM_REGION1_R1_W_V  0x00000001U
#define LP_APM_REGION1_R1_W_S  5
/** LP_APM_REGION1_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 1.
 */
#define LP_APM_REGION1_R1_R    (BIT(6))
#define LP_APM_REGION1_R1_R_M  (LP_APM_REGION1_R1_R_V << LP_APM_REGION1_R1_R_S)
#define LP_APM_REGION1_R1_R_V  0x00000001U
#define LP_APM_REGION1_R1_R_S  6
/** LP_APM_REGION1_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 1.
 */
#define LP_APM_REGION1_R2_X    (BIT(8))
#define LP_APM_REGION1_R2_X_M  (LP_APM_REGION1_R2_X_V << LP_APM_REGION1_R2_X_S)
#define LP_APM_REGION1_R2_X_V  0x00000001U
#define LP_APM_REGION1_R2_X_S  8
/** LP_APM_REGION1_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 1.
 */
#define LP_APM_REGION1_R2_W    (BIT(9))
#define LP_APM_REGION1_R2_W_M  (LP_APM_REGION1_R2_W_V << LP_APM_REGION1_R2_W_S)
#define LP_APM_REGION1_R2_W_V  0x00000001U
#define LP_APM_REGION1_R2_W_S  9
/** LP_APM_REGION1_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 1.
 */
#define LP_APM_REGION1_R2_R    (BIT(10))
#define LP_APM_REGION1_R2_R_M  (LP_APM_REGION1_R2_R_V << LP_APM_REGION1_R2_R_S)
#define LP_APM_REGION1_R2_R_V  0x00000001U
#define LP_APM_REGION1_R2_R_S  10
/** LP_APM_REGION1_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
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
 *  Configures start address of region 2.
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
 *  Configures end address of region 2.
 */
#define LP_APM_REGION2_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION2_ADDR_END_M  (LP_APM_REGION2_ADDR_END_V << LP_APM_REGION2_ADDR_END_S)
#define LP_APM_REGION2_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION2_ADDR_END_S  0

/** LP_APM_REGION2_ATTR_REG register
 *  Region access authority attribute register
 */
#define LP_APM_REGION2_ATTR_REG (DR_REG_LP_APM_BASE + 0x24)
/** LP_APM_REGION2_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 2.
 */
#define LP_APM_REGION2_R0_X    (BIT(0))
#define LP_APM_REGION2_R0_X_M  (LP_APM_REGION2_R0_X_V << LP_APM_REGION2_R0_X_S)
#define LP_APM_REGION2_R0_X_V  0x00000001U
#define LP_APM_REGION2_R0_X_S  0
/** LP_APM_REGION2_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 2.
 */
#define LP_APM_REGION2_R0_W    (BIT(1))
#define LP_APM_REGION2_R0_W_M  (LP_APM_REGION2_R0_W_V << LP_APM_REGION2_R0_W_S)
#define LP_APM_REGION2_R0_W_V  0x00000001U
#define LP_APM_REGION2_R0_W_S  1
/** LP_APM_REGION2_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 2.
 */
#define LP_APM_REGION2_R0_R    (BIT(2))
#define LP_APM_REGION2_R0_R_M  (LP_APM_REGION2_R0_R_V << LP_APM_REGION2_R0_R_S)
#define LP_APM_REGION2_R0_R_V  0x00000001U
#define LP_APM_REGION2_R0_R_S  2
/** LP_APM_REGION2_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 2.
 */
#define LP_APM_REGION2_R1_X    (BIT(4))
#define LP_APM_REGION2_R1_X_M  (LP_APM_REGION2_R1_X_V << LP_APM_REGION2_R1_X_S)
#define LP_APM_REGION2_R1_X_V  0x00000001U
#define LP_APM_REGION2_R1_X_S  4
/** LP_APM_REGION2_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 2.
 */
#define LP_APM_REGION2_R1_W    (BIT(5))
#define LP_APM_REGION2_R1_W_M  (LP_APM_REGION2_R1_W_V << LP_APM_REGION2_R1_W_S)
#define LP_APM_REGION2_R1_W_V  0x00000001U
#define LP_APM_REGION2_R1_W_S  5
/** LP_APM_REGION2_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 2.
 */
#define LP_APM_REGION2_R1_R    (BIT(6))
#define LP_APM_REGION2_R1_R_M  (LP_APM_REGION2_R1_R_V << LP_APM_REGION2_R1_R_S)
#define LP_APM_REGION2_R1_R_V  0x00000001U
#define LP_APM_REGION2_R1_R_S  6
/** LP_APM_REGION2_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 2.
 */
#define LP_APM_REGION2_R2_X    (BIT(8))
#define LP_APM_REGION2_R2_X_M  (LP_APM_REGION2_R2_X_V << LP_APM_REGION2_R2_X_S)
#define LP_APM_REGION2_R2_X_V  0x00000001U
#define LP_APM_REGION2_R2_X_S  8
/** LP_APM_REGION2_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 2.
 */
#define LP_APM_REGION2_R2_W    (BIT(9))
#define LP_APM_REGION2_R2_W_M  (LP_APM_REGION2_R2_W_V << LP_APM_REGION2_R2_W_S)
#define LP_APM_REGION2_R2_W_V  0x00000001U
#define LP_APM_REGION2_R2_W_S  9
/** LP_APM_REGION2_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 2.
 */
#define LP_APM_REGION2_R2_R    (BIT(10))
#define LP_APM_REGION2_R2_R_M  (LP_APM_REGION2_R2_R_V << LP_APM_REGION2_R2_R_S)
#define LP_APM_REGION2_R2_R_V  0x00000001U
#define LP_APM_REGION2_R2_R_S  10
/** LP_APM_REGION2_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
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
 *  Configures start address of region 3.
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
 *  Configures end address of region 3.
 */
#define LP_APM_REGION3_ADDR_END    0xFFFFFFFFU
#define LP_APM_REGION3_ADDR_END_M  (LP_APM_REGION3_ADDR_END_V << LP_APM_REGION3_ADDR_END_S)
#define LP_APM_REGION3_ADDR_END_V  0xFFFFFFFFU
#define LP_APM_REGION3_ADDR_END_S  0

/** LP_APM_REGION3_ATTR_REG register
 *  Region access authority attribute register
 */
#define LP_APM_REGION3_ATTR_REG (DR_REG_LP_APM_BASE + 0x30)
/** LP_APM_REGION3_R0_X : R/W; bitpos: [0]; default: 0;
 *  Configures the execution authority of REE_MODE 0 in region 3.
 */
#define LP_APM_REGION3_R0_X    (BIT(0))
#define LP_APM_REGION3_R0_X_M  (LP_APM_REGION3_R0_X_V << LP_APM_REGION3_R0_X_S)
#define LP_APM_REGION3_R0_X_V  0x00000001U
#define LP_APM_REGION3_R0_X_S  0
/** LP_APM_REGION3_R0_W : R/W; bitpos: [1]; default: 0;
 *  Configures the write authority of REE_MODE 0 in region 3.
 */
#define LP_APM_REGION3_R0_W    (BIT(1))
#define LP_APM_REGION3_R0_W_M  (LP_APM_REGION3_R0_W_V << LP_APM_REGION3_R0_W_S)
#define LP_APM_REGION3_R0_W_V  0x00000001U
#define LP_APM_REGION3_R0_W_S  1
/** LP_APM_REGION3_R0_R : R/W; bitpos: [2]; default: 0;
 *  Configures the read authority of REE_MODE 0 in region 3.
 */
#define LP_APM_REGION3_R0_R    (BIT(2))
#define LP_APM_REGION3_R0_R_M  (LP_APM_REGION3_R0_R_V << LP_APM_REGION3_R0_R_S)
#define LP_APM_REGION3_R0_R_V  0x00000001U
#define LP_APM_REGION3_R0_R_S  2
/** LP_APM_REGION3_R1_X : R/W; bitpos: [4]; default: 0;
 *  Configures the execution authority of REE_MODE 1 in region 3.
 */
#define LP_APM_REGION3_R1_X    (BIT(4))
#define LP_APM_REGION3_R1_X_M  (LP_APM_REGION3_R1_X_V << LP_APM_REGION3_R1_X_S)
#define LP_APM_REGION3_R1_X_V  0x00000001U
#define LP_APM_REGION3_R1_X_S  4
/** LP_APM_REGION3_R1_W : R/W; bitpos: [5]; default: 0;
 *  Configures the write authority of REE_MODE 1 in region 3.
 */
#define LP_APM_REGION3_R1_W    (BIT(5))
#define LP_APM_REGION3_R1_W_M  (LP_APM_REGION3_R1_W_V << LP_APM_REGION3_R1_W_S)
#define LP_APM_REGION3_R1_W_V  0x00000001U
#define LP_APM_REGION3_R1_W_S  5
/** LP_APM_REGION3_R1_R : R/W; bitpos: [6]; default: 0;
 *  Configures the read authority of REE_MODE 1 in region 3.
 */
#define LP_APM_REGION3_R1_R    (BIT(6))
#define LP_APM_REGION3_R1_R_M  (LP_APM_REGION3_R1_R_V << LP_APM_REGION3_R1_R_S)
#define LP_APM_REGION3_R1_R_V  0x00000001U
#define LP_APM_REGION3_R1_R_S  6
/** LP_APM_REGION3_R2_X : R/W; bitpos: [8]; default: 0;
 *  Configures the execution authority of REE_MODE 2 in region 3.
 */
#define LP_APM_REGION3_R2_X    (BIT(8))
#define LP_APM_REGION3_R2_X_M  (LP_APM_REGION3_R2_X_V << LP_APM_REGION3_R2_X_S)
#define LP_APM_REGION3_R2_X_V  0x00000001U
#define LP_APM_REGION3_R2_X_S  8
/** LP_APM_REGION3_R2_W : R/W; bitpos: [9]; default: 0;
 *  Configures the write authority of REE_MODE 2 in region 3.
 */
#define LP_APM_REGION3_R2_W    (BIT(9))
#define LP_APM_REGION3_R2_W_M  (LP_APM_REGION3_R2_W_V << LP_APM_REGION3_R2_W_S)
#define LP_APM_REGION3_R2_W_V  0x00000001U
#define LP_APM_REGION3_R2_W_S  9
/** LP_APM_REGION3_R2_R : R/W; bitpos: [10]; default: 0;
 *  Configures the read authority of REE_MODE 2 in region 3.
 */
#define LP_APM_REGION3_R2_R    (BIT(10))
#define LP_APM_REGION3_R2_R_M  (LP_APM_REGION3_R2_R_V << LP_APM_REGION3_R2_R_S)
#define LP_APM_REGION3_R2_R_V  0x00000001U
#define LP_APM_REGION3_R2_R_S  10
/** LP_APM_REGION3_LOCK : R/W; bitpos: [11]; default: 0;
 *  Set 1 to lock region0 configuration
 */
#define LP_APM_REGION3_LOCK    (BIT(11))
#define LP_APM_REGION3_LOCK_M  (LP_APM_REGION3_LOCK_V << LP_APM_REGION3_LOCK_S)
#define LP_APM_REGION3_LOCK_V  0x00000001U
#define LP_APM_REGION3_LOCK_S  11

/** LP_APM_FUNC_CTRL_REG register
 *  APM function control register
 */
#define LP_APM_FUNC_CTRL_REG (DR_REG_LP_APM_BASE + 0xc4)
/** LP_APM_M0_FUNC_EN : R/W; bitpos: [0]; default: 1;
 *  PMS M0 function enable
 */
#define LP_APM_M0_FUNC_EN    (BIT(0))
#define LP_APM_M0_FUNC_EN_M  (LP_APM_M0_FUNC_EN_V << LP_APM_M0_FUNC_EN_S)
#define LP_APM_M0_FUNC_EN_V  0x00000001U
#define LP_APM_M0_FUNC_EN_S  0
/** LP_APM_M1_FUNC_EN : R/W; bitpos: [1]; default: 1;
 *  PMS M1 function enable
 */
#define LP_APM_M1_FUNC_EN    (BIT(1))
#define LP_APM_M1_FUNC_EN_M  (LP_APM_M1_FUNC_EN_V << LP_APM_M1_FUNC_EN_S)
#define LP_APM_M1_FUNC_EN_V  0x00000001U
#define LP_APM_M1_FUNC_EN_S  1

/** LP_APM_M0_STATUS_REG register
 *  M0 status register
 */
#define LP_APM_M0_STATUS_REG (DR_REG_LP_APM_BASE + 0xc8)
/** LP_APM_M0_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.\\
 *  bit0: 1 represents authority_exception \\
 *  bit1: 1 represents space_exception  \\
 */
#define LP_APM_M0_EXCEPTION_STATUS    0x00000003U
#define LP_APM_M0_EXCEPTION_STATUS_M  (LP_APM_M0_EXCEPTION_STATUS_V << LP_APM_M0_EXCEPTION_STATUS_S)
#define LP_APM_M0_EXCEPTION_STATUS_V  0x00000003U
#define LP_APM_M0_EXCEPTION_STATUS_S  0

/** LP_APM_M0_STATUS_CLR_REG register
 *  M0 status clear register
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
 *  M0 exception_info0 register
 */
#define LP_APM_M0_EXCEPTION_INFO0_REG (DR_REG_LP_APM_BASE + 0xd0)
/** LP_APM_M0_EXCEPTION_REGION : RO; bitpos: [3:0]; default: 0;
 *  Represents exception region.
 */
#define LP_APM_M0_EXCEPTION_REGION    0x0000000FU
#define LP_APM_M0_EXCEPTION_REGION_M  (LP_APM_M0_EXCEPTION_REGION_V << LP_APM_M0_EXCEPTION_REGION_S)
#define LP_APM_M0_EXCEPTION_REGION_V  0x0000000FU
#define LP_APM_M0_EXCEPTION_REGION_S  0
/** LP_APM_M0_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define LP_APM_M0_EXCEPTION_MODE    0x00000003U
#define LP_APM_M0_EXCEPTION_MODE_M  (LP_APM_M0_EXCEPTION_MODE_V << LP_APM_M0_EXCEPTION_MODE_S)
#define LP_APM_M0_EXCEPTION_MODE_V  0x00000003U
#define LP_APM_M0_EXCEPTION_MODE_S  16
/** LP_APM_M0_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define LP_APM_M0_EXCEPTION_ID    0x0000001FU
#define LP_APM_M0_EXCEPTION_ID_M  (LP_APM_M0_EXCEPTION_ID_V << LP_APM_M0_EXCEPTION_ID_S)
#define LP_APM_M0_EXCEPTION_ID_V  0x0000001FU
#define LP_APM_M0_EXCEPTION_ID_S  18

/** LP_APM_M0_EXCEPTION_INFO1_REG register
 *  M0 exception_info1 register
 */
#define LP_APM_M0_EXCEPTION_INFO1_REG (DR_REG_LP_APM_BASE + 0xd4)
/** LP_APM_M0_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define LP_APM_M0_EXCEPTION_ADDR    0xFFFFFFFFU
#define LP_APM_M0_EXCEPTION_ADDR_M  (LP_APM_M0_EXCEPTION_ADDR_V << LP_APM_M0_EXCEPTION_ADDR_S)
#define LP_APM_M0_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define LP_APM_M0_EXCEPTION_ADDR_S  0

/** LP_APM_M1_STATUS_REG register
 *  M1 status register
 */
#define LP_APM_M1_STATUS_REG (DR_REG_LP_APM_BASE + 0xd8)
/** LP_APM_M1_EXCEPTION_STATUS : RO; bitpos: [1:0]; default: 0;
 *  Represents exception status.\\
 *  bit0: 1 represents authority_exception \\
 *  bit1: 1 represents space_exception  \\
 */
#define LP_APM_M1_EXCEPTION_STATUS    0x00000003U
#define LP_APM_M1_EXCEPTION_STATUS_M  (LP_APM_M1_EXCEPTION_STATUS_V << LP_APM_M1_EXCEPTION_STATUS_S)
#define LP_APM_M1_EXCEPTION_STATUS_V  0x00000003U
#define LP_APM_M1_EXCEPTION_STATUS_S  0

/** LP_APM_M1_STATUS_CLR_REG register
 *  M1 status clear register
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
 *  M1 exception_info0 register
 */
#define LP_APM_M1_EXCEPTION_INFO0_REG (DR_REG_LP_APM_BASE + 0xe0)
/** LP_APM_M1_EXCEPTION_REGION : RO; bitpos: [3:0]; default: 0;
 *  Represents exception region.
 */
#define LP_APM_M1_EXCEPTION_REGION    0x0000000FU
#define LP_APM_M1_EXCEPTION_REGION_M  (LP_APM_M1_EXCEPTION_REGION_V << LP_APM_M1_EXCEPTION_REGION_S)
#define LP_APM_M1_EXCEPTION_REGION_V  0x0000000FU
#define LP_APM_M1_EXCEPTION_REGION_S  0
/** LP_APM_M1_EXCEPTION_MODE : RO; bitpos: [17:16]; default: 0;
 *  Represents exception mode.
 */
#define LP_APM_M1_EXCEPTION_MODE    0x00000003U
#define LP_APM_M1_EXCEPTION_MODE_M  (LP_APM_M1_EXCEPTION_MODE_V << LP_APM_M1_EXCEPTION_MODE_S)
#define LP_APM_M1_EXCEPTION_MODE_V  0x00000003U
#define LP_APM_M1_EXCEPTION_MODE_S  16
/** LP_APM_M1_EXCEPTION_ID : RO; bitpos: [22:18]; default: 0;
 *  Represents exception id information.
 */
#define LP_APM_M1_EXCEPTION_ID    0x0000001FU
#define LP_APM_M1_EXCEPTION_ID_M  (LP_APM_M1_EXCEPTION_ID_V << LP_APM_M1_EXCEPTION_ID_S)
#define LP_APM_M1_EXCEPTION_ID_V  0x0000001FU
#define LP_APM_M1_EXCEPTION_ID_S  18

/** LP_APM_M1_EXCEPTION_INFO1_REG register
 *  M1 exception_info1 register
 */
#define LP_APM_M1_EXCEPTION_INFO1_REG (DR_REG_LP_APM_BASE + 0xe4)
/** LP_APM_M1_EXCEPTION_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents exception addr.
 */
#define LP_APM_M1_EXCEPTION_ADDR    0xFFFFFFFFU
#define LP_APM_M1_EXCEPTION_ADDR_M  (LP_APM_M1_EXCEPTION_ADDR_V << LP_APM_M1_EXCEPTION_ADDR_S)
#define LP_APM_M1_EXCEPTION_ADDR_V  0xFFFFFFFFU
#define LP_APM_M1_EXCEPTION_ADDR_S  0

/** LP_APM_INT_EN_REG register
 *  APM interrupt enable register
 */
#define LP_APM_INT_EN_REG (DR_REG_LP_APM_BASE + 0xe8)
/** LP_APM_M0_APM_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable APM M0 interrupt.\\
 *  0: disable \\
 *  1: enable  \\
 */
#define LP_APM_M0_APM_INT_EN    (BIT(0))
#define LP_APM_M0_APM_INT_EN_M  (LP_APM_M0_APM_INT_EN_V << LP_APM_M0_APM_INT_EN_S)
#define LP_APM_M0_APM_INT_EN_V  0x00000001U
#define LP_APM_M0_APM_INT_EN_S  0
/** LP_APM_M1_APM_INT_EN : R/W; bitpos: [1]; default: 0;
 *  Configures to enable APM M1 interrupt.\\
 *  0: disable \\
 *  1: enable  \\
 */
#define LP_APM_M1_APM_INT_EN    (BIT(1))
#define LP_APM_M1_APM_INT_EN_M  (LP_APM_M1_APM_INT_EN_V << LP_APM_M1_APM_INT_EN_S)
#define LP_APM_M1_APM_INT_EN_V  0x00000001U
#define LP_APM_M1_APM_INT_EN_S  1

/** LP_APM_CLOCK_GATE_REG register
 *  clock gating register
 */
#define LP_APM_CLOCK_GATE_REG (DR_REG_LP_APM_BASE + 0xec)
/** LP_APM_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.\\
 *  0: enable automatic clock gating \\
 *  1: keep the clock always on  \\
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
