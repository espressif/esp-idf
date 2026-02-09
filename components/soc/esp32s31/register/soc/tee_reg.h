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

/** TEE_M0_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M0_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x0)
/** TEE_M0_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures M$n security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M0_MODE    0x00000003U
#define TEE_M0_MODE_M  (TEE_M0_MODE_V << TEE_M0_MODE_S)
#define TEE_M0_MODE_V  0x00000003U
#define TEE_M0_MODE_S  0
/** TEE_M0_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M0_LOCK    (BIT(2))
#define TEE_M0_LOCK_M  (TEE_M0_LOCK_V << TEE_M0_LOCK_S)
#define TEE_M0_LOCK_V  0x00000001U
#define TEE_M0_LOCK_S  2

/** TEE_M1_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M1_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x4)
/** TEE_M1_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M$n security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M1_MODE    0x00000003U
#define TEE_M1_MODE_M  (TEE_M1_MODE_V << TEE_M1_MODE_S)
#define TEE_M1_MODE_V  0x00000003U
#define TEE_M1_MODE_S  0
/** TEE_M1_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m1 tee configuration
 */
#define TEE_M1_LOCK    (BIT(2))
#define TEE_M1_LOCK_M  (TEE_M1_LOCK_V << TEE_M1_LOCK_S)
#define TEE_M1_LOCK_V  0x00000001U
#define TEE_M1_LOCK_S  2

/** TEE_M3_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M3_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xc)
/** TEE_M3_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M3 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M3_MODE    0x00000003U
#define TEE_M3_MODE_M  (TEE_M3_MODE_V << TEE_M3_MODE_S)
#define TEE_M3_MODE_V  0x00000003U
#define TEE_M3_MODE_S  0
/** TEE_M3_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M3_LOCK    (BIT(2))
#define TEE_M3_LOCK_M  (TEE_M3_LOCK_V << TEE_M3_LOCK_S)
#define TEE_M3_LOCK_V  0x00000001U
#define TEE_M3_LOCK_S  2

/** TEE_M4_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M4_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x10)
/** TEE_M4_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M4 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M4_MODE    0x00000003U
#define TEE_M4_MODE_M  (TEE_M4_MODE_V << TEE_M4_MODE_S)
#define TEE_M4_MODE_V  0x00000003U
#define TEE_M4_MODE_S  0
/** TEE_M4_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M4_LOCK    (BIT(2))
#define TEE_M4_LOCK_M  (TEE_M4_LOCK_V << TEE_M4_LOCK_S)
#define TEE_M4_LOCK_V  0x00000001U
#define TEE_M4_LOCK_S  2

/** TEE_M5_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M5_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x14)
/** TEE_M5_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M5 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M5_MODE    0x00000003U
#define TEE_M5_MODE_M  (TEE_M5_MODE_V << TEE_M5_MODE_S)
#define TEE_M5_MODE_V  0x00000003U
#define TEE_M5_MODE_S  0
/** TEE_M5_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M5_LOCK    (BIT(2))
#define TEE_M5_LOCK_M  (TEE_M5_LOCK_V << TEE_M5_LOCK_S)
#define TEE_M5_LOCK_V  0x00000001U
#define TEE_M5_LOCK_S  2

/** TEE_M6_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M6_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x18)
/** TEE_M6_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M6 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M6_MODE    0x00000003U
#define TEE_M6_MODE_M  (TEE_M6_MODE_V << TEE_M6_MODE_S)
#define TEE_M6_MODE_V  0x00000003U
#define TEE_M6_MODE_S  0
/** TEE_M6_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M6_LOCK    (BIT(2))
#define TEE_M6_LOCK_M  (TEE_M6_LOCK_V << TEE_M6_LOCK_S)
#define TEE_M6_LOCK_V  0x00000001U
#define TEE_M6_LOCK_S  2

/** TEE_M7_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M7_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x1c)
/** TEE_M7_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M7 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M7_MODE    0x00000003U
#define TEE_M7_MODE_M  (TEE_M7_MODE_V << TEE_M7_MODE_S)
#define TEE_M7_MODE_V  0x00000003U
#define TEE_M7_MODE_S  0
/** TEE_M7_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M7_LOCK    (BIT(2))
#define TEE_M7_LOCK_M  (TEE_M7_LOCK_V << TEE_M7_LOCK_S)
#define TEE_M7_LOCK_V  0x00000001U
#define TEE_M7_LOCK_S  2

/** TEE_M8_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M8_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x20)
/** TEE_M8_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M8 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M8_MODE    0x00000003U
#define TEE_M8_MODE_M  (TEE_M8_MODE_V << TEE_M8_MODE_S)
#define TEE_M8_MODE_V  0x00000003U
#define TEE_M8_MODE_S  0
/** TEE_M8_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M8_LOCK    (BIT(2))
#define TEE_M8_LOCK_M  (TEE_M8_LOCK_V << TEE_M8_LOCK_S)
#define TEE_M8_LOCK_V  0x00000001U
#define TEE_M8_LOCK_S  2

/** TEE_M9_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M9_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x24)
/** TEE_M9_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M9 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M9_MODE    0x00000003U
#define TEE_M9_MODE_M  (TEE_M9_MODE_V << TEE_M9_MODE_S)
#define TEE_M9_MODE_V  0x00000003U
#define TEE_M9_MODE_S  0
/** TEE_M9_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M9_LOCK    (BIT(2))
#define TEE_M9_LOCK_M  (TEE_M9_LOCK_V << TEE_M9_LOCK_S)
#define TEE_M9_LOCK_V  0x00000001U
#define TEE_M9_LOCK_S  2

/** TEE_M10_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M10_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x28)
/** TEE_M10_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M10 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M10_MODE    0x00000003U
#define TEE_M10_MODE_M  (TEE_M10_MODE_V << TEE_M10_MODE_S)
#define TEE_M10_MODE_V  0x00000003U
#define TEE_M10_MODE_S  0
/** TEE_M10_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M10_LOCK    (BIT(2))
#define TEE_M10_LOCK_M  (TEE_M10_LOCK_V << TEE_M10_LOCK_S)
#define TEE_M10_LOCK_V  0x00000001U
#define TEE_M10_LOCK_S  2

/** TEE_M11_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M11_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x2c)
/** TEE_M11_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M11 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M11_MODE    0x00000003U
#define TEE_M11_MODE_M  (TEE_M11_MODE_V << TEE_M11_MODE_S)
#define TEE_M11_MODE_V  0x00000003U
#define TEE_M11_MODE_S  0
/** TEE_M11_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M11_LOCK    (BIT(2))
#define TEE_M11_LOCK_M  (TEE_M11_LOCK_V << TEE_M11_LOCK_S)
#define TEE_M11_LOCK_V  0x00000001U
#define TEE_M11_LOCK_S  2

/** TEE_M12_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M12_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x30)
/** TEE_M12_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M12 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M12_MODE    0x00000003U
#define TEE_M12_MODE_M  (TEE_M12_MODE_V << TEE_M12_MODE_S)
#define TEE_M12_MODE_V  0x00000003U
#define TEE_M12_MODE_S  0
/** TEE_M12_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M12_LOCK    (BIT(2))
#define TEE_M12_LOCK_M  (TEE_M12_LOCK_V << TEE_M12_LOCK_S)
#define TEE_M12_LOCK_V  0x00000001U
#define TEE_M12_LOCK_S  2

/** TEE_M13_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M13_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x34)
/** TEE_M13_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M13 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M13_MODE    0x00000003U
#define TEE_M13_MODE_M  (TEE_M13_MODE_V << TEE_M13_MODE_S)
#define TEE_M13_MODE_V  0x00000003U
#define TEE_M13_MODE_S  0
/** TEE_M13_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M13_LOCK    (BIT(2))
#define TEE_M13_LOCK_M  (TEE_M13_LOCK_V << TEE_M13_LOCK_S)
#define TEE_M13_LOCK_V  0x00000001U
#define TEE_M13_LOCK_S  2

/** TEE_M14_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M14_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x38)
/** TEE_M14_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M14 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M14_MODE    0x00000003U
#define TEE_M14_MODE_M  (TEE_M14_MODE_V << TEE_M14_MODE_S)
#define TEE_M14_MODE_V  0x00000003U
#define TEE_M14_MODE_S  0
/** TEE_M14_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M14_LOCK    (BIT(2))
#define TEE_M14_LOCK_M  (TEE_M14_LOCK_V << TEE_M14_LOCK_S)
#define TEE_M14_LOCK_V  0x00000001U
#define TEE_M14_LOCK_S  2

/** TEE_M15_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M15_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x3c)
/** TEE_M15_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M15 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M15_MODE    0x00000003U
#define TEE_M15_MODE_M  (TEE_M15_MODE_V << TEE_M15_MODE_S)
#define TEE_M15_MODE_V  0x00000003U
#define TEE_M15_MODE_S  0
/** TEE_M15_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m3 tee configuration
 */
