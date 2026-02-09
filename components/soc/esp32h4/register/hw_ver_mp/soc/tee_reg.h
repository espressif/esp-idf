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
 *  Configures M0 security level mode.
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
/** TEE_M1_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures M1 security level mode.
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M1_LOCK    (BIT(2))
#define TEE_M1_LOCK_M  (TEE_M1_LOCK_V << TEE_M1_LOCK_S)
#define TEE_M1_LOCK_V  0x00000001U
#define TEE_M1_LOCK_S  2

/** TEE_M2_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M2_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x8)
/** TEE_M2_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures M2 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M2_MODE    0x00000003U
#define TEE_M2_MODE_M  (TEE_M2_MODE_V << TEE_M2_MODE_S)
#define TEE_M2_MODE_V  0x00000003U
#define TEE_M2_MODE_S  0
/** TEE_M2_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M2_LOCK    (BIT(2))
#define TEE_M2_LOCK_M  (TEE_M2_LOCK_V << TEE_M2_LOCK_S)
#define TEE_M2_LOCK_V  0x00000001U
#define TEE_M2_LOCK_S  2

/** TEE_M3_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M3_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xc)
/** TEE_M3_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M3_LOCK    (BIT(2))
#define TEE_M3_LOCK_M  (TEE_M3_LOCK_V << TEE_M3_LOCK_S)
#define TEE_M3_LOCK_V  0x00000001U
#define TEE_M3_LOCK_S  2

/** TEE_M4_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M4_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x10)
/** TEE_M4_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M4_LOCK    (BIT(2))
#define TEE_M4_LOCK_M  (TEE_M4_LOCK_V << TEE_M4_LOCK_S)
#define TEE_M4_LOCK_V  0x00000001U
#define TEE_M4_LOCK_S  2

/** TEE_M5_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M5_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x14)
/** TEE_M5_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M5_LOCK    (BIT(2))
#define TEE_M5_LOCK_M  (TEE_M5_LOCK_V << TEE_M5_LOCK_S)
#define TEE_M5_LOCK_V  0x00000001U
#define TEE_M5_LOCK_S  2

/** TEE_M6_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M6_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x18)
/** TEE_M6_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M6_LOCK    (BIT(2))
#define TEE_M6_LOCK_M  (TEE_M6_LOCK_V << TEE_M6_LOCK_S)
#define TEE_M6_LOCK_V  0x00000001U
#define TEE_M6_LOCK_S  2

/** TEE_M7_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M7_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x1c)
/** TEE_M7_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M7_LOCK    (BIT(2))
#define TEE_M7_LOCK_M  (TEE_M7_LOCK_V << TEE_M7_LOCK_S)
#define TEE_M7_LOCK_V  0x00000001U
#define TEE_M7_LOCK_S  2

/** TEE_M8_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M8_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x20)
/** TEE_M8_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M8_LOCK    (BIT(2))
#define TEE_M8_LOCK_M  (TEE_M8_LOCK_V << TEE_M8_LOCK_S)
#define TEE_M8_LOCK_V  0x00000001U
#define TEE_M8_LOCK_S  2

/** TEE_M9_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M9_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x24)
/** TEE_M9_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M9_LOCK    (BIT(2))
#define TEE_M9_LOCK_M  (TEE_M9_LOCK_V << TEE_M9_LOCK_S)
#define TEE_M9_LOCK_V  0x00000001U
#define TEE_M9_LOCK_S  2

/** TEE_M10_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M10_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x28)
/** TEE_M10_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M10_LOCK    (BIT(2))
#define TEE_M10_LOCK_M  (TEE_M10_LOCK_V << TEE_M10_LOCK_S)
#define TEE_M10_LOCK_V  0x00000001U
#define TEE_M10_LOCK_S  2

/** TEE_M11_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M11_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x2c)
/** TEE_M11_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M11_LOCK    (BIT(2))
#define TEE_M11_LOCK_M  (TEE_M11_LOCK_V << TEE_M11_LOCK_S)
#define TEE_M11_LOCK_V  0x00000001U
#define TEE_M11_LOCK_S  2

/** TEE_M12_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M12_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x30)
/** TEE_M12_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M12_LOCK    (BIT(2))
#define TEE_M12_LOCK_M  (TEE_M12_LOCK_V << TEE_M12_LOCK_S)
#define TEE_M12_LOCK_V  0x00000001U
#define TEE_M12_LOCK_S  2

/** TEE_M13_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M13_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x34)
/** TEE_M13_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M13_LOCK    (BIT(2))
#define TEE_M13_LOCK_M  (TEE_M13_LOCK_V << TEE_M13_LOCK_S)
#define TEE_M13_LOCK_V  0x00000001U
#define TEE_M13_LOCK_S  2

/** TEE_M14_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M14_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x38)
/** TEE_M14_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M14_LOCK    (BIT(2))
#define TEE_M14_LOCK_M  (TEE_M14_LOCK_V << TEE_M14_LOCK_S)
#define TEE_M14_LOCK_V  0x00000001U
#define TEE_M14_LOCK_S  2

/** TEE_M15_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M15_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x3c)
/** TEE_M15_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M15_LOCK    (BIT(2))
#define TEE_M15_LOCK_M  (TEE_M15_LOCK_V << TEE_M15_LOCK_S)
#define TEE_M15_LOCK_V  0x00000001U
#define TEE_M15_LOCK_S  2

/** TEE_M16_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M16_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x40)
/** TEE_M16_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures M16 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M16_MODE    0x00000003U
#define TEE_M16_MODE_M  (TEE_M16_MODE_V << TEE_M16_MODE_S)
#define TEE_M16_MODE_V  0x00000003U
#define TEE_M16_MODE_S  0
/** TEE_M16_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M16_LOCK    (BIT(2))
#define TEE_M16_LOCK_M  (TEE_M16_LOCK_V << TEE_M16_LOCK_S)
#define TEE_M16_LOCK_V  0x00000001U
#define TEE_M16_LOCK_S  2

/** TEE_M17_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M17_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x44)
/** TEE_M17_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures M17 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M17_MODE    0x00000003U
#define TEE_M17_MODE_M  (TEE_M17_MODE_V << TEE_M17_MODE_S)
#define TEE_M17_MODE_V  0x00000003U
#define TEE_M17_MODE_S  0
/** TEE_M17_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M17_LOCK    (BIT(2))
#define TEE_M17_LOCK_M  (TEE_M17_LOCK_V << TEE_M17_LOCK_S)
#define TEE_M17_LOCK_V  0x00000001U
#define TEE_M17_LOCK_S  2

/** TEE_M18_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M18_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x48)
/** TEE_M18_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures M18 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M18_MODE    0x00000003U
#define TEE_M18_MODE_M  (TEE_M18_MODE_V << TEE_M18_MODE_S)
#define TEE_M18_MODE_V  0x00000003U
#define TEE_M18_MODE_S  0
/** TEE_M18_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M18_LOCK    (BIT(2))
#define TEE_M18_LOCK_M  (TEE_M18_LOCK_V << TEE_M18_LOCK_S)
#define TEE_M18_LOCK_V  0x00000001U
#define TEE_M18_LOCK_S  2

/** TEE_M19_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M19_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x4c)
/** TEE_M19_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures M19 security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define TEE_M19_MODE    0x00000003U
#define TEE_M19_MODE_M  (TEE_M19_MODE_V << TEE_M19_MODE_S)
#define TEE_M19_MODE_V  0x00000003U
#define TEE_M19_MODE_S  0
/** TEE_M19_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M19_LOCK    (BIT(2))
#define TEE_M19_LOCK_M  (TEE_M19_LOCK_V << TEE_M19_LOCK_S)
#define TEE_M19_LOCK_V  0x00000001U
#define TEE_M19_LOCK_S  2

/** TEE_M20_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M20_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x50)
/** TEE_M20_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M20_LOCK    (BIT(2))
#define TEE_M20_LOCK_M  (TEE_M20_LOCK_V << TEE_M20_LOCK_S)
#define TEE_M20_LOCK_V  0x00000001U
#define TEE_M20_LOCK_S  2

/** TEE_M21_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M21_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x54)
/** TEE_M21_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M21_LOCK    (BIT(2))
#define TEE_M21_LOCK_M  (TEE_M21_LOCK_V << TEE_M21_LOCK_S)
#define TEE_M21_LOCK_V  0x00000001U
#define TEE_M21_LOCK_S  2

/** TEE_M22_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M22_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x58)
/** TEE_M22_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M22_LOCK    (BIT(2))
#define TEE_M22_LOCK_M  (TEE_M22_LOCK_V << TEE_M22_LOCK_S)
#define TEE_M22_LOCK_V  0x00000001U
#define TEE_M22_LOCK_S  2

/** TEE_M23_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M23_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x5c)
/** TEE_M23_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M23_LOCK    (BIT(2))
#define TEE_M23_LOCK_M  (TEE_M23_LOCK_V << TEE_M23_LOCK_S)
#define TEE_M23_LOCK_V  0x00000001U
#define TEE_M23_LOCK_S  2

/** TEE_M24_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M24_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x60)
/** TEE_M24_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M24_LOCK    (BIT(2))
#define TEE_M24_LOCK_M  (TEE_M24_LOCK_V << TEE_M24_LOCK_S)
#define TEE_M24_LOCK_V  0x00000001U
#define TEE_M24_LOCK_S  2

/** TEE_M25_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M25_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x64)
/** TEE_M25_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M25_LOCK    (BIT(2))
#define TEE_M25_LOCK_M  (TEE_M25_LOCK_V << TEE_M25_LOCK_S)
#define TEE_M25_LOCK_V  0x00000001U
#define TEE_M25_LOCK_S  2

/** TEE_M26_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M26_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x68)
/** TEE_M26_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M26_LOCK    (BIT(2))
#define TEE_M26_LOCK_M  (TEE_M26_LOCK_V << TEE_M26_LOCK_S)
#define TEE_M26_LOCK_V  0x00000001U
#define TEE_M26_LOCK_S  2

/** TEE_M27_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M27_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x6c)
/** TEE_M27_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M27_LOCK    (BIT(2))
#define TEE_M27_LOCK_M  (TEE_M27_LOCK_V << TEE_M27_LOCK_S)
#define TEE_M27_LOCK_V  0x00000001U
#define TEE_M27_LOCK_S  2

/** TEE_M28_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M28_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x70)
/** TEE_M28_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M28_LOCK    (BIT(2))
#define TEE_M28_LOCK_M  (TEE_M28_LOCK_V << TEE_M28_LOCK_S)
#define TEE_M28_LOCK_V  0x00000001U
#define TEE_M28_LOCK_S  2

/** TEE_M29_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M29_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x74)
/** TEE_M29_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M29_LOCK    (BIT(2))
#define TEE_M29_LOCK_M  (TEE_M29_LOCK_V << TEE_M29_LOCK_S)
#define TEE_M29_LOCK_V  0x00000001U
#define TEE_M29_LOCK_S  2

/** TEE_M30_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M30_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x78)
/** TEE_M30_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M30_LOCK    (BIT(2))
#define TEE_M30_LOCK_M  (TEE_M30_LOCK_V << TEE_M30_LOCK_S)
#define TEE_M30_LOCK_V  0x00000001U
#define TEE_M30_LOCK_S  2

/** TEE_M31_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define TEE_M31_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x7c)
/** TEE_M31_MODE : R/W; bitpos: [1:0]; default: 0;
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
 *  Set 1 to lock m0 tee configuration
 */
#define TEE_M31_LOCK    (BIT(2))
#define TEE_M31_LOCK_M  (TEE_M31_LOCK_V << TEE_M31_LOCK_S)
#define TEE_M31_LOCK_V  0x00000001U
#define TEE_M31_LOCK_S  2

/** TEE_GPSPI0_CTRL_REG register
 *  gpspi0 read/write control register
 */
#define TEE_GPSPI0_CTRL_REG (DR_REG_TEE_BASE + 0x80)
/** TEE_READ_TEE_GPSPI0 : R/W; bitpos: [0]; default: 1;
 *  Configures gpspi0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_GPSPI0    (BIT(0))
#define TEE_READ_TEE_GPSPI0_M  (TEE_READ_TEE_GPSPI0_V << TEE_READ_TEE_GPSPI0_S)
#define TEE_READ_TEE_GPSPI0_V  0x00000001U
#define TEE_READ_TEE_GPSPI0_S  0
/** TEE_READ_REE0_GPSPI0 : R/W; bitpos: [1]; default: 0;
 *  Configures gpspi0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_GPSPI0    (BIT(1))
#define TEE_READ_REE0_GPSPI0_M  (TEE_READ_REE0_GPSPI0_V << TEE_READ_REE0_GPSPI0_S)
#define TEE_READ_REE0_GPSPI0_V  0x00000001U
#define TEE_READ_REE0_GPSPI0_S  1
/** TEE_READ_REE1_GPSPI0 : R/W; bitpos: [2]; default: 0;
 *  Configures gpspi0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_GPSPI0    (BIT(2))
#define TEE_READ_REE1_GPSPI0_M  (TEE_READ_REE1_GPSPI0_V << TEE_READ_REE1_GPSPI0_S)
#define TEE_READ_REE1_GPSPI0_V  0x00000001U
#define TEE_READ_REE1_GPSPI0_S  2
/** TEE_READ_REE2_GPSPI0 : R/W; bitpos: [3]; default: 0;
 *  Configures gpspi0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_GPSPI0    (BIT(3))
#define TEE_READ_REE2_GPSPI0_M  (TEE_READ_REE2_GPSPI0_V << TEE_READ_REE2_GPSPI0_S)
#define TEE_READ_REE2_GPSPI0_V  0x00000001U
#define TEE_READ_REE2_GPSPI0_S  3
/** TEE_WRITE_TEE_GPSPI0 : R/W; bitpos: [4]; default: 1;
 *  Configures gpspi0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_GPSPI0    (BIT(4))
#define TEE_WRITE_TEE_GPSPI0_M  (TEE_WRITE_TEE_GPSPI0_V << TEE_WRITE_TEE_GPSPI0_S)
#define TEE_WRITE_TEE_GPSPI0_V  0x00000001U
#define TEE_WRITE_TEE_GPSPI0_S  4
/** TEE_WRITE_REE0_GPSPI0 : R/W; bitpos: [5]; default: 0;
 *  Configures gpspi0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_GPSPI0    (BIT(5))
#define TEE_WRITE_REE0_GPSPI0_M  (TEE_WRITE_REE0_GPSPI0_V << TEE_WRITE_REE0_GPSPI0_S)
#define TEE_WRITE_REE0_GPSPI0_V  0x00000001U
#define TEE_WRITE_REE0_GPSPI0_S  5
/** TEE_WRITE_REE1_GPSPI0 : R/W; bitpos: [6]; default: 0;
 *  Configures gpspi0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_GPSPI0    (BIT(6))
#define TEE_WRITE_REE1_GPSPI0_M  (TEE_WRITE_REE1_GPSPI0_V << TEE_WRITE_REE1_GPSPI0_S)
#define TEE_WRITE_REE1_GPSPI0_V  0x00000001U
#define TEE_WRITE_REE1_GPSPI0_S  6
/** TEE_WRITE_REE2_GPSPI0 : R/W; bitpos: [7]; default: 0;
 *  Configures gpspi0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_GPSPI0    (BIT(7))
#define TEE_WRITE_REE2_GPSPI0_M  (TEE_WRITE_REE2_GPSPI0_V << TEE_WRITE_REE2_GPSPI0_S)
#define TEE_WRITE_REE2_GPSPI0_V  0x00000001U
#define TEE_WRITE_REE2_GPSPI0_S  7

/** TEE_GPSPI1_CTRL_REG register
 *  gpspi1 read/write control register
 */
#define TEE_GPSPI1_CTRL_REG (DR_REG_TEE_BASE + 0x84)
/** TEE_READ_TEE_GPSPI1 : R/W; bitpos: [0]; default: 1;
 *  Configures gpspi1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_GPSPI1    (BIT(0))
#define TEE_READ_TEE_GPSPI1_M  (TEE_READ_TEE_GPSPI1_V << TEE_READ_TEE_GPSPI1_S)
#define TEE_READ_TEE_GPSPI1_V  0x00000001U
#define TEE_READ_TEE_GPSPI1_S  0
/** TEE_READ_REE0_GPSPI1 : R/W; bitpos: [1]; default: 0;
 *  Configures gpspi1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_GPSPI1    (BIT(1))
#define TEE_READ_REE0_GPSPI1_M  (TEE_READ_REE0_GPSPI1_V << TEE_READ_REE0_GPSPI1_S)
#define TEE_READ_REE0_GPSPI1_V  0x00000001U
#define TEE_READ_REE0_GPSPI1_S  1
/** TEE_READ_REE1_GPSPI1 : R/W; bitpos: [2]; default: 0;
 *  Configures gpspi1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_GPSPI1    (BIT(2))
#define TEE_READ_REE1_GPSPI1_M  (TEE_READ_REE1_GPSPI1_V << TEE_READ_REE1_GPSPI1_S)
#define TEE_READ_REE1_GPSPI1_V  0x00000001U
#define TEE_READ_REE1_GPSPI1_S  2
/** TEE_READ_REE2_GPSPI1 : R/W; bitpos: [3]; default: 0;
 *  Configures gpspi1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_GPSPI1    (BIT(3))
#define TEE_READ_REE2_GPSPI1_M  (TEE_READ_REE2_GPSPI1_V << TEE_READ_REE2_GPSPI1_S)
#define TEE_READ_REE2_GPSPI1_V  0x00000001U
#define TEE_READ_REE2_GPSPI1_S  3
/** TEE_WRITE_TEE_GPSPI1 : R/W; bitpos: [4]; default: 1;
 *  Configures gpspi1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_GPSPI1    (BIT(4))
#define TEE_WRITE_TEE_GPSPI1_M  (TEE_WRITE_TEE_GPSPI1_V << TEE_WRITE_TEE_GPSPI1_S)
#define TEE_WRITE_TEE_GPSPI1_V  0x00000001U
#define TEE_WRITE_TEE_GPSPI1_S  4
/** TEE_WRITE_REE0_GPSPI1 : R/W; bitpos: [5]; default: 0;
 *  Configures gpspi1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_GPSPI1    (BIT(5))
#define TEE_WRITE_REE0_GPSPI1_M  (TEE_WRITE_REE0_GPSPI1_V << TEE_WRITE_REE0_GPSPI1_S)
#define TEE_WRITE_REE0_GPSPI1_V  0x00000001U
#define TEE_WRITE_REE0_GPSPI1_S  5
/** TEE_WRITE_REE1_GPSPI1 : R/W; bitpos: [6]; default: 0;
 *  Configures gpspi1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_GPSPI1    (BIT(6))
#define TEE_WRITE_REE1_GPSPI1_M  (TEE_WRITE_REE1_GPSPI1_V << TEE_WRITE_REE1_GPSPI1_S)
#define TEE_WRITE_REE1_GPSPI1_V  0x00000001U
#define TEE_WRITE_REE1_GPSPI1_S  6
/** TEE_WRITE_REE2_GPSPI1 : R/W; bitpos: [7]; default: 0;
 *  Configures gpspi1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_GPSPI1    (BIT(7))
#define TEE_WRITE_REE2_GPSPI1_M  (TEE_WRITE_REE2_GPSPI1_V << TEE_WRITE_REE2_GPSPI1_S)
#define TEE_WRITE_REE2_GPSPI1_V  0x00000001U
#define TEE_WRITE_REE2_GPSPI1_S  7

/** TEE_UART0_CTRL_REG register
 *  uart0 read/write control register
 */
#define TEE_UART0_CTRL_REG (DR_REG_TEE_BASE + 0x88)
/** TEE_READ_TEE_UART0 : R/W; bitpos: [0]; default: 1;
 *  Configures uart0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_UART0    (BIT(0))
#define TEE_READ_TEE_UART0_M  (TEE_READ_TEE_UART0_V << TEE_READ_TEE_UART0_S)
#define TEE_READ_TEE_UART0_V  0x00000001U
#define TEE_READ_TEE_UART0_S  0
/** TEE_READ_REE0_UART0 : R/W; bitpos: [1]; default: 0;
 *  Configures uart0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_UART0    (BIT(1))
#define TEE_READ_REE0_UART0_M  (TEE_READ_REE0_UART0_V << TEE_READ_REE0_UART0_S)
#define TEE_READ_REE0_UART0_V  0x00000001U
#define TEE_READ_REE0_UART0_S  1
/** TEE_READ_REE1_UART0 : R/W; bitpos: [2]; default: 0;
 *  Configures uart0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_UART0    (BIT(2))
#define TEE_READ_REE1_UART0_M  (TEE_READ_REE1_UART0_V << TEE_READ_REE1_UART0_S)
#define TEE_READ_REE1_UART0_V  0x00000001U
#define TEE_READ_REE1_UART0_S  2
/** TEE_READ_REE2_UART0 : R/W; bitpos: [3]; default: 0;
 *  Configures uart0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_UART0    (BIT(3))
#define TEE_READ_REE2_UART0_M  (TEE_READ_REE2_UART0_V << TEE_READ_REE2_UART0_S)
#define TEE_READ_REE2_UART0_V  0x00000001U
#define TEE_READ_REE2_UART0_S  3
/** TEE_WRITE_TEE_UART0 : R/W; bitpos: [4]; default: 1;
 *  Configures uart0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_UART0    (BIT(4))
#define TEE_WRITE_TEE_UART0_M  (TEE_WRITE_TEE_UART0_V << TEE_WRITE_TEE_UART0_S)
#define TEE_WRITE_TEE_UART0_V  0x00000001U
#define TEE_WRITE_TEE_UART0_S  4
/** TEE_WRITE_REE0_UART0 : R/W; bitpos: [5]; default: 0;
 *  Configures uart0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_UART0    (BIT(5))
#define TEE_WRITE_REE0_UART0_M  (TEE_WRITE_REE0_UART0_V << TEE_WRITE_REE0_UART0_S)
#define TEE_WRITE_REE0_UART0_V  0x00000001U
#define TEE_WRITE_REE0_UART0_S  5
/** TEE_WRITE_REE1_UART0 : R/W; bitpos: [6]; default: 0;
 *  Configures uart0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_UART0    (BIT(6))
#define TEE_WRITE_REE1_UART0_M  (TEE_WRITE_REE1_UART0_V << TEE_WRITE_REE1_UART0_S)
#define TEE_WRITE_REE1_UART0_V  0x00000001U
#define TEE_WRITE_REE1_UART0_S  6
/** TEE_WRITE_REE2_UART0 : R/W; bitpos: [7]; default: 0;
 *  Configures uart0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_UART0    (BIT(7))
#define TEE_WRITE_REE2_UART0_M  (TEE_WRITE_REE2_UART0_V << TEE_WRITE_REE2_UART0_S)
#define TEE_WRITE_REE2_UART0_V  0x00000001U
#define TEE_WRITE_REE2_UART0_S  7

/** TEE_UART1_CTRL_REG register
 *  uart1 read/write control register
 */
#define TEE_UART1_CTRL_REG (DR_REG_TEE_BASE + 0x8c)
/** TEE_READ_TEE_UART1 : R/W; bitpos: [0]; default: 1;
 *  Configures uart1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_UART1    (BIT(0))
#define TEE_READ_TEE_UART1_M  (TEE_READ_TEE_UART1_V << TEE_READ_TEE_UART1_S)
#define TEE_READ_TEE_UART1_V  0x00000001U
#define TEE_READ_TEE_UART1_S  0
/** TEE_READ_REE0_UART1 : R/W; bitpos: [1]; default: 0;
 *  Configures uart1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_UART1    (BIT(1))
#define TEE_READ_REE0_UART1_M  (TEE_READ_REE0_UART1_V << TEE_READ_REE0_UART1_S)
#define TEE_READ_REE0_UART1_V  0x00000001U
#define TEE_READ_REE0_UART1_S  1
/** TEE_READ_REE1_UART1 : R/W; bitpos: [2]; default: 0;
 *  Configures uart1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_UART1    (BIT(2))
#define TEE_READ_REE1_UART1_M  (TEE_READ_REE1_UART1_V << TEE_READ_REE1_UART1_S)
#define TEE_READ_REE1_UART1_V  0x00000001U
#define TEE_READ_REE1_UART1_S  2
/** TEE_READ_REE2_UART1 : R/W; bitpos: [3]; default: 0;
 *  Configures uart1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_UART1    (BIT(3))
#define TEE_READ_REE2_UART1_M  (TEE_READ_REE2_UART1_V << TEE_READ_REE2_UART1_S)
#define TEE_READ_REE2_UART1_V  0x00000001U
#define TEE_READ_REE2_UART1_S  3
/** TEE_WRITE_TEE_UART1 : R/W; bitpos: [4]; default: 1;
 *  Configures uart1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_UART1    (BIT(4))
#define TEE_WRITE_TEE_UART1_M  (TEE_WRITE_TEE_UART1_V << TEE_WRITE_TEE_UART1_S)
#define TEE_WRITE_TEE_UART1_V  0x00000001U
#define TEE_WRITE_TEE_UART1_S  4
/** TEE_WRITE_REE0_UART1 : R/W; bitpos: [5]; default: 0;
 *  Configures uart1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_UART1    (BIT(5))
#define TEE_WRITE_REE0_UART1_M  (TEE_WRITE_REE0_UART1_V << TEE_WRITE_REE0_UART1_S)
#define TEE_WRITE_REE0_UART1_V  0x00000001U
#define TEE_WRITE_REE0_UART1_S  5
/** TEE_WRITE_REE1_UART1 : R/W; bitpos: [6]; default: 0;
 *  Configures uart1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_UART1    (BIT(6))
#define TEE_WRITE_REE1_UART1_M  (TEE_WRITE_REE1_UART1_V << TEE_WRITE_REE1_UART1_S)
#define TEE_WRITE_REE1_UART1_V  0x00000001U
#define TEE_WRITE_REE1_UART1_S  6
/** TEE_WRITE_REE2_UART1 : R/W; bitpos: [7]; default: 0;
 *  Configures uart1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_UART1    (BIT(7))
#define TEE_WRITE_REE2_UART1_M  (TEE_WRITE_REE2_UART1_V << TEE_WRITE_REE2_UART1_S)
#define TEE_WRITE_REE2_UART1_V  0x00000001U
#define TEE_WRITE_REE2_UART1_S  7

/** TEE_UHCI_CTRL_REG register
 *  uhci read/write control register
 */
