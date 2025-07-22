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

/** TEE_M0_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M0_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x0)
/** TEE_M0_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 0.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M0_MODE    0x00000003U
#define TEE_M0_MODE_M  (TEE_M0_MODE_V << TEE_M0_MODE_S)
#define TEE_M0_MODE_V  0x00000003U
#define TEE_M0_MODE_S  0
/** TEE_M0_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M0_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M0_LOCK    (BIT(2))
#define TEE_M0_LOCK_M  (TEE_M0_LOCK_V << TEE_M0_LOCK_S)
#define TEE_M0_LOCK_V  0x00000001U
#define TEE_M0_LOCK_S  2

/** TEE_M1_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M1_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x4)
/** TEE_M1_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 1.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M1_MODE    0x00000003U
#define TEE_M1_MODE_M  (TEE_M1_MODE_V << TEE_M1_MODE_S)
#define TEE_M1_MODE_V  0x00000003U
#define TEE_M1_MODE_S  0
/** TEE_M1_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M1_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M1_LOCK    (BIT(2))
#define TEE_M1_LOCK_M  (TEE_M1_LOCK_V << TEE_M1_LOCK_S)
#define TEE_M1_LOCK_V  0x00000001U
#define TEE_M1_LOCK_S  2

/** TEE_M2_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M2_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x8)
/** TEE_M2_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 2.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M2_MODE    0x00000003U
#define TEE_M2_MODE_M  (TEE_M2_MODE_V << TEE_M2_MODE_S)
#define TEE_M2_MODE_V  0x00000003U
#define TEE_M2_MODE_S  0
/** TEE_M2_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M2_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M2_LOCK    (BIT(2))
#define TEE_M2_LOCK_M  (TEE_M2_LOCK_V << TEE_M2_LOCK_S)
#define TEE_M2_LOCK_V  0x00000001U
#define TEE_M2_LOCK_S  2

/** TEE_M3_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M3_MODE_CTRL_REG (DR_REG_TEE_BASE + 0xc)
/** TEE_M3_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 3.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M3_MODE    0x00000003U
#define TEE_M3_MODE_M  (TEE_M3_MODE_V << TEE_M3_MODE_S)
#define TEE_M3_MODE_V  0x00000003U
#define TEE_M3_MODE_S  0
/** TEE_M3_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M3_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M3_LOCK    (BIT(2))
#define TEE_M3_LOCK_M  (TEE_M3_LOCK_V << TEE_M3_LOCK_S)
#define TEE_M3_LOCK_V  0x00000001U
#define TEE_M3_LOCK_S  2

/** TEE_M4_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M4_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x10)
/** TEE_M4_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 4.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M4_MODE    0x00000003U
#define TEE_M4_MODE_M  (TEE_M4_MODE_V << TEE_M4_MODE_S)
#define TEE_M4_MODE_V  0x00000003U
#define TEE_M4_MODE_S  0
/** TEE_M4_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M4_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M4_LOCK    (BIT(2))
#define TEE_M4_LOCK_M  (TEE_M4_LOCK_V << TEE_M4_LOCK_S)
#define TEE_M4_LOCK_V  0x00000001U
#define TEE_M4_LOCK_S  2

/** TEE_M5_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M5_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x14)
/** TEE_M5_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 5.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M5_MODE    0x00000003U
#define TEE_M5_MODE_M  (TEE_M5_MODE_V << TEE_M5_MODE_S)
#define TEE_M5_MODE_V  0x00000003U
#define TEE_M5_MODE_S  0
/** TEE_M5_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M5_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M5_LOCK    (BIT(2))
#define TEE_M5_LOCK_M  (TEE_M5_LOCK_V << TEE_M5_LOCK_S)
#define TEE_M5_LOCK_V  0x00000001U
#define TEE_M5_LOCK_S  2

/** TEE_M6_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M6_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x18)
/** TEE_M6_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 6.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M6_MODE    0x00000003U
#define TEE_M6_MODE_M  (TEE_M6_MODE_V << TEE_M6_MODE_S)
#define TEE_M6_MODE_V  0x00000003U
#define TEE_M6_MODE_S  0
/** TEE_M6_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M6_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M6_LOCK    (BIT(2))
#define TEE_M6_LOCK_M  (TEE_M6_LOCK_V << TEE_M6_LOCK_S)
#define TEE_M6_LOCK_V  0x00000001U
#define TEE_M6_LOCK_S  2

/** TEE_M7_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M7_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x1c)
/** TEE_M7_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 7.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M7_MODE    0x00000003U
#define TEE_M7_MODE_M  (TEE_M7_MODE_V << TEE_M7_MODE_S)
#define TEE_M7_MODE_V  0x00000003U
#define TEE_M7_MODE_S  0
/** TEE_M7_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M7_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M7_LOCK    (BIT(2))
#define TEE_M7_LOCK_M  (TEE_M7_LOCK_V << TEE_M7_LOCK_S)
#define TEE_M7_LOCK_V  0x00000001U
#define TEE_M7_LOCK_S  2

/** TEE_M8_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M8_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x20)
/** TEE_M8_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 8.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M8_MODE    0x00000003U
#define TEE_M8_MODE_M  (TEE_M8_MODE_V << TEE_M8_MODE_S)
#define TEE_M8_MODE_V  0x00000003U
#define TEE_M8_MODE_S  0
/** TEE_M8_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M8_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M8_LOCK    (BIT(2))
#define TEE_M8_LOCK_M  (TEE_M8_LOCK_V << TEE_M8_LOCK_S)
#define TEE_M8_LOCK_V  0x00000001U
#define TEE_M8_LOCK_S  2

/** TEE_M9_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M9_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x24)
/** TEE_M9_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 9.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M9_MODE    0x00000003U
#define TEE_M9_MODE_M  (TEE_M9_MODE_V << TEE_M9_MODE_S)
#define TEE_M9_MODE_V  0x00000003U
#define TEE_M9_MODE_S  0
/** TEE_M9_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M9_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M9_LOCK    (BIT(2))
#define TEE_M9_LOCK_M  (TEE_M9_LOCK_V << TEE_M9_LOCK_S)
#define TEE_M9_LOCK_V  0x00000001U
#define TEE_M9_LOCK_S  2

/** TEE_M10_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M10_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x28)
/** TEE_M10_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 10.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M10_MODE    0x00000003U
#define TEE_M10_MODE_M  (TEE_M10_MODE_V << TEE_M10_MODE_S)
#define TEE_M10_MODE_V  0x00000003U
#define TEE_M10_MODE_S  0
/** TEE_M10_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M10_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M10_LOCK    (BIT(2))
#define TEE_M10_LOCK_M  (TEE_M10_LOCK_V << TEE_M10_LOCK_S)
#define TEE_M10_LOCK_V  0x00000001U
#define TEE_M10_LOCK_S  2

/** TEE_M11_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M11_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x2c)
/** TEE_M11_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 11.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M11_MODE    0x00000003U
#define TEE_M11_MODE_M  (TEE_M11_MODE_V << TEE_M11_MODE_S)
#define TEE_M11_MODE_V  0x00000003U
#define TEE_M11_MODE_S  0
/** TEE_M11_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M11_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M11_LOCK    (BIT(2))
#define TEE_M11_LOCK_M  (TEE_M11_LOCK_V << TEE_M11_LOCK_S)
#define TEE_M11_LOCK_V  0x00000001U
#define TEE_M11_LOCK_S  2

/** TEE_M12_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M12_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x30)
/** TEE_M12_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 12.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M12_MODE    0x00000003U
#define TEE_M12_MODE_M  (TEE_M12_MODE_V << TEE_M12_MODE_S)
#define TEE_M12_MODE_V  0x00000003U
#define TEE_M12_MODE_S  0
/** TEE_M12_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M12_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M12_LOCK    (BIT(2))
#define TEE_M12_LOCK_M  (TEE_M12_LOCK_V << TEE_M12_LOCK_S)
#define TEE_M12_LOCK_V  0x00000001U
#define TEE_M12_LOCK_S  2

/** TEE_M13_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M13_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x34)
/** TEE_M13_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 13.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M13_MODE    0x00000003U
#define TEE_M13_MODE_M  (TEE_M13_MODE_V << TEE_M13_MODE_S)
#define TEE_M13_MODE_V  0x00000003U
#define TEE_M13_MODE_S  0
/** TEE_M13_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M13_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M13_LOCK    (BIT(2))
#define TEE_M13_LOCK_M  (TEE_M13_LOCK_V << TEE_M13_LOCK_S)
#define TEE_M13_LOCK_V  0x00000001U
#define TEE_M13_LOCK_S  2

/** TEE_M14_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M14_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x38)
/** TEE_M14_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 14.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M14_MODE    0x00000003U
#define TEE_M14_MODE_M  (TEE_M14_MODE_V << TEE_M14_MODE_S)
#define TEE_M14_MODE_V  0x00000003U
#define TEE_M14_MODE_S  0
/** TEE_M14_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M14_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M14_LOCK    (BIT(2))
#define TEE_M14_LOCK_M  (TEE_M14_LOCK_V << TEE_M14_LOCK_S)
#define TEE_M14_LOCK_V  0x00000001U
#define TEE_M14_LOCK_S  2

/** TEE_M15_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M15_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x3c)
/** TEE_M15_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 15.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M15_MODE    0x00000003U
#define TEE_M15_MODE_M  (TEE_M15_MODE_V << TEE_M15_MODE_S)
#define TEE_M15_MODE_V  0x00000003U
#define TEE_M15_MODE_S  0
/** TEE_M15_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M15_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M15_LOCK    (BIT(2))
#define TEE_M15_LOCK_M  (TEE_M15_LOCK_V << TEE_M15_LOCK_S)
#define TEE_M15_LOCK_V  0x00000001U
#define TEE_M15_LOCK_S  2

/** TEE_M16_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M16_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x40)
/** TEE_M16_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 16.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M16_MODE    0x00000003U
#define TEE_M16_MODE_M  (TEE_M16_MODE_V << TEE_M16_MODE_S)
#define TEE_M16_MODE_V  0x00000003U
#define TEE_M16_MODE_S  0
/** TEE_M16_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M16_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M16_LOCK    (BIT(2))
#define TEE_M16_LOCK_M  (TEE_M16_LOCK_V << TEE_M16_LOCK_S)
#define TEE_M16_LOCK_V  0x00000001U
#define TEE_M16_LOCK_S  2

/** TEE_M17_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M17_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x44)
/** TEE_M17_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 17.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M17_MODE    0x00000003U
#define TEE_M17_MODE_M  (TEE_M17_MODE_V << TEE_M17_MODE_S)
#define TEE_M17_MODE_V  0x00000003U
#define TEE_M17_MODE_S  0
/** TEE_M17_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M17_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M17_LOCK    (BIT(2))
#define TEE_M17_LOCK_M  (TEE_M17_LOCK_V << TEE_M17_LOCK_S)
#define TEE_M17_LOCK_V  0x00000001U
#define TEE_M17_LOCK_S  2

/** TEE_M18_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M18_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x48)
/** TEE_M18_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 18.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M18_MODE    0x00000003U
#define TEE_M18_MODE_M  (TEE_M18_MODE_V << TEE_M18_MODE_S)
#define TEE_M18_MODE_V  0x00000003U
#define TEE_M18_MODE_S  0
/** TEE_M18_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M18_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M18_LOCK    (BIT(2))
#define TEE_M18_LOCK_M  (TEE_M18_LOCK_V << TEE_M18_LOCK_S)
#define TEE_M18_LOCK_V  0x00000001U
#define TEE_M18_LOCK_S  2

/** TEE_M19_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M19_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x4c)
/** TEE_M19_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 19.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M19_MODE    0x00000003U
#define TEE_M19_MODE_M  (TEE_M19_MODE_V << TEE_M19_MODE_S)
#define TEE_M19_MODE_V  0x00000003U
#define TEE_M19_MODE_S  0
/** TEE_M19_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M19_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M19_LOCK    (BIT(2))
#define TEE_M19_LOCK_M  (TEE_M19_LOCK_V << TEE_M19_LOCK_S)
#define TEE_M19_LOCK_V  0x00000001U
#define TEE_M19_LOCK_S  2

/** TEE_M20_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M20_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x50)
/** TEE_M20_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 20.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M20_MODE    0x00000003U
#define TEE_M20_MODE_M  (TEE_M20_MODE_V << TEE_M20_MODE_S)
#define TEE_M20_MODE_V  0x00000003U
#define TEE_M20_MODE_S  0
/** TEE_M20_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M20_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M20_LOCK    (BIT(2))
#define TEE_M20_LOCK_M  (TEE_M20_LOCK_V << TEE_M20_LOCK_S)
#define TEE_M20_LOCK_V  0x00000001U
#define TEE_M20_LOCK_S  2

/** TEE_M21_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M21_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x54)
/** TEE_M21_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 21.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M21_MODE    0x00000003U
#define TEE_M21_MODE_M  (TEE_M21_MODE_V << TEE_M21_MODE_S)
#define TEE_M21_MODE_V  0x00000003U
#define TEE_M21_MODE_S  0
/** TEE_M21_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M21_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M21_LOCK    (BIT(2))
#define TEE_M21_LOCK_M  (TEE_M21_LOCK_V << TEE_M21_LOCK_S)
#define TEE_M21_LOCK_V  0x00000001U
#define TEE_M21_LOCK_S  2

/** TEE_M22_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M22_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x58)
/** TEE_M22_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 22.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M22_MODE    0x00000003U
#define TEE_M22_MODE_M  (TEE_M22_MODE_V << TEE_M22_MODE_S)
#define TEE_M22_MODE_V  0x00000003U
#define TEE_M22_MODE_S  0
/** TEE_M22_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M22_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M22_LOCK    (BIT(2))
#define TEE_M22_LOCK_M  (TEE_M22_LOCK_V << TEE_M22_LOCK_S)
#define TEE_M22_LOCK_V  0x00000001U
#define TEE_M22_LOCK_S  2

/** TEE_M23_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M23_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x5c)
/** TEE_M23_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 23.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M23_MODE    0x00000003U
#define TEE_M23_MODE_M  (TEE_M23_MODE_V << TEE_M23_MODE_S)
#define TEE_M23_MODE_V  0x00000003U
#define TEE_M23_MODE_S  0
/** TEE_M23_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M23_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M23_LOCK    (BIT(2))
#define TEE_M23_LOCK_M  (TEE_M23_LOCK_V << TEE_M23_LOCK_S)
#define TEE_M23_LOCK_V  0x00000001U
#define TEE_M23_LOCK_S  2

/** TEE_M24_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M24_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x60)
/** TEE_M24_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 24.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M24_MODE    0x00000003U
#define TEE_M24_MODE_M  (TEE_M24_MODE_V << TEE_M24_MODE_S)
#define TEE_M24_MODE_V  0x00000003U
#define TEE_M24_MODE_S  0
/** TEE_M24_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M24_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M24_LOCK    (BIT(2))
#define TEE_M24_LOCK_M  (TEE_M24_LOCK_V << TEE_M24_LOCK_S)
#define TEE_M24_LOCK_V  0x00000001U
#define TEE_M24_LOCK_S  2

/** TEE_M25_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M25_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x64)
/** TEE_M25_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 25.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M25_MODE    0x00000003U
#define TEE_M25_MODE_M  (TEE_M25_MODE_V << TEE_M25_MODE_S)
#define TEE_M25_MODE_V  0x00000003U
#define TEE_M25_MODE_S  0
/** TEE_M25_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M25_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M25_LOCK    (BIT(2))
#define TEE_M25_LOCK_M  (TEE_M25_LOCK_V << TEE_M25_LOCK_S)
#define TEE_M25_LOCK_V  0x00000001U
#define TEE_M25_LOCK_S  2

/** TEE_M26_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M26_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x68)
/** TEE_M26_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 26.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M26_MODE    0x00000003U
#define TEE_M26_MODE_M  (TEE_M26_MODE_V << TEE_M26_MODE_S)
#define TEE_M26_MODE_V  0x00000003U
#define TEE_M26_MODE_S  0
/** TEE_M26_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M26_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M26_LOCK    (BIT(2))
#define TEE_M26_LOCK_M  (TEE_M26_LOCK_V << TEE_M26_LOCK_S)
#define TEE_M26_LOCK_V  0x00000001U
#define TEE_M26_LOCK_S  2

/** TEE_M27_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M27_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x6c)
/** TEE_M27_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 27.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M27_MODE    0x00000003U
#define TEE_M27_MODE_M  (TEE_M27_MODE_V << TEE_M27_MODE_S)
#define TEE_M27_MODE_V  0x00000003U
#define TEE_M27_MODE_S  0
/** TEE_M27_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M27_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M27_LOCK    (BIT(2))
#define TEE_M27_LOCK_M  (TEE_M27_LOCK_V << TEE_M27_LOCK_S)
#define TEE_M27_LOCK_V  0x00000001U
#define TEE_M27_LOCK_S  2

/** TEE_M28_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M28_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x70)
/** TEE_M28_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 28.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M28_MODE    0x00000003U
#define TEE_M28_MODE_M  (TEE_M28_MODE_V << TEE_M28_MODE_S)
#define TEE_M28_MODE_V  0x00000003U
#define TEE_M28_MODE_S  0
/** TEE_M28_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M28_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M28_LOCK    (BIT(2))
#define TEE_M28_LOCK_M  (TEE_M28_LOCK_V << TEE_M28_LOCK_S)
#define TEE_M28_LOCK_V  0x00000001U
#define TEE_M28_LOCK_S  2

/** TEE_M29_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M29_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x74)
/** TEE_M29_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 29.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M29_MODE    0x00000003U
#define TEE_M29_MODE_M  (TEE_M29_MODE_V << TEE_M29_MODE_S)
#define TEE_M29_MODE_V  0x00000003U
#define TEE_M29_MODE_S  0
/** TEE_M29_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M29_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M29_LOCK    (BIT(2))
#define TEE_M29_LOCK_M  (TEE_M29_LOCK_V << TEE_M29_LOCK_S)
#define TEE_M29_LOCK_V  0x00000001U
#define TEE_M29_LOCK_S  2

/** TEE_M30_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M30_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x78)
/** TEE_M30_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 30.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M30_MODE    0x00000003U
#define TEE_M30_MODE_M  (TEE_M30_MODE_V << TEE_M30_MODE_S)
#define TEE_M30_MODE_V  0x00000003U
#define TEE_M30_MODE_S  0
/** TEE_M30_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M30_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M30_LOCK    (BIT(2))
#define TEE_M30_LOCK_M  (TEE_M30_LOCK_V << TEE_M30_LOCK_S)
#define TEE_M30_LOCK_V  0x00000001U
#define TEE_M30_LOCK_S  2

/** TEE_M31_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define TEE_M31_MODE_CTRL_REG (DR_REG_TEE_BASE + 0x7c)
/** TEE_M31_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the security mode for master 31.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define TEE_M31_MODE    0x00000003U
#define TEE_M31_MODE_M  (TEE_M31_MODE_V << TEE_M31_MODE_S)
#define TEE_M31_MODE_V  0x00000003U
#define TEE_M31_MODE_S  0
/** TEE_M31_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures whether to lock the value of TEE_M31_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define TEE_M31_LOCK    (BIT(2))
#define TEE_M31_LOCK_M  (TEE_M31_LOCK_V << TEE_M31_LOCK_S)
#define TEE_M31_LOCK_V  0x00000001U
#define TEE_M31_LOCK_S  2

/** TEE_UART0_CTRL_REG register
 *  UART0 read/write control register
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
 *  UART1 read/write control register
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

/** TEE_UHCI0_CTRL_REG register
 *  UHCI read/write control register
 */
