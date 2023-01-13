/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** TEE_M0_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M0_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x0)
/** TEE_M0_MODE : R/W; bitpos: [1:0]; default: 0;
 *  M0 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M0_MODE    0x00000003U
#define TEE_M0_MODE_M  (TEE_M0_MODE_V << TEE_M0_MODE_S)
#define TEE_M0_MODE_V  0x00000003U
#define TEE_M0_MODE_S  0

/** TEE_M1_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M1_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x4)
/** TEE_M1_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M1 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M1_MODE    0x00000003U
#define TEE_M1_MODE_M  (TEE_M1_MODE_V << TEE_M1_MODE_S)
#define TEE_M1_MODE_V  0x00000003U
#define TEE_M1_MODE_S  0

/** TEE_M2_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M2_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x8)
/** TEE_M2_MODE : R/W; bitpos: [1:0]; default: 0;
 *  M2 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M2_MODE    0x00000003U
#define TEE_M2_MODE_M  (TEE_M2_MODE_V << TEE_M2_MODE_S)
#define TEE_M2_MODE_V  0x00000003U
#define TEE_M2_MODE_S  0

/** TEE_M3_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M3_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xc)
/** TEE_M3_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M3 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M3_MODE    0x00000003U
#define TEE_M3_MODE_M  (TEE_M3_MODE_V << TEE_M3_MODE_S)
#define TEE_M3_MODE_V  0x00000003U
#define TEE_M3_MODE_S  0

/** TEE_M4_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M4_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x10)
/** TEE_M4_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M4 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M4_MODE    0x00000003U
#define TEE_M4_MODE_M  (TEE_M4_MODE_V << TEE_M4_MODE_S)
#define TEE_M4_MODE_V  0x00000003U
#define TEE_M4_MODE_S  0

/** TEE_M5_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M5_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x14)
/** TEE_M5_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M5 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M5_MODE    0x00000003U
#define TEE_M5_MODE_M  (TEE_M5_MODE_V << TEE_M5_MODE_S)
#define TEE_M5_MODE_V  0x00000003U
#define TEE_M5_MODE_S  0

/** TEE_M6_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M6_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x18)
/** TEE_M6_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M6 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M6_MODE    0x00000003U
#define TEE_M6_MODE_M  (TEE_M6_MODE_V << TEE_M6_MODE_S)
#define TEE_M6_MODE_V  0x00000003U
#define TEE_M6_MODE_S  0

/** TEE_M7_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M7_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x1c)
/** TEE_M7_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M7 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M7_MODE    0x00000003U
#define TEE_M7_MODE_M  (TEE_M7_MODE_V << TEE_M7_MODE_S)
#define TEE_M7_MODE_V  0x00000003U
#define TEE_M7_MODE_S  0

/** TEE_M8_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M8_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x20)
/** TEE_M8_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M8 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M8_MODE    0x00000003U
#define TEE_M8_MODE_M  (TEE_M8_MODE_V << TEE_M8_MODE_S)
#define TEE_M8_MODE_V  0x00000003U
#define TEE_M8_MODE_S  0

/** TEE_M9_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M9_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x24)
/** TEE_M9_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M9 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M9_MODE    0x00000003U
#define TEE_M9_MODE_M  (TEE_M9_MODE_V << TEE_M9_MODE_S)
#define TEE_M9_MODE_V  0x00000003U
#define TEE_M9_MODE_S  0

/** TEE_M10_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M10_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x28)
/** TEE_M10_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M10 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M10_MODE    0x00000003U
#define TEE_M10_MODE_M  (TEE_M10_MODE_V << TEE_M10_MODE_S)
#define TEE_M10_MODE_V  0x00000003U
#define TEE_M10_MODE_S  0

/** TEE_M11_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M11_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x2c)
/** TEE_M11_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M11 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M11_MODE    0x00000003U
#define TEE_M11_MODE_M  (TEE_M11_MODE_V << TEE_M11_MODE_S)
#define TEE_M11_MODE_V  0x00000003U
#define TEE_M11_MODE_S  0

/** TEE_M12_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M12_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x30)
/** TEE_M12_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M12 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M12_MODE    0x00000003U
#define TEE_M12_MODE_M  (TEE_M12_MODE_V << TEE_M12_MODE_S)
#define TEE_M12_MODE_V  0x00000003U
#define TEE_M12_MODE_S  0

/** TEE_M13_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M13_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x34)
/** TEE_M13_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M13 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M13_MODE    0x00000003U
#define TEE_M13_MODE_M  (TEE_M13_MODE_V << TEE_M13_MODE_S)
#define TEE_M13_MODE_V  0x00000003U
#define TEE_M13_MODE_S  0

/** TEE_M14_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M14_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x38)
/** TEE_M14_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M14 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M14_MODE    0x00000003U
#define TEE_M14_MODE_M  (TEE_M14_MODE_V << TEE_M14_MODE_S)
#define TEE_M14_MODE_V  0x00000003U
#define TEE_M14_MODE_S  0

/** TEE_M15_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M15_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x3c)
/** TEE_M15_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M15 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M15_MODE    0x00000003U
#define TEE_M15_MODE_M  (TEE_M15_MODE_V << TEE_M15_MODE_S)
#define TEE_M15_MODE_V  0x00000003U
#define TEE_M15_MODE_S  0

/** TEE_M16_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M16_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x40)
/** TEE_M16_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M16 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M16_MODE    0x00000003U
#define TEE_M16_MODE_M  (TEE_M16_MODE_V << TEE_M16_MODE_S)
#define TEE_M16_MODE_V  0x00000003U
#define TEE_M16_MODE_S  0

/** TEE_M17_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M17_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x44)
/** TEE_M17_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M17 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M17_MODE    0x00000003U
#define TEE_M17_MODE_M  (TEE_M17_MODE_V << TEE_M17_MODE_S)
#define TEE_M17_MODE_V  0x00000003U
#define TEE_M17_MODE_S  0

/** TEE_M18_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M18_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x48)
/** TEE_M18_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M18 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M18_MODE    0x00000003U
#define TEE_M18_MODE_M  (TEE_M18_MODE_V << TEE_M18_MODE_S)
#define TEE_M18_MODE_V  0x00000003U
#define TEE_M18_MODE_S  0

/** TEE_M19_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M19_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x4c)
/** TEE_M19_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M19 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M19_MODE    0x00000003U
#define TEE_M19_MODE_M  (TEE_M19_MODE_V << TEE_M19_MODE_S)
#define TEE_M19_MODE_V  0x00000003U
#define TEE_M19_MODE_S  0

/** TEE_M20_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M20_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x50)
/** TEE_M20_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M20 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M20_MODE    0x00000003U
#define TEE_M20_MODE_M  (TEE_M20_MODE_V << TEE_M20_MODE_S)
#define TEE_M20_MODE_V  0x00000003U
#define TEE_M20_MODE_S  0

/** TEE_M21_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M21_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x54)
/** TEE_M21_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M21 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M21_MODE    0x00000003U
#define TEE_M21_MODE_M  (TEE_M21_MODE_V << TEE_M21_MODE_S)
#define TEE_M21_MODE_V  0x00000003U
#define TEE_M21_MODE_S  0

/** TEE_M22_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M22_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x58)
/** TEE_M22_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M22 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M22_MODE    0x00000003U
#define TEE_M22_MODE_M  (TEE_M22_MODE_V << TEE_M22_MODE_S)
#define TEE_M22_MODE_V  0x00000003U
#define TEE_M22_MODE_S  0

/** TEE_M23_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M23_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x5c)
/** TEE_M23_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M23 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M23_MODE    0x00000003U
#define TEE_M23_MODE_M  (TEE_M23_MODE_V << TEE_M23_MODE_S)
#define TEE_M23_MODE_V  0x00000003U
#define TEE_M23_MODE_S  0

/** TEE_M24_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M24_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x60)
/** TEE_M24_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M24 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M24_MODE    0x00000003U
#define TEE_M24_MODE_M  (TEE_M24_MODE_V << TEE_M24_MODE_S)
#define TEE_M24_MODE_V  0x00000003U
#define TEE_M24_MODE_S  0

/** TEE_M25_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M25_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x64)
/** TEE_M25_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M25 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M25_MODE    0x00000003U
#define TEE_M25_MODE_M  (TEE_M25_MODE_V << TEE_M25_MODE_S)
#define TEE_M25_MODE_V  0x00000003U
#define TEE_M25_MODE_S  0

/** TEE_M26_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M26_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x68)
/** TEE_M26_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M26 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M26_MODE    0x00000003U
#define TEE_M26_MODE_M  (TEE_M26_MODE_V << TEE_M26_MODE_S)
#define TEE_M26_MODE_V  0x00000003U
#define TEE_M26_MODE_S  0

/** TEE_M27_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M27_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x6c)
/** TEE_M27_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M27 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M27_MODE    0x00000003U
#define TEE_M27_MODE_M  (TEE_M27_MODE_V << TEE_M27_MODE_S)
#define TEE_M27_MODE_V  0x00000003U
#define TEE_M27_MODE_S  0

/** TEE_M28_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M28_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x70)
/** TEE_M28_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M28 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M28_MODE    0x00000003U
#define TEE_M28_MODE_M  (TEE_M28_MODE_V << TEE_M28_MODE_S)
#define TEE_M28_MODE_V  0x00000003U
#define TEE_M28_MODE_S  0

/** TEE_M29_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M29_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x74)
/** TEE_M29_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M29 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M29_MODE    0x00000003U
#define TEE_M29_MODE_M  (TEE_M29_MODE_V << TEE_M29_MODE_S)
#define TEE_M29_MODE_V  0x00000003U
#define TEE_M29_MODE_S  0

/** TEE_M30_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M30_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x78)
/** TEE_M30_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M30 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M30_MODE    0x00000003U
#define TEE_M30_MODE_M  (TEE_M30_MODE_V << TEE_M30_MODE_S)
#define TEE_M30_MODE_V  0x00000003U
#define TEE_M30_MODE_S  0

/** TEE_M31_MODE_CTRL_REG register
 *  Tee mode control register
 */