#define TEE_M15_LOCK    (BIT(2))
#define TEE_M15_LOCK_M  (TEE_M15_LOCK_V << TEE_M15_LOCK_S)
#define TEE_M15_LOCK_V  0x00000001U
#define TEE_M15_LOCK_S  2

/** TEE_M20_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M20_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x50)
/** TEE_M20_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M20 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M20_MODE    0x00000003U
#define TEE_M20_MODE_M  (TEE_M20_MODE_V << TEE_M20_MODE_S)
#define TEE_M20_MODE_V  0x00000003U
#define TEE_M20_MODE_S  0
/** TEE_M20_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M20_LOCK    (BIT(2))
#define TEE_M20_LOCK_M  (TEE_M20_LOCK_V << TEE_M20_LOCK_S)
#define TEE_M20_LOCK_V  0x00000001U
#define TEE_M20_LOCK_S  2

/** TEE_M21_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M21_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x54)
/** TEE_M21_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M21 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M21_MODE    0x00000003U
#define TEE_M21_MODE_M  (TEE_M21_MODE_V << TEE_M21_MODE_S)
#define TEE_M21_MODE_V  0x00000003U
#define TEE_M21_MODE_S  0
/** TEE_M21_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M21_LOCK    (BIT(2))
#define TEE_M21_LOCK_M  (TEE_M21_LOCK_V << TEE_M21_LOCK_S)
#define TEE_M21_LOCK_V  0x00000001U
#define TEE_M21_LOCK_S  2

/** TEE_M22_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M22_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x58)
/** TEE_M22_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M22 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M22_MODE    0x00000003U
#define TEE_M22_MODE_M  (TEE_M22_MODE_V << TEE_M22_MODE_S)
#define TEE_M22_MODE_V  0x00000003U
#define TEE_M22_MODE_S  0
/** TEE_M22_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M22_LOCK    (BIT(2))
#define TEE_M22_LOCK_M  (TEE_M22_LOCK_V << TEE_M22_LOCK_S)
#define TEE_M22_LOCK_V  0x00000001U
#define TEE_M22_LOCK_S  2

/** TEE_M23_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M23_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x5c)
/** TEE_M23_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M23 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M23_MODE    0x00000003U
#define TEE_M23_MODE_M  (TEE_M23_MODE_V << TEE_M23_MODE_S)
#define TEE_M23_MODE_V  0x00000003U
#define TEE_M23_MODE_S  0
/** TEE_M23_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M23_LOCK    (BIT(2))
#define TEE_M23_LOCK_M  (TEE_M23_LOCK_V << TEE_M23_LOCK_S)
#define TEE_M23_LOCK_V  0x00000001U
#define TEE_M23_LOCK_S  2

/** TEE_M24_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M24_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x60)
/** TEE_M24_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M24 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M24_MODE    0x00000003U
#define TEE_M24_MODE_M  (TEE_M24_MODE_V << TEE_M24_MODE_S)
#define TEE_M24_MODE_V  0x00000003U
#define TEE_M24_MODE_S  0
/** TEE_M24_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M24_LOCK    (BIT(2))
#define TEE_M24_LOCK_M  (TEE_M24_LOCK_V << TEE_M24_LOCK_S)
#define TEE_M24_LOCK_V  0x00000001U
#define TEE_M24_LOCK_S  2

/** TEE_M25_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M25_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x64)
/** TEE_M25_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M25 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M25_MODE    0x00000003U
#define TEE_M25_MODE_M  (TEE_M25_MODE_V << TEE_M25_MODE_S)
#define TEE_M25_MODE_V  0x00000003U
#define TEE_M25_MODE_S  0
/** TEE_M25_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M25_LOCK    (BIT(2))
#define TEE_M25_LOCK_M  (TEE_M25_LOCK_V << TEE_M25_LOCK_S)
#define TEE_M25_LOCK_V  0x00000001U
#define TEE_M25_LOCK_S  2

/** TEE_M26_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M26_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x68)
/** TEE_M26_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M26 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M26_MODE    0x00000003U
#define TEE_M26_MODE_M  (TEE_M26_MODE_V << TEE_M26_MODE_S)
#define TEE_M26_MODE_V  0x00000003U
#define TEE_M26_MODE_S  0
/** TEE_M26_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M26_LOCK    (BIT(2))
#define TEE_M26_LOCK_M  (TEE_M26_LOCK_V << TEE_M26_LOCK_S)
#define TEE_M26_LOCK_V  0x00000001U
#define TEE_M26_LOCK_S  2

/** TEE_M27_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M27_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x6c)
/** TEE_M27_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M27 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M27_MODE    0x00000003U
#define TEE_M27_MODE_M  (TEE_M27_MODE_V << TEE_M27_MODE_S)
#define TEE_M27_MODE_V  0x00000003U
#define TEE_M27_MODE_S  0
/** TEE_M27_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M27_LOCK    (BIT(2))
#define TEE_M27_LOCK_M  (TEE_M27_LOCK_V << TEE_M27_LOCK_S)
#define TEE_M27_LOCK_V  0x00000001U
#define TEE_M27_LOCK_S  2

/** TEE_M28_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M28_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x70)
/** TEE_M28_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M28 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M28_MODE    0x00000003U
#define TEE_M28_MODE_M  (TEE_M28_MODE_V << TEE_M28_MODE_S)
#define TEE_M28_MODE_V  0x00000003U
#define TEE_M28_MODE_S  0
/** TEE_M28_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M28_LOCK    (BIT(2))
#define TEE_M28_LOCK_M  (TEE_M28_LOCK_V << TEE_M28_LOCK_S)
#define TEE_M28_LOCK_V  0x00000001U
#define TEE_M28_LOCK_S  2

/** TEE_M29_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M29_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x74)
/** TEE_M29_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M29 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M29_MODE    0x00000003U
#define TEE_M29_MODE_M  (TEE_M29_MODE_V << TEE_M29_MODE_S)
#define TEE_M29_MODE_V  0x00000003U
#define TEE_M29_MODE_S  0
/** TEE_M29_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M29_LOCK    (BIT(2))
#define TEE_M29_LOCK_M  (TEE_M29_LOCK_V << TEE_M29_LOCK_S)
#define TEE_M29_LOCK_V  0x00000001U
#define TEE_M29_LOCK_S  2

/** TEE_M30_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M30_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x78)
/** TEE_M30_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M30 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M30_MODE    0x00000003U
#define TEE_M30_MODE_M  (TEE_M30_MODE_V << TEE_M30_MODE_S)
#define TEE_M30_MODE_V  0x00000003U
#define TEE_M30_MODE_S  0
/** TEE_M30_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M30_LOCK    (BIT(2))
#define TEE_M30_LOCK_M  (TEE_M30_LOCK_V << TEE_M30_LOCK_S)
#define TEE_M30_LOCK_V  0x00000001U
#define TEE_M30_LOCK_S  2

/** TEE_M31_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M31_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x7c)
/** TEE_M31_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M31 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M31_MODE    0x00000003U
#define TEE_M31_MODE_M  (TEE_M31_MODE_V << TEE_M31_MODE_S)
#define TEE_M31_MODE_V  0x00000003U
#define TEE_M31_MODE_S  0
/** TEE_M31_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M31_LOCK    (BIT(2))
#define TEE_M31_LOCK_M  (TEE_M31_LOCK_V << TEE_M31_LOCK_S)
#define TEE_M31_LOCK_V  0x00000001U
#define TEE_M31_LOCK_S  2

/** TEE_M32_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M32_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x80)
/** TEE_M32_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M32 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M32_MODE    0x00000003U
#define TEE_M32_MODE_M  (TEE_M32_MODE_V << TEE_M32_MODE_S)
#define TEE_M32_MODE_V  0x00000003U
#define TEE_M32_MODE_S  0
/** TEE_M32_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M32_LOCK    (BIT(2))
#define TEE_M32_LOCK_M  (TEE_M32_LOCK_V << TEE_M32_LOCK_S)
#define TEE_M32_LOCK_V  0x00000001U
#define TEE_M32_LOCK_S  2

/** TEE_M33_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M33_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x84)
/** TEE_M33_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M33 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M33_MODE    0x00000003U
#define TEE_M33_MODE_M  (TEE_M33_MODE_V << TEE_M33_MODE_S)
#define TEE_M33_MODE_V  0x00000003U
#define TEE_M33_MODE_S  0
/** TEE_M33_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M33_LOCK    (BIT(2))
#define TEE_M33_LOCK_M  (TEE_M33_LOCK_V << TEE_M33_LOCK_S)
#define TEE_M33_LOCK_V  0x00000001U
#define TEE_M33_LOCK_S  2

/** TEE_M34_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M34_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x88)
/** TEE_M34_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M34 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M34_MODE    0x00000003U
#define TEE_M34_MODE_M  (TEE_M34_MODE_V << TEE_M34_MODE_S)
#define TEE_M34_MODE_V  0x00000003U
#define TEE_M34_MODE_S  0
/** TEE_M34_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M34_LOCK    (BIT(2))
#define TEE_M34_LOCK_M  (TEE_M34_LOCK_V << TEE_M34_LOCK_S)
#define TEE_M34_LOCK_V  0x00000001U
#define TEE_M34_LOCK_S  2

/** TEE_M35_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M35_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x8c)
/** TEE_M35_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M35 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M35_MODE    0x00000003U
#define TEE_M35_MODE_M  (TEE_M35_MODE_V << TEE_M35_MODE_S)
#define TEE_M35_MODE_V  0x00000003U
#define TEE_M35_MODE_S  0
/** TEE_M35_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M35_LOCK    (BIT(2))
#define TEE_M35_LOCK_M  (TEE_M35_LOCK_V << TEE_M35_LOCK_S)
#define TEE_M35_LOCK_V  0x00000001U
#define TEE_M35_LOCK_S  2

/** TEE_M36_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M36_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x90)
/** TEE_M36_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M36 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M36_MODE    0x00000003U
#define TEE_M36_MODE_M  (TEE_M36_MODE_V << TEE_M36_MODE_S)
#define TEE_M36_MODE_V  0x00000003U
#define TEE_M36_MODE_S  0
/** TEE_M36_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M36_LOCK    (BIT(2))
#define TEE_M36_LOCK_M  (TEE_M36_LOCK_V << TEE_M36_LOCK_S)
#define TEE_M36_LOCK_V  0x00000001U
#define TEE_M36_LOCK_S  2

/** TEE_M37_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M37_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x94)
/** TEE_M37_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M37 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M37_MODE    0x00000003U
#define TEE_M37_MODE_M  (TEE_M37_MODE_V << TEE_M37_MODE_S)
#define TEE_M37_MODE_V  0x00000003U
#define TEE_M37_MODE_S  0
/** TEE_M37_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M37_LOCK    (BIT(2))
#define TEE_M37_LOCK_M  (TEE_M37_LOCK_V << TEE_M37_LOCK_S)
#define TEE_M37_LOCK_V  0x00000001U
#define TEE_M37_LOCK_S  2

/** TEE_M38_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M38_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x98)
/** TEE_M38_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M38 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M38_MODE    0x00000003U
#define TEE_M38_MODE_M  (TEE_M38_MODE_V << TEE_M38_MODE_S)
#define TEE_M38_MODE_V  0x00000003U
#define TEE_M38_MODE_S  0
/** TEE_M38_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M38_LOCK    (BIT(2))
#define TEE_M38_LOCK_M  (TEE_M38_LOCK_V << TEE_M38_LOCK_S)
#define TEE_M38_LOCK_V  0x00000001U
#define TEE_M38_LOCK_S  2

/** TEE_M39_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M39_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x9c)
/** TEE_M39_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M39 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M39_MODE    0x00000003U
#define TEE_M39_MODE_M  (TEE_M39_MODE_V << TEE_M39_MODE_S)
#define TEE_M39_MODE_V  0x00000003U
#define TEE_M39_MODE_S  0
/** TEE_M39_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M39_LOCK    (BIT(2))
#define TEE_M39_LOCK_M  (TEE_M39_LOCK_V << TEE_M39_LOCK_S)
#define TEE_M39_LOCK_V  0x00000001U
#define TEE_M39_LOCK_S  2

/** TEE_M40_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M40_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xa0)
/** TEE_M40_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M40 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M40_MODE    0x00000003U
#define TEE_M40_MODE_M  (TEE_M40_MODE_V << TEE_M40_MODE_S)
#define TEE_M40_MODE_V  0x00000003U
#define TEE_M40_MODE_S  0
/** TEE_M40_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M40_LOCK    (BIT(2))
#define TEE_M40_LOCK_M  (TEE_M40_LOCK_V << TEE_M40_LOCK_S)
#define TEE_M40_LOCK_V  0x00000001U
#define TEE_M40_LOCK_S  2

/** TEE_M41_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M41_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xa4)
/** TEE_M41_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M41 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M41_MODE    0x00000003U
#define TEE_M41_MODE_M  (TEE_M41_MODE_V << TEE_M41_MODE_S)
#define TEE_M41_MODE_V  0x00000003U
#define TEE_M41_MODE_S  0
/** TEE_M41_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M41_LOCK    (BIT(2))
#define TEE_M41_LOCK_M  (TEE_M41_LOCK_V << TEE_M41_LOCK_S)
#define TEE_M41_LOCK_V  0x00000001U
#define TEE_M41_LOCK_S  2

/** TEE_M42_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M42_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xa8)
/** TEE_M42_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M42 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M42_MODE    0x00000003U
#define TEE_M42_MODE_M  (TEE_M42_MODE_V << TEE_M42_MODE_S)
#define TEE_M42_MODE_V  0x00000003U
#define TEE_M42_MODE_S  0
/** TEE_M42_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M42_LOCK    (BIT(2))
#define TEE_M42_LOCK_M  (TEE_M42_LOCK_V << TEE_M42_LOCK_S)
#define TEE_M42_LOCK_V  0x00000001U
#define TEE_M42_LOCK_S  2

/** TEE_M43_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M43_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xac)
/** TEE_M43_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M43 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M43_MODE    0x00000003U
#define TEE_M43_MODE_M  (TEE_M43_MODE_V << TEE_M43_MODE_S)
#define TEE_M43_MODE_V  0x00000003U
#define TEE_M43_MODE_S  0
/** TEE_M43_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M43_LOCK    (BIT(2))
#define TEE_M43_LOCK_M  (TEE_M43_LOCK_V << TEE_M43_LOCK_S)
#define TEE_M43_LOCK_V  0x00000001U
#define TEE_M43_LOCK_S  2

/** TEE_M44_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M44_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xb0)
/** TEE_M44_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M44 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M44_MODE    0x00000003U
#define TEE_M44_MODE_M  (TEE_M44_MODE_V << TEE_M44_MODE_S)
#define TEE_M44_MODE_V  0x00000003U
#define TEE_M44_MODE_S  0
/** TEE_M44_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M44_LOCK    (BIT(2))
#define TEE_M44_LOCK_M  (TEE_M44_LOCK_V << TEE_M44_LOCK_S)
#define TEE_M44_LOCK_V  0x00000001U
#define TEE_M44_LOCK_S  2

/** TEE_M45_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M45_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xb4)
/** TEE_M45_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M45 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M45_MODE    0x00000003U
#define TEE_M45_MODE_M  (TEE_M45_MODE_V << TEE_M45_MODE_S)
#define TEE_M45_MODE_V  0x00000003U
#define TEE_M45_MODE_S  0
/** TEE_M45_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M45_LOCK    (BIT(2))
#define TEE_M45_LOCK_M  (TEE_M45_LOCK_V << TEE_M45_LOCK_S)
#define TEE_M45_LOCK_V  0x00000001U
#define TEE_M45_LOCK_S  2

/** TEE_M46_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M46_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xb8)
/** TEE_M46_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M46 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M46_MODE    0x00000003U
#define TEE_M46_MODE_M  (TEE_M46_MODE_V << TEE_M46_MODE_S)
#define TEE_M46_MODE_V  0x00000003U
#define TEE_M46_MODE_S  0
/** TEE_M46_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M46_LOCK    (BIT(2))
#define TEE_M46_LOCK_M  (TEE_M46_LOCK_V << TEE_M46_LOCK_S)
#define TEE_M46_LOCK_V  0x00000001U
#define TEE_M46_LOCK_S  2

/** TEE_M47_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M47_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xbc)
/** TEE_M47_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M47 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M47_MODE    0x00000003U
#define TEE_M47_MODE_M  (TEE_M47_MODE_V << TEE_M47_MODE_S)
#define TEE_M47_MODE_V  0x00000003U
#define TEE_M47_MODE_S  0
/** TEE_M47_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M47_LOCK    (BIT(2))
#define TEE_M47_LOCK_M  (TEE_M47_LOCK_V << TEE_M47_LOCK_S)
#define TEE_M47_LOCK_V  0x00000001U
#define TEE_M47_LOCK_S  2

/** TEE_M48_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M48_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xc0)
/** TEE_M48_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M48 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M48_MODE    0x00000003U
#define TEE_M48_MODE_M  (TEE_M48_MODE_V << TEE_M48_MODE_S)
#define TEE_M48_MODE_V  0x00000003U
#define TEE_M48_MODE_S  0
/** TEE_M48_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M48_LOCK    (BIT(2))
#define TEE_M48_LOCK_M  (TEE_M48_LOCK_V << TEE_M48_LOCK_S)
#define TEE_M48_LOCK_V  0x00000001U
#define TEE_M48_LOCK_S  2

/** TEE_M49_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M49_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xc4)
/** TEE_M49_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M49 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M49_MODE    0x00000003U
#define TEE_M49_MODE_M  (TEE_M49_MODE_V << TEE_M49_MODE_S)
#define TEE_M49_MODE_V  0x00000003U
#define TEE_M49_MODE_S  0
/** TEE_M49_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M49_LOCK    (BIT(2))
#define TEE_M49_LOCK_M  (TEE_M49_LOCK_V << TEE_M49_LOCK_S)
#define TEE_M49_LOCK_V  0x00000001U
#define TEE_M49_LOCK_S  2

/** TEE_M50_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M50_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xc8)
/** TEE_M50_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M50 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M50_MODE    0x00000003U
#define TEE_M50_MODE_M  (TEE_M50_MODE_V << TEE_M50_MODE_S)
#define TEE_M50_MODE_V  0x00000003U
#define TEE_M50_MODE_S  0
/** TEE_M50_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M50_LOCK    (BIT(2))
#define TEE_M50_LOCK_M  (TEE_M50_LOCK_V << TEE_M50_LOCK_S)
#define TEE_M50_LOCK_V  0x00000001U
#define TEE_M50_LOCK_S  2

/** TEE_M51_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M51_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xcc)
/** TEE_M51_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M51 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M51_MODE    0x00000003U
#define TEE_M51_MODE_M  (TEE_M51_MODE_V << TEE_M51_MODE_S)
#define TEE_M51_MODE_V  0x00000003U
#define TEE_M51_MODE_S  0
/** TEE_M51_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M51_LOCK    (BIT(2))
#define TEE_M51_LOCK_M  (TEE_M51_LOCK_V << TEE_M51_LOCK_S)
#define TEE_M51_LOCK_V  0x00000001U
#define TEE_M51_LOCK_S  2

/** TEE_M52_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M52_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xd0)
/** TEE_M52_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M52 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M52_MODE    0x00000003U
#define TEE_M52_MODE_M  (TEE_M52_MODE_V << TEE_M52_MODE_S)
#define TEE_M52_MODE_V  0x00000003U
#define TEE_M52_MODE_S  0
/** TEE_M52_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M52_LOCK    (BIT(2))
#define TEE_M52_LOCK_M  (TEE_M52_LOCK_V << TEE_M52_LOCK_S)
#define TEE_M52_LOCK_V  0x00000001U
#define TEE_M52_LOCK_S  2

/** TEE_M53_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M53_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xd4)
/** TEE_M53_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M53 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M53_MODE    0x00000003U
#define TEE_M53_MODE_M  (TEE_M53_MODE_V << TEE_M53_MODE_S)
#define TEE_M53_MODE_V  0x00000003U
#define TEE_M53_MODE_S  0
/** TEE_M53_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M53_LOCK    (BIT(2))
#define TEE_M53_LOCK_M  (TEE_M53_LOCK_V << TEE_M53_LOCK_S)
#define TEE_M53_LOCK_V  0x00000001U
#define TEE_M53_LOCK_S  2

/** TEE_M54_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M54_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xd8)
/** TEE_M54_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M54 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M54_MODE    0x00000003U
#define TEE_M54_MODE_M  (TEE_M54_MODE_V << TEE_M54_MODE_S)
#define TEE_M54_MODE_V  0x00000003U
#define TEE_M54_MODE_S  0
/** TEE_M54_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M54_LOCK    (BIT(2))
#define TEE_M54_LOCK_M  (TEE_M54_LOCK_V << TEE_M54_LOCK_S)
#define TEE_M54_LOCK_V  0x00000001U
#define TEE_M54_LOCK_S  2

/** TEE_M55_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M55_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xdc)
/** TEE_M55_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M55 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M55_MODE    0x00000003U
#define TEE_M55_MODE_M  (TEE_M55_MODE_V << TEE_M55_MODE_S)
#define TEE_M55_MODE_V  0x00000003U
#define TEE_M55_MODE_S  0
/** TEE_M55_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M55_LOCK    (BIT(2))
#define TEE_M55_LOCK_M  (TEE_M55_LOCK_V << TEE_M55_LOCK_S)
#define TEE_M55_LOCK_V  0x00000001U
#define TEE_M55_LOCK_S  2

/** TEE_M56_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M56_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xe0)
/** TEE_M56_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M56 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M56_MODE    0x00000003U
#define TEE_M56_MODE_M  (TEE_M56_MODE_V << TEE_M56_MODE_S)
#define TEE_M56_MODE_V  0x00000003U
#define TEE_M56_MODE_S  0
/** TEE_M56_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M56_LOCK    (BIT(2))
#define TEE_M56_LOCK_M  (TEE_M56_LOCK_V << TEE_M56_LOCK_S)
#define TEE_M56_LOCK_V  0x00000001U
#define TEE_M56_LOCK_S  2

/** TEE_M57_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M57_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xe4)
/** TEE_M57_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M57 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M57_MODE    0x00000003U
#define TEE_M57_MODE_M  (TEE_M57_MODE_V << TEE_M57_MODE_S)
#define TEE_M57_MODE_V  0x00000003U
#define TEE_M57_MODE_S  0
/** TEE_M57_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M57_LOCK    (BIT(2))
#define TEE_M57_LOCK_M  (TEE_M57_LOCK_V << TEE_M57_LOCK_S)
#define TEE_M57_LOCK_V  0x00000001U
#define TEE_M57_LOCK_S  2

/** TEE_M58_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M58_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xe8)
/** TEE_M58_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M58 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M58_MODE    0x00000003U
#define TEE_M58_MODE_M  (TEE_M58_MODE_V << TEE_M58_MODE_S)
#define TEE_M58_MODE_V  0x00000003U
#define TEE_M58_MODE_S  0
/** TEE_M58_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M58_LOCK    (BIT(2))
#define TEE_M58_LOCK_M  (TEE_M58_LOCK_V << TEE_M58_LOCK_S)
#define TEE_M58_LOCK_V  0x00000001U
#define TEE_M58_LOCK_S  2

/** TEE_M59_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M59_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xec)
/** TEE_M59_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M59 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M59_MODE    0x00000003U
#define TEE_M59_MODE_M  (TEE_M59_MODE_V << TEE_M59_MODE_S)
#define TEE_M59_MODE_V  0x00000003U
#define TEE_M59_MODE_S  0
/** TEE_M59_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M59_LOCK    (BIT(2))
#define TEE_M59_LOCK_M  (TEE_M59_LOCK_V << TEE_M59_LOCK_S)
#define TEE_M59_LOCK_V  0x00000001U
#define TEE_M59_LOCK_S  2

/** TEE_M60_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M60_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xf0)
/** TEE_M60_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M60 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M60_MODE    0x00000003U
#define TEE_M60_MODE_M  (TEE_M60_MODE_V << TEE_M60_MODE_S)
#define TEE_M60_MODE_V  0x00000003U
#define TEE_M60_MODE_S  0
/** TEE_M60_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M60_LOCK    (BIT(2))
#define TEE_M60_LOCK_M  (TEE_M60_LOCK_V << TEE_M60_LOCK_S)
#define TEE_M60_LOCK_V  0x00000001U
#define TEE_M60_LOCK_S  2

/** TEE_M61_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M61_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xf4)
/** TEE_M61_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M61 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M61_MODE    0x00000003U
#define TEE_M61_MODE_M  (TEE_M61_MODE_V << TEE_M61_MODE_S)
#define TEE_M61_MODE_V  0x00000003U
#define TEE_M61_MODE_S  0
/** TEE_M61_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M61_LOCK    (BIT(2))
#define TEE_M61_LOCK_M  (TEE_M61_LOCK_V << TEE_M61_LOCK_S)
#define TEE_M61_LOCK_V  0x00000001U
#define TEE_M61_LOCK_S  2

/** TEE_M62_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M62_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xf8)
/** TEE_M62_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M62 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M62_MODE    0x00000003U
#define TEE_M62_MODE_M  (TEE_M62_MODE_V << TEE_M62_MODE_S)
#define TEE_M62_MODE_V  0x00000003U
#define TEE_M62_MODE_S  0
/** TEE_M62_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M62_LOCK    (BIT(2))
#define TEE_M62_LOCK_M  (TEE_M62_LOCK_V << TEE_M62_LOCK_S)
#define TEE_M62_LOCK_V  0x00000001U
#define TEE_M62_LOCK_S  2

/** TEE_M63_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M63_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xfc)
/** TEE_M63_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M63 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M63_MODE    0x00000003U
#define TEE_M63_MODE_M  (TEE_M63_MODE_V << TEE_M63_MODE_S)
#define TEE_M63_MODE_V  0x00000003U
#define TEE_M63_MODE_S  0
/** TEE_M63_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M63_LOCK    (BIT(2))
#define TEE_M63_LOCK_M  (TEE_M63_LOCK_V << TEE_M63_LOCK_S)
#define TEE_M63_LOCK_V  0x00000001U
#define TEE_M63_LOCK_S  2

/** TEE_M64_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M64_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x100)
/** TEE_M64_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M64 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M64_MODE    0x00000003U
#define TEE_M64_MODE_M  (TEE_M64_MODE_V << TEE_M64_MODE_S)
#define TEE_M64_MODE_V  0x00000003U
#define TEE_M64_MODE_S  0
/** TEE_M64_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M64_LOCK    (BIT(2))
#define TEE_M64_LOCK_M  (TEE_M64_LOCK_V << TEE_M64_LOCK_S)
#define TEE_M64_LOCK_V  0x00000001U
#define TEE_M64_LOCK_S  2

/** TEE_M65_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M65_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x104)
/** TEE_M65_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M65 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M65_MODE    0x00000003U
#define TEE_M65_MODE_M  (TEE_M65_MODE_V << TEE_M65_MODE_S)
#define TEE_M65_MODE_V  0x00000003U
#define TEE_M65_MODE_S  0
/** TEE_M65_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M65_LOCK    (BIT(2))
#define TEE_M65_LOCK_M  (TEE_M65_LOCK_V << TEE_M65_LOCK_S)
#define TEE_M65_LOCK_V  0x00000001U
#define TEE_M65_LOCK_S  2

/** TEE_M66_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M66_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x108)
/** TEE_M66_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M66 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M66_MODE    0x00000003U
#define TEE_M66_MODE_M  (TEE_M66_MODE_V << TEE_M66_MODE_S)
#define TEE_M66_MODE_V  0x00000003U
#define TEE_M66_MODE_S  0
/** TEE_M66_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M66_LOCK    (BIT(2))
#define TEE_M66_LOCK_M  (TEE_M66_LOCK_V << TEE_M66_LOCK_S)
#define TEE_M66_LOCK_V  0x00000001U
#define TEE_M66_LOCK_S  2

/** TEE_M67_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M67_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x10c)
/** TEE_M67_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M67 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M67_MODE    0x00000003U
#define TEE_M67_MODE_M  (TEE_M67_MODE_V << TEE_M67_MODE_S)
#define TEE_M67_MODE_V  0x00000003U
#define TEE_M67_MODE_S  0
/** TEE_M67_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M67_LOCK    (BIT(2))
#define TEE_M67_LOCK_M  (TEE_M67_LOCK_V << TEE_M67_LOCK_S)
#define TEE_M67_LOCK_V  0x00000001U
#define TEE_M67_LOCK_S  2

/** TEE_M68_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M68_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x110)
/** TEE_M68_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M68 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M68_MODE    0x00000003U
#define TEE_M68_MODE_M  (TEE_M68_MODE_V << TEE_M68_MODE_S)
#define TEE_M68_MODE_V  0x00000003U
#define TEE_M68_MODE_S  0
/** TEE_M68_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M68_LOCK    (BIT(2))
#define TEE_M68_LOCK_M  (TEE_M68_LOCK_V << TEE_M68_LOCK_S)
#define TEE_M68_LOCK_V  0x00000001U
#define TEE_M68_LOCK_S  2

/** TEE_M69_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M69_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x114)
/** TEE_M69_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M69 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M69_MODE    0x00000003U
#define TEE_M69_MODE_M  (TEE_M69_MODE_V << TEE_M69_MODE_S)
#define TEE_M69_MODE_V  0x00000003U
#define TEE_M69_MODE_S  0
/** TEE_M69_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M69_LOCK    (BIT(2))
#define TEE_M69_LOCK_M  (TEE_M69_LOCK_V << TEE_M69_LOCK_S)
#define TEE_M69_LOCK_V  0x00000001U
#define TEE_M69_LOCK_S  2

/** TEE_M70_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M70_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x118)
/** TEE_M70_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M70 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M70_MODE    0x00000003U
#define TEE_M70_MODE_M  (TEE_M70_MODE_V << TEE_M70_MODE_S)
#define TEE_M70_MODE_V  0x00000003U
#define TEE_M70_MODE_S  0
/** TEE_M70_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M70_LOCK    (BIT(2))
#define TEE_M70_LOCK_M  (TEE_M70_LOCK_V << TEE_M70_LOCK_S)
#define TEE_M70_LOCK_V  0x00000001U
#define TEE_M70_LOCK_S  2

/** TEE_M71_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M71_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x11c)
/** TEE_M71_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M71 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M71_MODE    0x00000003U
#define TEE_M71_MODE_M  (TEE_M71_MODE_V << TEE_M71_MODE_S)
#define TEE_M71_MODE_V  0x00000003U
#define TEE_M71_MODE_S  0
/** TEE_M71_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M71_LOCK    (BIT(2))
#define TEE_M71_LOCK_M  (TEE_M71_LOCK_V << TEE_M71_LOCK_S)
#define TEE_M71_LOCK_V  0x00000001U
#define TEE_M71_LOCK_S  2

/** TEE_M72_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M72_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x120)
/** TEE_M72_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M72 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M72_MODE    0x00000003U
#define TEE_M72_MODE_M  (TEE_M72_MODE_V << TEE_M72_MODE_S)
#define TEE_M72_MODE_V  0x00000003U
#define TEE_M72_MODE_S  0
/** TEE_M72_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M72_LOCK    (BIT(2))
#define TEE_M72_LOCK_M  (TEE_M72_LOCK_V << TEE_M72_LOCK_S)
#define TEE_M72_LOCK_V  0x00000001U
#define TEE_M72_LOCK_S  2

/** TEE_M73_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M73_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x124)
/** TEE_M73_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M73 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M73_MODE    0x00000003U
#define TEE_M73_MODE_M  (TEE_M73_MODE_V << TEE_M73_MODE_S)
#define TEE_M73_MODE_V  0x00000003U
#define TEE_M73_MODE_S  0
/** TEE_M73_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M73_LOCK    (BIT(2))
#define TEE_M73_LOCK_M  (TEE_M73_LOCK_V << TEE_M73_LOCK_S)
#define TEE_M73_LOCK_V  0x00000001U
#define TEE_M73_LOCK_S  2

/** TEE_M74_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M74_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x128)
/** TEE_M74_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M74 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M74_MODE    0x00000003U
#define TEE_M74_MODE_M  (TEE_M74_MODE_V << TEE_M74_MODE_S)
#define TEE_M74_MODE_V  0x00000003U
#define TEE_M74_MODE_S  0
/** TEE_M74_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M74_LOCK    (BIT(2))
#define TEE_M74_LOCK_M  (TEE_M74_LOCK_V << TEE_M74_LOCK_S)
#define TEE_M74_LOCK_V  0x00000001U
#define TEE_M74_LOCK_S  2

/** TEE_M75_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M75_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x12c)
/** TEE_M75_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M75 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M75_MODE    0x00000003U
#define TEE_M75_MODE_M  (TEE_M75_MODE_V << TEE_M75_MODE_S)
#define TEE_M75_MODE_V  0x00000003U
#define TEE_M75_MODE_S  0
/** TEE_M75_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M75_LOCK    (BIT(2))
#define TEE_M75_LOCK_M  (TEE_M75_LOCK_V << TEE_M75_LOCK_S)
#define TEE_M75_LOCK_V  0x00000001U
#define TEE_M75_LOCK_S  2

/** TEE_M76_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M76_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x130)
/** TEE_M76_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M76 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M76_MODE    0x00000003U
#define TEE_M76_MODE_M  (TEE_M76_MODE_V << TEE_M76_MODE_S)
#define TEE_M76_MODE_V  0x00000003U
#define TEE_M76_MODE_S  0
/** TEE_M76_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M76_LOCK    (BIT(2))
#define TEE_M76_LOCK_M  (TEE_M76_LOCK_V << TEE_M76_LOCK_S)
#define TEE_M76_LOCK_V  0x00000001U
#define TEE_M76_LOCK_S  2

/** TEE_M77_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M77_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x134)
/** TEE_M77_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M77 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M77_MODE    0x00000003U
#define TEE_M77_MODE_M  (TEE_M77_MODE_V << TEE_M77_MODE_S)
#define TEE_M77_MODE_V  0x00000003U
#define TEE_M77_MODE_S  0
/** TEE_M77_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M77_LOCK    (BIT(2))
#define TEE_M77_LOCK_M  (TEE_M77_LOCK_V << TEE_M77_LOCK_S)
#define TEE_M77_LOCK_V  0x00000001U
#define TEE_M77_LOCK_S  2

/** TEE_M78_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M78_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x138)
/** TEE_M78_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M78 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M78_MODE    0x00000003U
#define TEE_M78_MODE_M  (TEE_M78_MODE_V << TEE_M78_MODE_S)
#define TEE_M78_MODE_V  0x00000003U
#define TEE_M78_MODE_S  0
/** TEE_M78_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M78_LOCK    (BIT(2))
#define TEE_M78_LOCK_M  (TEE_M78_LOCK_V << TEE_M78_LOCK_S)
#define TEE_M78_LOCK_V  0x00000001U
#define TEE_M78_LOCK_S  2

/** TEE_M79_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M79_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x13c)
/** TEE_M79_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M79 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M79_MODE    0x00000003U
#define TEE_M79_MODE_M  (TEE_M79_MODE_V << TEE_M79_MODE_S)
#define TEE_M79_MODE_V  0x00000003U
#define TEE_M79_MODE_S  0
/** TEE_M79_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M79_LOCK    (BIT(2))
#define TEE_M79_LOCK_M  (TEE_M79_LOCK_V << TEE_M79_LOCK_S)
#define TEE_M79_LOCK_V  0x00000001U
#define TEE_M79_LOCK_S  2

/** TEE_M80_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M80_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x140)
/** TEE_M80_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M80 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M80_MODE    0x00000003U
#define TEE_M80_MODE_M  (TEE_M80_MODE_V << TEE_M80_MODE_S)
#define TEE_M80_MODE_V  0x00000003U
#define TEE_M80_MODE_S  0
/** TEE_M80_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M80_LOCK    (BIT(2))
#define TEE_M80_LOCK_M  (TEE_M80_LOCK_V << TEE_M80_LOCK_S)
#define TEE_M80_LOCK_V  0x00000001U
#define TEE_M80_LOCK_S  2

/** TEE_M81_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M81_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x144)
/** TEE_M81_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M81 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M81_MODE    0x00000003U
#define TEE_M81_MODE_M  (TEE_M81_MODE_V << TEE_M81_MODE_S)
#define TEE_M81_MODE_V  0x00000003U
#define TEE_M81_MODE_S  0
/** TEE_M81_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M81_LOCK    (BIT(2))
#define TEE_M81_LOCK_M  (TEE_M81_LOCK_V << TEE_M81_LOCK_S)
#define TEE_M81_LOCK_V  0x00000001U
#define TEE_M81_LOCK_S  2

/** TEE_M82_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M82_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x148)
/** TEE_M82_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M82 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M82_MODE    0x00000003U
#define TEE_M82_MODE_M  (TEE_M82_MODE_V << TEE_M82_MODE_S)
#define TEE_M82_MODE_V  0x00000003U
#define TEE_M82_MODE_S  0
/** TEE_M82_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M82_LOCK    (BIT(2))
#define TEE_M82_LOCK_M  (TEE_M82_LOCK_V << TEE_M82_LOCK_S)
#define TEE_M82_LOCK_V  0x00000001U
#define TEE_M82_LOCK_S  2

/** TEE_M83_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M83_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x14c)
/** TEE_M83_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M83 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M83_MODE    0x00000003U
#define TEE_M83_MODE_M  (TEE_M83_MODE_V << TEE_M83_MODE_S)
#define TEE_M83_MODE_V  0x00000003U
#define TEE_M83_MODE_S  0
/** TEE_M83_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M83_LOCK    (BIT(2))
#define TEE_M83_LOCK_M  (TEE_M83_LOCK_V << TEE_M83_LOCK_S)
#define TEE_M83_LOCK_V  0x00000001U
#define TEE_M83_LOCK_S  2

/** TEE_M84_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M84_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x150)
/** TEE_M84_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M84 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M84_MODE    0x00000003U
#define TEE_M84_MODE_M  (TEE_M84_MODE_V << TEE_M84_MODE_S)
#define TEE_M84_MODE_V  0x00000003U
#define TEE_M84_MODE_S  0
/** TEE_M84_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M84_LOCK    (BIT(2))
#define TEE_M84_LOCK_M  (TEE_M84_LOCK_V << TEE_M84_LOCK_S)
#define TEE_M84_LOCK_V  0x00000001U
#define TEE_M84_LOCK_S  2

/** TEE_M85_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M85_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x154)
/** TEE_M85_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M85 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M85_MODE    0x00000003U
#define TEE_M85_MODE_M  (TEE_M85_MODE_V << TEE_M85_MODE_S)
#define TEE_M85_MODE_V  0x00000003U
#define TEE_M85_MODE_S  0
/** TEE_M85_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M85_LOCK    (BIT(2))
#define TEE_M85_LOCK_M  (TEE_M85_LOCK_V << TEE_M85_LOCK_S)
#define TEE_M85_LOCK_V  0x00000001U
#define TEE_M85_LOCK_S  2

/** TEE_M86_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M86_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x158)
/** TEE_M86_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M86 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M86_MODE    0x00000003U
#define TEE_M86_MODE_M  (TEE_M86_MODE_V << TEE_M86_MODE_S)
#define TEE_M86_MODE_V  0x00000003U
#define TEE_M86_MODE_S  0
/** TEE_M86_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M86_LOCK    (BIT(2))
#define TEE_M86_LOCK_M  (TEE_M86_LOCK_V << TEE_M86_LOCK_S)
#define TEE_M86_LOCK_V  0x00000001U
#define TEE_M86_LOCK_S  2

/** TEE_M87_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M87_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x15c)
/** TEE_M87_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M87 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M87_MODE    0x00000003U
#define TEE_M87_MODE_M  (TEE_M87_MODE_V << TEE_M87_MODE_S)
#define TEE_M87_MODE_V  0x00000003U
#define TEE_M87_MODE_S  0
/** TEE_M87_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m20 tee configuration
 */