#define TEE_UHCI_CTRL_REG (DR_REG_TEE_BASE + 0x90)
/** TEE_READ_TEE_UHCI : R/W; bitpos: [0]; default: 1;
 *  Configures uhci registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_UHCI    (BIT(0))
#define TEE_READ_TEE_UHCI_M  (TEE_READ_TEE_UHCI_V << TEE_READ_TEE_UHCI_S)
#define TEE_READ_TEE_UHCI_V  0x00000001U
#define TEE_READ_TEE_UHCI_S  0
/** TEE_READ_REE0_UHCI : R/W; bitpos: [1]; default: 0;
 *  Configures uhci registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_UHCI    (BIT(1))
#define TEE_READ_REE0_UHCI_M  (TEE_READ_REE0_UHCI_V << TEE_READ_REE0_UHCI_S)
#define TEE_READ_REE0_UHCI_V  0x00000001U
#define TEE_READ_REE0_UHCI_S  1
/** TEE_READ_REE1_UHCI : R/W; bitpos: [2]; default: 0;
 *  Configures uhci registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_UHCI    (BIT(2))
#define TEE_READ_REE1_UHCI_M  (TEE_READ_REE1_UHCI_V << TEE_READ_REE1_UHCI_S)
#define TEE_READ_REE1_UHCI_V  0x00000001U
#define TEE_READ_REE1_UHCI_S  2
/** TEE_READ_REE2_UHCI : R/W; bitpos: [3]; default: 0;
 *  Configures uhci registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_UHCI    (BIT(3))
#define TEE_READ_REE2_UHCI_M  (TEE_READ_REE2_UHCI_V << TEE_READ_REE2_UHCI_S)
#define TEE_READ_REE2_UHCI_V  0x00000001U
#define TEE_READ_REE2_UHCI_S  3
/** TEE_WRITE_TEE_UHCI : R/W; bitpos: [4]; default: 1;
 *  Configures uhci registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_UHCI    (BIT(4))
#define TEE_WRITE_TEE_UHCI_M  (TEE_WRITE_TEE_UHCI_V << TEE_WRITE_TEE_UHCI_S)
#define TEE_WRITE_TEE_UHCI_V  0x00000001U
#define TEE_WRITE_TEE_UHCI_S  4
/** TEE_WRITE_REE0_UHCI : R/W; bitpos: [5]; default: 0;
 *  Configures uhci registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_UHCI    (BIT(5))
#define TEE_WRITE_REE0_UHCI_M  (TEE_WRITE_REE0_UHCI_V << TEE_WRITE_REE0_UHCI_S)
#define TEE_WRITE_REE0_UHCI_V  0x00000001U
#define TEE_WRITE_REE0_UHCI_S  5
/** TEE_WRITE_REE1_UHCI : R/W; bitpos: [6]; default: 0;
 *  Configures uhci registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_UHCI    (BIT(6))
#define TEE_WRITE_REE1_UHCI_M  (TEE_WRITE_REE1_UHCI_V << TEE_WRITE_REE1_UHCI_S)
#define TEE_WRITE_REE1_UHCI_V  0x00000001U
#define TEE_WRITE_REE1_UHCI_S  6
/** TEE_WRITE_REE2_UHCI : R/W; bitpos: [7]; default: 0;
 *  Configures uhci registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_UHCI    (BIT(7))
#define TEE_WRITE_REE2_UHCI_M  (TEE_WRITE_REE2_UHCI_V << TEE_WRITE_REE2_UHCI_S)
#define TEE_WRITE_REE2_UHCI_V  0x00000001U
#define TEE_WRITE_REE2_UHCI_S  7

/** TEE_I2C0_CTRL_REG register
 *  i2c0 read/write control register
 */
#define TEE_I2C0_CTRL_REG (DR_REG_TEE_BASE + 0x94)
/** TEE_READ_TEE_I2C0 : R/W; bitpos: [0]; default: 1;
 *  Configures i2c0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_I2C0    (BIT(0))
#define TEE_READ_TEE_I2C0_M  (TEE_READ_TEE_I2C0_V << TEE_READ_TEE_I2C0_S)
#define TEE_READ_TEE_I2C0_V  0x00000001U
#define TEE_READ_TEE_I2C0_S  0
/** TEE_READ_REE0_I2C0 : R/W; bitpos: [1]; default: 0;
 *  Configures i2c0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_I2C0    (BIT(1))
#define TEE_READ_REE0_I2C0_M  (TEE_READ_REE0_I2C0_V << TEE_READ_REE0_I2C0_S)
#define TEE_READ_REE0_I2C0_V  0x00000001U
#define TEE_READ_REE0_I2C0_S  1
/** TEE_READ_REE1_I2C0 : R/W; bitpos: [2]; default: 0;
 *  Configures i2c0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_I2C0    (BIT(2))
#define TEE_READ_REE1_I2C0_M  (TEE_READ_REE1_I2C0_V << TEE_READ_REE1_I2C0_S)
#define TEE_READ_REE1_I2C0_V  0x00000001U
#define TEE_READ_REE1_I2C0_S  2
/** TEE_READ_REE2_I2C0 : R/W; bitpos: [3]; default: 0;
 *  Configures i2c0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_I2C0    (BIT(3))
#define TEE_READ_REE2_I2C0_M  (TEE_READ_REE2_I2C0_V << TEE_READ_REE2_I2C0_S)
#define TEE_READ_REE2_I2C0_V  0x00000001U
#define TEE_READ_REE2_I2C0_S  3
/** TEE_WRITE_TEE_I2C0 : R/W; bitpos: [4]; default: 1;
 *  Configures i2c0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_I2C0    (BIT(4))
#define TEE_WRITE_TEE_I2C0_M  (TEE_WRITE_TEE_I2C0_V << TEE_WRITE_TEE_I2C0_S)
#define TEE_WRITE_TEE_I2C0_V  0x00000001U
#define TEE_WRITE_TEE_I2C0_S  4
/** TEE_WRITE_REE0_I2C0 : R/W; bitpos: [5]; default: 0;
 *  Configures i2c0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_I2C0    (BIT(5))
#define TEE_WRITE_REE0_I2C0_M  (TEE_WRITE_REE0_I2C0_V << TEE_WRITE_REE0_I2C0_S)
#define TEE_WRITE_REE0_I2C0_V  0x00000001U
#define TEE_WRITE_REE0_I2C0_S  5
/** TEE_WRITE_REE1_I2C0 : R/W; bitpos: [6]; default: 0;
 *  Configures i2c0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_I2C0    (BIT(6))
#define TEE_WRITE_REE1_I2C0_M  (TEE_WRITE_REE1_I2C0_V << TEE_WRITE_REE1_I2C0_S)
#define TEE_WRITE_REE1_I2C0_V  0x00000001U
#define TEE_WRITE_REE1_I2C0_S  6
/** TEE_WRITE_REE2_I2C0 : R/W; bitpos: [7]; default: 0;
 *  Configures i2c0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_I2C0    (BIT(7))
#define TEE_WRITE_REE2_I2C0_M  (TEE_WRITE_REE2_I2C0_V << TEE_WRITE_REE2_I2C0_S)
#define TEE_WRITE_REE2_I2C0_V  0x00000001U
#define TEE_WRITE_REE2_I2C0_S  7

/** TEE_I2C1_CTRL_REG register
 *  i2c1 read/write control register
 */
#define TEE_I2C1_CTRL_REG (DR_REG_TEE_BASE + 0x98)
/** TEE_READ_TEE_I2C1 : R/W; bitpos: [0]; default: 1;
 *  Configures i2c1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_I2C1    (BIT(0))
#define TEE_READ_TEE_I2C1_M  (TEE_READ_TEE_I2C1_V << TEE_READ_TEE_I2C1_S)
#define TEE_READ_TEE_I2C1_V  0x00000001U
#define TEE_READ_TEE_I2C1_S  0
/** TEE_READ_REE0_I2C1 : R/W; bitpos: [1]; default: 0;
 *  Configures i2c1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_I2C1    (BIT(1))
#define TEE_READ_REE0_I2C1_M  (TEE_READ_REE0_I2C1_V << TEE_READ_REE0_I2C1_S)
#define TEE_READ_REE0_I2C1_V  0x00000001U
#define TEE_READ_REE0_I2C1_S  1
/** TEE_READ_REE1_I2C1 : R/W; bitpos: [2]; default: 0;
 *  Configures i2c1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_I2C1    (BIT(2))
#define TEE_READ_REE1_I2C1_M  (TEE_READ_REE1_I2C1_V << TEE_READ_REE1_I2C1_S)
#define TEE_READ_REE1_I2C1_V  0x00000001U
#define TEE_READ_REE1_I2C1_S  2
/** TEE_READ_REE2_I2C1 : R/W; bitpos: [3]; default: 0;
 *  Configures i2c1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_I2C1    (BIT(3))
#define TEE_READ_REE2_I2C1_M  (TEE_READ_REE2_I2C1_V << TEE_READ_REE2_I2C1_S)
#define TEE_READ_REE2_I2C1_V  0x00000001U
#define TEE_READ_REE2_I2C1_S  3
/** TEE_WRITE_TEE_I2C1 : R/W; bitpos: [4]; default: 1;
 *  Configures i2c1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_I2C1    (BIT(4))
#define TEE_WRITE_TEE_I2C1_M  (TEE_WRITE_TEE_I2C1_V << TEE_WRITE_TEE_I2C1_S)
#define TEE_WRITE_TEE_I2C1_V  0x00000001U
#define TEE_WRITE_TEE_I2C1_S  4
/** TEE_WRITE_REE0_I2C1 : R/W; bitpos: [5]; default: 0;
 *  Configures i2c1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_I2C1    (BIT(5))
#define TEE_WRITE_REE0_I2C1_M  (TEE_WRITE_REE0_I2C1_V << TEE_WRITE_REE0_I2C1_S)
#define TEE_WRITE_REE0_I2C1_V  0x00000001U
#define TEE_WRITE_REE0_I2C1_S  5
/** TEE_WRITE_REE1_I2C1 : R/W; bitpos: [6]; default: 0;
 *  Configures i2c1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_I2C1    (BIT(6))
#define TEE_WRITE_REE1_I2C1_M  (TEE_WRITE_REE1_I2C1_V << TEE_WRITE_REE1_I2C1_S)
#define TEE_WRITE_REE1_I2C1_V  0x00000001U
#define TEE_WRITE_REE1_I2C1_S  6
/** TEE_WRITE_REE2_I2C1 : R/W; bitpos: [7]; default: 0;
 *  Configures i2c1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_I2C1    (BIT(7))
#define TEE_WRITE_REE2_I2C1_M  (TEE_WRITE_REE2_I2C1_V << TEE_WRITE_REE2_I2C1_S)
#define TEE_WRITE_REE2_I2C1_V  0x00000001U
#define TEE_WRITE_REE2_I2C1_S  7

/** TEE_I2S_CTRL_REG register
 *  i2s read/write control register
 */
#define TEE_I2S_CTRL_REG (DR_REG_TEE_BASE + 0x9c)
/** TEE_READ_TEE_I2S : R/W; bitpos: [0]; default: 1;
 *  Configures i2s registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_I2S    (BIT(0))
#define TEE_READ_TEE_I2S_M  (TEE_READ_TEE_I2S_V << TEE_READ_TEE_I2S_S)
#define TEE_READ_TEE_I2S_V  0x00000001U
#define TEE_READ_TEE_I2S_S  0
/** TEE_READ_REE0_I2S : R/W; bitpos: [1]; default: 0;
 *  Configures i2s registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_I2S    (BIT(1))
#define TEE_READ_REE0_I2S_M  (TEE_READ_REE0_I2S_V << TEE_READ_REE0_I2S_S)
#define TEE_READ_REE0_I2S_V  0x00000001U
#define TEE_READ_REE0_I2S_S  1
/** TEE_READ_REE1_I2S : R/W; bitpos: [2]; default: 0;
 *  Configures i2s registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_I2S    (BIT(2))
#define TEE_READ_REE1_I2S_M  (TEE_READ_REE1_I2S_V << TEE_READ_REE1_I2S_S)
#define TEE_READ_REE1_I2S_V  0x00000001U
#define TEE_READ_REE1_I2S_S  2
/** TEE_READ_REE2_I2S : R/W; bitpos: [3]; default: 0;
 *  Configures i2s registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_I2S    (BIT(3))
#define TEE_READ_REE2_I2S_M  (TEE_READ_REE2_I2S_V << TEE_READ_REE2_I2S_S)
#define TEE_READ_REE2_I2S_V  0x00000001U
#define TEE_READ_REE2_I2S_S  3
/** TEE_WRITE_TEE_I2S : R/W; bitpos: [4]; default: 1;
 *  Configures i2s registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_I2S    (BIT(4))
#define TEE_WRITE_TEE_I2S_M  (TEE_WRITE_TEE_I2S_V << TEE_WRITE_TEE_I2S_S)
#define TEE_WRITE_TEE_I2S_V  0x00000001U
#define TEE_WRITE_TEE_I2S_S  4
/** TEE_WRITE_REE0_I2S : R/W; bitpos: [5]; default: 0;
 *  Configures i2s registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_I2S    (BIT(5))
#define TEE_WRITE_REE0_I2S_M  (TEE_WRITE_REE0_I2S_V << TEE_WRITE_REE0_I2S_S)
#define TEE_WRITE_REE0_I2S_V  0x00000001U
#define TEE_WRITE_REE0_I2S_S  5
/** TEE_WRITE_REE1_I2S : R/W; bitpos: [6]; default: 0;
 *  Configures i2s registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_I2S    (BIT(6))
#define TEE_WRITE_REE1_I2S_M  (TEE_WRITE_REE1_I2S_V << TEE_WRITE_REE1_I2S_S)
#define TEE_WRITE_REE1_I2S_V  0x00000001U
#define TEE_WRITE_REE1_I2S_S  6
/** TEE_WRITE_REE2_I2S : R/W; bitpos: [7]; default: 0;
 *  Configures i2s registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_I2S    (BIT(7))
#define TEE_WRITE_REE2_I2S_M  (TEE_WRITE_REE2_I2S_V << TEE_WRITE_REE2_I2S_S)
#define TEE_WRITE_REE2_I2S_V  0x00000001U
#define TEE_WRITE_REE2_I2S_S  7

/** TEE_PARL_IO_CTRL_REG register
 *  parl_io read/write control register
 */
#define TEE_PARL_IO_CTRL_REG (DR_REG_TEE_BASE + 0xa0)
/** TEE_READ_TEE_PARL_IO : R/W; bitpos: [0]; default: 1;
 *  Configures parl_io registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PARL_IO    (BIT(0))
#define TEE_READ_TEE_PARL_IO_M  (TEE_READ_TEE_PARL_IO_V << TEE_READ_TEE_PARL_IO_S)
#define TEE_READ_TEE_PARL_IO_V  0x00000001U
#define TEE_READ_TEE_PARL_IO_S  0
/** TEE_READ_REE0_PARL_IO : R/W; bitpos: [1]; default: 0;
 *  Configures parl_io registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PARL_IO    (BIT(1))
#define TEE_READ_REE0_PARL_IO_M  (TEE_READ_REE0_PARL_IO_V << TEE_READ_REE0_PARL_IO_S)
#define TEE_READ_REE0_PARL_IO_V  0x00000001U
#define TEE_READ_REE0_PARL_IO_S  1
/** TEE_READ_REE1_PARL_IO : R/W; bitpos: [2]; default: 0;
 *  Configures parl_io registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PARL_IO    (BIT(2))
#define TEE_READ_REE1_PARL_IO_M  (TEE_READ_REE1_PARL_IO_V << TEE_READ_REE1_PARL_IO_S)
#define TEE_READ_REE1_PARL_IO_V  0x00000001U
#define TEE_READ_REE1_PARL_IO_S  2
/** TEE_READ_REE2_PARL_IO : R/W; bitpos: [3]; default: 0;
 *  Configures parl_io registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PARL_IO    (BIT(3))
#define TEE_READ_REE2_PARL_IO_M  (TEE_READ_REE2_PARL_IO_V << TEE_READ_REE2_PARL_IO_S)
#define TEE_READ_REE2_PARL_IO_V  0x00000001U
#define TEE_READ_REE2_PARL_IO_S  3
/** TEE_WRITE_TEE_PARL_IO : R/W; bitpos: [4]; default: 1;
 *  Configures parl_io registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PARL_IO    (BIT(4))
#define TEE_WRITE_TEE_PARL_IO_M  (TEE_WRITE_TEE_PARL_IO_V << TEE_WRITE_TEE_PARL_IO_S)
#define TEE_WRITE_TEE_PARL_IO_V  0x00000001U
#define TEE_WRITE_TEE_PARL_IO_S  4
/** TEE_WRITE_REE0_PARL_IO : R/W; bitpos: [5]; default: 0;
 *  Configures parl_io registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PARL_IO    (BIT(5))
#define TEE_WRITE_REE0_PARL_IO_M  (TEE_WRITE_REE0_PARL_IO_V << TEE_WRITE_REE0_PARL_IO_S)
#define TEE_WRITE_REE0_PARL_IO_V  0x00000001U
#define TEE_WRITE_REE0_PARL_IO_S  5
/** TEE_WRITE_REE1_PARL_IO : R/W; bitpos: [6]; default: 0;
 *  Configures parl_io registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PARL_IO    (BIT(6))
#define TEE_WRITE_REE1_PARL_IO_M  (TEE_WRITE_REE1_PARL_IO_V << TEE_WRITE_REE1_PARL_IO_S)
#define TEE_WRITE_REE1_PARL_IO_V  0x00000001U
#define TEE_WRITE_REE1_PARL_IO_S  6
/** TEE_WRITE_REE2_PARL_IO : R/W; bitpos: [7]; default: 0;
 *  Configures parl_io registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PARL_IO    (BIT(7))
#define TEE_WRITE_REE2_PARL_IO_M  (TEE_WRITE_REE2_PARL_IO_V << TEE_WRITE_REE2_PARL_IO_S)
#define TEE_WRITE_REE2_PARL_IO_V  0x00000001U
#define TEE_WRITE_REE2_PARL_IO_S  7

/** TEE_PWM0_CTRL_REG register
 *  pwm0 read/write control register
 */
#define TEE_PWM0_CTRL_REG (DR_REG_TEE_BASE + 0xa4)
/** TEE_READ_TEE_PWM0 : R/W; bitpos: [0]; default: 1;
 *  Configures pwm0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PWM0    (BIT(0))
#define TEE_READ_TEE_PWM0_M  (TEE_READ_TEE_PWM0_V << TEE_READ_TEE_PWM0_S)
#define TEE_READ_TEE_PWM0_V  0x00000001U
#define TEE_READ_TEE_PWM0_S  0
/** TEE_READ_REE0_PWM0 : R/W; bitpos: [1]; default: 0;
 *  Configures pwm0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PWM0    (BIT(1))
#define TEE_READ_REE0_PWM0_M  (TEE_READ_REE0_PWM0_V << TEE_READ_REE0_PWM0_S)
#define TEE_READ_REE0_PWM0_V  0x00000001U
#define TEE_READ_REE0_PWM0_S  1
/** TEE_READ_REE1_PWM0 : R/W; bitpos: [2]; default: 0;
 *  Configures pwm0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PWM0    (BIT(2))
#define TEE_READ_REE1_PWM0_M  (TEE_READ_REE1_PWM0_V << TEE_READ_REE1_PWM0_S)
#define TEE_READ_REE1_PWM0_V  0x00000001U
#define TEE_READ_REE1_PWM0_S  2
/** TEE_READ_REE2_PWM0 : R/W; bitpos: [3]; default: 0;
 *  Configures pwm0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PWM0    (BIT(3))
#define TEE_READ_REE2_PWM0_M  (TEE_READ_REE2_PWM0_V << TEE_READ_REE2_PWM0_S)
#define TEE_READ_REE2_PWM0_V  0x00000001U
#define TEE_READ_REE2_PWM0_S  3
/** TEE_WRITE_TEE_PWM0 : R/W; bitpos: [4]; default: 1;
 *  Configures pwm0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PWM0    (BIT(4))
#define TEE_WRITE_TEE_PWM0_M  (TEE_WRITE_TEE_PWM0_V << TEE_WRITE_TEE_PWM0_S)
#define TEE_WRITE_TEE_PWM0_V  0x00000001U
#define TEE_WRITE_TEE_PWM0_S  4
/** TEE_WRITE_REE0_PWM0 : R/W; bitpos: [5]; default: 0;
 *  Configures pwm0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PWM0    (BIT(5))
#define TEE_WRITE_REE0_PWM0_M  (TEE_WRITE_REE0_PWM0_V << TEE_WRITE_REE0_PWM0_S)
#define TEE_WRITE_REE0_PWM0_V  0x00000001U
#define TEE_WRITE_REE0_PWM0_S  5
/** TEE_WRITE_REE1_PWM0 : R/W; bitpos: [6]; default: 0;
 *  Configures pwm0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PWM0    (BIT(6))
#define TEE_WRITE_REE1_PWM0_M  (TEE_WRITE_REE1_PWM0_V << TEE_WRITE_REE1_PWM0_S)
#define TEE_WRITE_REE1_PWM0_V  0x00000001U
#define TEE_WRITE_REE1_PWM0_S  6
/** TEE_WRITE_REE2_PWM0 : R/W; bitpos: [7]; default: 0;
 *  Configures pwm0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PWM0    (BIT(7))
#define TEE_WRITE_REE2_PWM0_M  (TEE_WRITE_REE2_PWM0_V << TEE_WRITE_REE2_PWM0_S)
#define TEE_WRITE_REE2_PWM0_V  0x00000001U
#define TEE_WRITE_REE2_PWM0_S  7

/** TEE_PWM1_CTRL_REG register
 *  pwm1 read/write control register
 */
#define TEE_PWM1_CTRL_REG (DR_REG_TEE_BASE + 0xa8)
/** TEE_READ_TEE_PWM1 : R/W; bitpos: [0]; default: 1;
 *  Configures pwm1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PWM1    (BIT(0))
#define TEE_READ_TEE_PWM1_M  (TEE_READ_TEE_PWM1_V << TEE_READ_TEE_PWM1_S)
#define TEE_READ_TEE_PWM1_V  0x00000001U
#define TEE_READ_TEE_PWM1_S  0
/** TEE_READ_REE0_PWM1 : R/W; bitpos: [1]; default: 0;
 *  Configures pwm1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PWM1    (BIT(1))
#define TEE_READ_REE0_PWM1_M  (TEE_READ_REE0_PWM1_V << TEE_READ_REE0_PWM1_S)
#define TEE_READ_REE0_PWM1_V  0x00000001U
#define TEE_READ_REE0_PWM1_S  1
/** TEE_READ_REE1_PWM1 : R/W; bitpos: [2]; default: 0;
 *  Configures pwm1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PWM1    (BIT(2))
#define TEE_READ_REE1_PWM1_M  (TEE_READ_REE1_PWM1_V << TEE_READ_REE1_PWM1_S)
#define TEE_READ_REE1_PWM1_V  0x00000001U
#define TEE_READ_REE1_PWM1_S  2
/** TEE_READ_REE2_PWM1 : R/W; bitpos: [3]; default: 0;
 *  Configures pwm1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PWM1    (BIT(3))
#define TEE_READ_REE2_PWM1_M  (TEE_READ_REE2_PWM1_V << TEE_READ_REE2_PWM1_S)
#define TEE_READ_REE2_PWM1_V  0x00000001U
#define TEE_READ_REE2_PWM1_S  3
/** TEE_WRITE_TEE_PWM1 : R/W; bitpos: [4]; default: 1;
 *  Configures pwm1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PWM1    (BIT(4))
#define TEE_WRITE_TEE_PWM1_M  (TEE_WRITE_TEE_PWM1_V << TEE_WRITE_TEE_PWM1_S)
#define TEE_WRITE_TEE_PWM1_V  0x00000001U
#define TEE_WRITE_TEE_PWM1_S  4
/** TEE_WRITE_REE0_PWM1 : R/W; bitpos: [5]; default: 0;
 *  Configures pwm1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PWM1    (BIT(5))
#define TEE_WRITE_REE0_PWM1_M  (TEE_WRITE_REE0_PWM1_V << TEE_WRITE_REE0_PWM1_S)
#define TEE_WRITE_REE0_PWM1_V  0x00000001U
#define TEE_WRITE_REE0_PWM1_S  5
/** TEE_WRITE_REE1_PWM1 : R/W; bitpos: [6]; default: 0;
 *  Configures pwm1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PWM1    (BIT(6))
#define TEE_WRITE_REE1_PWM1_M  (TEE_WRITE_REE1_PWM1_V << TEE_WRITE_REE1_PWM1_S)
#define TEE_WRITE_REE1_PWM1_V  0x00000001U
#define TEE_WRITE_REE1_PWM1_S  6
/** TEE_WRITE_REE2_PWM1 : R/W; bitpos: [7]; default: 0;
 *  Configures pwm1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PWM1    (BIT(7))
#define TEE_WRITE_REE2_PWM1_M  (TEE_WRITE_REE2_PWM1_V << TEE_WRITE_REE2_PWM1_S)
#define TEE_WRITE_REE2_PWM1_V  0x00000001U
#define TEE_WRITE_REE2_PWM1_S  7

/** TEE_LEDC_CTRL_REG register
 *  ledc read/write control register
 */