#define TEE_UHCI0_CTRL_REG (DR_REG_TEE_BASE + 0x90)
/** TEE_READ_TEE_UHCI0 : R/W; bitpos: [0]; default: 1;
 *  Configures uhci0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_UHCI0    (BIT(0))
#define TEE_READ_TEE_UHCI0_M  (TEE_READ_TEE_UHCI0_V << TEE_READ_TEE_UHCI0_S)
#define TEE_READ_TEE_UHCI0_V  0x00000001U
#define TEE_READ_TEE_UHCI0_S  0
/** TEE_READ_REE0_UHCI0 : R/W; bitpos: [1]; default: 0;
 *  Configures uhci0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_UHCI0    (BIT(1))
#define TEE_READ_REE0_UHCI0_M  (TEE_READ_REE0_UHCI0_V << TEE_READ_REE0_UHCI0_S)
#define TEE_READ_REE0_UHCI0_V  0x00000001U
#define TEE_READ_REE0_UHCI0_S  1
/** TEE_READ_REE1_UHCI0 : R/W; bitpos: [2]; default: 0;
 *  Configures uhci0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_UHCI0    (BIT(2))
#define TEE_READ_REE1_UHCI0_M  (TEE_READ_REE1_UHCI0_V << TEE_READ_REE1_UHCI0_S)
#define TEE_READ_REE1_UHCI0_V  0x00000001U
#define TEE_READ_REE1_UHCI0_S  2
/** TEE_READ_REE2_UHCI0 : R/W; bitpos: [3]; default: 0;
 *  Configures uhci0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_UHCI0    (BIT(3))
#define TEE_READ_REE2_UHCI0_M  (TEE_READ_REE2_UHCI0_V << TEE_READ_REE2_UHCI0_S)
#define TEE_READ_REE2_UHCI0_V  0x00000001U
#define TEE_READ_REE2_UHCI0_S  3
/** TEE_WRITE_TEE_UHCI0 : R/W; bitpos: [4]; default: 1;
 *  Configures uhci0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_UHCI0    (BIT(4))
#define TEE_WRITE_TEE_UHCI0_M  (TEE_WRITE_TEE_UHCI0_V << TEE_WRITE_TEE_UHCI0_S)
#define TEE_WRITE_TEE_UHCI0_V  0x00000001U
#define TEE_WRITE_TEE_UHCI0_S  4
/** TEE_WRITE_REE0_UHCI0 : R/W; bitpos: [5]; default: 0;
 *  Configures uhci0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_UHCI0    (BIT(5))
#define TEE_WRITE_REE0_UHCI0_M  (TEE_WRITE_REE0_UHCI0_V << TEE_WRITE_REE0_UHCI0_S)
#define TEE_WRITE_REE0_UHCI0_V  0x00000001U
#define TEE_WRITE_REE0_UHCI0_S  5
/** TEE_WRITE_REE1_UHCI0 : R/W; bitpos: [6]; default: 0;
 *  Configures uhci0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_UHCI0    (BIT(6))
#define TEE_WRITE_REE1_UHCI0_M  (TEE_WRITE_REE1_UHCI0_V << TEE_WRITE_REE1_UHCI0_S)
#define TEE_WRITE_REE1_UHCI0_V  0x00000001U
#define TEE_WRITE_REE1_UHCI0_S  6
/** TEE_WRITE_REE2_UHCI0 : R/W; bitpos: [7]; default: 0;
 *  Configures uhci0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_UHCI0    (BIT(7))
#define TEE_WRITE_REE2_UHCI0_M  (TEE_WRITE_REE2_UHCI0_V << TEE_WRITE_REE2_UHCI0_S)
#define TEE_WRITE_REE2_UHCI0_V  0x00000001U
#define TEE_WRITE_REE2_UHCI0_S  7

/** TEE_I2C_EXT0_CTRL_REG register
 *  I2C read/write control register
 */