#define TEE_M87_LOCK    (BIT(2))
#define TEE_M87_LOCK_M  (TEE_M87_LOCK_V << TEE_M87_LOCK_S)
#define TEE_M87_LOCK_V  0x00000001U
#define TEE_M87_LOCK_S  2

/** TEE_SUPERVISOR_PRIV_SEL_REG register
 *  TEE mode control register
 */
#define TEE_SUPERVISOR_PRIV_SEL_REG (DR_REG_TEE_BASE + 0x200)
/** TEE_CORE0_SUPERVISOR_PRIV_SEL : R/W; bitpos: [0]; default: 0;
 *  Configures the privilege of the core0 supervisor mode .
 *  0: same as user_mode
 *  1: same as machine_mode
 */
#define TEE_CORE0_SUPERVISOR_PRIV_SEL    (BIT(0))
#define TEE_CORE0_SUPERVISOR_PRIV_SEL_M  (TEE_CORE0_SUPERVISOR_PRIV_SEL_V << TEE_CORE0_SUPERVISOR_PRIV_SEL_S)
#define TEE_CORE0_SUPERVISOR_PRIV_SEL_V  0x00000001U
#define TEE_CORE0_SUPERVISOR_PRIV_SEL_S  0
/** TEE_CORE1_SUPERVISOR_PRIV_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures the privilege of the core0 supervisor mode .
 *  0: same as user_mode
 *  1: same as machine_mode
 */