#define TEE_LEDC_CTRL_REG (DR_REG_TEE_BASE + 0xac)
/** TEE_READ_TEE_LEDC : R/W; bitpos: [0]; default: 1;
 *  Configures ledc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_LEDC    (BIT(0))
#define TEE_READ_TEE_LEDC_M  (TEE_READ_TEE_LEDC_V << TEE_READ_TEE_LEDC_S)
#define TEE_READ_TEE_LEDC_V  0x00000001U
#define TEE_READ_TEE_LEDC_S  0
/** TEE_READ_REE0_LEDC : R/W; bitpos: [1]; default: 0;
 *  Configures ledc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_LEDC    (BIT(1))
#define TEE_READ_REE0_LEDC_M  (TEE_READ_REE0_LEDC_V << TEE_READ_REE0_LEDC_S)
#define TEE_READ_REE0_LEDC_V  0x00000001U
#define TEE_READ_REE0_LEDC_S  1
/** TEE_READ_REE1_LEDC : R/W; bitpos: [2]; default: 0;
 *  Configures ledc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_LEDC    (BIT(2))
#define TEE_READ_REE1_LEDC_M  (TEE_READ_REE1_LEDC_V << TEE_READ_REE1_LEDC_S)
#define TEE_READ_REE1_LEDC_V  0x00000001U
#define TEE_READ_REE1_LEDC_S  2
/** TEE_READ_REE2_LEDC : R/W; bitpos: [3]; default: 0;
 *  Configures ledc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_LEDC    (BIT(3))
#define TEE_READ_REE2_LEDC_M  (TEE_READ_REE2_LEDC_V << TEE_READ_REE2_LEDC_S)
#define TEE_READ_REE2_LEDC_V  0x00000001U
#define TEE_READ_REE2_LEDC_S  3
/** TEE_WRITE_TEE_LEDC : R/W; bitpos: [4]; default: 1;
 *  Configures ledc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_LEDC    (BIT(4))
#define TEE_WRITE_TEE_LEDC_M  (TEE_WRITE_TEE_LEDC_V << TEE_WRITE_TEE_LEDC_S)
#define TEE_WRITE_TEE_LEDC_V  0x00000001U
#define TEE_WRITE_TEE_LEDC_S  4
/** TEE_WRITE_REE0_LEDC : R/W; bitpos: [5]; default: 0;
 *  Configures ledc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_LEDC    (BIT(5))
#define TEE_WRITE_REE0_LEDC_M  (TEE_WRITE_REE0_LEDC_V << TEE_WRITE_REE0_LEDC_S)
#define TEE_WRITE_REE0_LEDC_V  0x00000001U
#define TEE_WRITE_REE0_LEDC_S  5
/** TEE_WRITE_REE1_LEDC : R/W; bitpos: [6]; default: 0;
 *  Configures ledc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_LEDC    (BIT(6))
#define TEE_WRITE_REE1_LEDC_M  (TEE_WRITE_REE1_LEDC_V << TEE_WRITE_REE1_LEDC_S)
#define TEE_WRITE_REE1_LEDC_V  0x00000001U
#define TEE_WRITE_REE1_LEDC_S  6
/** TEE_WRITE_REE2_LEDC : R/W; bitpos: [7]; default: 0;
 *  Configures ledc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_LEDC    (BIT(7))
#define TEE_WRITE_REE2_LEDC_M  (TEE_WRITE_REE2_LEDC_V << TEE_WRITE_REE2_LEDC_S)
#define TEE_WRITE_REE2_LEDC_V  0x00000001U
#define TEE_WRITE_REE2_LEDC_S  7

/** TEE_CAN_CTRL_REG register
 *  can read/write control register
 */
#define TEE_CAN_CTRL_REG (DR_REG_TEE_BASE + 0xb0)
/** TEE_READ_TEE_CAN : R/W; bitpos: [0]; default: 1;
 *  Configures can registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_CAN    (BIT(0))
#define TEE_READ_TEE_CAN_M  (TEE_READ_TEE_CAN_V << TEE_READ_TEE_CAN_S)
#define TEE_READ_TEE_CAN_V  0x00000001U
#define TEE_READ_TEE_CAN_S  0
/** TEE_READ_REE0_CAN : R/W; bitpos: [1]; default: 0;
 *  Configures can registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_CAN    (BIT(1))
#define TEE_READ_REE0_CAN_M  (TEE_READ_REE0_CAN_V << TEE_READ_REE0_CAN_S)
#define TEE_READ_REE0_CAN_V  0x00000001U
#define TEE_READ_REE0_CAN_S  1
/** TEE_READ_REE1_CAN : R/W; bitpos: [2]; default: 0;
 *  Configures can registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_CAN    (BIT(2))
#define TEE_READ_REE1_CAN_M  (TEE_READ_REE1_CAN_V << TEE_READ_REE1_CAN_S)
#define TEE_READ_REE1_CAN_V  0x00000001U
#define TEE_READ_REE1_CAN_S  2
/** TEE_READ_REE2_CAN : R/W; bitpos: [3]; default: 0;
 *  Configures can registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_CAN    (BIT(3))
#define TEE_READ_REE2_CAN_M  (TEE_READ_REE2_CAN_V << TEE_READ_REE2_CAN_S)
#define TEE_READ_REE2_CAN_V  0x00000001U
#define TEE_READ_REE2_CAN_S  3
/** TEE_WRITE_TEE_CAN : R/W; bitpos: [4]; default: 1;
 *  Configures can registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_CAN    (BIT(4))
#define TEE_WRITE_TEE_CAN_M  (TEE_WRITE_TEE_CAN_V << TEE_WRITE_TEE_CAN_S)
#define TEE_WRITE_TEE_CAN_V  0x00000001U
#define TEE_WRITE_TEE_CAN_S  4
/** TEE_WRITE_REE0_CAN : R/W; bitpos: [5]; default: 0;
 *  Configures can registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_CAN    (BIT(5))
#define TEE_WRITE_REE0_CAN_M  (TEE_WRITE_REE0_CAN_V << TEE_WRITE_REE0_CAN_S)
#define TEE_WRITE_REE0_CAN_V  0x00000001U
#define TEE_WRITE_REE0_CAN_S  5
/** TEE_WRITE_REE1_CAN : R/W; bitpos: [6]; default: 0;
 *  Configures can registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_CAN    (BIT(6))
#define TEE_WRITE_REE1_CAN_M  (TEE_WRITE_REE1_CAN_V << TEE_WRITE_REE1_CAN_S)
#define TEE_WRITE_REE1_CAN_V  0x00000001U
#define TEE_WRITE_REE1_CAN_S  6
/** TEE_WRITE_REE2_CAN : R/W; bitpos: [7]; default: 0;
 *  Configures can registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_CAN    (BIT(7))
#define TEE_WRITE_REE2_CAN_M  (TEE_WRITE_REE2_CAN_V << TEE_WRITE_REE2_CAN_S)
#define TEE_WRITE_REE2_CAN_V  0x00000001U
#define TEE_WRITE_REE2_CAN_S  7

/** TEE_USB_SERIAL_JTAG_CTRL_REG register
 *  usb_serial_jtag read/write control register
 */
#define TEE_USB_SERIAL_JTAG_CTRL_REG (DR_REG_TEE_BASE + 0xb4)
/** TEE_READ_TEE_USB_SERIAL_JTAG : R/W; bitpos: [0]; default: 1;
 *  Configures usb_serial_jtag registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_USB_SERIAL_JTAG    (BIT(0))
#define TEE_READ_TEE_USB_SERIAL_JTAG_M  (TEE_READ_TEE_USB_SERIAL_JTAG_V << TEE_READ_TEE_USB_SERIAL_JTAG_S)
#define TEE_READ_TEE_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_TEE_USB_SERIAL_JTAG_S  0
/** TEE_READ_REE0_USB_SERIAL_JTAG : R/W; bitpos: [1]; default: 0;
 *  Configures usb_serial_jtag registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_USB_SERIAL_JTAG    (BIT(1))
#define TEE_READ_REE0_USB_SERIAL_JTAG_M  (TEE_READ_REE0_USB_SERIAL_JTAG_V << TEE_READ_REE0_USB_SERIAL_JTAG_S)
#define TEE_READ_REE0_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_REE0_USB_SERIAL_JTAG_S  1
/** TEE_READ_REE1_USB_SERIAL_JTAG : R/W; bitpos: [2]; default: 0;
 *  Configures usb_serial_jtag registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_USB_SERIAL_JTAG    (BIT(2))
#define TEE_READ_REE1_USB_SERIAL_JTAG_M  (TEE_READ_REE1_USB_SERIAL_JTAG_V << TEE_READ_REE1_USB_SERIAL_JTAG_S)
#define TEE_READ_REE1_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_REE1_USB_SERIAL_JTAG_S  2
/** TEE_READ_REE2_USB_SERIAL_JTAG : R/W; bitpos: [3]; default: 0;
 *  Configures usb_serial_jtag registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_USB_SERIAL_JTAG    (BIT(3))
#define TEE_READ_REE2_USB_SERIAL_JTAG_M  (TEE_READ_REE2_USB_SERIAL_JTAG_V << TEE_READ_REE2_USB_SERIAL_JTAG_S)
#define TEE_READ_REE2_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_REE2_USB_SERIAL_JTAG_S  3
/** TEE_WRITE_TEE_USB_SERIAL_JTAG : R/W; bitpos: [4]; default: 1;
 *  Configures usb_serial_jtag registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_USB_SERIAL_JTAG    (BIT(4))
#define TEE_WRITE_TEE_USB_SERIAL_JTAG_M  (TEE_WRITE_TEE_USB_SERIAL_JTAG_V << TEE_WRITE_TEE_USB_SERIAL_JTAG_S)
#define TEE_WRITE_TEE_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_TEE_USB_SERIAL_JTAG_S  4
/** TEE_WRITE_REE0_USB_SERIAL_JTAG : R/W; bitpos: [5]; default: 0;
 *  Configures usb_serial_jtag registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_USB_SERIAL_JTAG    (BIT(5))
#define TEE_WRITE_REE0_USB_SERIAL_JTAG_M  (TEE_WRITE_REE0_USB_SERIAL_JTAG_V << TEE_WRITE_REE0_USB_SERIAL_JTAG_S)
#define TEE_WRITE_REE0_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_REE0_USB_SERIAL_JTAG_S  5
/** TEE_WRITE_REE1_USB_SERIAL_JTAG : R/W; bitpos: [6]; default: 0;
 *  Configures usb_serial_jtag registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_USB_SERIAL_JTAG    (BIT(6))
#define TEE_WRITE_REE1_USB_SERIAL_JTAG_M  (TEE_WRITE_REE1_USB_SERIAL_JTAG_V << TEE_WRITE_REE1_USB_SERIAL_JTAG_S)
#define TEE_WRITE_REE1_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_REE1_USB_SERIAL_JTAG_S  6
/** TEE_WRITE_REE2_USB_SERIAL_JTAG : R/W; bitpos: [7]; default: 0;
 *  Configures usb_serial_jtag registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_USB_SERIAL_JTAG    (BIT(7))
#define TEE_WRITE_REE2_USB_SERIAL_JTAG_M  (TEE_WRITE_REE2_USB_SERIAL_JTAG_V << TEE_WRITE_REE2_USB_SERIAL_JTAG_S)
#define TEE_WRITE_REE2_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_REE2_USB_SERIAL_JTAG_S  7

/** TEE_RMT_CTRL_REG register
 *  rmt read/write control register
 */
#define TEE_RMT_CTRL_REG (DR_REG_TEE_BASE + 0xb8)
/** TEE_READ_TEE_RMT : R/W; bitpos: [0]; default: 1;
 *  Configures rmt registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_RMT    (BIT(0))
#define TEE_READ_TEE_RMT_M  (TEE_READ_TEE_RMT_V << TEE_READ_TEE_RMT_S)
#define TEE_READ_TEE_RMT_V  0x00000001U
#define TEE_READ_TEE_RMT_S  0
/** TEE_READ_REE0_RMT : R/W; bitpos: [1]; default: 0;
 *  Configures rmt registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_RMT    (BIT(1))
#define TEE_READ_REE0_RMT_M  (TEE_READ_REE0_RMT_V << TEE_READ_REE0_RMT_S)
#define TEE_READ_REE0_RMT_V  0x00000001U
#define TEE_READ_REE0_RMT_S  1
/** TEE_READ_REE1_RMT : R/W; bitpos: [2]; default: 0;
 *  Configures rmt registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_RMT    (BIT(2))
#define TEE_READ_REE1_RMT_M  (TEE_READ_REE1_RMT_V << TEE_READ_REE1_RMT_S)
#define TEE_READ_REE1_RMT_V  0x00000001U
#define TEE_READ_REE1_RMT_S  2
/** TEE_READ_REE2_RMT : R/W; bitpos: [3]; default: 0;
 *  Configures rmt registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_RMT    (BIT(3))
#define TEE_READ_REE2_RMT_M  (TEE_READ_REE2_RMT_V << TEE_READ_REE2_RMT_S)
#define TEE_READ_REE2_RMT_V  0x00000001U
#define TEE_READ_REE2_RMT_S  3
/** TEE_WRITE_TEE_RMT : R/W; bitpos: [4]; default: 1;
 *  Configures rmt registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_RMT    (BIT(4))
#define TEE_WRITE_TEE_RMT_M  (TEE_WRITE_TEE_RMT_V << TEE_WRITE_TEE_RMT_S)
#define TEE_WRITE_TEE_RMT_V  0x00000001U
#define TEE_WRITE_TEE_RMT_S  4
/** TEE_WRITE_REE0_RMT : R/W; bitpos: [5]; default: 0;
 *  Configures rmt registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_RMT    (BIT(5))
#define TEE_WRITE_REE0_RMT_M  (TEE_WRITE_REE0_RMT_V << TEE_WRITE_REE0_RMT_S)
#define TEE_WRITE_REE0_RMT_V  0x00000001U
#define TEE_WRITE_REE0_RMT_S  5
/** TEE_WRITE_REE1_RMT : R/W; bitpos: [6]; default: 0;
 *  Configures rmt registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_RMT    (BIT(6))
#define TEE_WRITE_REE1_RMT_M  (TEE_WRITE_REE1_RMT_V << TEE_WRITE_REE1_RMT_S)
#define TEE_WRITE_REE1_RMT_V  0x00000001U
#define TEE_WRITE_REE1_RMT_S  6
/** TEE_WRITE_REE2_RMT : R/W; bitpos: [7]; default: 0;
 *  Configures rmt registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_RMT    (BIT(7))
#define TEE_WRITE_REE2_RMT_M  (TEE_WRITE_REE2_RMT_V << TEE_WRITE_REE2_RMT_S)
#define TEE_WRITE_REE2_RMT_V  0x00000001U
#define TEE_WRITE_REE2_RMT_S  7

/** TEE_GDMA_CTRL_REG register
 *  gdma read/write control register
 */
#define TEE_GDMA_CTRL_REG (DR_REG_TEE_BASE + 0xbc)
/** TEE_READ_TEE_GDMA : R/W; bitpos: [0]; default: 1;
 *  Configures gdma registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_GDMA    (BIT(0))
#define TEE_READ_TEE_GDMA_M  (TEE_READ_TEE_GDMA_V << TEE_READ_TEE_GDMA_S)
#define TEE_READ_TEE_GDMA_V  0x00000001U
#define TEE_READ_TEE_GDMA_S  0
/** TEE_READ_REE0_GDMA : R/W; bitpos: [1]; default: 0;
 *  Configures gdma registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_GDMA    (BIT(1))
#define TEE_READ_REE0_GDMA_M  (TEE_READ_REE0_GDMA_V << TEE_READ_REE0_GDMA_S)
#define TEE_READ_REE0_GDMA_V  0x00000001U
#define TEE_READ_REE0_GDMA_S  1
/** TEE_READ_REE1_GDMA : R/W; bitpos: [2]; default: 0;
 *  Configures gdma registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_GDMA    (BIT(2))
#define TEE_READ_REE1_GDMA_M  (TEE_READ_REE1_GDMA_V << TEE_READ_REE1_GDMA_S)
#define TEE_READ_REE1_GDMA_V  0x00000001U
#define TEE_READ_REE1_GDMA_S  2
/** TEE_READ_REE2_GDMA : R/W; bitpos: [3]; default: 0;
 *  Configures gdma registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_GDMA    (BIT(3))
#define TEE_READ_REE2_GDMA_M  (TEE_READ_REE2_GDMA_V << TEE_READ_REE2_GDMA_S)
#define TEE_READ_REE2_GDMA_V  0x00000001U
#define TEE_READ_REE2_GDMA_S  3
/** TEE_WRITE_TEE_GDMA : R/W; bitpos: [4]; default: 1;
 *  Configures gdma registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_GDMA    (BIT(4))
#define TEE_WRITE_TEE_GDMA_M  (TEE_WRITE_TEE_GDMA_V << TEE_WRITE_TEE_GDMA_S)
#define TEE_WRITE_TEE_GDMA_V  0x00000001U
#define TEE_WRITE_TEE_GDMA_S  4
/** TEE_WRITE_REE0_GDMA : R/W; bitpos: [5]; default: 0;
 *  Configures gdma registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_GDMA    (BIT(5))
#define TEE_WRITE_REE0_GDMA_M  (TEE_WRITE_REE0_GDMA_V << TEE_WRITE_REE0_GDMA_S)
#define TEE_WRITE_REE0_GDMA_V  0x00000001U
#define TEE_WRITE_REE0_GDMA_S  5
/** TEE_WRITE_REE1_GDMA : R/W; bitpos: [6]; default: 0;
 *  Configures gdma registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_GDMA    (BIT(6))
#define TEE_WRITE_REE1_GDMA_M  (TEE_WRITE_REE1_GDMA_V << TEE_WRITE_REE1_GDMA_S)
#define TEE_WRITE_REE1_GDMA_V  0x00000001U
#define TEE_WRITE_REE1_GDMA_S  6
/** TEE_WRITE_REE2_GDMA : R/W; bitpos: [7]; default: 0;
 *  Configures gdma registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_GDMA    (BIT(7))
#define TEE_WRITE_REE2_GDMA_M  (TEE_WRITE_REE2_GDMA_V << TEE_WRITE_REE2_GDMA_S)
#define TEE_WRITE_REE2_GDMA_V  0x00000001U
#define TEE_WRITE_REE2_GDMA_S  7

/** TEE_REGDMA_CTRL_REG register
 *  regdma read/write control register
 */
#define TEE_REGDMA_CTRL_REG (DR_REG_TEE_BASE + 0xc0)
/** TEE_READ_TEE_REGDMA : R/W; bitpos: [0]; default: 1;
 *  Configures regdma registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_REGDMA    (BIT(0))
#define TEE_READ_TEE_REGDMA_M  (TEE_READ_TEE_REGDMA_V << TEE_READ_TEE_REGDMA_S)
#define TEE_READ_TEE_REGDMA_V  0x00000001U
#define TEE_READ_TEE_REGDMA_S  0
/** TEE_READ_REE0_REGDMA : R/W; bitpos: [1]; default: 0;
 *  Configures regdma registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_REGDMA    (BIT(1))
#define TEE_READ_REE0_REGDMA_M  (TEE_READ_REE0_REGDMA_V << TEE_READ_REE0_REGDMA_S)
#define TEE_READ_REE0_REGDMA_V  0x00000001U
#define TEE_READ_REE0_REGDMA_S  1
/** TEE_READ_REE1_REGDMA : R/W; bitpos: [2]; default: 0;
 *  Configures regdma registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_REGDMA    (BIT(2))
#define TEE_READ_REE1_REGDMA_M  (TEE_READ_REE1_REGDMA_V << TEE_READ_REE1_REGDMA_S)
#define TEE_READ_REE1_REGDMA_V  0x00000001U
#define TEE_READ_REE1_REGDMA_S  2
/** TEE_READ_REE2_REGDMA : R/W; bitpos: [3]; default: 0;
 *  Configures regdma registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_REGDMA    (BIT(3))
#define TEE_READ_REE2_REGDMA_M  (TEE_READ_REE2_REGDMA_V << TEE_READ_REE2_REGDMA_S)
#define TEE_READ_REE2_REGDMA_V  0x00000001U
#define TEE_READ_REE2_REGDMA_S  3
/** TEE_WRITE_TEE_REGDMA : R/W; bitpos: [4]; default: 1;
 *  Configures regdma registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_REGDMA    (BIT(4))
#define TEE_WRITE_TEE_REGDMA_M  (TEE_WRITE_TEE_REGDMA_V << TEE_WRITE_TEE_REGDMA_S)
#define TEE_WRITE_TEE_REGDMA_V  0x00000001U
#define TEE_WRITE_TEE_REGDMA_S  4
/** TEE_WRITE_REE0_REGDMA : R/W; bitpos: [5]; default: 0;
 *  Configures regdma registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_REGDMA    (BIT(5))
#define TEE_WRITE_REE0_REGDMA_M  (TEE_WRITE_REE0_REGDMA_V << TEE_WRITE_REE0_REGDMA_S)
#define TEE_WRITE_REE0_REGDMA_V  0x00000001U
#define TEE_WRITE_REE0_REGDMA_S  5
/** TEE_WRITE_REE1_REGDMA : R/W; bitpos: [6]; default: 0;
 *  Configures regdma registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_REGDMA    (BIT(6))
#define TEE_WRITE_REE1_REGDMA_M  (TEE_WRITE_REE1_REGDMA_V << TEE_WRITE_REE1_REGDMA_S)
#define TEE_WRITE_REE1_REGDMA_V  0x00000001U
#define TEE_WRITE_REE1_REGDMA_S  6
/** TEE_WRITE_REE2_REGDMA : R/W; bitpos: [7]; default: 0;
 *  Configures regdma registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_REGDMA    (BIT(7))
#define TEE_WRITE_REE2_REGDMA_M  (TEE_WRITE_REE2_REGDMA_V << TEE_WRITE_REE2_REGDMA_S)
#define TEE_WRITE_REE2_REGDMA_V  0x00000001U
#define TEE_WRITE_REE2_REGDMA_S  7

/** TEE_ETM_CTRL_REG register
 *  etm read/write control register
 */
#define TEE_ETM_CTRL_REG (DR_REG_TEE_BASE + 0xc4)
/** TEE_READ_TEE_ETM : R/W; bitpos: [0]; default: 1;
 *  Configures etm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_ETM    (BIT(0))
#define TEE_READ_TEE_ETM_M  (TEE_READ_TEE_ETM_V << TEE_READ_TEE_ETM_S)
#define TEE_READ_TEE_ETM_V  0x00000001U
#define TEE_READ_TEE_ETM_S  0
/** TEE_READ_REE0_ETM : R/W; bitpos: [1]; default: 0;
 *  Configures etm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_ETM    (BIT(1))
#define TEE_READ_REE0_ETM_M  (TEE_READ_REE0_ETM_V << TEE_READ_REE0_ETM_S)
#define TEE_READ_REE0_ETM_V  0x00000001U
#define TEE_READ_REE0_ETM_S  1
/** TEE_READ_REE1_ETM : R/W; bitpos: [2]; default: 0;
 *  Configures etm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_ETM    (BIT(2))
#define TEE_READ_REE1_ETM_M  (TEE_READ_REE1_ETM_V << TEE_READ_REE1_ETM_S)
#define TEE_READ_REE1_ETM_V  0x00000001U
#define TEE_READ_REE1_ETM_S  2
/** TEE_READ_REE2_ETM : R/W; bitpos: [3]; default: 0;
 *  Configures etm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_ETM    (BIT(3))
#define TEE_READ_REE2_ETM_M  (TEE_READ_REE2_ETM_V << TEE_READ_REE2_ETM_S)
#define TEE_READ_REE2_ETM_V  0x00000001U
#define TEE_READ_REE2_ETM_S  3
/** TEE_WRITE_TEE_ETM : R/W; bitpos: [4]; default: 1;
 *  Configures etm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_ETM    (BIT(4))
#define TEE_WRITE_TEE_ETM_M  (TEE_WRITE_TEE_ETM_V << TEE_WRITE_TEE_ETM_S)
#define TEE_WRITE_TEE_ETM_V  0x00000001U
#define TEE_WRITE_TEE_ETM_S  4
/** TEE_WRITE_REE0_ETM : R/W; bitpos: [5]; default: 0;
 *  Configures etm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_ETM    (BIT(5))
#define TEE_WRITE_REE0_ETM_M  (TEE_WRITE_REE0_ETM_V << TEE_WRITE_REE0_ETM_S)
#define TEE_WRITE_REE0_ETM_V  0x00000001U
#define TEE_WRITE_REE0_ETM_S  5
/** TEE_WRITE_REE1_ETM : R/W; bitpos: [6]; default: 0;
 *  Configures etm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_ETM    (BIT(6))
#define TEE_WRITE_REE1_ETM_M  (TEE_WRITE_REE1_ETM_V << TEE_WRITE_REE1_ETM_S)
#define TEE_WRITE_REE1_ETM_V  0x00000001U
#define TEE_WRITE_REE1_ETM_S  6
/** TEE_WRITE_REE2_ETM : R/W; bitpos: [7]; default: 0;
 *  Configures etm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_ETM    (BIT(7))
#define TEE_WRITE_REE2_ETM_M  (TEE_WRITE_REE2_ETM_V << TEE_WRITE_REE2_ETM_S)
#define TEE_WRITE_REE2_ETM_V  0x00000001U
#define TEE_WRITE_REE2_ETM_S  7

/** TEE_INTMTX_CORE0_CTRL_REG register
 *  intmtx_core0 read/write control register
 */