#define TEE_I2C_EXT0_CTRL_REG (DR_REG_TEE_BASE + 0x94)
/** TEE_READ_TEE_I2C_EXT0 : R/W; bitpos: [0]; default: 1;
 *  Configures i2c_ext0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_I2C_EXT0    (BIT(0))
#define TEE_READ_TEE_I2C_EXT0_M  (TEE_READ_TEE_I2C_EXT0_V << TEE_READ_TEE_I2C_EXT0_S)
#define TEE_READ_TEE_I2C_EXT0_V  0x00000001U
#define TEE_READ_TEE_I2C_EXT0_S  0
/** TEE_READ_REE0_I2C_EXT0 : R/W; bitpos: [1]; default: 0;
 *  Configures i2c_ext0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_I2C_EXT0    (BIT(1))
#define TEE_READ_REE0_I2C_EXT0_M  (TEE_READ_REE0_I2C_EXT0_V << TEE_READ_REE0_I2C_EXT0_S)
#define TEE_READ_REE0_I2C_EXT0_V  0x00000001U
#define TEE_READ_REE0_I2C_EXT0_S  1
/** TEE_READ_REE1_I2C_EXT0 : R/W; bitpos: [2]; default: 0;
 *  Configures i2c_ext0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_I2C_EXT0    (BIT(2))
#define TEE_READ_REE1_I2C_EXT0_M  (TEE_READ_REE1_I2C_EXT0_V << TEE_READ_REE1_I2C_EXT0_S)
#define TEE_READ_REE1_I2C_EXT0_V  0x00000001U
#define TEE_READ_REE1_I2C_EXT0_S  2
/** TEE_READ_REE2_I2C_EXT0 : R/W; bitpos: [3]; default: 0;
 *  Configures i2c_ext0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_I2C_EXT0    (BIT(3))
#define TEE_READ_REE2_I2C_EXT0_M  (TEE_READ_REE2_I2C_EXT0_V << TEE_READ_REE2_I2C_EXT0_S)
#define TEE_READ_REE2_I2C_EXT0_V  0x00000001U
#define TEE_READ_REE2_I2C_EXT0_S  3
/** TEE_WRITE_TEE_I2C_EXT0 : R/W; bitpos: [4]; default: 1;
 *  Configures i2c_ext0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_I2C_EXT0    (BIT(4))
#define TEE_WRITE_TEE_I2C_EXT0_M  (TEE_WRITE_TEE_I2C_EXT0_V << TEE_WRITE_TEE_I2C_EXT0_S)
#define TEE_WRITE_TEE_I2C_EXT0_V  0x00000001U
#define TEE_WRITE_TEE_I2C_EXT0_S  4
/** TEE_WRITE_REE0_I2C_EXT0 : R/W; bitpos: [5]; default: 0;
 *  Configures i2c_ext0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_I2C_EXT0    (BIT(5))
#define TEE_WRITE_REE0_I2C_EXT0_M  (TEE_WRITE_REE0_I2C_EXT0_V << TEE_WRITE_REE0_I2C_EXT0_S)
#define TEE_WRITE_REE0_I2C_EXT0_V  0x00000001U
#define TEE_WRITE_REE0_I2C_EXT0_S  5
/** TEE_WRITE_REE1_I2C_EXT0 : R/W; bitpos: [6]; default: 0;
 *  Configures i2c_ext0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_I2C_EXT0    (BIT(6))
#define TEE_WRITE_REE1_I2C_EXT0_M  (TEE_WRITE_REE1_I2C_EXT0_V << TEE_WRITE_REE1_I2C_EXT0_S)
#define TEE_WRITE_REE1_I2C_EXT0_V  0x00000001U
#define TEE_WRITE_REE1_I2C_EXT0_S  6
/** TEE_WRITE_REE2_I2C_EXT0 : R/W; bitpos: [7]; default: 0;
 *  Configures i2c_ext0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_I2C_EXT0    (BIT(7))
#define TEE_WRITE_REE2_I2C_EXT0_M  (TEE_WRITE_REE2_I2C_EXT0_V << TEE_WRITE_REE2_I2C_EXT0_S)
#define TEE_WRITE_REE2_I2C_EXT0_V  0x00000001U
#define TEE_WRITE_REE2_I2C_EXT0_S  7

/** TEE_I2S_CTRL_REG register
 *  I2S read/write control register
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
 *  PARL_IO read/write control register
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

/** TEE_PWM_CTRL_REG register
 *  MCPWM read/write control register
 */