#define TEE_M31_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x7c)
/** TEE_M31_MODE : R/W; bitpos: [1:0]; default: 3;
 *  M31 security level mode: 2'd3: ree_mode2. 2'd2: ree_mode1. 2'd1: ree_mode0. 2'd0:
 *  tee_mode
 */
#define TEE_M31_MODE    0x00000003U
#define TEE_M31_MODE_M  (TEE_M31_MODE_V << TEE_M31_MODE_S)
#define TEE_M31_MODE_V  0x00000003U
#define TEE_M31_MODE_S  0

/** TEE_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define TEE_CLOCK_GATE_REG (DR_REG_TEE_BASE + 0x80)
/** TEE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  reg_clk_en
 */
#define TEE_CLK_EN    (BIT(0))
#define TEE_CLK_EN_M  (TEE_CLK_EN_V << TEE_CLK_EN_S)
#define TEE_CLK_EN_V  0x00000001U
#define TEE_CLK_EN_S  0

/** TEE_DATE_REG register
 *  Version register
 */
#define TEE_DATE_REG (DR_REG_TEE_BASE + 0xffc)
/** TEE_DATE_REG : R/W; bitpos: [27:0]; default: 35672706;
 *  reg_tee_date
 */
#define TEE_DATE    0x0FFFFFFFU
#define TEE_DATE_M  (TEE_DATE_V << TEE_DATE_S)
#define TEE_DATE_V  0x0FFFFFFFU
#define TEE_DATE_S  0

#ifdef __cplusplus
}
#endif