#define TEE_INTMTX_CORE0_CTRL_REG (DR_REG_TEE_BASE + 0xc8)
/** TEE_READ_TEE_INTMTX_CORE0 : R/W; bitpos: [0]; default: 1;
 *  Configures intmtx_core0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_INTMTX_CORE0    (BIT(0))
#define TEE_READ_TEE_INTMTX_CORE0_M  (TEE_READ_TEE_INTMTX_CORE0_V << TEE_READ_TEE_INTMTX_CORE0_S)
#define TEE_READ_TEE_INTMTX_CORE0_V  0x00000001U
#define TEE_READ_TEE_INTMTX_CORE0_S  0
/** TEE_READ_REE0_INTMTX_CORE0 : R/W; bitpos: [1]; default: 0;
 *  Configures intmtx_core0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_INTMTX_CORE0    (BIT(1))
#define TEE_READ_REE0_INTMTX_CORE0_M  (TEE_READ_REE0_INTMTX_CORE0_V << TEE_READ_REE0_INTMTX_CORE0_S)
#define TEE_READ_REE0_INTMTX_CORE0_V  0x00000001U
#define TEE_READ_REE0_INTMTX_CORE0_S  1
/** TEE_READ_REE1_INTMTX_CORE0 : R/W; bitpos: [2]; default: 0;
 *  Configures intmtx_core0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_INTMTX_CORE0    (BIT(2))
#define TEE_READ_REE1_INTMTX_CORE0_M  (TEE_READ_REE1_INTMTX_CORE0_V << TEE_READ_REE1_INTMTX_CORE0_S)
#define TEE_READ_REE1_INTMTX_CORE0_V  0x00000001U
#define TEE_READ_REE1_INTMTX_CORE0_S  2
/** TEE_READ_REE2_INTMTX_CORE0 : R/W; bitpos: [3]; default: 0;
 *  Configures intmtx_core0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_INTMTX_CORE0    (BIT(3))
#define TEE_READ_REE2_INTMTX_CORE0_M  (TEE_READ_REE2_INTMTX_CORE0_V << TEE_READ_REE2_INTMTX_CORE0_S)
#define TEE_READ_REE2_INTMTX_CORE0_V  0x00000001U
#define TEE_READ_REE2_INTMTX_CORE0_S  3
/** TEE_WRITE_TEE_INTMTX_CORE0 : R/W; bitpos: [4]; default: 1;
 *  Configures intmtx_core0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_INTMTX_CORE0    (BIT(4))
#define TEE_WRITE_TEE_INTMTX_CORE0_M  (TEE_WRITE_TEE_INTMTX_CORE0_V << TEE_WRITE_TEE_INTMTX_CORE0_S)
#define TEE_WRITE_TEE_INTMTX_CORE0_V  0x00000001U
#define TEE_WRITE_TEE_INTMTX_CORE0_S  4
/** TEE_WRITE_REE0_INTMTX_CORE0 : R/W; bitpos: [5]; default: 0;
 *  Configures intmtx_core0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_INTMTX_CORE0    (BIT(5))
#define TEE_WRITE_REE0_INTMTX_CORE0_M  (TEE_WRITE_REE0_INTMTX_CORE0_V << TEE_WRITE_REE0_INTMTX_CORE0_S)
#define TEE_WRITE_REE0_INTMTX_CORE0_V  0x00000001U
#define TEE_WRITE_REE0_INTMTX_CORE0_S  5
/** TEE_WRITE_REE1_INTMTX_CORE0 : R/W; bitpos: [6]; default: 0;
 *  Configures intmtx_core0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_INTMTX_CORE0    (BIT(6))
#define TEE_WRITE_REE1_INTMTX_CORE0_M  (TEE_WRITE_REE1_INTMTX_CORE0_V << TEE_WRITE_REE1_INTMTX_CORE0_S)
#define TEE_WRITE_REE1_INTMTX_CORE0_V  0x00000001U
#define TEE_WRITE_REE1_INTMTX_CORE0_S  6
/** TEE_WRITE_REE2_INTMTX_CORE0 : R/W; bitpos: [7]; default: 0;
 *  Configures intmtx_core0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_INTMTX_CORE0    (BIT(7))
#define TEE_WRITE_REE2_INTMTX_CORE0_M  (TEE_WRITE_REE2_INTMTX_CORE0_V << TEE_WRITE_REE2_INTMTX_CORE0_S)
#define TEE_WRITE_REE2_INTMTX_CORE0_V  0x00000001U
#define TEE_WRITE_REE2_INTMTX_CORE0_S  7

/** TEE_INTMTX_CORE1_CTRL_REG register
 *  intmtx_core1 read/write control register
 */
#define TEE_INTMTX_CORE1_CTRL_REG (DR_REG_TEE_BASE + 0xcc)
/** TEE_READ_TEE_INTMTX_CORE1 : R/W; bitpos: [0]; default: 1;
 *  Configures intmtx_core1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_INTMTX_CORE1    (BIT(0))
#define TEE_READ_TEE_INTMTX_CORE1_M  (TEE_READ_TEE_INTMTX_CORE1_V << TEE_READ_TEE_INTMTX_CORE1_S)
#define TEE_READ_TEE_INTMTX_CORE1_V  0x00000001U
#define TEE_READ_TEE_INTMTX_CORE1_S  0
/** TEE_READ_REE0_INTMTX_CORE1 : R/W; bitpos: [1]; default: 0;
 *  Configures intmtx_core1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_INTMTX_CORE1    (BIT(1))
#define TEE_READ_REE0_INTMTX_CORE1_M  (TEE_READ_REE0_INTMTX_CORE1_V << TEE_READ_REE0_INTMTX_CORE1_S)
#define TEE_READ_REE0_INTMTX_CORE1_V  0x00000001U
#define TEE_READ_REE0_INTMTX_CORE1_S  1
/** TEE_READ_REE1_INTMTX_CORE1 : R/W; bitpos: [2]; default: 0;
 *  Configures intmtx_core1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_INTMTX_CORE1    (BIT(2))
#define TEE_READ_REE1_INTMTX_CORE1_M  (TEE_READ_REE1_INTMTX_CORE1_V << TEE_READ_REE1_INTMTX_CORE1_S)
#define TEE_READ_REE1_INTMTX_CORE1_V  0x00000001U
#define TEE_READ_REE1_INTMTX_CORE1_S  2
/** TEE_READ_REE2_INTMTX_CORE1 : R/W; bitpos: [3]; default: 0;
 *  Configures intmtx_core1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_INTMTX_CORE1    (BIT(3))
#define TEE_READ_REE2_INTMTX_CORE1_M  (TEE_READ_REE2_INTMTX_CORE1_V << TEE_READ_REE2_INTMTX_CORE1_S)
#define TEE_READ_REE2_INTMTX_CORE1_V  0x00000001U
#define TEE_READ_REE2_INTMTX_CORE1_S  3
/** TEE_WRITE_TEE_INTMTX_CORE1 : R/W; bitpos: [4]; default: 1;
 *  Configures intmtx_core1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_INTMTX_CORE1    (BIT(4))
#define TEE_WRITE_TEE_INTMTX_CORE1_M  (TEE_WRITE_TEE_INTMTX_CORE1_V << TEE_WRITE_TEE_INTMTX_CORE1_S)
#define TEE_WRITE_TEE_INTMTX_CORE1_V  0x00000001U
#define TEE_WRITE_TEE_INTMTX_CORE1_S  4
/** TEE_WRITE_REE0_INTMTX_CORE1 : R/W; bitpos: [5]; default: 0;
 *  Configures intmtx_core1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_INTMTX_CORE1    (BIT(5))
#define TEE_WRITE_REE0_INTMTX_CORE1_M  (TEE_WRITE_REE0_INTMTX_CORE1_V << TEE_WRITE_REE0_INTMTX_CORE1_S)
#define TEE_WRITE_REE0_INTMTX_CORE1_V  0x00000001U
#define TEE_WRITE_REE0_INTMTX_CORE1_S  5
/** TEE_WRITE_REE1_INTMTX_CORE1 : R/W; bitpos: [6]; default: 0;
 *  Configures intmtx_core1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_INTMTX_CORE1    (BIT(6))
#define TEE_WRITE_REE1_INTMTX_CORE1_M  (TEE_WRITE_REE1_INTMTX_CORE1_V << TEE_WRITE_REE1_INTMTX_CORE1_S)
#define TEE_WRITE_REE1_INTMTX_CORE1_V  0x00000001U
#define TEE_WRITE_REE1_INTMTX_CORE1_S  6
/** TEE_WRITE_REE2_INTMTX_CORE1 : R/W; bitpos: [7]; default: 0;
 *  Configures intmtx_core1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_INTMTX_CORE1    (BIT(7))
#define TEE_WRITE_REE2_INTMTX_CORE1_M  (TEE_WRITE_REE2_INTMTX_CORE1_V << TEE_WRITE_REE2_INTMTX_CORE1_S)
#define TEE_WRITE_REE2_INTMTX_CORE1_V  0x00000001U
#define TEE_WRITE_REE2_INTMTX_CORE1_S  7

/** TEE_APB_ADC_CTRL_REG register
 *  apb_adc read/write control register
 */
#define TEE_APB_ADC_CTRL_REG (DR_REG_TEE_BASE + 0xd0)
/** TEE_READ_TEE_APB_ADC : R/W; bitpos: [0]; default: 1;
 *  Configures apb_adc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_APB_ADC    (BIT(0))
#define TEE_READ_TEE_APB_ADC_M  (TEE_READ_TEE_APB_ADC_V << TEE_READ_TEE_APB_ADC_S)
#define TEE_READ_TEE_APB_ADC_V  0x00000001U
#define TEE_READ_TEE_APB_ADC_S  0
/** TEE_READ_REE0_APB_ADC : R/W; bitpos: [1]; default: 0;
 *  Configures apb_adc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_APB_ADC    (BIT(1))
#define TEE_READ_REE0_APB_ADC_M  (TEE_READ_REE0_APB_ADC_V << TEE_READ_REE0_APB_ADC_S)
#define TEE_READ_REE0_APB_ADC_V  0x00000001U
#define TEE_READ_REE0_APB_ADC_S  1
/** TEE_READ_REE1_APB_ADC : R/W; bitpos: [2]; default: 0;
 *  Configures apb_adc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_APB_ADC    (BIT(2))
#define TEE_READ_REE1_APB_ADC_M  (TEE_READ_REE1_APB_ADC_V << TEE_READ_REE1_APB_ADC_S)
#define TEE_READ_REE1_APB_ADC_V  0x00000001U
#define TEE_READ_REE1_APB_ADC_S  2
/** TEE_READ_REE2_APB_ADC : R/W; bitpos: [3]; default: 0;
 *  Configures apb_adc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_APB_ADC    (BIT(3))
#define TEE_READ_REE2_APB_ADC_M  (TEE_READ_REE2_APB_ADC_V << TEE_READ_REE2_APB_ADC_S)
#define TEE_READ_REE2_APB_ADC_V  0x00000001U
#define TEE_READ_REE2_APB_ADC_S  3
/** TEE_WRITE_TEE_APB_ADC : R/W; bitpos: [4]; default: 1;
 *  Configures apb_adc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_APB_ADC    (BIT(4))
#define TEE_WRITE_TEE_APB_ADC_M  (TEE_WRITE_TEE_APB_ADC_V << TEE_WRITE_TEE_APB_ADC_S)
#define TEE_WRITE_TEE_APB_ADC_V  0x00000001U
#define TEE_WRITE_TEE_APB_ADC_S  4
/** TEE_WRITE_REE0_APB_ADC : R/W; bitpos: [5]; default: 0;
 *  Configures apb_adc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_APB_ADC    (BIT(5))
#define TEE_WRITE_REE0_APB_ADC_M  (TEE_WRITE_REE0_APB_ADC_V << TEE_WRITE_REE0_APB_ADC_S)
#define TEE_WRITE_REE0_APB_ADC_V  0x00000001U
#define TEE_WRITE_REE0_APB_ADC_S  5
/** TEE_WRITE_REE1_APB_ADC : R/W; bitpos: [6]; default: 0;
 *  Configures apb_adc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_APB_ADC    (BIT(6))
#define TEE_WRITE_REE1_APB_ADC_M  (TEE_WRITE_REE1_APB_ADC_V << TEE_WRITE_REE1_APB_ADC_S)
#define TEE_WRITE_REE1_APB_ADC_V  0x00000001U
#define TEE_WRITE_REE1_APB_ADC_S  6
/** TEE_WRITE_REE2_APB_ADC : R/W; bitpos: [7]; default: 0;
 *  Configures apb_adc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_APB_ADC    (BIT(7))
#define TEE_WRITE_REE2_APB_ADC_M  (TEE_WRITE_REE2_APB_ADC_V << TEE_WRITE_REE2_APB_ADC_S)
#define TEE_WRITE_REE2_APB_ADC_V  0x00000001U
#define TEE_WRITE_REE2_APB_ADC_S  7

/** TEE_TIMERGROUP0_CTRL_REG register
 *  timergroup0 read/write control register
 */
#define TEE_TIMERGROUP0_CTRL_REG (DR_REG_TEE_BASE + 0xd4)
/** TEE_READ_TEE_TIMERGROUP0 : R/W; bitpos: [0]; default: 1;
 *  Configures timergroup0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_TIMERGROUP0    (BIT(0))
#define TEE_READ_TEE_TIMERGROUP0_M  (TEE_READ_TEE_TIMERGROUP0_V << TEE_READ_TEE_TIMERGROUP0_S)
#define TEE_READ_TEE_TIMERGROUP0_V  0x00000001U
#define TEE_READ_TEE_TIMERGROUP0_S  0
/** TEE_READ_REE0_TIMERGROUP0 : R/W; bitpos: [1]; default: 0;
 *  Configures timergroup0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_TIMERGROUP0    (BIT(1))
#define TEE_READ_REE0_TIMERGROUP0_M  (TEE_READ_REE0_TIMERGROUP0_V << TEE_READ_REE0_TIMERGROUP0_S)
#define TEE_READ_REE0_TIMERGROUP0_V  0x00000001U
#define TEE_READ_REE0_TIMERGROUP0_S  1
/** TEE_READ_REE1_TIMERGROUP0 : R/W; bitpos: [2]; default: 0;
 *  Configures timergroup0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_TIMERGROUP0    (BIT(2))
#define TEE_READ_REE1_TIMERGROUP0_M  (TEE_READ_REE1_TIMERGROUP0_V << TEE_READ_REE1_TIMERGROUP0_S)
#define TEE_READ_REE1_TIMERGROUP0_V  0x00000001U
#define TEE_READ_REE1_TIMERGROUP0_S  2
/** TEE_READ_REE2_TIMERGROUP0 : R/W; bitpos: [3]; default: 0;
 *  Configures timergroup0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_TIMERGROUP0    (BIT(3))
#define TEE_READ_REE2_TIMERGROUP0_M  (TEE_READ_REE2_TIMERGROUP0_V << TEE_READ_REE2_TIMERGROUP0_S)
#define TEE_READ_REE2_TIMERGROUP0_V  0x00000001U
#define TEE_READ_REE2_TIMERGROUP0_S  3
/** TEE_WRITE_TEE_TIMERGROUP0 : R/W; bitpos: [4]; default: 1;
 *  Configures timergroup0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_TIMERGROUP0    (BIT(4))
#define TEE_WRITE_TEE_TIMERGROUP0_M  (TEE_WRITE_TEE_TIMERGROUP0_V << TEE_WRITE_TEE_TIMERGROUP0_S)
#define TEE_WRITE_TEE_TIMERGROUP0_V  0x00000001U
#define TEE_WRITE_TEE_TIMERGROUP0_S  4
/** TEE_WRITE_REE0_TIMERGROUP0 : R/W; bitpos: [5]; default: 0;
 *  Configures timergroup0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_TIMERGROUP0    (BIT(5))
#define TEE_WRITE_REE0_TIMERGROUP0_M  (TEE_WRITE_REE0_TIMERGROUP0_V << TEE_WRITE_REE0_TIMERGROUP0_S)
#define TEE_WRITE_REE0_TIMERGROUP0_V  0x00000001U
#define TEE_WRITE_REE0_TIMERGROUP0_S  5
/** TEE_WRITE_REE1_TIMERGROUP0 : R/W; bitpos: [6]; default: 0;
 *  Configures timergroup0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_TIMERGROUP0    (BIT(6))
#define TEE_WRITE_REE1_TIMERGROUP0_M  (TEE_WRITE_REE1_TIMERGROUP0_V << TEE_WRITE_REE1_TIMERGROUP0_S)
#define TEE_WRITE_REE1_TIMERGROUP0_V  0x00000001U
#define TEE_WRITE_REE1_TIMERGROUP0_S  6
/** TEE_WRITE_REE2_TIMERGROUP0 : R/W; bitpos: [7]; default: 0;
 *  Configures timergroup0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_TIMERGROUP0    (BIT(7))
#define TEE_WRITE_REE2_TIMERGROUP0_M  (TEE_WRITE_REE2_TIMERGROUP0_V << TEE_WRITE_REE2_TIMERGROUP0_S)
#define TEE_WRITE_REE2_TIMERGROUP0_V  0x00000001U
#define TEE_WRITE_REE2_TIMERGROUP0_S  7

/** TEE_TIMERGROUP1_CTRL_REG register
 *  timergroup1 read/write control register
 */
#define TEE_TIMERGROUP1_CTRL_REG (DR_REG_TEE_BASE + 0xd8)
/** TEE_READ_TEE_TIMERGROUP1 : R/W; bitpos: [0]; default: 1;
 *  Configures timergroup1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_TIMERGROUP1    (BIT(0))
#define TEE_READ_TEE_TIMERGROUP1_M  (TEE_READ_TEE_TIMERGROUP1_V << TEE_READ_TEE_TIMERGROUP1_S)
#define TEE_READ_TEE_TIMERGROUP1_V  0x00000001U
#define TEE_READ_TEE_TIMERGROUP1_S  0
/** TEE_READ_REE0_TIMERGROUP1 : R/W; bitpos: [1]; default: 0;
 *  Configures timergroup1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_TIMERGROUP1    (BIT(1))
#define TEE_READ_REE0_TIMERGROUP1_M  (TEE_READ_REE0_TIMERGROUP1_V << TEE_READ_REE0_TIMERGROUP1_S)
#define TEE_READ_REE0_TIMERGROUP1_V  0x00000001U
#define TEE_READ_REE0_TIMERGROUP1_S  1
/** TEE_READ_REE1_TIMERGROUP1 : R/W; bitpos: [2]; default: 0;
 *  Configures timergroup1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_TIMERGROUP1    (BIT(2))
#define TEE_READ_REE1_TIMERGROUP1_M  (TEE_READ_REE1_TIMERGROUP1_V << TEE_READ_REE1_TIMERGROUP1_S)
#define TEE_READ_REE1_TIMERGROUP1_V  0x00000001U
#define TEE_READ_REE1_TIMERGROUP1_S  2
/** TEE_READ_REE2_TIMERGROUP1 : R/W; bitpos: [3]; default: 0;
 *  Configures timergroup1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_TIMERGROUP1    (BIT(3))
#define TEE_READ_REE2_TIMERGROUP1_M  (TEE_READ_REE2_TIMERGROUP1_V << TEE_READ_REE2_TIMERGROUP1_S)
#define TEE_READ_REE2_TIMERGROUP1_V  0x00000001U
#define TEE_READ_REE2_TIMERGROUP1_S  3
/** TEE_WRITE_TEE_TIMERGROUP1 : R/W; bitpos: [4]; default: 1;
 *  Configures timergroup1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_TIMERGROUP1    (BIT(4))
#define TEE_WRITE_TEE_TIMERGROUP1_M  (TEE_WRITE_TEE_TIMERGROUP1_V << TEE_WRITE_TEE_TIMERGROUP1_S)
#define TEE_WRITE_TEE_TIMERGROUP1_V  0x00000001U
#define TEE_WRITE_TEE_TIMERGROUP1_S  4
/** TEE_WRITE_REE0_TIMERGROUP1 : R/W; bitpos: [5]; default: 0;
 *  Configures timergroup1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_TIMERGROUP1    (BIT(5))
#define TEE_WRITE_REE0_TIMERGROUP1_M  (TEE_WRITE_REE0_TIMERGROUP1_V << TEE_WRITE_REE0_TIMERGROUP1_S)
#define TEE_WRITE_REE0_TIMERGROUP1_V  0x00000001U
#define TEE_WRITE_REE0_TIMERGROUP1_S  5
/** TEE_WRITE_REE1_TIMERGROUP1 : R/W; bitpos: [6]; default: 0;
 *  Configures timergroup1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_TIMERGROUP1    (BIT(6))
#define TEE_WRITE_REE1_TIMERGROUP1_M  (TEE_WRITE_REE1_TIMERGROUP1_V << TEE_WRITE_REE1_TIMERGROUP1_S)
#define TEE_WRITE_REE1_TIMERGROUP1_V  0x00000001U
#define TEE_WRITE_REE1_TIMERGROUP1_S  6
/** TEE_WRITE_REE2_TIMERGROUP1 : R/W; bitpos: [7]; default: 0;
 *  Configures timergroup1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_TIMERGROUP1    (BIT(7))
#define TEE_WRITE_REE2_TIMERGROUP1_M  (TEE_WRITE_REE2_TIMERGROUP1_V << TEE_WRITE_REE2_TIMERGROUP1_S)
#define TEE_WRITE_REE2_TIMERGROUP1_V  0x00000001U
#define TEE_WRITE_REE2_TIMERGROUP1_S  7

/** TEE_SYSTIMER_CTRL_REG register
 *  systimer read/write control register
 */
#define TEE_SYSTIMER_CTRL_REG (DR_REG_TEE_BASE + 0xdc)
/** TEE_READ_TEE_SYSTIMER : R/W; bitpos: [0]; default: 1;
 *  Configures systimer registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_SYSTIMER    (BIT(0))
#define TEE_READ_TEE_SYSTIMER_M  (TEE_READ_TEE_SYSTIMER_V << TEE_READ_TEE_SYSTIMER_S)
#define TEE_READ_TEE_SYSTIMER_V  0x00000001U
#define TEE_READ_TEE_SYSTIMER_S  0
/** TEE_READ_REE0_SYSTIMER : R/W; bitpos: [1]; default: 0;
 *  Configures systimer registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_SYSTIMER    (BIT(1))
#define TEE_READ_REE0_SYSTIMER_M  (TEE_READ_REE0_SYSTIMER_V << TEE_READ_REE0_SYSTIMER_S)
#define TEE_READ_REE0_SYSTIMER_V  0x00000001U
#define TEE_READ_REE0_SYSTIMER_S  1
/** TEE_READ_REE1_SYSTIMER : R/W; bitpos: [2]; default: 0;
 *  Configures systimer registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_SYSTIMER    (BIT(2))
#define TEE_READ_REE1_SYSTIMER_M  (TEE_READ_REE1_SYSTIMER_V << TEE_READ_REE1_SYSTIMER_S)
#define TEE_READ_REE1_SYSTIMER_V  0x00000001U
#define TEE_READ_REE1_SYSTIMER_S  2
/** TEE_READ_REE2_SYSTIMER : R/W; bitpos: [3]; default: 0;
 *  Configures systimer registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_SYSTIMER    (BIT(3))
#define TEE_READ_REE2_SYSTIMER_M  (TEE_READ_REE2_SYSTIMER_V << TEE_READ_REE2_SYSTIMER_S)
#define TEE_READ_REE2_SYSTIMER_V  0x00000001U
#define TEE_READ_REE2_SYSTIMER_S  3
/** TEE_WRITE_TEE_SYSTIMER : R/W; bitpos: [4]; default: 1;
 *  Configures systimer registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_SYSTIMER    (BIT(4))
#define TEE_WRITE_TEE_SYSTIMER_M  (TEE_WRITE_TEE_SYSTIMER_V << TEE_WRITE_TEE_SYSTIMER_S)
#define TEE_WRITE_TEE_SYSTIMER_V  0x00000001U
#define TEE_WRITE_TEE_SYSTIMER_S  4
/** TEE_WRITE_REE0_SYSTIMER : R/W; bitpos: [5]; default: 0;
 *  Configures systimer registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_SYSTIMER    (BIT(5))
#define TEE_WRITE_REE0_SYSTIMER_M  (TEE_WRITE_REE0_SYSTIMER_V << TEE_WRITE_REE0_SYSTIMER_S)
#define TEE_WRITE_REE0_SYSTIMER_V  0x00000001U
#define TEE_WRITE_REE0_SYSTIMER_S  5
/** TEE_WRITE_REE1_SYSTIMER : R/W; bitpos: [6]; default: 0;
 *  Configures systimer registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_SYSTIMER    (BIT(6))
#define TEE_WRITE_REE1_SYSTIMER_M  (TEE_WRITE_REE1_SYSTIMER_V << TEE_WRITE_REE1_SYSTIMER_S)
#define TEE_WRITE_REE1_SYSTIMER_V  0x00000001U
#define TEE_WRITE_REE1_SYSTIMER_S  6
/** TEE_WRITE_REE2_SYSTIMER : R/W; bitpos: [7]; default: 0;
 *  Configures systimer registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_SYSTIMER    (BIT(7))
#define TEE_WRITE_REE2_SYSTIMER_M  (TEE_WRITE_REE2_SYSTIMER_V << TEE_WRITE_REE2_SYSTIMER_S)
#define TEE_WRITE_REE2_SYSTIMER_V  0x00000001U
#define TEE_WRITE_REE2_SYSTIMER_S  7

/** TEE_MISC_CTRL_REG register
 *  misc read/write control register
 */