#define TEE_CORE1_SUPERVISOR_PRIV_SEL    (BIT(1))
#define TEE_CORE1_SUPERVISOR_PRIV_SEL_M  (TEE_CORE1_SUPERVISOR_PRIV_SEL_V << TEE_CORE1_SUPERVISOR_PRIV_SEL_S)
#define TEE_CORE1_SUPERVISOR_PRIV_SEL_V  0x00000001U
#define TEE_CORE1_SUPERVISOR_PRIV_SEL_S  1
/** TEE_CORE0_SUPERVISOR_PRIV_SEL_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock reg_core0_supervisor_priv_sel configuration
 */
#define TEE_CORE0_SUPERVISOR_PRIV_SEL_LOCK    (BIT(2))
#define TEE_CORE0_SUPERVISOR_PRIV_SEL_LOCK_M  (TEE_CORE0_SUPERVISOR_PRIV_SEL_LOCK_V << TEE_CORE0_SUPERVISOR_PRIV_SEL_LOCK_S)
#define TEE_CORE0_SUPERVISOR_PRIV_SEL_LOCK_V  0x00000001U
#define TEE_CORE0_SUPERVISOR_PRIV_SEL_LOCK_S  2
/** TEE_CORE1_SUPERVISOR_PRIV_SEL_LOCK : R/W; bitpos: [3]; default: 0;
 *  Set 1 to lock reg_core1_supervisor_priv_sel configuration
 */