#define TEE_PWM_CTRL_REG (DR_REG_TEE_BASE + 0xa4)
/** TEE_READ_TEE_PWM : R/W; bitpos: [0]; default: 1;
 *  Configures pwm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_PWM    (BIT(0))
#define TEE_READ_TEE_PWM_M  (TEE_READ_TEE_PWM_V << TEE_READ_TEE_PWM_S)
#define TEE_READ_TEE_PWM_V  0x00000001U
#define TEE_READ_TEE_PWM_S  0
/** TEE_READ_REE0_PWM : R/W; bitpos: [1]; default: 0;
 *  Configures pwm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_PWM    (BIT(1))
#define TEE_READ_REE0_PWM_M  (TEE_READ_REE0_PWM_V << TEE_READ_REE0_PWM_S)
#define TEE_READ_REE0_PWM_V  0x00000001U
#define TEE_READ_REE0_PWM_S  1
/** TEE_READ_REE1_PWM : R/W; bitpos: [2]; default: 0;
 *  Configures pwm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_PWM    (BIT(2))
#define TEE_READ_REE1_PWM_M  (TEE_READ_REE1_PWM_V << TEE_READ_REE1_PWM_S)
#define TEE_READ_REE1_PWM_V  0x00000001U
#define TEE_READ_REE1_PWM_S  2
/** TEE_READ_REE2_PWM : R/W; bitpos: [3]; default: 0;
 *  Configures pwm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_PWM    (BIT(3))
#define TEE_READ_REE2_PWM_M  (TEE_READ_REE2_PWM_V << TEE_READ_REE2_PWM_S)
#define TEE_READ_REE2_PWM_V  0x00000001U
#define TEE_READ_REE2_PWM_S  3
/** TEE_WRITE_TEE_PWM : R/W; bitpos: [4]; default: 1;
 *  Configures pwm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_PWM    (BIT(4))
#define TEE_WRITE_TEE_PWM_M  (TEE_WRITE_TEE_PWM_V << TEE_WRITE_TEE_PWM_S)
#define TEE_WRITE_TEE_PWM_V  0x00000001U
#define TEE_WRITE_TEE_PWM_S  4
/** TEE_WRITE_REE0_PWM : R/W; bitpos: [5]; default: 0;
 *  Configures pwm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_PWM    (BIT(5))
#define TEE_WRITE_REE0_PWM_M  (TEE_WRITE_REE0_PWM_V << TEE_WRITE_REE0_PWM_S)
#define TEE_WRITE_REE0_PWM_V  0x00000001U
#define TEE_WRITE_REE0_PWM_S  5
/** TEE_WRITE_REE1_PWM : R/W; bitpos: [6]; default: 0;
 *  Configures pwm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_PWM    (BIT(6))
#define TEE_WRITE_REE1_PWM_M  (TEE_WRITE_REE1_PWM_V << TEE_WRITE_REE1_PWM_S)
#define TEE_WRITE_REE1_PWM_V  0x00000001U
#define TEE_WRITE_REE1_PWM_S  6
/** TEE_WRITE_REE2_PWM : R/W; bitpos: [7]; default: 0;
 *  Configures pwm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_PWM    (BIT(7))
#define TEE_WRITE_REE2_PWM_M  (TEE_WRITE_REE2_PWM_V << TEE_WRITE_REE2_PWM_S)
#define TEE_WRITE_REE2_PWM_V  0x00000001U
#define TEE_WRITE_REE2_PWM_S  7

/** TEE_LEDC_CTRL_REG register
 *  LEDC read/write control register
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

/** TEE_TWAI0_CTRL_REG register
 *  TWAI0 read/write control register
 */