#define TEE_MISC_CTRL_REG (DR_REG_TEE_BASE + 0xe0)
/** TEE_READ_TEE_MISC : R/W; bitpos: [0]; default: 1;
 *  Configures misc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_MISC    (BIT(0))
#define TEE_READ_TEE_MISC_M  (TEE_READ_TEE_MISC_V << TEE_READ_TEE_MISC_S)
#define TEE_READ_TEE_MISC_V  0x00000001U
#define TEE_READ_TEE_MISC_S  0
/** TEE_READ_REE0_MISC : R/W; bitpos: [1]; default: 0;
 *  Configures misc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_MISC    (BIT(1))
#define TEE_READ_REE0_MISC_M  (TEE_READ_REE0_MISC_V << TEE_READ_REE0_MISC_S)
#define TEE_READ_REE0_MISC_V  0x00000001U
#define TEE_READ_REE0_MISC_S  1
/** TEE_READ_REE1_MISC : R/W; bitpos: [2]; default: 0;
 *  Configures misc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_MISC    (BIT(2))
#define TEE_READ_REE1_MISC_M  (TEE_READ_REE1_MISC_V << TEE_READ_REE1_MISC_S)
#define TEE_READ_REE1_MISC_V  0x00000001U
#define TEE_READ_REE1_MISC_S  2
/** TEE_READ_REE2_MISC : R/W; bitpos: [3]; default: 0;
 *  Configures misc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_MISC    (BIT(3))
#define TEE_READ_REE2_MISC_M  (TEE_READ_REE2_MISC_V << TEE_READ_REE2_MISC_S)
#define TEE_READ_REE2_MISC_V  0x00000001U
#define TEE_READ_REE2_MISC_S  3
/** TEE_WRITE_TEE_MISC : R/W; bitpos: [4]; default: 1;
 *  Configures misc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_MISC    (BIT(4))
#define TEE_WRITE_TEE_MISC_M  (TEE_WRITE_TEE_MISC_V << TEE_WRITE_TEE_MISC_S)
#define TEE_WRITE_TEE_MISC_V  0x00000001U
#define TEE_WRITE_TEE_MISC_S  4
/** TEE_WRITE_REE0_MISC : R/W; bitpos: [5]; default: 0;
 *  Configures misc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_MISC    (BIT(5))
#define TEE_WRITE_REE0_MISC_M  (TEE_WRITE_REE0_MISC_V << TEE_WRITE_REE0_MISC_S)
#define TEE_WRITE_REE0_MISC_V  0x00000001U
#define TEE_WRITE_REE0_MISC_S  5
/** TEE_WRITE_REE1_MISC : R/W; bitpos: [6]; default: 0;
 *  Configures misc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_MISC    (BIT(6))
#define TEE_WRITE_REE1_MISC_M  (TEE_WRITE_REE1_MISC_V << TEE_WRITE_REE1_MISC_S)
#define TEE_WRITE_REE1_MISC_V  0x00000001U
#define TEE_WRITE_REE1_MISC_S  6
/** TEE_WRITE_REE2_MISC : R/W; bitpos: [7]; default: 0;
 *  Configures misc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_MISC    (BIT(7))
#define TEE_WRITE_REE2_MISC_M  (TEE_WRITE_REE2_MISC_V << TEE_WRITE_REE2_MISC_S)
#define TEE_WRITE_REE2_MISC_V  0x00000001U
#define TEE_WRITE_REE2_MISC_S  7

/** TEE_SRC_CTRL_REG register
 *  src read/write control register
 */
#define TEE_SRC_CTRL_REG (DR_REG_TEE_BASE + 0xe4)
/** TEE_READ_TEE_SRC : R/W; bitpos: [0]; default: 1;
 *  Configures src registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_SRC    (BIT(0))
#define TEE_READ_TEE_SRC_M  (TEE_READ_TEE_SRC_V << TEE_READ_TEE_SRC_S)
#define TEE_READ_TEE_SRC_V  0x00000001U
#define TEE_READ_TEE_SRC_S  0
/** TEE_READ_REE0_SRC : R/W; bitpos: [1]; default: 0;
 *  Configures src registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_SRC    (BIT(1))
#define TEE_READ_REE0_SRC_M  (TEE_READ_REE0_SRC_V << TEE_READ_REE0_SRC_S)
#define TEE_READ_REE0_SRC_V  0x00000001U
#define TEE_READ_REE0_SRC_S  1
/** TEE_READ_REE1_SRC : R/W; bitpos: [2]; default: 0;
 *  Configures src registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_SRC    (BIT(2))
#define TEE_READ_REE1_SRC_M  (TEE_READ_REE1_SRC_V << TEE_READ_REE1_SRC_S)
#define TEE_READ_REE1_SRC_V  0x00000001U
#define TEE_READ_REE1_SRC_S  2
/** TEE_READ_REE2_SRC : R/W; bitpos: [3]; default: 0;
 *  Configures src registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_SRC    (BIT(3))
#define TEE_READ_REE2_SRC_M  (TEE_READ_REE2_SRC_V << TEE_READ_REE2_SRC_S)
#define TEE_READ_REE2_SRC_V  0x00000001U
#define TEE_READ_REE2_SRC_S  3
/** TEE_WRITE_TEE_SRC : R/W; bitpos: [4]; default: 1;
 *  Configures src registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_SRC    (BIT(4))
#define TEE_WRITE_TEE_SRC_M  (TEE_WRITE_TEE_SRC_V << TEE_WRITE_TEE_SRC_S)
#define TEE_WRITE_TEE_SRC_V  0x00000001U
#define TEE_WRITE_TEE_SRC_S  4
/** TEE_WRITE_REE0_SRC : R/W; bitpos: [5]; default: 0;
 *  Configures src registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_SRC    (BIT(5))
#define TEE_WRITE_REE0_SRC_M  (TEE_WRITE_REE0_SRC_V << TEE_WRITE_REE0_SRC_S)
#define TEE_WRITE_REE0_SRC_V  0x00000001U
#define TEE_WRITE_REE0_SRC_S  5
/** TEE_WRITE_REE1_SRC : R/W; bitpos: [6]; default: 0;
 *  Configures src registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_SRC    (BIT(6))
#define TEE_WRITE_REE1_SRC_M  (TEE_WRITE_REE1_SRC_V << TEE_WRITE_REE1_SRC_S)
#define TEE_WRITE_REE1_SRC_V  0x00000001U
#define TEE_WRITE_REE1_SRC_S  6
/** TEE_WRITE_REE2_SRC : R/W; bitpos: [7]; default: 0;
 *  Configures src registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_SRC    (BIT(7))
#define TEE_WRITE_REE2_SRC_M  (TEE_WRITE_REE2_SRC_V << TEE_WRITE_REE2_SRC_S)
#define TEE_WRITE_REE2_SRC_V  0x00000001U
#define TEE_WRITE_REE2_SRC_S  7

/** TEE_USB_OTG_FS_CORE_CTRL_REG register
 *  usb_otg_fs_core read/write control register
 */
#define TEE_USB_OTG_FS_CORE_CTRL_REG (DR_REG_TEE_BASE + 0xe8)
/** TEE_READ_TEE_USB_OTG_FS_CORE : R/W; bitpos: [0]; default: 1;
 *  Configures usb_otg_fs_core registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_USB_OTG_FS_CORE    (BIT(0))
#define TEE_READ_TEE_USB_OTG_FS_CORE_M  (TEE_READ_TEE_USB_OTG_FS_CORE_V << TEE_READ_TEE_USB_OTG_FS_CORE_S)
#define TEE_READ_TEE_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_READ_TEE_USB_OTG_FS_CORE_S  0
/** TEE_READ_REE0_USB_OTG_FS_CORE : R/W; bitpos: [1]; default: 0;
 *  Configures usb_otg_fs_core registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_USB_OTG_FS_CORE    (BIT(1))
#define TEE_READ_REE0_USB_OTG_FS_CORE_M  (TEE_READ_REE0_USB_OTG_FS_CORE_V << TEE_READ_REE0_USB_OTG_FS_CORE_S)
#define TEE_READ_REE0_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_READ_REE0_USB_OTG_FS_CORE_S  1
/** TEE_READ_REE1_USB_OTG_FS_CORE : R/W; bitpos: [2]; default: 0;
 *  Configures usb_otg_fs_core registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_USB_OTG_FS_CORE    (BIT(2))
#define TEE_READ_REE1_USB_OTG_FS_CORE_M  (TEE_READ_REE1_USB_OTG_FS_CORE_V << TEE_READ_REE1_USB_OTG_FS_CORE_S)
#define TEE_READ_REE1_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_READ_REE1_USB_OTG_FS_CORE_S  2
/** TEE_READ_REE2_USB_OTG_FS_CORE : R/W; bitpos: [3]; default: 0;
 *  Configures usb_otg_fs_core registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_USB_OTG_FS_CORE    (BIT(3))
#define TEE_READ_REE2_USB_OTG_FS_CORE_M  (TEE_READ_REE2_USB_OTG_FS_CORE_V << TEE_READ_REE2_USB_OTG_FS_CORE_S)
#define TEE_READ_REE2_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_READ_REE2_USB_OTG_FS_CORE_S  3
/** TEE_WRITE_TEE_USB_OTG_FS_CORE : R/W; bitpos: [4]; default: 1;
 *  Configures usb_otg_fs_core registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_USB_OTG_FS_CORE    (BIT(4))
#define TEE_WRITE_TEE_USB_OTG_FS_CORE_M  (TEE_WRITE_TEE_USB_OTG_FS_CORE_V << TEE_WRITE_TEE_USB_OTG_FS_CORE_S)
#define TEE_WRITE_TEE_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_WRITE_TEE_USB_OTG_FS_CORE_S  4
/** TEE_WRITE_REE0_USB_OTG_FS_CORE : R/W; bitpos: [5]; default: 0;
 *  Configures usb_otg_fs_core registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_USB_OTG_FS_CORE    (BIT(5))
#define TEE_WRITE_REE0_USB_OTG_FS_CORE_M  (TEE_WRITE_REE0_USB_OTG_FS_CORE_V << TEE_WRITE_REE0_USB_OTG_FS_CORE_S)
#define TEE_WRITE_REE0_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_WRITE_REE0_USB_OTG_FS_CORE_S  5
/** TEE_WRITE_REE1_USB_OTG_FS_CORE : R/W; bitpos: [6]; default: 0;
 *  Configures usb_otg_fs_core registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_USB_OTG_FS_CORE    (BIT(6))
#define TEE_WRITE_REE1_USB_OTG_FS_CORE_M  (TEE_WRITE_REE1_USB_OTG_FS_CORE_V << TEE_WRITE_REE1_USB_OTG_FS_CORE_S)
#define TEE_WRITE_REE1_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_WRITE_REE1_USB_OTG_FS_CORE_S  6
/** TEE_WRITE_REE2_USB_OTG_FS_CORE : R/W; bitpos: [7]; default: 0;
 *  Configures usb_otg_fs_core registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_USB_OTG_FS_CORE    (BIT(7))
#define TEE_WRITE_REE2_USB_OTG_FS_CORE_M  (TEE_WRITE_REE2_USB_OTG_FS_CORE_V << TEE_WRITE_REE2_USB_OTG_FS_CORE_S)
#define TEE_WRITE_REE2_USB_OTG_FS_CORE_V  0x00000001U
#define TEE_WRITE_REE2_USB_OTG_FS_CORE_S  7

/** TEE_USB_OTG_FS_PHY_CTRL_REG register
 *  usb_otg_fs_phy read/write control register
 */
#define TEE_USB_OTG_FS_PHY_CTRL_REG (DR_REG_TEE_BASE + 0xec)
/** TEE_READ_TEE_USB_OTG_FS_PHY : R/W; bitpos: [0]; default: 1;
 *  Configures usb_otg_fs_phy registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_USB_OTG_FS_PHY    (BIT(0))
#define TEE_READ_TEE_USB_OTG_FS_PHY_M  (TEE_READ_TEE_USB_OTG_FS_PHY_V << TEE_READ_TEE_USB_OTG_FS_PHY_S)
#define TEE_READ_TEE_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_READ_TEE_USB_OTG_FS_PHY_S  0
/** TEE_READ_REE0_USB_OTG_FS_PHY : R/W; bitpos: [1]; default: 0;
 *  Configures usb_otg_fs_phy registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_USB_OTG_FS_PHY    (BIT(1))
#define TEE_READ_REE0_USB_OTG_FS_PHY_M  (TEE_READ_REE0_USB_OTG_FS_PHY_V << TEE_READ_REE0_USB_OTG_FS_PHY_S)
#define TEE_READ_REE0_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_READ_REE0_USB_OTG_FS_PHY_S  1
/** TEE_READ_REE1_USB_OTG_FS_PHY : R/W; bitpos: [2]; default: 0;
 *  Configures usb_otg_fs_phy registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_USB_OTG_FS_PHY    (BIT(2))
#define TEE_READ_REE1_USB_OTG_FS_PHY_M  (TEE_READ_REE1_USB_OTG_FS_PHY_V << TEE_READ_REE1_USB_OTG_FS_PHY_S)
#define TEE_READ_REE1_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_READ_REE1_USB_OTG_FS_PHY_S  2
/** TEE_READ_REE2_USB_OTG_FS_PHY : R/W; bitpos: [3]; default: 0;
 *  Configures usb_otg_fs_phy registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_USB_OTG_FS_PHY    (BIT(3))
#define TEE_READ_REE2_USB_OTG_FS_PHY_M  (TEE_READ_REE2_USB_OTG_FS_PHY_V << TEE_READ_REE2_USB_OTG_FS_PHY_S)
#define TEE_READ_REE2_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_READ_REE2_USB_OTG_FS_PHY_S  3
/** TEE_WRITE_TEE_USB_OTG_FS_PHY : R/W; bitpos: [4]; default: 1;
 *  Configures usb_otg_fs_phy registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_USB_OTG_FS_PHY    (BIT(4))
#define TEE_WRITE_TEE_USB_OTG_FS_PHY_M  (TEE_WRITE_TEE_USB_OTG_FS_PHY_V << TEE_WRITE_TEE_USB_OTG_FS_PHY_S)
#define TEE_WRITE_TEE_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_WRITE_TEE_USB_OTG_FS_PHY_S  4
/** TEE_WRITE_REE0_USB_OTG_FS_PHY : R/W; bitpos: [5]; default: 0;
 *  Configures usb_otg_fs_phy registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_USB_OTG_FS_PHY    (BIT(5))
#define TEE_WRITE_REE0_USB_OTG_FS_PHY_M  (TEE_WRITE_REE0_USB_OTG_FS_PHY_V << TEE_WRITE_REE0_USB_OTG_FS_PHY_S)
#define TEE_WRITE_REE0_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_WRITE_REE0_USB_OTG_FS_PHY_S  5
/** TEE_WRITE_REE1_USB_OTG_FS_PHY : R/W; bitpos: [6]; default: 0;
 *  Configures usb_otg_fs_phy registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_USB_OTG_FS_PHY    (BIT(6))
#define TEE_WRITE_REE1_USB_OTG_FS_PHY_M  (TEE_WRITE_REE1_USB_OTG_FS_PHY_V << TEE_WRITE_REE1_USB_OTG_FS_PHY_S)
#define TEE_WRITE_REE1_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_WRITE_REE1_USB_OTG_FS_PHY_S  6
/** TEE_WRITE_REE2_USB_OTG_FS_PHY : R/W; bitpos: [7]; default: 0;
 *  Configures usb_otg_fs_phy registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_USB_OTG_FS_PHY    (BIT(7))
#define TEE_WRITE_REE2_USB_OTG_FS_PHY_M  (TEE_WRITE_REE2_USB_OTG_FS_PHY_V << TEE_WRITE_REE2_USB_OTG_FS_PHY_S)
#define TEE_WRITE_REE2_USB_OTG_FS_PHY_V  0x00000001U
#define TEE_WRITE_REE2_USB_OTG_FS_PHY_S  7

/** TEE_PVT_MONITOR_CTRL_REG register
 *  pvt_monitor read/write control register
 */
#define TEE_PVT_MONITOR_CTRL_REG (DR_REG_TEE_BASE + 0xf0)
/** TEE_READ_TEE_PVT_MONITOR : R/W; bitpos: [0]; default: 1;
 *  Configures pvt_monitor registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PVT_MONITOR    (BIT(0))
#define TEE_READ_TEE_PVT_MONITOR_M  (TEE_READ_TEE_PVT_MONITOR_V << TEE_READ_TEE_PVT_MONITOR_S)
#define TEE_READ_TEE_PVT_MONITOR_V  0x00000001U
#define TEE_READ_TEE_PVT_MONITOR_S  0
/** TEE_READ_REE0_PVT_MONITOR : R/W; bitpos: [1]; default: 0;
 *  Configures pvt_monitor registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PVT_MONITOR    (BIT(1))
#define TEE_READ_REE0_PVT_MONITOR_M  (TEE_READ_REE0_PVT_MONITOR_V << TEE_READ_REE0_PVT_MONITOR_S)
#define TEE_READ_REE0_PVT_MONITOR_V  0x00000001U
#define TEE_READ_REE0_PVT_MONITOR_S  1
/** TEE_READ_REE1_PVT_MONITOR : R/W; bitpos: [2]; default: 0;
 *  Configures pvt_monitor registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PVT_MONITOR    (BIT(2))
#define TEE_READ_REE1_PVT_MONITOR_M  (TEE_READ_REE1_PVT_MONITOR_V << TEE_READ_REE1_PVT_MONITOR_S)
#define TEE_READ_REE1_PVT_MONITOR_V  0x00000001U
#define TEE_READ_REE1_PVT_MONITOR_S  2
/** TEE_READ_REE2_PVT_MONITOR : R/W; bitpos: [3]; default: 0;
 *  Configures pvt_monitor registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PVT_MONITOR    (BIT(3))
#define TEE_READ_REE2_PVT_MONITOR_M  (TEE_READ_REE2_PVT_MONITOR_V << TEE_READ_REE2_PVT_MONITOR_S)
#define TEE_READ_REE2_PVT_MONITOR_V  0x00000001U
#define TEE_READ_REE2_PVT_MONITOR_S  3
/** TEE_WRITE_TEE_PVT_MONITOR : R/W; bitpos: [4]; default: 1;
 *  Configures pvt_monitor registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PVT_MONITOR    (BIT(4))
#define TEE_WRITE_TEE_PVT_MONITOR_M  (TEE_WRITE_TEE_PVT_MONITOR_V << TEE_WRITE_TEE_PVT_MONITOR_S)
#define TEE_WRITE_TEE_PVT_MONITOR_V  0x00000001U
#define TEE_WRITE_TEE_PVT_MONITOR_S  4
/** TEE_WRITE_REE0_PVT_MONITOR : R/W; bitpos: [5]; default: 0;
 *  Configures pvt_monitor registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PVT_MONITOR    (BIT(5))
#define TEE_WRITE_REE0_PVT_MONITOR_M  (TEE_WRITE_REE0_PVT_MONITOR_V << TEE_WRITE_REE0_PVT_MONITOR_S)
#define TEE_WRITE_REE0_PVT_MONITOR_V  0x00000001U
#define TEE_WRITE_REE0_PVT_MONITOR_S  5
/** TEE_WRITE_REE1_PVT_MONITOR : R/W; bitpos: [6]; default: 0;
 *  Configures pvt_monitor registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PVT_MONITOR    (BIT(6))
#define TEE_WRITE_REE1_PVT_MONITOR_M  (TEE_WRITE_REE1_PVT_MONITOR_V << TEE_WRITE_REE1_PVT_MONITOR_S)
#define TEE_WRITE_REE1_PVT_MONITOR_V  0x00000001U
#define TEE_WRITE_REE1_PVT_MONITOR_S  6
/** TEE_WRITE_REE2_PVT_MONITOR : R/W; bitpos: [7]; default: 0;
 *  Configures pvt_monitor registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PVT_MONITOR    (BIT(7))
#define TEE_WRITE_REE2_PVT_MONITOR_M  (TEE_WRITE_REE2_PVT_MONITOR_V << TEE_WRITE_REE2_PVT_MONITOR_S)
#define TEE_WRITE_REE2_PVT_MONITOR_V  0x00000001U
#define TEE_WRITE_REE2_PVT_MONITOR_S  7

/** TEE_PCNT_CTRL_REG register
 *  pcnt read/write control register
 */
#define TEE_PCNT_CTRL_REG (DR_REG_TEE_BASE + 0xf4)
/** TEE_READ_TEE_PCNT : R/W; bitpos: [0]; default: 1;
 *  Configures pcnt registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PCNT    (BIT(0))
#define TEE_READ_TEE_PCNT_M  (TEE_READ_TEE_PCNT_V << TEE_READ_TEE_PCNT_S)
#define TEE_READ_TEE_PCNT_V  0x00000001U
#define TEE_READ_TEE_PCNT_S  0
/** TEE_READ_REE0_PCNT : R/W; bitpos: [1]; default: 0;
 *  Configures pcnt registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PCNT    (BIT(1))
#define TEE_READ_REE0_PCNT_M  (TEE_READ_REE0_PCNT_V << TEE_READ_REE0_PCNT_S)
#define TEE_READ_REE0_PCNT_V  0x00000001U
#define TEE_READ_REE0_PCNT_S  1
/** TEE_READ_REE1_PCNT : R/W; bitpos: [2]; default: 0;
 *  Configures pcnt registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PCNT    (BIT(2))
#define TEE_READ_REE1_PCNT_M  (TEE_READ_REE1_PCNT_V << TEE_READ_REE1_PCNT_S)
#define TEE_READ_REE1_PCNT_V  0x00000001U
#define TEE_READ_REE1_PCNT_S  2
/** TEE_READ_REE2_PCNT : R/W; bitpos: [3]; default: 0;
 *  Configures pcnt registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PCNT    (BIT(3))
#define TEE_READ_REE2_PCNT_M  (TEE_READ_REE2_PCNT_V << TEE_READ_REE2_PCNT_S)
#define TEE_READ_REE2_PCNT_V  0x00000001U
#define TEE_READ_REE2_PCNT_S  3
/** TEE_WRITE_TEE_PCNT : R/W; bitpos: [4]; default: 1;
 *  Configures pcnt registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PCNT    (BIT(4))
#define TEE_WRITE_TEE_PCNT_M  (TEE_WRITE_TEE_PCNT_V << TEE_WRITE_TEE_PCNT_S)
#define TEE_WRITE_TEE_PCNT_V  0x00000001U
#define TEE_WRITE_TEE_PCNT_S  4
/** TEE_WRITE_REE0_PCNT : R/W; bitpos: [5]; default: 0;
 *  Configures pcnt registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PCNT    (BIT(5))
#define TEE_WRITE_REE0_PCNT_M  (TEE_WRITE_REE0_PCNT_V << TEE_WRITE_REE0_PCNT_S)
#define TEE_WRITE_REE0_PCNT_V  0x00000001U
#define TEE_WRITE_REE0_PCNT_S  5
/** TEE_WRITE_REE1_PCNT : R/W; bitpos: [6]; default: 0;
 *  Configures pcnt registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PCNT    (BIT(6))
#define TEE_WRITE_REE1_PCNT_M  (TEE_WRITE_REE1_PCNT_V << TEE_WRITE_REE1_PCNT_S)
#define TEE_WRITE_REE1_PCNT_V  0x00000001U
#define TEE_WRITE_REE1_PCNT_S  6
/** TEE_WRITE_REE2_PCNT : R/W; bitpos: [7]; default: 0;
 *  Configures pcnt registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PCNT    (BIT(7))
#define TEE_WRITE_REE2_PCNT_M  (TEE_WRITE_REE2_PCNT_V << TEE_WRITE_REE2_PCNT_S)
#define TEE_WRITE_REE2_PCNT_V  0x00000001U
#define TEE_WRITE_REE2_PCNT_S  7

/** TEE_IOMUX_CTRL_REG register
 *  iomux read/write control register
 */