#define TEE_CORE1_SUPERVISOR_PRIV_SEL_LOCK    (BIT(3))
#define TEE_CORE1_SUPERVISOR_PRIV_SEL_LOCK_M  (TEE_CORE1_SUPERVISOR_PRIV_SEL_LOCK_V << TEE_CORE1_SUPERVISOR_PRIV_SEL_LOCK_S)
#define TEE_CORE1_SUPERVISOR_PRIV_SEL_LOCK_V  0x00000001U
#define TEE_CORE1_SUPERVISOR_PRIV_SEL_LOCK_S  3

/** TEE_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define TEE_CLOCK_GATE_REG (DR_REG_TEE_BASE + 0x3f8)
/** TEE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: enable automatic clock gating
 *  1: keep the clock always on
 */
#define TEE_CLK_EN    (BIT(0))
#define TEE_CLK_EN_M  (TEE_CLK_EN_V << TEE_CLK_EN_S)
#define TEE_CLK_EN_V  0x00000001U
#define TEE_CLK_EN_S  0

/** TEE_DATE_REG register
 *  Version control register
 */
#define TEE_DATE_REG (DR_REG_TEE_BASE + 0x3fc)
/** TEE_DATE : R/W; bitpos: [27:0]; default: 38822480;
 *  Version control register
 */
#define TEE_DATE    0x0FFFFFFFU
#define TEE_DATE_M  (TEE_DATE_V << TEE_DATE_S)
#define TEE_DATE_V  0x0FFFFFFFU
#define TEE_DATE_S  0

#ifdef __cplusplus
}
#endif