#define TEE_TWAI0_CTRL_REG (DR_REG_TEE_BASE + 0xb0)
/** TEE_READ_TEE_TWAI0 : R/W; bitpos: [0]; default: 1;
 *  Configures can0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_TWAI0    (BIT(0))
#define TEE_READ_TEE_TWAI0_M  (TEE_READ_TEE_TWAI0_V << TEE_READ_TEE_TWAI0_S)
#define TEE_READ_TEE_TWAI0_V  0x00000001U
#define TEE_READ_TEE_TWAI0_S  0
/** TEE_READ_REE0_TWAI0 : R/W; bitpos: [1]; default: 0;
 *  Configures can0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_TWAI0    (BIT(1))
#define TEE_READ_REE0_TWAI0_M  (TEE_READ_REE0_TWAI0_V << TEE_READ_REE0_TWAI0_S)
#define TEE_READ_REE0_TWAI0_V  0x00000001U
#define TEE_READ_REE0_TWAI0_S  1
/** TEE_READ_REE1_TWAI0 : R/W; bitpos: [2]; default: 0;
 *  Configures can0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_TWAI0    (BIT(2))
#define TEE_READ_REE1_TWAI0_M  (TEE_READ_REE1_TWAI0_V << TEE_READ_REE1_TWAI0_S)
#define TEE_READ_REE1_TWAI0_V  0x00000001U
#define TEE_READ_REE1_TWAI0_S  2
/** TEE_READ_REE2_TWAI0 : R/W; bitpos: [3]; default: 0;
 *  Configures can0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_TWAI0    (BIT(3))
#define TEE_READ_REE2_TWAI0_M  (TEE_READ_REE2_TWAI0_V << TEE_READ_REE2_TWAI0_S)
#define TEE_READ_REE2_TWAI0_V  0x00000001U
#define TEE_READ_REE2_TWAI0_S  3
/** TEE_WRITE_TEE_TWAI0 : R/W; bitpos: [4]; default: 1;
 *  Configures can0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_TWAI0    (BIT(4))
#define TEE_WRITE_TEE_TWAI0_M  (TEE_WRITE_TEE_TWAI0_V << TEE_WRITE_TEE_TWAI0_S)
#define TEE_WRITE_TEE_TWAI0_V  0x00000001U
#define TEE_WRITE_TEE_TWAI0_S  4
/** TEE_WRITE_REE0_TWAI0 : R/W; bitpos: [5]; default: 0;
 *  Configures can0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_TWAI0    (BIT(5))
#define TEE_WRITE_REE0_TWAI0_M  (TEE_WRITE_REE0_TWAI0_V << TEE_WRITE_REE0_TWAI0_S)
#define TEE_WRITE_REE0_TWAI0_V  0x00000001U
#define TEE_WRITE_REE0_TWAI0_S  5
/** TEE_WRITE_REE1_TWAI0 : R/W; bitpos: [6]; default: 0;
 *  Configures can0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_TWAI0    (BIT(6))
#define TEE_WRITE_REE1_TWAI0_M  (TEE_WRITE_REE1_TWAI0_V << TEE_WRITE_REE1_TWAI0_S)
#define TEE_WRITE_REE1_TWAI0_V  0x00000001U
#define TEE_WRITE_REE1_TWAI0_S  6
/** TEE_WRITE_REE2_TWAI0 : R/W; bitpos: [7]; default: 0;
 *  Configures can0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_TWAI0    (BIT(7))
#define TEE_WRITE_REE2_TWAI0_M  (TEE_WRITE_REE2_TWAI0_V << TEE_WRITE_REE2_TWAI0_S)
#define TEE_WRITE_REE2_TWAI0_V  0x00000001U
#define TEE_WRITE_REE2_TWAI0_S  7

/** TEE_USB_SERIAL_JTAG_CTRL_REG register
 *  USB_SERIAL_JTAG read/write control register
 */
#define TEE_USB_SERIAL_JTAG_CTRL_REG (DR_REG_TEE_BASE + 0xb4)
/** TEE_READ_TEE_USB_SERIAL_JTAG : R/W; bitpos: [0]; default: 1;
 *  Configures usb_device registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_USB_SERIAL_JTAG    (BIT(0))
#define TEE_READ_TEE_USB_SERIAL_JTAG_M  (TEE_READ_TEE_USB_SERIAL_JTAG_V << TEE_READ_TEE_USB_SERIAL_JTAG_S)
#define TEE_READ_TEE_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_TEE_USB_SERIAL_JTAG_S  0
/** TEE_READ_REE0_USB_SERIAL_JTAG : R/W; bitpos: [1]; default: 0;
 *  Configures usb_device registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_USB_SERIAL_JTAG    (BIT(1))
#define TEE_READ_REE0_USB_SERIAL_JTAG_M  (TEE_READ_REE0_USB_SERIAL_JTAG_V << TEE_READ_REE0_USB_SERIAL_JTAG_S)
#define TEE_READ_REE0_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_REE0_USB_SERIAL_JTAG_S  1
/** TEE_READ_REE1_USB_SERIAL_JTAG : R/W; bitpos: [2]; default: 0;
 *  Configures usb_device registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_USB_SERIAL_JTAG    (BIT(2))
#define TEE_READ_REE1_USB_SERIAL_JTAG_M  (TEE_READ_REE1_USB_SERIAL_JTAG_V << TEE_READ_REE1_USB_SERIAL_JTAG_S)
#define TEE_READ_REE1_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_REE1_USB_SERIAL_JTAG_S  2
/** TEE_READ_REE2_USB_SERIAL_JTAG : R/W; bitpos: [3]; default: 0;
 *  Configures usb_device registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_USB_SERIAL_JTAG    (BIT(3))
#define TEE_READ_REE2_USB_SERIAL_JTAG_M  (TEE_READ_REE2_USB_SERIAL_JTAG_V << TEE_READ_REE2_USB_SERIAL_JTAG_S)
#define TEE_READ_REE2_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_READ_REE2_USB_SERIAL_JTAG_S  3
/** TEE_WRITE_TEE_USB_SERIAL_JTAG : R/W; bitpos: [4]; default: 1;
 *  Configures usb_device registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_USB_SERIAL_JTAG    (BIT(4))
#define TEE_WRITE_TEE_USB_SERIAL_JTAG_M  (TEE_WRITE_TEE_USB_SERIAL_JTAG_V << TEE_WRITE_TEE_USB_SERIAL_JTAG_S)
#define TEE_WRITE_TEE_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_TEE_USB_SERIAL_JTAG_S  4
/** TEE_WRITE_REE0_USB_SERIAL_JTAG : R/W; bitpos: [5]; default: 0;
 *  Configures usb_device registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_USB_SERIAL_JTAG    (BIT(5))
#define TEE_WRITE_REE0_USB_SERIAL_JTAG_M  (TEE_WRITE_REE0_USB_SERIAL_JTAG_V << TEE_WRITE_REE0_USB_SERIAL_JTAG_S)
#define TEE_WRITE_REE0_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_REE0_USB_SERIAL_JTAG_S  5
/** TEE_WRITE_REE1_USB_SERIAL_JTAG : R/W; bitpos: [6]; default: 0;
 *  Configures usb_device registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_USB_SERIAL_JTAG    (BIT(6))
#define TEE_WRITE_REE1_USB_SERIAL_JTAG_M  (TEE_WRITE_REE1_USB_SERIAL_JTAG_V << TEE_WRITE_REE1_USB_SERIAL_JTAG_S)
#define TEE_WRITE_REE1_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_REE1_USB_SERIAL_JTAG_S  6
/** TEE_WRITE_REE2_USB_SERIAL_JTAG : R/W; bitpos: [7]; default: 0;
 *  Configures usb_device registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_USB_SERIAL_JTAG    (BIT(7))
#define TEE_WRITE_REE2_USB_SERIAL_JTAG_M  (TEE_WRITE_REE2_USB_SERIAL_JTAG_V << TEE_WRITE_REE2_USB_SERIAL_JTAG_S)
#define TEE_WRITE_REE2_USB_SERIAL_JTAG_V  0x00000001U
#define TEE_WRITE_REE2_USB_SERIAL_JTAG_S  7

/** TEE_RMT_CTRL_REG register
 *  RMT read/write control register
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
 *  GDMA read/write control register
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

/** TEE_ETM_CTRL_REG register
 *  SOC_ETM read/write control register
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

/** TEE_INTMTX_CTRL_REG register
 *  INTMTX read/write control register
 */