#define TEE_IOMUX_CTRL_REG (DR_REG_TEE_BASE + 0xf8)
/** TEE_READ_TEE_IOMUX : R/W; bitpos: [0]; default: 1;
 *  Configures iomux registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_IOMUX    (BIT(0))
#define TEE_READ_TEE_IOMUX_M  (TEE_READ_TEE_IOMUX_V << TEE_READ_TEE_IOMUX_S)
#define TEE_READ_TEE_IOMUX_V  0x00000001U
#define TEE_READ_TEE_IOMUX_S  0
/** TEE_READ_REE0_IOMUX : R/W; bitpos: [1]; default: 0;
 *  Configures iomux registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_IOMUX    (BIT(1))
#define TEE_READ_REE0_IOMUX_M  (TEE_READ_REE0_IOMUX_V << TEE_READ_REE0_IOMUX_S)
#define TEE_READ_REE0_IOMUX_V  0x00000001U
#define TEE_READ_REE0_IOMUX_S  1
/** TEE_READ_REE1_IOMUX : R/W; bitpos: [2]; default: 0;
 *  Configures iomux registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_IOMUX    (BIT(2))
#define TEE_READ_REE1_IOMUX_M  (TEE_READ_REE1_IOMUX_V << TEE_READ_REE1_IOMUX_S)
#define TEE_READ_REE1_IOMUX_V  0x00000001U
#define TEE_READ_REE1_IOMUX_S  2
/** TEE_READ_REE2_IOMUX : R/W; bitpos: [3]; default: 0;
 *  Configures iomux registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_IOMUX    (BIT(3))
#define TEE_READ_REE2_IOMUX_M  (TEE_READ_REE2_IOMUX_V << TEE_READ_REE2_IOMUX_S)
#define TEE_READ_REE2_IOMUX_V  0x00000001U
#define TEE_READ_REE2_IOMUX_S  3
/** TEE_WRITE_TEE_IOMUX : R/W; bitpos: [4]; default: 1;
 *  Configures iomux registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_IOMUX    (BIT(4))
#define TEE_WRITE_TEE_IOMUX_M  (TEE_WRITE_TEE_IOMUX_V << TEE_WRITE_TEE_IOMUX_S)
#define TEE_WRITE_TEE_IOMUX_V  0x00000001U
#define TEE_WRITE_TEE_IOMUX_S  4
/** TEE_WRITE_REE0_IOMUX : R/W; bitpos: [5]; default: 0;
 *  Configures iomux registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_IOMUX    (BIT(5))
#define TEE_WRITE_REE0_IOMUX_M  (TEE_WRITE_REE0_IOMUX_V << TEE_WRITE_REE0_IOMUX_S)
#define TEE_WRITE_REE0_IOMUX_V  0x00000001U
#define TEE_WRITE_REE0_IOMUX_S  5
/** TEE_WRITE_REE1_IOMUX : R/W; bitpos: [6]; default: 0;
 *  Configures iomux registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_IOMUX    (BIT(6))
#define TEE_WRITE_REE1_IOMUX_M  (TEE_WRITE_REE1_IOMUX_V << TEE_WRITE_REE1_IOMUX_S)
#define TEE_WRITE_REE1_IOMUX_V  0x00000001U
#define TEE_WRITE_REE1_IOMUX_S  6
/** TEE_WRITE_REE2_IOMUX : R/W; bitpos: [7]; default: 0;
 *  Configures iomux registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_IOMUX    (BIT(7))
#define TEE_WRITE_REE2_IOMUX_M  (TEE_WRITE_REE2_IOMUX_V << TEE_WRITE_REE2_IOMUX_S)
#define TEE_WRITE_REE2_IOMUX_V  0x00000001U
#define TEE_WRITE_REE2_IOMUX_S  7

/** TEE_PSRAM_MEM_MONITOR_CTRL_REG register
 *  psram_mem_monitor read/write control register
 */
#define TEE_PSRAM_MEM_MONITOR_CTRL_REG (DR_REG_TEE_BASE + 0xfc)
/** TEE_READ_TEE_PSRAM_MEM_MONITOR : R/W; bitpos: [0]; default: 1;
 *  Configures psram_mem_monitor registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PSRAM_MEM_MONITOR    (BIT(0))
#define TEE_READ_TEE_PSRAM_MEM_MONITOR_M  (TEE_READ_TEE_PSRAM_MEM_MONITOR_V << TEE_READ_TEE_PSRAM_MEM_MONITOR_S)
#define TEE_READ_TEE_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_READ_TEE_PSRAM_MEM_MONITOR_S  0
/** TEE_READ_REE0_PSRAM_MEM_MONITOR : R/W; bitpos: [1]; default: 0;
 *  Configures psram_mem_monitor registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PSRAM_MEM_MONITOR    (BIT(1))
#define TEE_READ_REE0_PSRAM_MEM_MONITOR_M  (TEE_READ_REE0_PSRAM_MEM_MONITOR_V << TEE_READ_REE0_PSRAM_MEM_MONITOR_S)
#define TEE_READ_REE0_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_READ_REE0_PSRAM_MEM_MONITOR_S  1
/** TEE_READ_REE1_PSRAM_MEM_MONITOR : R/W; bitpos: [2]; default: 0;
 *  Configures psram_mem_monitor registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PSRAM_MEM_MONITOR    (BIT(2))
#define TEE_READ_REE1_PSRAM_MEM_MONITOR_M  (TEE_READ_REE1_PSRAM_MEM_MONITOR_V << TEE_READ_REE1_PSRAM_MEM_MONITOR_S)
#define TEE_READ_REE1_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_READ_REE1_PSRAM_MEM_MONITOR_S  2
/** TEE_READ_REE2_PSRAM_MEM_MONITOR : R/W; bitpos: [3]; default: 0;
 *  Configures psram_mem_monitor registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PSRAM_MEM_MONITOR    (BIT(3))
#define TEE_READ_REE2_PSRAM_MEM_MONITOR_M  (TEE_READ_REE2_PSRAM_MEM_MONITOR_V << TEE_READ_REE2_PSRAM_MEM_MONITOR_S)
#define TEE_READ_REE2_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_READ_REE2_PSRAM_MEM_MONITOR_S  3
/** TEE_WRITE_TEE_PSRAM_MEM_MONITOR : R/W; bitpos: [4]; default: 1;
 *  Configures psram_mem_monitor registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PSRAM_MEM_MONITOR    (BIT(4))
#define TEE_WRITE_TEE_PSRAM_MEM_MONITOR_M  (TEE_WRITE_TEE_PSRAM_MEM_MONITOR_V << TEE_WRITE_TEE_PSRAM_MEM_MONITOR_S)
#define TEE_WRITE_TEE_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_WRITE_TEE_PSRAM_MEM_MONITOR_S  4
/** TEE_WRITE_REE0_PSRAM_MEM_MONITOR : R/W; bitpos: [5]; default: 0;
 *  Configures psram_mem_monitor registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PSRAM_MEM_MONITOR    (BIT(5))
#define TEE_WRITE_REE0_PSRAM_MEM_MONITOR_M  (TEE_WRITE_REE0_PSRAM_MEM_MONITOR_V << TEE_WRITE_REE0_PSRAM_MEM_MONITOR_S)
#define TEE_WRITE_REE0_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_WRITE_REE0_PSRAM_MEM_MONITOR_S  5
/** TEE_WRITE_REE1_PSRAM_MEM_MONITOR : R/W; bitpos: [6]; default: 0;
 *  Configures psram_mem_monitor registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PSRAM_MEM_MONITOR    (BIT(6))
#define TEE_WRITE_REE1_PSRAM_MEM_MONITOR_M  (TEE_WRITE_REE1_PSRAM_MEM_MONITOR_V << TEE_WRITE_REE1_PSRAM_MEM_MONITOR_S)
#define TEE_WRITE_REE1_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_WRITE_REE1_PSRAM_MEM_MONITOR_S  6
/** TEE_WRITE_REE2_PSRAM_MEM_MONITOR : R/W; bitpos: [7]; default: 0;
 *  Configures psram_mem_monitor registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PSRAM_MEM_MONITOR    (BIT(7))
#define TEE_WRITE_REE2_PSRAM_MEM_MONITOR_M  (TEE_WRITE_REE2_PSRAM_MEM_MONITOR_V << TEE_WRITE_REE2_PSRAM_MEM_MONITOR_S)
#define TEE_WRITE_REE2_PSRAM_MEM_MONITOR_V  0x00000001U
#define TEE_WRITE_REE2_PSRAM_MEM_MONITOR_S  7

/** TEE_MEM_ACS_MONITOR_CTRL_REG register
 *  mem_acs_monitor read/write control register
 */
#define TEE_MEM_ACS_MONITOR_CTRL_REG (DR_REG_TEE_BASE + 0x100)
/** TEE_READ_TEE_MEM_ACS_MONITOR : R/W; bitpos: [0]; default: 1;
 *  Configures mem_acs_monitor registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_MEM_ACS_MONITOR    (BIT(0))
#define TEE_READ_TEE_MEM_ACS_MONITOR_M  (TEE_READ_TEE_MEM_ACS_MONITOR_V << TEE_READ_TEE_MEM_ACS_MONITOR_S)
#define TEE_READ_TEE_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_READ_TEE_MEM_ACS_MONITOR_S  0
/** TEE_READ_REE0_MEM_ACS_MONITOR : R/W; bitpos: [1]; default: 0;
 *  Configures mem_acs_monitor registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_MEM_ACS_MONITOR    (BIT(1))
#define TEE_READ_REE0_MEM_ACS_MONITOR_M  (TEE_READ_REE0_MEM_ACS_MONITOR_V << TEE_READ_REE0_MEM_ACS_MONITOR_S)
#define TEE_READ_REE0_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_READ_REE0_MEM_ACS_MONITOR_S  1
/** TEE_READ_REE1_MEM_ACS_MONITOR : R/W; bitpos: [2]; default: 0;
 *  Configures mem_acs_monitor registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_MEM_ACS_MONITOR    (BIT(2))
#define TEE_READ_REE1_MEM_ACS_MONITOR_M  (TEE_READ_REE1_MEM_ACS_MONITOR_V << TEE_READ_REE1_MEM_ACS_MONITOR_S)
#define TEE_READ_REE1_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_READ_REE1_MEM_ACS_MONITOR_S  2
/** TEE_READ_REE2_MEM_ACS_MONITOR : R/W; bitpos: [3]; default: 0;
 *  Configures mem_acs_monitor registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_MEM_ACS_MONITOR    (BIT(3))
#define TEE_READ_REE2_MEM_ACS_MONITOR_M  (TEE_READ_REE2_MEM_ACS_MONITOR_V << TEE_READ_REE2_MEM_ACS_MONITOR_S)
#define TEE_READ_REE2_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_READ_REE2_MEM_ACS_MONITOR_S  3
/** TEE_WRITE_TEE_MEM_ACS_MONITOR : R/W; bitpos: [4]; default: 1;
 *  Configures mem_acs_monitor registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_MEM_ACS_MONITOR    (BIT(4))
#define TEE_WRITE_TEE_MEM_ACS_MONITOR_M  (TEE_WRITE_TEE_MEM_ACS_MONITOR_V << TEE_WRITE_TEE_MEM_ACS_MONITOR_S)
#define TEE_WRITE_TEE_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_WRITE_TEE_MEM_ACS_MONITOR_S  4
/** TEE_WRITE_REE0_MEM_ACS_MONITOR : R/W; bitpos: [5]; default: 0;
 *  Configures mem_acs_monitor registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_MEM_ACS_MONITOR    (BIT(5))
#define TEE_WRITE_REE0_MEM_ACS_MONITOR_M  (TEE_WRITE_REE0_MEM_ACS_MONITOR_V << TEE_WRITE_REE0_MEM_ACS_MONITOR_S)
#define TEE_WRITE_REE0_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_WRITE_REE0_MEM_ACS_MONITOR_S  5
/** TEE_WRITE_REE1_MEM_ACS_MONITOR : R/W; bitpos: [6]; default: 0;
 *  Configures mem_acs_monitor registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_MEM_ACS_MONITOR    (BIT(6))
#define TEE_WRITE_REE1_MEM_ACS_MONITOR_M  (TEE_WRITE_REE1_MEM_ACS_MONITOR_V << TEE_WRITE_REE1_MEM_ACS_MONITOR_S)
#define TEE_WRITE_REE1_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_WRITE_REE1_MEM_ACS_MONITOR_S  6
/** TEE_WRITE_REE2_MEM_ACS_MONITOR : R/W; bitpos: [7]; default: 0;
 *  Configures mem_acs_monitor registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_MEM_ACS_MONITOR    (BIT(7))
#define TEE_WRITE_REE2_MEM_ACS_MONITOR_M  (TEE_WRITE_REE2_MEM_ACS_MONITOR_V << TEE_WRITE_REE2_MEM_ACS_MONITOR_S)
#define TEE_WRITE_REE2_MEM_ACS_MONITOR_V  0x00000001U
#define TEE_WRITE_REE2_MEM_ACS_MONITOR_S  7

/** TEE_HP_SYSTEM_REG_CTRL_REG register
 *  hp_system_reg read/write control register
 */
#define TEE_HP_SYSTEM_REG_CTRL_REG (DR_REG_TEE_BASE + 0x104)
/** TEE_READ_TEE_HP_SYSTEM_REG : R/W; bitpos: [0]; default: 1;
 *  Configures hp_system_reg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_HP_SYSTEM_REG    (BIT(0))
#define TEE_READ_TEE_HP_SYSTEM_REG_M  (TEE_READ_TEE_HP_SYSTEM_REG_V << TEE_READ_TEE_HP_SYSTEM_REG_S)
#define TEE_READ_TEE_HP_SYSTEM_REG_V  0x00000001U
#define TEE_READ_TEE_HP_SYSTEM_REG_S  0
/** TEE_READ_REE0_HP_SYSTEM_REG : R/W; bitpos: [1]; default: 0;
 *  Configures hp_system_reg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_HP_SYSTEM_REG    (BIT(1))
#define TEE_READ_REE0_HP_SYSTEM_REG_M  (TEE_READ_REE0_HP_SYSTEM_REG_V << TEE_READ_REE0_HP_SYSTEM_REG_S)
#define TEE_READ_REE0_HP_SYSTEM_REG_V  0x00000001U
#define TEE_READ_REE0_HP_SYSTEM_REG_S  1
/** TEE_READ_REE1_HP_SYSTEM_REG : R/W; bitpos: [2]; default: 0;
 *  Configures hp_system_reg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_HP_SYSTEM_REG    (BIT(2))
#define TEE_READ_REE1_HP_SYSTEM_REG_M  (TEE_READ_REE1_HP_SYSTEM_REG_V << TEE_READ_REE1_HP_SYSTEM_REG_S)
#define TEE_READ_REE1_HP_SYSTEM_REG_V  0x00000001U
#define TEE_READ_REE1_HP_SYSTEM_REG_S  2
/** TEE_READ_REE2_HP_SYSTEM_REG : R/W; bitpos: [3]; default: 0;
 *  Configures hp_system_reg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_HP_SYSTEM_REG    (BIT(3))
#define TEE_READ_REE2_HP_SYSTEM_REG_M  (TEE_READ_REE2_HP_SYSTEM_REG_V << TEE_READ_REE2_HP_SYSTEM_REG_S)
#define TEE_READ_REE2_HP_SYSTEM_REG_V  0x00000001U
#define TEE_READ_REE2_HP_SYSTEM_REG_S  3
/** TEE_WRITE_TEE_HP_SYSTEM_REG : R/W; bitpos: [4]; default: 1;
 *  Configures hp_system_reg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_HP_SYSTEM_REG    (BIT(4))
#define TEE_WRITE_TEE_HP_SYSTEM_REG_M  (TEE_WRITE_TEE_HP_SYSTEM_REG_V << TEE_WRITE_TEE_HP_SYSTEM_REG_S)
#define TEE_WRITE_TEE_HP_SYSTEM_REG_V  0x00000001U
#define TEE_WRITE_TEE_HP_SYSTEM_REG_S  4
/** TEE_WRITE_REE0_HP_SYSTEM_REG : R/W; bitpos: [5]; default: 0;
 *  Configures hp_system_reg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_HP_SYSTEM_REG    (BIT(5))
#define TEE_WRITE_REE0_HP_SYSTEM_REG_M  (TEE_WRITE_REE0_HP_SYSTEM_REG_V << TEE_WRITE_REE0_HP_SYSTEM_REG_S)
#define TEE_WRITE_REE0_HP_SYSTEM_REG_V  0x00000001U
#define TEE_WRITE_REE0_HP_SYSTEM_REG_S  5
/** TEE_WRITE_REE1_HP_SYSTEM_REG : R/W; bitpos: [6]; default: 0;
 *  Configures hp_system_reg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_HP_SYSTEM_REG    (BIT(6))
#define TEE_WRITE_REE1_HP_SYSTEM_REG_M  (TEE_WRITE_REE1_HP_SYSTEM_REG_V << TEE_WRITE_REE1_HP_SYSTEM_REG_S)
#define TEE_WRITE_REE1_HP_SYSTEM_REG_V  0x00000001U
#define TEE_WRITE_REE1_HP_SYSTEM_REG_S  6
/** TEE_WRITE_REE2_HP_SYSTEM_REG : R/W; bitpos: [7]; default: 0;
 *  Configures hp_system_reg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_HP_SYSTEM_REG    (BIT(7))
#define TEE_WRITE_REE2_HP_SYSTEM_REG_M  (TEE_WRITE_REE2_HP_SYSTEM_REG_V << TEE_WRITE_REE2_HP_SYSTEM_REG_S)
#define TEE_WRITE_REE2_HP_SYSTEM_REG_V  0x00000001U
#define TEE_WRITE_REE2_HP_SYSTEM_REG_S  7

/** TEE_PCR_REG_CTRL_REG register
 *  pcr_reg read/write control register
 */
#define TEE_PCR_REG_CTRL_REG (DR_REG_TEE_BASE + 0x108)
/** TEE_READ_TEE_PCR_REG : R/W; bitpos: [0]; default: 1;
 *  Configures pcr_reg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PCR_REG    (BIT(0))
#define TEE_READ_TEE_PCR_REG_M  (TEE_READ_TEE_PCR_REG_V << TEE_READ_TEE_PCR_REG_S)
#define TEE_READ_TEE_PCR_REG_V  0x00000001U
#define TEE_READ_TEE_PCR_REG_S  0
/** TEE_READ_REE0_PCR_REG : R/W; bitpos: [1]; default: 0;
 *  Configures pcr_reg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PCR_REG    (BIT(1))
#define TEE_READ_REE0_PCR_REG_M  (TEE_READ_REE0_PCR_REG_V << TEE_READ_REE0_PCR_REG_S)
#define TEE_READ_REE0_PCR_REG_V  0x00000001U
#define TEE_READ_REE0_PCR_REG_S  1
/** TEE_READ_REE1_PCR_REG : R/W; bitpos: [2]; default: 0;
 *  Configures pcr_reg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PCR_REG    (BIT(2))
#define TEE_READ_REE1_PCR_REG_M  (TEE_READ_REE1_PCR_REG_V << TEE_READ_REE1_PCR_REG_S)
#define TEE_READ_REE1_PCR_REG_V  0x00000001U
#define TEE_READ_REE1_PCR_REG_S  2
/** TEE_READ_REE2_PCR_REG : R/W; bitpos: [3]; default: 0;
 *  Configures pcr_reg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PCR_REG    (BIT(3))
#define TEE_READ_REE2_PCR_REG_M  (TEE_READ_REE2_PCR_REG_V << TEE_READ_REE2_PCR_REG_S)
#define TEE_READ_REE2_PCR_REG_V  0x00000001U
#define TEE_READ_REE2_PCR_REG_S  3
/** TEE_WRITE_TEE_PCR_REG : R/W; bitpos: [4]; default: 1;
 *  Configures pcr_reg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PCR_REG    (BIT(4))
#define TEE_WRITE_TEE_PCR_REG_M  (TEE_WRITE_TEE_PCR_REG_V << TEE_WRITE_TEE_PCR_REG_S)
#define TEE_WRITE_TEE_PCR_REG_V  0x00000001U
#define TEE_WRITE_TEE_PCR_REG_S  4
/** TEE_WRITE_REE0_PCR_REG : R/W; bitpos: [5]; default: 0;
 *  Configures pcr_reg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PCR_REG    (BIT(5))
#define TEE_WRITE_REE0_PCR_REG_M  (TEE_WRITE_REE0_PCR_REG_V << TEE_WRITE_REE0_PCR_REG_S)
#define TEE_WRITE_REE0_PCR_REG_V  0x00000001U
#define TEE_WRITE_REE0_PCR_REG_S  5
/** TEE_WRITE_REE1_PCR_REG : R/W; bitpos: [6]; default: 0;
 *  Configures pcr_reg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PCR_REG    (BIT(6))
#define TEE_WRITE_REE1_PCR_REG_M  (TEE_WRITE_REE1_PCR_REG_V << TEE_WRITE_REE1_PCR_REG_S)
#define TEE_WRITE_REE1_PCR_REG_V  0x00000001U
#define TEE_WRITE_REE1_PCR_REG_S  6
/** TEE_WRITE_REE2_PCR_REG : R/W; bitpos: [7]; default: 0;
 *  Configures pcr_reg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PCR_REG    (BIT(7))
#define TEE_WRITE_REE2_PCR_REG_M  (TEE_WRITE_REE2_PCR_REG_V << TEE_WRITE_REE2_PCR_REG_S)
#define TEE_WRITE_REE2_PCR_REG_V  0x00000001U
#define TEE_WRITE_REE2_PCR_REG_S  7

/** TEE_MSPI_CTRL_REG register
 *  mspi read/write control register
 */
#define TEE_MSPI_CTRL_REG (DR_REG_TEE_BASE + 0x10c)
/** TEE_READ_TEE_MSPI : R/W; bitpos: [0]; default: 1;
 *  Configures mspi registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_MSPI    (BIT(0))
#define TEE_READ_TEE_MSPI_M  (TEE_READ_TEE_MSPI_V << TEE_READ_TEE_MSPI_S)
#define TEE_READ_TEE_MSPI_V  0x00000001U
#define TEE_READ_TEE_MSPI_S  0
/** TEE_READ_REE0_MSPI : R/W; bitpos: [1]; default: 0;
 *  Configures mspi registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_MSPI    (BIT(1))
#define TEE_READ_REE0_MSPI_M  (TEE_READ_REE0_MSPI_V << TEE_READ_REE0_MSPI_S)
#define TEE_READ_REE0_MSPI_V  0x00000001U
#define TEE_READ_REE0_MSPI_S  1
/** TEE_READ_REE1_MSPI : R/W; bitpos: [2]; default: 0;
 *  Configures mspi registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_MSPI    (BIT(2))
#define TEE_READ_REE1_MSPI_M  (TEE_READ_REE1_MSPI_V << TEE_READ_REE1_MSPI_S)
#define TEE_READ_REE1_MSPI_V  0x00000001U
#define TEE_READ_REE1_MSPI_S  2
/** TEE_READ_REE2_MSPI : R/W; bitpos: [3]; default: 0;
 *  Configures mspi registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_MSPI    (BIT(3))
#define TEE_READ_REE2_MSPI_M  (TEE_READ_REE2_MSPI_V << TEE_READ_REE2_MSPI_S)
#define TEE_READ_REE2_MSPI_V  0x00000001U
#define TEE_READ_REE2_MSPI_S  3
/** TEE_WRITE_TEE_MSPI : R/W; bitpos: [4]; default: 1;
 *  Configures mspi registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_MSPI    (BIT(4))
#define TEE_WRITE_TEE_MSPI_M  (TEE_WRITE_TEE_MSPI_V << TEE_WRITE_TEE_MSPI_S)
#define TEE_WRITE_TEE_MSPI_V  0x00000001U
#define TEE_WRITE_TEE_MSPI_S  4
/** TEE_WRITE_REE0_MSPI : R/W; bitpos: [5]; default: 0;
 *  Configures mspi registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_MSPI    (BIT(5))
#define TEE_WRITE_REE0_MSPI_M  (TEE_WRITE_REE0_MSPI_V << TEE_WRITE_REE0_MSPI_S)
#define TEE_WRITE_REE0_MSPI_V  0x00000001U
#define TEE_WRITE_REE0_MSPI_S  5
/** TEE_WRITE_REE1_MSPI : R/W; bitpos: [6]; default: 0;
 *  Configures mspi registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_MSPI    (BIT(6))
#define TEE_WRITE_REE1_MSPI_M  (TEE_WRITE_REE1_MSPI_V << TEE_WRITE_REE1_MSPI_S)
#define TEE_WRITE_REE1_MSPI_V  0x00000001U
#define TEE_WRITE_REE1_MSPI_S  6
/** TEE_WRITE_REE2_MSPI : R/W; bitpos: [7]; default: 0;
 *  Configures mspi registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_MSPI    (BIT(7))
#define TEE_WRITE_REE2_MSPI_M  (TEE_WRITE_REE2_MSPI_V << TEE_WRITE_REE2_MSPI_S)
#define TEE_WRITE_REE2_MSPI_V  0x00000001U
#define TEE_WRITE_REE2_MSPI_S  7

/** TEE_HP_APM_CTRL_REG register
 *  hp_apm read/write control register
 */
#define TEE_HP_APM_CTRL_REG (DR_REG_TEE_BASE + 0x110)
/** TEE_READ_TEE_HP_APM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_HP_APM    (BIT(0))
#define TEE_READ_TEE_HP_APM_M  (TEE_READ_TEE_HP_APM_V << TEE_READ_TEE_HP_APM_S)
#define TEE_READ_TEE_HP_APM_V  0x00000001U
#define TEE_READ_TEE_HP_APM_S  0
/** TEE_READ_REE0_HP_APM : HRO; bitpos: [1]; default: 0;
 *  Configures hp_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_HP_APM    (BIT(1))
#define TEE_READ_REE0_HP_APM_M  (TEE_READ_REE0_HP_APM_V << TEE_READ_REE0_HP_APM_S)
#define TEE_READ_REE0_HP_APM_V  0x00000001U
#define TEE_READ_REE0_HP_APM_S  1
/** TEE_READ_REE1_HP_APM : HRO; bitpos: [2]; default: 0;
 *  Configures hp_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_HP_APM    (BIT(2))
#define TEE_READ_REE1_HP_APM_M  (TEE_READ_REE1_HP_APM_V << TEE_READ_REE1_HP_APM_S)
#define TEE_READ_REE1_HP_APM_V  0x00000001U
#define TEE_READ_REE1_HP_APM_S  2
/** TEE_READ_REE2_HP_APM : HRO; bitpos: [3]; default: 0;
 *  Configures hp_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_HP_APM    (BIT(3))
#define TEE_READ_REE2_HP_APM_M  (TEE_READ_REE2_HP_APM_V << TEE_READ_REE2_HP_APM_S)
#define TEE_READ_REE2_HP_APM_V  0x00000001U
#define TEE_READ_REE2_HP_APM_S  3
/** TEE_WRITE_TEE_HP_APM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_HP_APM    (BIT(4))
#define TEE_WRITE_TEE_HP_APM_M  (TEE_WRITE_TEE_HP_APM_V << TEE_WRITE_TEE_HP_APM_S)
#define TEE_WRITE_TEE_HP_APM_V  0x00000001U
#define TEE_WRITE_TEE_HP_APM_S  4
/** TEE_WRITE_REE0_HP_APM : HRO; bitpos: [5]; default: 0;
 *  Configures hp_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_HP_APM    (BIT(5))
#define TEE_WRITE_REE0_HP_APM_M  (TEE_WRITE_REE0_HP_APM_V << TEE_WRITE_REE0_HP_APM_S)
#define TEE_WRITE_REE0_HP_APM_V  0x00000001U
#define TEE_WRITE_REE0_HP_APM_S  5
/** TEE_WRITE_REE1_HP_APM : HRO; bitpos: [6]; default: 0;
 *  Configures hp_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_HP_APM    (BIT(6))
#define TEE_WRITE_REE1_HP_APM_M  (TEE_WRITE_REE1_HP_APM_V << TEE_WRITE_REE1_HP_APM_S)
#define TEE_WRITE_REE1_HP_APM_V  0x00000001U
#define TEE_WRITE_REE1_HP_APM_S  6
/** TEE_WRITE_REE2_HP_APM : HRO; bitpos: [7]; default: 0;
 *  Configures hp_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_HP_APM    (BIT(7))
#define TEE_WRITE_REE2_HP_APM_M  (TEE_WRITE_REE2_HP_APM_V << TEE_WRITE_REE2_HP_APM_S)
#define TEE_WRITE_REE2_HP_APM_V  0x00000001U
#define TEE_WRITE_REE2_HP_APM_S  7

/** TEE_HP_MEM_APM_CTRL_REG register
 *  hp_mem_apm read/write control register
 */
#define TEE_HP_MEM_APM_CTRL_REG (DR_REG_TEE_BASE + 0x114)
/** TEE_READ_TEE_HP_MEM_APM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_mem_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_HP_MEM_APM    (BIT(0))
#define TEE_READ_TEE_HP_MEM_APM_M  (TEE_READ_TEE_HP_MEM_APM_V << TEE_READ_TEE_HP_MEM_APM_S)
#define TEE_READ_TEE_HP_MEM_APM_V  0x00000001U
#define TEE_READ_TEE_HP_MEM_APM_S  0
/** TEE_READ_REE0_HP_MEM_APM : HRO; bitpos: [1]; default: 0;
 *  Configures hp_mem_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_HP_MEM_APM    (BIT(1))
#define TEE_READ_REE0_HP_MEM_APM_M  (TEE_READ_REE0_HP_MEM_APM_V << TEE_READ_REE0_HP_MEM_APM_S)
#define TEE_READ_REE0_HP_MEM_APM_V  0x00000001U
#define TEE_READ_REE0_HP_MEM_APM_S  1
/** TEE_READ_REE1_HP_MEM_APM : HRO; bitpos: [2]; default: 0;
 *  Configures hp_mem_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_HP_MEM_APM    (BIT(2))
#define TEE_READ_REE1_HP_MEM_APM_M  (TEE_READ_REE1_HP_MEM_APM_V << TEE_READ_REE1_HP_MEM_APM_S)
#define TEE_READ_REE1_HP_MEM_APM_V  0x00000001U
#define TEE_READ_REE1_HP_MEM_APM_S  2
/** TEE_READ_REE2_HP_MEM_APM : HRO; bitpos: [3]; default: 0;
 *  Configures hp_mem_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_HP_MEM_APM    (BIT(3))
#define TEE_READ_REE2_HP_MEM_APM_M  (TEE_READ_REE2_HP_MEM_APM_V << TEE_READ_REE2_HP_MEM_APM_S)
#define TEE_READ_REE2_HP_MEM_APM_V  0x00000001U
#define TEE_READ_REE2_HP_MEM_APM_S  3
/** TEE_WRITE_TEE_HP_MEM_APM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_mem_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_HP_MEM_APM    (BIT(4))
#define TEE_WRITE_TEE_HP_MEM_APM_M  (TEE_WRITE_TEE_HP_MEM_APM_V << TEE_WRITE_TEE_HP_MEM_APM_S)
#define TEE_WRITE_TEE_HP_MEM_APM_V  0x00000001U
#define TEE_WRITE_TEE_HP_MEM_APM_S  4
/** TEE_WRITE_REE0_HP_MEM_APM : HRO; bitpos: [5]; default: 0;
 *  Configures hp_mem_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_HP_MEM_APM    (BIT(5))
#define TEE_WRITE_REE0_HP_MEM_APM_M  (TEE_WRITE_REE0_HP_MEM_APM_V << TEE_WRITE_REE0_HP_MEM_APM_S)
#define TEE_WRITE_REE0_HP_MEM_APM_V  0x00000001U
#define TEE_WRITE_REE0_HP_MEM_APM_S  5
/** TEE_WRITE_REE1_HP_MEM_APM : HRO; bitpos: [6]; default: 0;
 *  Configures hp_mem_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_HP_MEM_APM    (BIT(6))
#define TEE_WRITE_REE1_HP_MEM_APM_M  (TEE_WRITE_REE1_HP_MEM_APM_V << TEE_WRITE_REE1_HP_MEM_APM_S)
#define TEE_WRITE_REE1_HP_MEM_APM_V  0x00000001U
#define TEE_WRITE_REE1_HP_MEM_APM_S  6
/** TEE_WRITE_REE2_HP_MEM_APM : HRO; bitpos: [7]; default: 0;
 *  Configures hp_mem_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_HP_MEM_APM    (BIT(7))
#define TEE_WRITE_REE2_HP_MEM_APM_M  (TEE_WRITE_REE2_HP_MEM_APM_V << TEE_WRITE_REE2_HP_MEM_APM_S)
#define TEE_WRITE_REE2_HP_MEM_APM_V  0x00000001U
#define TEE_WRITE_REE2_HP_MEM_APM_S  7

/** TEE_CPU_APM_CTRL_REG register
 *  cpu_apm read/write control register
 */
#define TEE_CPU_APM_CTRL_REG (DR_REG_TEE_BASE + 0x118)
/** TEE_READ_TEE_CPU_APM : R/W; bitpos: [0]; default: 1;
 *  Configures cpu_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_CPU_APM    (BIT(0))
#define TEE_READ_TEE_CPU_APM_M  (TEE_READ_TEE_CPU_APM_V << TEE_READ_TEE_CPU_APM_S)
#define TEE_READ_TEE_CPU_APM_V  0x00000001U
#define TEE_READ_TEE_CPU_APM_S  0
/** TEE_READ_REE0_CPU_APM : HRO; bitpos: [1]; default: 0;
 *  Configures cpu_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_CPU_APM    (BIT(1))
#define TEE_READ_REE0_CPU_APM_M  (TEE_READ_REE0_CPU_APM_V << TEE_READ_REE0_CPU_APM_S)
#define TEE_READ_REE0_CPU_APM_V  0x00000001U
#define TEE_READ_REE0_CPU_APM_S  1
/** TEE_READ_REE1_CPU_APM : HRO; bitpos: [2]; default: 0;
 *  Configures cpu_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_CPU_APM    (BIT(2))
#define TEE_READ_REE1_CPU_APM_M  (TEE_READ_REE1_CPU_APM_V << TEE_READ_REE1_CPU_APM_S)
#define TEE_READ_REE1_CPU_APM_V  0x00000001U
#define TEE_READ_REE1_CPU_APM_S  2
/** TEE_READ_REE2_CPU_APM : HRO; bitpos: [3]; default: 0;
 *  Configures cpu_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_CPU_APM    (BIT(3))
#define TEE_READ_REE2_CPU_APM_M  (TEE_READ_REE2_CPU_APM_V << TEE_READ_REE2_CPU_APM_S)
#define TEE_READ_REE2_CPU_APM_V  0x00000001U
#define TEE_READ_REE2_CPU_APM_S  3
/** TEE_WRITE_TEE_CPU_APM : R/W; bitpos: [4]; default: 1;
 *  Configures cpu_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_CPU_APM    (BIT(4))
#define TEE_WRITE_TEE_CPU_APM_M  (TEE_WRITE_TEE_CPU_APM_V << TEE_WRITE_TEE_CPU_APM_S)
#define TEE_WRITE_TEE_CPU_APM_V  0x00000001U
#define TEE_WRITE_TEE_CPU_APM_S  4
/** TEE_WRITE_REE0_CPU_APM : HRO; bitpos: [5]; default: 0;
 *  Configures cpu_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_CPU_APM    (BIT(5))
#define TEE_WRITE_REE0_CPU_APM_M  (TEE_WRITE_REE0_CPU_APM_V << TEE_WRITE_REE0_CPU_APM_S)
#define TEE_WRITE_REE0_CPU_APM_V  0x00000001U
#define TEE_WRITE_REE0_CPU_APM_S  5
/** TEE_WRITE_REE1_CPU_APM : HRO; bitpos: [6]; default: 0;
 *  Configures cpu_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_CPU_APM    (BIT(6))
#define TEE_WRITE_REE1_CPU_APM_M  (TEE_WRITE_REE1_CPU_APM_V << TEE_WRITE_REE1_CPU_APM_S)
#define TEE_WRITE_REE1_CPU_APM_V  0x00000001U
#define TEE_WRITE_REE1_CPU_APM_S  6
/** TEE_WRITE_REE2_CPU_APM : HRO; bitpos: [7]; default: 0;
 *  Configures cpu_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_CPU_APM    (BIT(7))
#define TEE_WRITE_REE2_CPU_APM_M  (TEE_WRITE_REE2_CPU_APM_V << TEE_WRITE_REE2_CPU_APM_S)
#define TEE_WRITE_REE2_CPU_APM_V  0x00000001U
#define TEE_WRITE_REE2_CPU_APM_S  7

/** TEE_TEE_CTRL_REG register
 *  tee read/write control register
 */
#define TEE_TEE_CTRL_REG (DR_REG_TEE_BASE + 0x11c)
/** TEE_READ_TEE_TEE : R/W; bitpos: [0]; default: 1;
 *  Configures tee registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_TEE    (BIT(0))
#define TEE_READ_TEE_TEE_M  (TEE_READ_TEE_TEE_V << TEE_READ_TEE_TEE_S)
#define TEE_READ_TEE_TEE_V  0x00000001U
#define TEE_READ_TEE_TEE_S  0
/** TEE_READ_REE0_TEE : HRO; bitpos: [1]; default: 0;
 *  Configures tee registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_TEE    (BIT(1))
#define TEE_READ_REE0_TEE_M  (TEE_READ_REE0_TEE_V << TEE_READ_REE0_TEE_S)
#define TEE_READ_REE0_TEE_V  0x00000001U
#define TEE_READ_REE0_TEE_S  1
/** TEE_READ_REE1_TEE : HRO; bitpos: [2]; default: 0;
 *  Configures tee registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_TEE    (BIT(2))
#define TEE_READ_REE1_TEE_M  (TEE_READ_REE1_TEE_V << TEE_READ_REE1_TEE_S)
#define TEE_READ_REE1_TEE_V  0x00000001U
#define TEE_READ_REE1_TEE_S  2
/** TEE_READ_REE2_TEE : HRO; bitpos: [3]; default: 0;
 *  Configures tee registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_TEE    (BIT(3))
#define TEE_READ_REE2_TEE_M  (TEE_READ_REE2_TEE_V << TEE_READ_REE2_TEE_S)
#define TEE_READ_REE2_TEE_V  0x00000001U
#define TEE_READ_REE2_TEE_S  3
/** TEE_WRITE_TEE_TEE : R/W; bitpos: [4]; default: 1;
 *  Configures tee registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_TEE    (BIT(4))
#define TEE_WRITE_TEE_TEE_M  (TEE_WRITE_TEE_TEE_V << TEE_WRITE_TEE_TEE_S)
#define TEE_WRITE_TEE_TEE_V  0x00000001U
#define TEE_WRITE_TEE_TEE_S  4
/** TEE_WRITE_REE0_TEE : HRO; bitpos: [5]; default: 0;
 *  Configures tee registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_TEE    (BIT(5))
#define TEE_WRITE_REE0_TEE_M  (TEE_WRITE_REE0_TEE_V << TEE_WRITE_REE0_TEE_S)
#define TEE_WRITE_REE0_TEE_V  0x00000001U
#define TEE_WRITE_REE0_TEE_S  5
/** TEE_WRITE_REE1_TEE : HRO; bitpos: [6]; default: 0;
 *  Configures tee registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_TEE    (BIT(6))
#define TEE_WRITE_REE1_TEE_M  (TEE_WRITE_REE1_TEE_V << TEE_WRITE_REE1_TEE_S)
#define TEE_WRITE_REE1_TEE_V  0x00000001U
#define TEE_WRITE_REE1_TEE_S  6
/** TEE_WRITE_REE2_TEE : HRO; bitpos: [7]; default: 0;
 *  Configures tee registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_TEE    (BIT(7))
#define TEE_WRITE_REE2_TEE_M  (TEE_WRITE_REE2_TEE_V << TEE_WRITE_REE2_TEE_S)
#define TEE_WRITE_REE2_TEE_V  0x00000001U
#define TEE_WRITE_REE2_TEE_S  7

/** TEE_KM_CTRL_REG register
 *  crypt read/write control register
 */
#define TEE_KM_CTRL_REG (DR_REG_TEE_BASE + 0x120)
/** TEE_READ_TEE_KM : R/W; bitpos: [0]; default: 1;
 *  Configures km registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_KM    (BIT(0))
#define TEE_READ_TEE_KM_M  (TEE_READ_TEE_KM_V << TEE_READ_TEE_KM_S)
#define TEE_READ_TEE_KM_V  0x00000001U
#define TEE_READ_TEE_KM_S  0
/** TEE_READ_REE0_KM : R/W; bitpos: [1]; default: 0;
 *  Configures km registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_KM    (BIT(1))
#define TEE_READ_REE0_KM_M  (TEE_READ_REE0_KM_V << TEE_READ_REE0_KM_S)
#define TEE_READ_REE0_KM_V  0x00000001U
#define TEE_READ_REE0_KM_S  1
/** TEE_READ_REE1_KM : R/W; bitpos: [2]; default: 0;
 *  Configures km registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_KM    (BIT(2))
#define TEE_READ_REE1_KM_M  (TEE_READ_REE1_KM_V << TEE_READ_REE1_KM_S)
#define TEE_READ_REE1_KM_V  0x00000001U
#define TEE_READ_REE1_KM_S  2
/** TEE_READ_REE2_KM : R/W; bitpos: [3]; default: 0;
 *  Configures km registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_KM    (BIT(3))
#define TEE_READ_REE2_KM_M  (TEE_READ_REE2_KM_V << TEE_READ_REE2_KM_S)
#define TEE_READ_REE2_KM_V  0x00000001U
#define TEE_READ_REE2_KM_S  3
/** TEE_WRITE_TEE_KM : R/W; bitpos: [4]; default: 1;
 *  Configures km registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_KM    (BIT(4))
#define TEE_WRITE_TEE_KM_M  (TEE_WRITE_TEE_KM_V << TEE_WRITE_TEE_KM_S)
#define TEE_WRITE_TEE_KM_V  0x00000001U
#define TEE_WRITE_TEE_KM_S  4
/** TEE_WRITE_REE0_KM : R/W; bitpos: [5]; default: 0;
 *  Configures km registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_KM    (BIT(5))
#define TEE_WRITE_REE0_KM_M  (TEE_WRITE_REE0_KM_V << TEE_WRITE_REE0_KM_S)
#define TEE_WRITE_REE0_KM_V  0x00000001U
#define TEE_WRITE_REE0_KM_S  5
/** TEE_WRITE_REE1_KM : R/W; bitpos: [6]; default: 0;
 *  Configures km registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_KM    (BIT(6))
#define TEE_WRITE_REE1_KM_M  (TEE_WRITE_REE1_KM_V << TEE_WRITE_REE1_KM_S)
#define TEE_WRITE_REE1_KM_V  0x00000001U
#define TEE_WRITE_REE1_KM_S  6
/** TEE_WRITE_REE2_KM : R/W; bitpos: [7]; default: 0;
 *  Configures km registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_KM    (BIT(7))
#define TEE_WRITE_REE2_KM_M  (TEE_WRITE_REE2_KM_V << TEE_WRITE_REE2_KM_S)
#define TEE_WRITE_REE2_KM_V  0x00000001U
#define TEE_WRITE_REE2_KM_S  7

/** TEE_CRYPT_CTRL_REG register
 *  crypt read/write control register
 */
#define TEE_CRYPT_CTRL_REG (DR_REG_TEE_BASE + 0x124)
/** TEE_READ_TEE_CRYPT : R/W; bitpos: [0]; default: 1;
 *  Configures crypt registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_CRYPT    (BIT(0))
#define TEE_READ_TEE_CRYPT_M  (TEE_READ_TEE_CRYPT_V << TEE_READ_TEE_CRYPT_S)
#define TEE_READ_TEE_CRYPT_V  0x00000001U
#define TEE_READ_TEE_CRYPT_S  0
/** TEE_READ_REE0_CRYPT : R/W; bitpos: [1]; default: 0;
 *  Configures crypt registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_CRYPT    (BIT(1))
#define TEE_READ_REE0_CRYPT_M  (TEE_READ_REE0_CRYPT_V << TEE_READ_REE0_CRYPT_S)
#define TEE_READ_REE0_CRYPT_V  0x00000001U
#define TEE_READ_REE0_CRYPT_S  1
/** TEE_READ_REE1_CRYPT : R/W; bitpos: [2]; default: 0;
 *  Configures crypt registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_CRYPT    (BIT(2))
#define TEE_READ_REE1_CRYPT_M  (TEE_READ_REE1_CRYPT_V << TEE_READ_REE1_CRYPT_S)
#define TEE_READ_REE1_CRYPT_V  0x00000001U
#define TEE_READ_REE1_CRYPT_S  2
/** TEE_READ_REE2_CRYPT : R/W; bitpos: [3]; default: 0;
 *  Configures crypt registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_CRYPT    (BIT(3))
#define TEE_READ_REE2_CRYPT_M  (TEE_READ_REE2_CRYPT_V << TEE_READ_REE2_CRYPT_S)
#define TEE_READ_REE2_CRYPT_V  0x00000001U
#define TEE_READ_REE2_CRYPT_S  3
/** TEE_WRITE_TEE_CRYPT : R/W; bitpos: [4]; default: 1;
 *  Configures crypt registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_CRYPT    (BIT(4))
#define TEE_WRITE_TEE_CRYPT_M  (TEE_WRITE_TEE_CRYPT_V << TEE_WRITE_TEE_CRYPT_S)
#define TEE_WRITE_TEE_CRYPT_V  0x00000001U
#define TEE_WRITE_TEE_CRYPT_S  4
/** TEE_WRITE_REE0_CRYPT : R/W; bitpos: [5]; default: 0;
 *  Configures crypt registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_CRYPT    (BIT(5))
#define TEE_WRITE_REE0_CRYPT_M  (TEE_WRITE_REE0_CRYPT_V << TEE_WRITE_REE0_CRYPT_S)
#define TEE_WRITE_REE0_CRYPT_V  0x00000001U
#define TEE_WRITE_REE0_CRYPT_S  5
/** TEE_WRITE_REE1_CRYPT : R/W; bitpos: [6]; default: 0;
 *  Configures crypt registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_CRYPT    (BIT(6))
#define TEE_WRITE_REE1_CRYPT_M  (TEE_WRITE_REE1_CRYPT_V << TEE_WRITE_REE1_CRYPT_S)
#define TEE_WRITE_REE1_CRYPT_V  0x00000001U
#define TEE_WRITE_REE1_CRYPT_S  6
/** TEE_WRITE_REE2_CRYPT : R/W; bitpos: [7]; default: 0;
 *  Configures crypt registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_CRYPT    (BIT(7))
#define TEE_WRITE_REE2_CRYPT_M  (TEE_WRITE_REE2_CRYPT_V << TEE_WRITE_REE2_CRYPT_S)
#define TEE_WRITE_REE2_CRYPT_V  0x00000001U
#define TEE_WRITE_REE2_CRYPT_S  7

/** TEE_CORE0_TRACE_CTRL_REG register
 *  core0_trace read/write control register
 */
#define TEE_CORE0_TRACE_CTRL_REG (DR_REG_TEE_BASE + 0x128)
/** TEE_READ_TEE_CORE0_TRACE : R/W; bitpos: [0]; default: 1;
 *  Configures core0_trace registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_CORE0_TRACE    (BIT(0))
#define TEE_READ_TEE_CORE0_TRACE_M  (TEE_READ_TEE_CORE0_TRACE_V << TEE_READ_TEE_CORE0_TRACE_S)
#define TEE_READ_TEE_CORE0_TRACE_V  0x00000001U
#define TEE_READ_TEE_CORE0_TRACE_S  0
/** TEE_READ_REE0_CORE0_TRACE : R/W; bitpos: [1]; default: 0;
 *  Configures core0_trace registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_CORE0_TRACE    (BIT(1))
#define TEE_READ_REE0_CORE0_TRACE_M  (TEE_READ_REE0_CORE0_TRACE_V << TEE_READ_REE0_CORE0_TRACE_S)
#define TEE_READ_REE0_CORE0_TRACE_V  0x00000001U
#define TEE_READ_REE0_CORE0_TRACE_S  1
/** TEE_READ_REE1_CORE0_TRACE : R/W; bitpos: [2]; default: 0;
 *  Configures core0_trace registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_CORE0_TRACE    (BIT(2))
#define TEE_READ_REE1_CORE0_TRACE_M  (TEE_READ_REE1_CORE0_TRACE_V << TEE_READ_REE1_CORE0_TRACE_S)
#define TEE_READ_REE1_CORE0_TRACE_V  0x00000001U
#define TEE_READ_REE1_CORE0_TRACE_S  2
/** TEE_READ_REE2_CORE0_TRACE : R/W; bitpos: [3]; default: 0;
 *  Configures core0_trace registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_CORE0_TRACE    (BIT(3))
#define TEE_READ_REE2_CORE0_TRACE_M  (TEE_READ_REE2_CORE0_TRACE_V << TEE_READ_REE2_CORE0_TRACE_S)
#define TEE_READ_REE2_CORE0_TRACE_V  0x00000001U
#define TEE_READ_REE2_CORE0_TRACE_S  3
/** TEE_WRITE_TEE_CORE0_TRACE : R/W; bitpos: [4]; default: 1;
 *  Configures core0_trace registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_CORE0_TRACE    (BIT(4))
#define TEE_WRITE_TEE_CORE0_TRACE_M  (TEE_WRITE_TEE_CORE0_TRACE_V << TEE_WRITE_TEE_CORE0_TRACE_S)
#define TEE_WRITE_TEE_CORE0_TRACE_V  0x00000001U
#define TEE_WRITE_TEE_CORE0_TRACE_S  4
/** TEE_WRITE_REE0_CORE0_TRACE : R/W; bitpos: [5]; default: 0;
 *  Configures core0_trace registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_CORE0_TRACE    (BIT(5))
#define TEE_WRITE_REE0_CORE0_TRACE_M  (TEE_WRITE_REE0_CORE0_TRACE_V << TEE_WRITE_REE0_CORE0_TRACE_S)
#define TEE_WRITE_REE0_CORE0_TRACE_V  0x00000001U
#define TEE_WRITE_REE0_CORE0_TRACE_S  5
/** TEE_WRITE_REE1_CORE0_TRACE : R/W; bitpos: [6]; default: 0;
 *  Configures core0_trace registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_CORE0_TRACE    (BIT(6))
#define TEE_WRITE_REE1_CORE0_TRACE_M  (TEE_WRITE_REE1_CORE0_TRACE_V << TEE_WRITE_REE1_CORE0_TRACE_S)
#define TEE_WRITE_REE1_CORE0_TRACE_V  0x00000001U
#define TEE_WRITE_REE1_CORE0_TRACE_S  6
/** TEE_WRITE_REE2_CORE0_TRACE : R/W; bitpos: [7]; default: 0;
 *  Configures core0_trace registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_CORE0_TRACE    (BIT(7))
#define TEE_WRITE_REE2_CORE0_TRACE_M  (TEE_WRITE_REE2_CORE0_TRACE_V << TEE_WRITE_REE2_CORE0_TRACE_S)
#define TEE_WRITE_REE2_CORE0_TRACE_V  0x00000001U
#define TEE_WRITE_REE2_CORE0_TRACE_S  7

/** TEE_CORE1_TRACE_CTRL_REG register
 *  core1_trace read/write control register
 */