#define TEE_INTMTX_CTRL_REG (DR_REG_TEE_BASE + 0xc8)
/** TEE_READ_TEE_INTMTX_CORE : R/W; bitpos: [0]; default: 1;
 *  Configures intmtx registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_INTMTX_CORE    (BIT(0))
#define TEE_READ_TEE_INTMTX_CORE_M  (TEE_READ_TEE_INTMTX_CORE_V << TEE_READ_TEE_INTMTX_CORE_S)
#define TEE_READ_TEE_INTMTX_CORE_V  0x00000001U
#define TEE_READ_TEE_INTMTX_CORE_S  0
/** TEE_READ_REE0_INTMTX_CORE : R/W; bitpos: [1]; default: 0;
 *  Configures intmtx registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_INTMTX_CORE    (BIT(1))
#define TEE_READ_REE0_INTMTX_CORE_M  (TEE_READ_REE0_INTMTX_CORE_V << TEE_READ_REE0_INTMTX_CORE_S)
#define TEE_READ_REE0_INTMTX_CORE_V  0x00000001U
#define TEE_READ_REE0_INTMTX_CORE_S  1
/** TEE_READ_REE1_INTMTX_CORE : R/W; bitpos: [2]; default: 0;
 *  Configures intmtx registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_INTMTX_CORE    (BIT(2))
#define TEE_READ_REE1_INTMTX_CORE_M  (TEE_READ_REE1_INTMTX_CORE_V << TEE_READ_REE1_INTMTX_CORE_S)
#define TEE_READ_REE1_INTMTX_CORE_V  0x00000001U
#define TEE_READ_REE1_INTMTX_CORE_S  2
/** TEE_READ_REE2_INTMTX_CORE : R/W; bitpos: [3]; default: 0;
 *  Configures intmtx registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_INTMTX_CORE    (BIT(3))
#define TEE_READ_REE2_INTMTX_CORE_M  (TEE_READ_REE2_INTMTX_CORE_V << TEE_READ_REE2_INTMTX_CORE_S)
#define TEE_READ_REE2_INTMTX_CORE_V  0x00000001U
#define TEE_READ_REE2_INTMTX_CORE_S  3
/** TEE_WRITE_TEE_INTMTX_CORE : R/W; bitpos: [4]; default: 1;
 *  Configures intmtx registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_INTMTX_CORE    (BIT(4))
#define TEE_WRITE_TEE_INTMTX_CORE_M  (TEE_WRITE_TEE_INTMTX_CORE_V << TEE_WRITE_TEE_INTMTX_CORE_S)
#define TEE_WRITE_TEE_INTMTX_CORE_V  0x00000001U
#define TEE_WRITE_TEE_INTMTX_CORE_S  4
/** TEE_WRITE_REE0_INTMTX_CORE : R/W; bitpos: [5]; default: 0;
 *  Configures intmtx registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_INTMTX_CORE    (BIT(5))
#define TEE_WRITE_REE0_INTMTX_CORE_M  (TEE_WRITE_REE0_INTMTX_CORE_V << TEE_WRITE_REE0_INTMTX_CORE_S)
#define TEE_WRITE_REE0_INTMTX_CORE_V  0x00000001U
#define TEE_WRITE_REE0_INTMTX_CORE_S  5
/** TEE_WRITE_REE1_INTMTX_CORE : R/W; bitpos: [6]; default: 0;
 *  Configures intmtx registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_INTMTX_CORE    (BIT(6))
#define TEE_WRITE_REE1_INTMTX_CORE_M  (TEE_WRITE_REE1_INTMTX_CORE_V << TEE_WRITE_REE1_INTMTX_CORE_S)
#define TEE_WRITE_REE1_INTMTX_CORE_V  0x00000001U
#define TEE_WRITE_REE1_INTMTX_CORE_S  6
/** TEE_WRITE_REE2_INTMTX_CORE : R/W; bitpos: [7]; default: 0;
 *  Configures intmtx registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_INTMTX_CORE    (BIT(7))
#define TEE_WRITE_REE2_INTMTX_CORE_M  (TEE_WRITE_REE2_INTMTX_CORE_V << TEE_WRITE_REE2_INTMTX_CORE_S)
#define TEE_WRITE_REE2_INTMTX_CORE_V  0x00000001U
#define TEE_WRITE_REE2_INTMTX_CORE_S  7

/** TEE_APB_ADC_CTRL_REG register
 *  SAR ADC read/write control register
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
 *  TIMG0 read/write control register
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
 *  TIMG1 read/write control register
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
 *  SYSTIMER read/write control register
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

/** TEE_PCNT_CTRL_REG register
 *  PCNT read/write control register
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
 *  IO MUX read/write control register
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
 *  PSRAM_MEM_MONITOR read/write control register
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
 *  TCM_MEM_MONITOR read/write control register
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
 *  HP_SYSREG read/write control register
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
 *  PCR read/write control register
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
 *  SPI01 read/write control register
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
 *  HP_APM and LP_APM0 read/write control register
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

/** TEE_CPU_APM_CTRL_REG register
 *  CPU_APM_REG read/write control register
 */
#define TEE_CPU_APM_CTRL_REG (DR_REG_TEE_BASE + 0x114)
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
 *  TEE read/write control register
 */
#define TEE_TEE_CTRL_REG (DR_REG_TEE_BASE + 0x118)
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

/** TEE_CRYPT_CTRL_REG register
 *  CRYPT read/write control register, including security peripherals from AES to ECDSA
 *  address range
 */
#define TEE_CRYPT_CTRL_REG (DR_REG_TEE_BASE + 0x11c)
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

/** TEE_TRACE_CTRL_REG register
 *  TRACE read/write control register
 */