#define TEE_CORE1_TRACE_CTRL_REG (DR_REG_TEE_BASE + 0x12c)
/** TEE_READ_TEE_CORE1_TRACE : R/W; bitpos: [0]; default: 1;
 *  Configures core1_trace registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_CORE1_TRACE    (BIT(0))
#define TEE_READ_TEE_CORE1_TRACE_M  (TEE_READ_TEE_CORE1_TRACE_V << TEE_READ_TEE_CORE1_TRACE_S)
#define TEE_READ_TEE_CORE1_TRACE_V  0x00000001U
#define TEE_READ_TEE_CORE1_TRACE_S  0
/** TEE_READ_REE0_CORE1_TRACE : R/W; bitpos: [1]; default: 0;
 *  Configures core1_trace registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_CORE1_TRACE    (BIT(1))
#define TEE_READ_REE0_CORE1_TRACE_M  (TEE_READ_REE0_CORE1_TRACE_V << TEE_READ_REE0_CORE1_TRACE_S)
#define TEE_READ_REE0_CORE1_TRACE_V  0x00000001U
#define TEE_READ_REE0_CORE1_TRACE_S  1
/** TEE_READ_REE1_CORE1_TRACE : R/W; bitpos: [2]; default: 0;
 *  Configures core1_trace registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_CORE1_TRACE    (BIT(2))
#define TEE_READ_REE1_CORE1_TRACE_M  (TEE_READ_REE1_CORE1_TRACE_V << TEE_READ_REE1_CORE1_TRACE_S)
#define TEE_READ_REE1_CORE1_TRACE_V  0x00000001U
#define TEE_READ_REE1_CORE1_TRACE_S  2
/** TEE_READ_REE2_CORE1_TRACE : R/W; bitpos: [3]; default: 0;
 *  Configures core1_trace registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_CORE1_TRACE    (BIT(3))
#define TEE_READ_REE2_CORE1_TRACE_M  (TEE_READ_REE2_CORE1_TRACE_V << TEE_READ_REE2_CORE1_TRACE_S)
#define TEE_READ_REE2_CORE1_TRACE_V  0x00000001U
#define TEE_READ_REE2_CORE1_TRACE_S  3
/** TEE_WRITE_TEE_CORE1_TRACE : R/W; bitpos: [4]; default: 1;
 *  Configures core1_trace registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_CORE1_TRACE    (BIT(4))
#define TEE_WRITE_TEE_CORE1_TRACE_M  (TEE_WRITE_TEE_CORE1_TRACE_V << TEE_WRITE_TEE_CORE1_TRACE_S)
#define TEE_WRITE_TEE_CORE1_TRACE_V  0x00000001U
#define TEE_WRITE_TEE_CORE1_TRACE_S  4
/** TEE_WRITE_REE0_CORE1_TRACE : R/W; bitpos: [5]; default: 0;
 *  Configures core1_trace registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_CORE1_TRACE    (BIT(5))
#define TEE_WRITE_REE0_CORE1_TRACE_M  (TEE_WRITE_REE0_CORE1_TRACE_V << TEE_WRITE_REE0_CORE1_TRACE_S)
#define TEE_WRITE_REE0_CORE1_TRACE_V  0x00000001U
#define TEE_WRITE_REE0_CORE1_TRACE_S  5
/** TEE_WRITE_REE1_CORE1_TRACE : R/W; bitpos: [6]; default: 0;
 *  Configures core1_trace registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_CORE1_TRACE    (BIT(6))
#define TEE_WRITE_REE1_CORE1_TRACE_M  (TEE_WRITE_REE1_CORE1_TRACE_V << TEE_WRITE_REE1_CORE1_TRACE_S)
#define TEE_WRITE_REE1_CORE1_TRACE_V  0x00000001U
#define TEE_WRITE_REE1_CORE1_TRACE_S  6
/** TEE_WRITE_REE2_CORE1_TRACE : R/W; bitpos: [7]; default: 0;
 *  Configures core1_trace registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_CORE1_TRACE    (BIT(7))
#define TEE_WRITE_REE2_CORE1_TRACE_M  (TEE_WRITE_REE2_CORE1_TRACE_V << TEE_WRITE_REE2_CORE1_TRACE_S)
#define TEE_WRITE_REE2_CORE1_TRACE_V  0x00000001U
#define TEE_WRITE_REE2_CORE1_TRACE_S  7

/** TEE_CPU_BUS_MONITOR_CTRL_REG register
 *  cpu_bus_monitor read/write control register
 */
#define TEE_CPU_BUS_MONITOR_CTRL_REG (DR_REG_TEE_BASE + 0x130)
/** TEE_READ_TEE_CPU_BUS_MONITOR : R/W; bitpos: [0]; default: 1;
 *  Configures cpu_bus_monitor registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_CPU_BUS_MONITOR    (BIT(0))
#define TEE_READ_TEE_CPU_BUS_MONITOR_M  (TEE_READ_TEE_CPU_BUS_MONITOR_V << TEE_READ_TEE_CPU_BUS_MONITOR_S)
#define TEE_READ_TEE_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_READ_TEE_CPU_BUS_MONITOR_S  0
/** TEE_READ_REE0_CPU_BUS_MONITOR : R/W; bitpos: [1]; default: 0;
 *  Configures cpu_bus_monitor registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_CPU_BUS_MONITOR    (BIT(1))
#define TEE_READ_REE0_CPU_BUS_MONITOR_M  (TEE_READ_REE0_CPU_BUS_MONITOR_V << TEE_READ_REE0_CPU_BUS_MONITOR_S)
#define TEE_READ_REE0_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_READ_REE0_CPU_BUS_MONITOR_S  1
/** TEE_READ_REE1_CPU_BUS_MONITOR : R/W; bitpos: [2]; default: 0;
 *  Configures cpu_bus_monitor registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_CPU_BUS_MONITOR    (BIT(2))
#define TEE_READ_REE1_CPU_BUS_MONITOR_M  (TEE_READ_REE1_CPU_BUS_MONITOR_V << TEE_READ_REE1_CPU_BUS_MONITOR_S)
#define TEE_READ_REE1_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_READ_REE1_CPU_BUS_MONITOR_S  2
/** TEE_READ_REE2_CPU_BUS_MONITOR : R/W; bitpos: [3]; default: 0;
 *  Configures cpu_bus_monitor registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_CPU_BUS_MONITOR    (BIT(3))
#define TEE_READ_REE2_CPU_BUS_MONITOR_M  (TEE_READ_REE2_CPU_BUS_MONITOR_V << TEE_READ_REE2_CPU_BUS_MONITOR_S)
#define TEE_READ_REE2_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_READ_REE2_CPU_BUS_MONITOR_S  3
/** TEE_WRITE_TEE_CPU_BUS_MONITOR : R/W; bitpos: [4]; default: 1;
 *  Configures cpu_bus_monitor registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_CPU_BUS_MONITOR    (BIT(4))
#define TEE_WRITE_TEE_CPU_BUS_MONITOR_M  (TEE_WRITE_TEE_CPU_BUS_MONITOR_V << TEE_WRITE_TEE_CPU_BUS_MONITOR_S)
#define TEE_WRITE_TEE_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_WRITE_TEE_CPU_BUS_MONITOR_S  4
/** TEE_WRITE_REE0_CPU_BUS_MONITOR : R/W; bitpos: [5]; default: 0;
 *  Configures cpu_bus_monitor registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_CPU_BUS_MONITOR    (BIT(5))
#define TEE_WRITE_REE0_CPU_BUS_MONITOR_M  (TEE_WRITE_REE0_CPU_BUS_MONITOR_V << TEE_WRITE_REE0_CPU_BUS_MONITOR_S)
#define TEE_WRITE_REE0_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_WRITE_REE0_CPU_BUS_MONITOR_S  5
/** TEE_WRITE_REE1_CPU_BUS_MONITOR : R/W; bitpos: [6]; default: 0;
 *  Configures cpu_bus_monitor registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_CPU_BUS_MONITOR    (BIT(6))
#define TEE_WRITE_REE1_CPU_BUS_MONITOR_M  (TEE_WRITE_REE1_CPU_BUS_MONITOR_V << TEE_WRITE_REE1_CPU_BUS_MONITOR_S)
#define TEE_WRITE_REE1_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_WRITE_REE1_CPU_BUS_MONITOR_S  6
/** TEE_WRITE_REE2_CPU_BUS_MONITOR : R/W; bitpos: [7]; default: 0;
 *  Configures cpu_bus_monitor registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_CPU_BUS_MONITOR    (BIT(7))
#define TEE_WRITE_REE2_CPU_BUS_MONITOR_M  (TEE_WRITE_REE2_CPU_BUS_MONITOR_V << TEE_WRITE_REE2_CPU_BUS_MONITOR_S)
#define TEE_WRITE_REE2_CPU_BUS_MONITOR_V  0x00000001U
#define TEE_WRITE_REE2_CPU_BUS_MONITOR_S  7

/** TEE_INTPRI_REG_CTRL_REG register
 *  intpri_reg read/write control register
 */
#define TEE_INTPRI_REG_CTRL_REG (DR_REG_TEE_BASE + 0x134)
/** TEE_READ_TEE_INTPRI_REG : R/W; bitpos: [0]; default: 1;
 *  Configures intpri_reg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_INTPRI_REG    (BIT(0))
#define TEE_READ_TEE_INTPRI_REG_M  (TEE_READ_TEE_INTPRI_REG_V << TEE_READ_TEE_INTPRI_REG_S)
#define TEE_READ_TEE_INTPRI_REG_V  0x00000001U
#define TEE_READ_TEE_INTPRI_REG_S  0
/** TEE_READ_REE0_INTPRI_REG : R/W; bitpos: [1]; default: 0;
 *  Configures intpri_reg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_INTPRI_REG    (BIT(1))
#define TEE_READ_REE0_INTPRI_REG_M  (TEE_READ_REE0_INTPRI_REG_V << TEE_READ_REE0_INTPRI_REG_S)
#define TEE_READ_REE0_INTPRI_REG_V  0x00000001U
#define TEE_READ_REE0_INTPRI_REG_S  1
/** TEE_READ_REE1_INTPRI_REG : R/W; bitpos: [2]; default: 0;
 *  Configures intpri_reg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_INTPRI_REG    (BIT(2))
#define TEE_READ_REE1_INTPRI_REG_M  (TEE_READ_REE1_INTPRI_REG_V << TEE_READ_REE1_INTPRI_REG_S)
#define TEE_READ_REE1_INTPRI_REG_V  0x00000001U
#define TEE_READ_REE1_INTPRI_REG_S  2
/** TEE_READ_REE2_INTPRI_REG : R/W; bitpos: [3]; default: 0;
 *  Configures intpri_reg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_INTPRI_REG    (BIT(3))
#define TEE_READ_REE2_INTPRI_REG_M  (TEE_READ_REE2_INTPRI_REG_V << TEE_READ_REE2_INTPRI_REG_S)
#define TEE_READ_REE2_INTPRI_REG_V  0x00000001U
#define TEE_READ_REE2_INTPRI_REG_S  3
/** TEE_WRITE_TEE_INTPRI_REG : R/W; bitpos: [4]; default: 1;
 *  Configures intpri_reg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_INTPRI_REG    (BIT(4))
#define TEE_WRITE_TEE_INTPRI_REG_M  (TEE_WRITE_TEE_INTPRI_REG_V << TEE_WRITE_TEE_INTPRI_REG_S)
#define TEE_WRITE_TEE_INTPRI_REG_V  0x00000001U
#define TEE_WRITE_TEE_INTPRI_REG_S  4
/** TEE_WRITE_REE0_INTPRI_REG : R/W; bitpos: [5]; default: 0;
 *  Configures intpri_reg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_INTPRI_REG    (BIT(5))
#define TEE_WRITE_REE0_INTPRI_REG_M  (TEE_WRITE_REE0_INTPRI_REG_V << TEE_WRITE_REE0_INTPRI_REG_S)
#define TEE_WRITE_REE0_INTPRI_REG_V  0x00000001U
#define TEE_WRITE_REE0_INTPRI_REG_S  5
/** TEE_WRITE_REE1_INTPRI_REG : R/W; bitpos: [6]; default: 0;
 *  Configures intpri_reg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_INTPRI_REG    (BIT(6))
#define TEE_WRITE_REE1_INTPRI_REG_M  (TEE_WRITE_REE1_INTPRI_REG_V << TEE_WRITE_REE1_INTPRI_REG_S)
#define TEE_WRITE_REE1_INTPRI_REG_V  0x00000001U
#define TEE_WRITE_REE1_INTPRI_REG_S  6
/** TEE_WRITE_REE2_INTPRI_REG : R/W; bitpos: [7]; default: 0;
 *  Configures intpri_reg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_INTPRI_REG    (BIT(7))
#define TEE_WRITE_REE2_INTPRI_REG_M  (TEE_WRITE_REE2_INTPRI_REG_V << TEE_WRITE_REE2_INTPRI_REG_S)
#define TEE_WRITE_REE2_INTPRI_REG_V  0x00000001U
#define TEE_WRITE_REE2_INTPRI_REG_S  7

/** TEE_CACHE_CFG_CTRL_REG register
 *  cache_cfg read/write control register
 */
#define TEE_CACHE_CFG_CTRL_REG (DR_REG_TEE_BASE + 0x138)
/** TEE_READ_TEE_CACHE_CFG : R/W; bitpos: [0]; default: 1;
 *  Configures cache_cfg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_CACHE_CFG    (BIT(0))
#define TEE_READ_TEE_CACHE_CFG_M  (TEE_READ_TEE_CACHE_CFG_V << TEE_READ_TEE_CACHE_CFG_S)
#define TEE_READ_TEE_CACHE_CFG_V  0x00000001U
#define TEE_READ_TEE_CACHE_CFG_S  0
/** TEE_READ_REE0_CACHE_CFG : R/W; bitpos: [1]; default: 0;
 *  Configures cache_cfg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_CACHE_CFG    (BIT(1))
#define TEE_READ_REE0_CACHE_CFG_M  (TEE_READ_REE0_CACHE_CFG_V << TEE_READ_REE0_CACHE_CFG_S)
#define TEE_READ_REE0_CACHE_CFG_V  0x00000001U
#define TEE_READ_REE0_CACHE_CFG_S  1
/** TEE_READ_REE1_CACHE_CFG : R/W; bitpos: [2]; default: 0;
 *  Configures cache_cfg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_CACHE_CFG    (BIT(2))
#define TEE_READ_REE1_CACHE_CFG_M  (TEE_READ_REE1_CACHE_CFG_V << TEE_READ_REE1_CACHE_CFG_S)
#define TEE_READ_REE1_CACHE_CFG_V  0x00000001U
#define TEE_READ_REE1_CACHE_CFG_S  2
/** TEE_READ_REE2_CACHE_CFG : R/W; bitpos: [3]; default: 0;
 *  Configures cache_cfg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_CACHE_CFG    (BIT(3))
#define TEE_READ_REE2_CACHE_CFG_M  (TEE_READ_REE2_CACHE_CFG_V << TEE_READ_REE2_CACHE_CFG_S)
#define TEE_READ_REE2_CACHE_CFG_V  0x00000001U
#define TEE_READ_REE2_CACHE_CFG_S  3
/** TEE_WRITE_TEE_CACHE_CFG : R/W; bitpos: [4]; default: 1;
 *  Configures cache_cfg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_CACHE_CFG    (BIT(4))
#define TEE_WRITE_TEE_CACHE_CFG_M  (TEE_WRITE_TEE_CACHE_CFG_V << TEE_WRITE_TEE_CACHE_CFG_S)
#define TEE_WRITE_TEE_CACHE_CFG_V  0x00000001U
#define TEE_WRITE_TEE_CACHE_CFG_S  4
/** TEE_WRITE_REE0_CACHE_CFG : R/W; bitpos: [5]; default: 0;
 *  Configures cache_cfg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_CACHE_CFG    (BIT(5))
#define TEE_WRITE_REE0_CACHE_CFG_M  (TEE_WRITE_REE0_CACHE_CFG_V << TEE_WRITE_REE0_CACHE_CFG_S)
#define TEE_WRITE_REE0_CACHE_CFG_V  0x00000001U
#define TEE_WRITE_REE0_CACHE_CFG_S  5
/** TEE_WRITE_REE1_CACHE_CFG : R/W; bitpos: [6]; default: 0;
 *  Configures cache_cfg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_CACHE_CFG    (BIT(6))
#define TEE_WRITE_REE1_CACHE_CFG_M  (TEE_WRITE_REE1_CACHE_CFG_V << TEE_WRITE_REE1_CACHE_CFG_S)
#define TEE_WRITE_REE1_CACHE_CFG_V  0x00000001U
#define TEE_WRITE_REE1_CACHE_CFG_S  6
/** TEE_WRITE_REE2_CACHE_CFG : R/W; bitpos: [7]; default: 0;
 *  Configures cache_cfg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_CACHE_CFG    (BIT(7))
#define TEE_WRITE_REE2_CACHE_CFG_M  (TEE_WRITE_REE2_CACHE_CFG_V << TEE_WRITE_REE2_CACHE_CFG_S)
#define TEE_WRITE_REE2_CACHE_CFG_V  0x00000001U
#define TEE_WRITE_REE2_CACHE_CFG_S  7

/** TEE_MODEM_CTRL_REG register
 *  modem read/write control register
 */
#define TEE_MODEM_CTRL_REG (DR_REG_TEE_BASE + 0x13c)
/** TEE_READ_TEE_MODEM : R/W; bitpos: [0]; default: 1;
 *  Configures modem registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_MODEM    (BIT(0))
#define TEE_READ_TEE_MODEM_M  (TEE_READ_TEE_MODEM_V << TEE_READ_TEE_MODEM_S)
#define TEE_READ_TEE_MODEM_V  0x00000001U
#define TEE_READ_TEE_MODEM_S  0
/** TEE_READ_REE0_MODEM : R/W; bitpos: [1]; default: 0;
 *  Configures modem registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_MODEM    (BIT(1))
#define TEE_READ_REE0_MODEM_M  (TEE_READ_REE0_MODEM_V << TEE_READ_REE0_MODEM_S)
#define TEE_READ_REE0_MODEM_V  0x00000001U
#define TEE_READ_REE0_MODEM_S  1
/** TEE_READ_REE1_MODEM : R/W; bitpos: [2]; default: 0;
 *  Configures modem registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_MODEM    (BIT(2))
#define TEE_READ_REE1_MODEM_M  (TEE_READ_REE1_MODEM_V << TEE_READ_REE1_MODEM_S)
#define TEE_READ_REE1_MODEM_V  0x00000001U
#define TEE_READ_REE1_MODEM_S  2
/** TEE_READ_REE2_MODEM : R/W; bitpos: [3]; default: 0;
 *  Configures modem registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_MODEM    (BIT(3))
#define TEE_READ_REE2_MODEM_M  (TEE_READ_REE2_MODEM_V << TEE_READ_REE2_MODEM_S)
#define TEE_READ_REE2_MODEM_V  0x00000001U
#define TEE_READ_REE2_MODEM_S  3
/** TEE_WRITE_TEE_MODEM : R/W; bitpos: [4]; default: 1;
 *  Configures modem registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_MODEM    (BIT(4))
#define TEE_WRITE_TEE_MODEM_M  (TEE_WRITE_TEE_MODEM_V << TEE_WRITE_TEE_MODEM_S)
#define TEE_WRITE_TEE_MODEM_V  0x00000001U
#define TEE_WRITE_TEE_MODEM_S  4
/** TEE_WRITE_REE0_MODEM : R/W; bitpos: [5]; default: 0;
 *  Configures modem registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_MODEM    (BIT(5))
#define TEE_WRITE_REE0_MODEM_M  (TEE_WRITE_REE0_MODEM_V << TEE_WRITE_REE0_MODEM_S)
#define TEE_WRITE_REE0_MODEM_V  0x00000001U
#define TEE_WRITE_REE0_MODEM_S  5
/** TEE_WRITE_REE1_MODEM : R/W; bitpos: [6]; default: 0;
 *  Configures modem registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_MODEM    (BIT(6))
#define TEE_WRITE_REE1_MODEM_M  (TEE_WRITE_REE1_MODEM_V << TEE_WRITE_REE1_MODEM_S)
#define TEE_WRITE_REE1_MODEM_V  0x00000001U
#define TEE_WRITE_REE1_MODEM_S  6
/** TEE_WRITE_REE2_MODEM : R/W; bitpos: [7]; default: 0;
 *  Configures modem registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_MODEM    (BIT(7))
#define TEE_WRITE_REE2_MODEM_M  (TEE_WRITE_REE2_MODEM_V << TEE_WRITE_REE2_MODEM_S)
#define TEE_WRITE_REE2_MODEM_V  0x00000001U
#define TEE_WRITE_REE2_MODEM_S  7

/** TEE_ZERO_DET_CTRL_REG register
 *  zero_det read/write control register
 */
#define TEE_ZERO_DET_CTRL_REG (DR_REG_TEE_BASE + 0x140)
/** TEE_READ_TEE_ZERO_DET : R/W; bitpos: [0]; default: 1;
 *  Configures zero_det registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_ZERO_DET    (BIT(0))
#define TEE_READ_TEE_ZERO_DET_M  (TEE_READ_TEE_ZERO_DET_V << TEE_READ_TEE_ZERO_DET_S)
#define TEE_READ_TEE_ZERO_DET_V  0x00000001U
#define TEE_READ_TEE_ZERO_DET_S  0
/** TEE_READ_REE0_ZERO_DET : R/W; bitpos: [1]; default: 0;
 *  Configures zero_det registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_ZERO_DET    (BIT(1))
#define TEE_READ_REE0_ZERO_DET_M  (TEE_READ_REE0_ZERO_DET_V << TEE_READ_REE0_ZERO_DET_S)
#define TEE_READ_REE0_ZERO_DET_V  0x00000001U
#define TEE_READ_REE0_ZERO_DET_S  1
/** TEE_READ_REE1_ZERO_DET : R/W; bitpos: [2]; default: 0;
 *  Configures zero_det registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_ZERO_DET    (BIT(2))
#define TEE_READ_REE1_ZERO_DET_M  (TEE_READ_REE1_ZERO_DET_V << TEE_READ_REE1_ZERO_DET_S)
#define TEE_READ_REE1_ZERO_DET_V  0x00000001U
#define TEE_READ_REE1_ZERO_DET_S  2
/** TEE_READ_REE2_ZERO_DET : R/W; bitpos: [3]; default: 0;
 *  Configures zero_det registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_ZERO_DET    (BIT(3))
#define TEE_READ_REE2_ZERO_DET_M  (TEE_READ_REE2_ZERO_DET_V << TEE_READ_REE2_ZERO_DET_S)
#define TEE_READ_REE2_ZERO_DET_V  0x00000001U
#define TEE_READ_REE2_ZERO_DET_S  3
/** TEE_WRITE_TEE_ZERO_DET : R/W; bitpos: [4]; default: 1;
 *  Configures zero_det registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_ZERO_DET    (BIT(4))
#define TEE_WRITE_TEE_ZERO_DET_M  (TEE_WRITE_TEE_ZERO_DET_V << TEE_WRITE_TEE_ZERO_DET_S)
#define TEE_WRITE_TEE_ZERO_DET_V  0x00000001U
#define TEE_WRITE_TEE_ZERO_DET_S  4
/** TEE_WRITE_REE0_ZERO_DET : R/W; bitpos: [5]; default: 0;
 *  Configures zero_det registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_ZERO_DET    (BIT(5))
#define TEE_WRITE_REE0_ZERO_DET_M  (TEE_WRITE_REE0_ZERO_DET_V << TEE_WRITE_REE0_ZERO_DET_S)
#define TEE_WRITE_REE0_ZERO_DET_V  0x00000001U
#define TEE_WRITE_REE0_ZERO_DET_S  5
/** TEE_WRITE_REE1_ZERO_DET : R/W; bitpos: [6]; default: 0;
 *  Configures zero_det registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_ZERO_DET    (BIT(6))
#define TEE_WRITE_REE1_ZERO_DET_M  (TEE_WRITE_REE1_ZERO_DET_V << TEE_WRITE_REE1_ZERO_DET_S)
#define TEE_WRITE_REE1_ZERO_DET_V  0x00000001U
#define TEE_WRITE_REE1_ZERO_DET_S  6
/** TEE_WRITE_REE2_ZERO_DET : R/W; bitpos: [7]; default: 0;
 *  Configures zero_det registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_ZERO_DET    (BIT(7))
#define TEE_WRITE_REE2_ZERO_DET_M  (TEE_WRITE_REE2_ZERO_DET_V << TEE_WRITE_REE2_ZERO_DET_S)
#define TEE_WRITE_REE2_ZERO_DET_V  0x00000001U
#define TEE_WRITE_REE2_ZERO_DET_S  7

/** TEE_BUS_ERR_CONF_REG register
 *  Clock gating register
 */
#define TEE_BUS_ERR_CONF_REG (DR_REG_TEE_BASE + 0xff0)
/** TEE_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether return error response to cpu when access blocked
 *  0: disable error response
 *  1: enable error response
 */
#define TEE_BUS_ERR_RESP_EN    (BIT(0))
#define TEE_BUS_ERR_RESP_EN_M  (TEE_BUS_ERR_RESP_EN_V << TEE_BUS_ERR_RESP_EN_S)
#define TEE_BUS_ERR_RESP_EN_V  0x00000001U
#define TEE_BUS_ERR_RESP_EN_S  0

/** TEE_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define TEE_CLOCK_GATE_REG (DR_REG_TEE_BASE + 0xff8)
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
#define TEE_DATE_REG (DR_REG_TEE_BASE + 0xffc)
/** TEE_DATE : R/W; bitpos: [27:0]; default: 38810240;
 *  Version control register
 */
#define TEE_DATE    0x0FFFFFFFU
#define TEE_DATE_M  (TEE_DATE_V << TEE_DATE_S)
#define TEE_DATE_V  0x0FFFFFFFU
#define TEE_DATE_S  0

#ifdef __cplusplus
}
#endif