#define TEE_TRACE_CTRL_REG (DR_REG_TEE_BASE + 0x120)
/** TEE_READ_TEE_TRACE : R/W; bitpos: [0]; default: 1;
 *  Configures trace registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_TRACE    (BIT(0))
#define TEE_READ_TEE_TRACE_M  (TEE_READ_TEE_TRACE_V << TEE_READ_TEE_TRACE_S)
#define TEE_READ_TEE_TRACE_V  0x00000001U
#define TEE_READ_TEE_TRACE_S  0
/** TEE_READ_REE0_TRACE : R/W; bitpos: [1]; default: 0;
 *  Configures trace registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_TRACE    (BIT(1))
#define TEE_READ_REE0_TRACE_M  (TEE_READ_REE0_TRACE_V << TEE_READ_REE0_TRACE_S)
#define TEE_READ_REE0_TRACE_V  0x00000001U
#define TEE_READ_REE0_TRACE_S  1
/** TEE_READ_REE1_TRACE : R/W; bitpos: [2]; default: 0;
 *  Configures trace registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_TRACE    (BIT(2))
#define TEE_READ_REE1_TRACE_M  (TEE_READ_REE1_TRACE_V << TEE_READ_REE1_TRACE_S)
#define TEE_READ_REE1_TRACE_V  0x00000001U
#define TEE_READ_REE1_TRACE_S  2
/** TEE_READ_REE2_TRACE : R/W; bitpos: [3]; default: 0;
 *  Configures trace registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_TRACE    (BIT(3))
#define TEE_READ_REE2_TRACE_M  (TEE_READ_REE2_TRACE_V << TEE_READ_REE2_TRACE_S)
#define TEE_READ_REE2_TRACE_V  0x00000001U
#define TEE_READ_REE2_TRACE_S  3
/** TEE_WRITE_TEE_TRACE : R/W; bitpos: [4]; default: 1;
 *  Configures trace registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_TRACE    (BIT(4))
#define TEE_WRITE_TEE_TRACE_M  (TEE_WRITE_TEE_TRACE_V << TEE_WRITE_TEE_TRACE_S)
#define TEE_WRITE_TEE_TRACE_V  0x00000001U
#define TEE_WRITE_TEE_TRACE_S  4
/** TEE_WRITE_REE0_TRACE : R/W; bitpos: [5]; default: 0;
 *  Configures trace registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_TRACE    (BIT(5))
#define TEE_WRITE_REE0_TRACE_M  (TEE_WRITE_REE0_TRACE_V << TEE_WRITE_REE0_TRACE_S)
#define TEE_WRITE_REE0_TRACE_V  0x00000001U
#define TEE_WRITE_REE0_TRACE_S  5
/** TEE_WRITE_REE1_TRACE : R/W; bitpos: [6]; default: 0;
 *  Configures trace registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_TRACE    (BIT(6))
#define TEE_WRITE_REE1_TRACE_M  (TEE_WRITE_REE1_TRACE_V << TEE_WRITE_REE1_TRACE_S)
#define TEE_WRITE_REE1_TRACE_V  0x00000001U
#define TEE_WRITE_REE1_TRACE_S  6
/** TEE_WRITE_REE2_TRACE : R/W; bitpos: [7]; default: 0;
 *  Configures trace registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_TRACE    (BIT(7))
#define TEE_WRITE_REE2_TRACE_M  (TEE_WRITE_REE2_TRACE_V << TEE_WRITE_REE2_TRACE_S)
#define TEE_WRITE_REE2_TRACE_V  0x00000001U
#define TEE_WRITE_REE2_TRACE_S  7

/** TEE_CPU_BUS_MONITOR_CTRL_REG register
 *  BUS_MONITOR read/write control register
 */
#define TEE_CPU_BUS_MONITOR_CTRL_REG (DR_REG_TEE_BASE + 0x128)
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
 *  INTPRI_REG read/write control register
 */
#define TEE_INTPRI_REG_CTRL_REG (DR_REG_TEE_BASE + 0x12c)
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

/** TEE_TWAI1_CTRL_REG register
 *  TWAI1 read/write control register
 */
#define TEE_TWAI1_CTRL_REG (DR_REG_TEE_BASE + 0x138)
/** TEE_READ_TEE_TWAI1 : R/W; bitpos: [0]; default: 1;
 *  Configures can1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_TWAI1    (BIT(0))
#define TEE_READ_TEE_TWAI1_M  (TEE_READ_TEE_TWAI1_V << TEE_READ_TEE_TWAI1_S)
#define TEE_READ_TEE_TWAI1_V  0x00000001U
#define TEE_READ_TEE_TWAI1_S  0
/** TEE_READ_REE0_TWAI1 : R/W; bitpos: [1]; default: 0;
 *  Configures can1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_TWAI1    (BIT(1))
#define TEE_READ_REE0_TWAI1_M  (TEE_READ_REE0_TWAI1_V << TEE_READ_REE0_TWAI1_S)
#define TEE_READ_REE0_TWAI1_V  0x00000001U
#define TEE_READ_REE0_TWAI1_S  1
/** TEE_READ_REE1_TWAI1 : R/W; bitpos: [2]; default: 0;
 *  Configures can1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_TWAI1    (BIT(2))
#define TEE_READ_REE1_TWAI1_M  (TEE_READ_REE1_TWAI1_V << TEE_READ_REE1_TWAI1_S)
#define TEE_READ_REE1_TWAI1_V  0x00000001U
#define TEE_READ_REE1_TWAI1_S  2
/** TEE_READ_REE2_TWAI1 : R/W; bitpos: [3]; default: 0;
 *  Configures can1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_TWAI1    (BIT(3))
#define TEE_READ_REE2_TWAI1_M  (TEE_READ_REE2_TWAI1_V << TEE_READ_REE2_TWAI1_S)
#define TEE_READ_REE2_TWAI1_V  0x00000001U
#define TEE_READ_REE2_TWAI1_S  3
/** TEE_WRITE_TEE_TWAI1 : R/W; bitpos: [4]; default: 1;
 *  Configures can1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_TWAI1    (BIT(4))
#define TEE_WRITE_TEE_TWAI1_M  (TEE_WRITE_TEE_TWAI1_V << TEE_WRITE_TEE_TWAI1_S)
#define TEE_WRITE_TEE_TWAI1_V  0x00000001U
#define TEE_WRITE_TEE_TWAI1_S  4
/** TEE_WRITE_REE0_TWAI1 : R/W; bitpos: [5]; default: 0;
 *  Configures can1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_TWAI1    (BIT(5))
#define TEE_WRITE_REE0_TWAI1_M  (TEE_WRITE_REE0_TWAI1_V << TEE_WRITE_REE0_TWAI1_S)
#define TEE_WRITE_REE0_TWAI1_V  0x00000001U
#define TEE_WRITE_REE0_TWAI1_S  5
/** TEE_WRITE_REE1_TWAI1 : R/W; bitpos: [6]; default: 0;
 *  Configures can1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_TWAI1    (BIT(6))
#define TEE_WRITE_REE1_TWAI1_M  (TEE_WRITE_REE1_TWAI1_V << TEE_WRITE_REE1_TWAI1_S)
#define TEE_WRITE_REE1_TWAI1_V  0x00000001U
#define TEE_WRITE_REE1_TWAI1_S  6
/** TEE_WRITE_REE2_TWAI1 : R/W; bitpos: [7]; default: 0;
 *  Configures can1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_TWAI1    (BIT(7))
#define TEE_WRITE_REE2_TWAI1_M  (TEE_WRITE_REE2_TWAI1_V << TEE_WRITE_REE2_TWAI1_S)
#define TEE_WRITE_REE2_TWAI1_V  0x00000001U
#define TEE_WRITE_REE2_TWAI1_S  7

/** TEE_SPI2_CTRL_REG register
 *  SPI2 read/write control register
 */
#define TEE_SPI2_CTRL_REG (DR_REG_TEE_BASE + 0x13c)
/** TEE_READ_TEE_SPI2 : R/W; bitpos: [0]; default: 1;
 *  Configures spi2 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_SPI2    (BIT(0))
#define TEE_READ_TEE_SPI2_M  (TEE_READ_TEE_SPI2_V << TEE_READ_TEE_SPI2_S)
#define TEE_READ_TEE_SPI2_V  0x00000001U
#define TEE_READ_TEE_SPI2_S  0
/** TEE_READ_REE0_SPI2 : R/W; bitpos: [1]; default: 0;
 *  Configures spi2 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_SPI2    (BIT(1))
#define TEE_READ_REE0_SPI2_M  (TEE_READ_REE0_SPI2_V << TEE_READ_REE0_SPI2_S)
#define TEE_READ_REE0_SPI2_V  0x00000001U
#define TEE_READ_REE0_SPI2_S  1
/** TEE_READ_REE1_SPI2 : R/W; bitpos: [2]; default: 0;
 *  Configures spi2 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_SPI2    (BIT(2))
#define TEE_READ_REE1_SPI2_M  (TEE_READ_REE1_SPI2_V << TEE_READ_REE1_SPI2_S)
#define TEE_READ_REE1_SPI2_V  0x00000001U
#define TEE_READ_REE1_SPI2_S  2
/** TEE_READ_REE2_SPI2 : R/W; bitpos: [3]; default: 0;
 *  Configures spi2 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_SPI2    (BIT(3))
#define TEE_READ_REE2_SPI2_M  (TEE_READ_REE2_SPI2_V << TEE_READ_REE2_SPI2_S)
#define TEE_READ_REE2_SPI2_V  0x00000001U
#define TEE_READ_REE2_SPI2_S  3
/** TEE_WRITE_TEE_SPI2 : R/W; bitpos: [4]; default: 1;
 *  Configures spi2 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_SPI2    (BIT(4))
#define TEE_WRITE_TEE_SPI2_M  (TEE_WRITE_TEE_SPI2_V << TEE_WRITE_TEE_SPI2_S)
#define TEE_WRITE_TEE_SPI2_V  0x00000001U
#define TEE_WRITE_TEE_SPI2_S  4
/** TEE_WRITE_REE0_SPI2 : R/W; bitpos: [5]; default: 0;
 *  Configures spi2 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_SPI2    (BIT(5))
#define TEE_WRITE_REE0_SPI2_M  (TEE_WRITE_REE0_SPI2_V << TEE_WRITE_REE0_SPI2_S)
#define TEE_WRITE_REE0_SPI2_V  0x00000001U
#define TEE_WRITE_REE0_SPI2_S  5
/** TEE_WRITE_REE1_SPI2 : R/W; bitpos: [6]; default: 0;
 *  Configures spi2 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_SPI2    (BIT(6))
#define TEE_WRITE_REE1_SPI2_M  (TEE_WRITE_REE1_SPI2_V << TEE_WRITE_REE1_SPI2_S)
#define TEE_WRITE_REE1_SPI2_V  0x00000001U
#define TEE_WRITE_REE1_SPI2_S  6
/** TEE_WRITE_REE2_SPI2 : R/W; bitpos: [7]; default: 0;
 *  Configures spi2 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_SPI2    (BIT(7))
#define TEE_WRITE_REE2_SPI2_M  (TEE_WRITE_REE2_SPI2_V << TEE_WRITE_REE2_SPI2_S)
#define TEE_WRITE_REE2_SPI2_V  0x00000001U
#define TEE_WRITE_REE2_SPI2_S  7

/** TEE_BS_CTRL_REG register
 *  BITSCRAMBLER read/write control register
 */
#define TEE_BS_CTRL_REG (DR_REG_TEE_BASE + 0x140)
/** TEE_READ_TEE_BS : R/W; bitpos: [0]; default: 1;
 *  Configures bs registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_TEE_BS    (BIT(0))
#define TEE_READ_TEE_BS_M  (TEE_READ_TEE_BS_V << TEE_READ_TEE_BS_S)
#define TEE_READ_TEE_BS_V  0x00000001U
#define TEE_READ_TEE_BS_S  0
/** TEE_READ_REE0_BS : R/W; bitpos: [1]; default: 0;
 *  Configures bs registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE0_BS    (BIT(1))
#define TEE_READ_REE0_BS_M  (TEE_READ_REE0_BS_V << TEE_READ_REE0_BS_S)
#define TEE_READ_REE0_BS_V  0x00000001U
#define TEE_READ_REE0_BS_S  1
/** TEE_READ_REE1_BS : R/W; bitpos: [2]; default: 0;
 *  Configures bs registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE1_BS    (BIT(2))
#define TEE_READ_REE1_BS_M  (TEE_READ_REE1_BS_V << TEE_READ_REE1_BS_S)
#define TEE_READ_REE1_BS_V  0x00000001U
#define TEE_READ_REE1_BS_S  2
/** TEE_READ_REE2_BS : R/W; bitpos: [3]; default: 0;
 *  Configures bs registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define TEE_READ_REE2_BS    (BIT(3))
#define TEE_READ_REE2_BS_M  (TEE_READ_REE2_BS_V << TEE_READ_REE2_BS_S)
#define TEE_READ_REE2_BS_V  0x00000001U
#define TEE_READ_REE2_BS_S  3
/** TEE_WRITE_TEE_BS : R/W; bitpos: [4]; default: 1;
 *  Configures bs registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_TEE_BS    (BIT(4))
#define TEE_WRITE_TEE_BS_M  (TEE_WRITE_TEE_BS_V << TEE_WRITE_TEE_BS_S)
#define TEE_WRITE_TEE_BS_V  0x00000001U
#define TEE_WRITE_TEE_BS_S  4
/** TEE_WRITE_REE0_BS : R/W; bitpos: [5]; default: 0;
 *  Configures bs registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE0_BS    (BIT(5))
#define TEE_WRITE_REE0_BS_M  (TEE_WRITE_REE0_BS_V << TEE_WRITE_REE0_BS_S)
#define TEE_WRITE_REE0_BS_V  0x00000001U
#define TEE_WRITE_REE0_BS_S  5
/** TEE_WRITE_REE1_BS : R/W; bitpos: [6]; default: 0;
 *  Configures bs registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE1_BS    (BIT(6))
#define TEE_WRITE_REE1_BS_M  (TEE_WRITE_REE1_BS_V << TEE_WRITE_REE1_BS_S)
#define TEE_WRITE_REE1_BS_V  0x00000001U
#define TEE_WRITE_REE1_BS_S  6
/** TEE_WRITE_REE2_BS : R/W; bitpos: [7]; default: 0;
 *  Configures bs registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define TEE_WRITE_REE2_BS    (BIT(7))
#define TEE_WRITE_REE2_BS_M  (TEE_WRITE_REE2_BS_V << TEE_WRITE_REE2_BS_S)
#define TEE_WRITE_REE2_BS_V  0x00000001U
#define TEE_WRITE_REE2_BS_S  7

/** TEE_BUS_ERR_CONF_REG register
 *  Error message return configuration register
 */
#define TEE_BUS_ERR_CONF_REG (DR_REG_TEE_BASE + 0xff0)
/** TEE_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether to return error message to CPU when access is blocked.
 *  0: Disable
 *  1: Enable
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
 *  0: Enable automatic clock gating
 *  1: Keep the clock always on
 */
#define TEE_CLK_EN    (BIT(0))
#define TEE_CLK_EN_M  (TEE_CLK_EN_V << TEE_CLK_EN_S)
#define TEE_CLK_EN_V  0x00000001U
#define TEE_CLK_EN_S  0

/** TEE_DATE_REG register
 *  Version control register
 */
#define TEE_DATE_REG (DR_REG_TEE_BASE + 0xffc)
/** TEE_DATE : R/W; bitpos: [27:0]; default: 37773824;
 *  Version control register.
 */
#define TEE_DATE    0x0FFFFFFFU
#define TEE_DATE_M  (TEE_DATE_V << TEE_DATE_S)
#define TEE_DATE_V  0x0FFFFFFFU
#define TEE_DATE_S  0

#ifdef __cplusplus
}
#endif
