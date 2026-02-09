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

/** LP_TEE_M2_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define LP_TEE_M2_MODE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x0)
/** LP_TEE_M2_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M$n security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define LP_TEE_M2_MODE    0x00000003U
#define LP_TEE_M2_MODE_M  (LP_TEE_M2_MODE_V << LP_TEE_M2_MODE_S)
#define LP_TEE_M2_MODE_V  0x00000003U
#define LP_TEE_M2_MODE_S  0
/** LP_TEE_M2_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m2 tee configuration
 */
#define LP_TEE_M2_LOCK    (BIT(2))
#define LP_TEE_M2_LOCK_M  (LP_TEE_M2_LOCK_V << LP_TEE_M2_LOCK_S)
#define LP_TEE_M2_LOCK_V  0x00000001U
#define LP_TEE_M2_LOCK_S  2

/** LP_TEE_M16_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define LP_TEE_M16_MODE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x4)
/** LP_TEE_M16_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M$n security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define LP_TEE_M16_MODE    0x00000003U
#define LP_TEE_M16_MODE_M  (LP_TEE_M16_MODE_V << LP_TEE_M16_MODE_S)
#define LP_TEE_M16_MODE_V  0x00000003U
#define LP_TEE_M16_MODE_S  0
/** LP_TEE_M16_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m16 tee configuration
 */
#define LP_TEE_M16_LOCK    (BIT(2))
#define LP_TEE_M16_LOCK_M  (LP_TEE_M16_LOCK_V << LP_TEE_M16_LOCK_S)
#define LP_TEE_M16_LOCK_V  0x00000001U
#define LP_TEE_M16_LOCK_S  2

/** LP_TEE_M17_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define LP_TEE_M17_MODE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x8)
/** LP_TEE_M17_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M$n security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define LP_TEE_M17_MODE    0x00000003U
#define LP_TEE_M17_MODE_M  (LP_TEE_M17_MODE_V << LP_TEE_M17_MODE_S)
#define LP_TEE_M17_MODE_V  0x00000003U
#define LP_TEE_M17_MODE_S  0
/** LP_TEE_M17_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m17 tee configuration
 */
#define LP_TEE_M17_LOCK    (BIT(2))
#define LP_TEE_M17_LOCK_M  (LP_TEE_M17_LOCK_V << LP_TEE_M17_LOCK_S)
#define LP_TEE_M17_LOCK_V  0x00000001U
#define LP_TEE_M17_LOCK_S  2

/** LP_TEE_M18_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define LP_TEE_M18_MODE_CTRL_REG (DR_REG_LP_TEE_BASE + 0xc)
/** LP_TEE_M18_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M$n security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define LP_TEE_M18_MODE    0x00000003U
#define LP_TEE_M18_MODE_M  (LP_TEE_M18_MODE_V << LP_TEE_M18_MODE_S)
#define LP_TEE_M18_MODE_V  0x00000003U
#define LP_TEE_M18_MODE_S  0
/** LP_TEE_M18_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m18 tee configuration
 */
#define LP_TEE_M18_LOCK    (BIT(2))
#define LP_TEE_M18_LOCK_M  (LP_TEE_M18_LOCK_V << LP_TEE_M18_LOCK_S)
#define LP_TEE_M18_LOCK_V  0x00000001U
#define LP_TEE_M18_LOCK_S  2

/** LP_TEE_M19_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define LP_TEE_M19_MODE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x10)
/** LP_TEE_M19_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M$n security level mode.
 *  0: tee_mode
 *  1: ree_mode0
 *  2: ree_mode1
 *  3: ree_mode2
 */
#define LP_TEE_M19_MODE    0x00000003U
#define LP_TEE_M19_MODE_M  (LP_TEE_M19_MODE_V << LP_TEE_M19_MODE_S)
#define LP_TEE_M19_MODE_V  0x00000003U
#define LP_TEE_M19_MODE_S  0
/** LP_TEE_M19_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m19 tee configuration
 */
#define LP_TEE_M19_LOCK    (BIT(2))
#define LP_TEE_M19_LOCK_M  (LP_TEE_M19_LOCK_V << LP_TEE_M19_LOCK_S)
#define LP_TEE_M19_LOCK_V  0x00000001U
#define LP_TEE_M19_LOCK_S  2

/** LP_TEE_FORCE_ACC_HP_REG register
 *  Force access to hpmem configuration register
 */
#define LP_TEE_FORCE_ACC_HP_REG (DR_REG_LP_TEE_BASE + 0x14)
/** LP_TEE_FORCE_ACC_HPMEM_EN : R/W; bitpos: [7:0]; default: 0;
 *  Configures whether to allow LP CPU to force access to HP_MEM regardless of
 *  permission management.
 *  0x5a: enable force access HP_MEM
 *  other: disable force access HP_MEM
 */
#define LP_TEE_FORCE_ACC_HPMEM_EN    0x000000FFU
#define LP_TEE_FORCE_ACC_HPMEM_EN_M  (LP_TEE_FORCE_ACC_HPMEM_EN_V << LP_TEE_FORCE_ACC_HPMEM_EN_S)
#define LP_TEE_FORCE_ACC_HPMEM_EN_V  0x000000FFU
#define LP_TEE_FORCE_ACC_HPMEM_EN_S  0

/** LP_TEE_LP_GPIO_SECURITY_REG register
 *  need des
 */
#define LP_TEE_LP_GPIO_SECURITY_REG (DR_REG_LP_TEE_BASE + 0x18)
/** LP_TEE_LP_GPIO_LOCK : R/W; bitpos: [7:0]; default: 0;
 *  This field decides whether lp_gpio_config can be locked, or not. 0 (default):
 *  unlocked. 1: locked.
 */
#define LP_TEE_LP_GPIO_LOCK    0x000000FFU
#define LP_TEE_LP_GPIO_LOCK_M  (LP_TEE_LP_GPIO_LOCK_V << LP_TEE_LP_GPIO_LOCK_S)
#define LP_TEE_LP_GPIO_LOCK_V  0x000000FFU
#define LP_TEE_LP_GPIO_LOCK_S  0

/** LP_TEE_HP_GPIO_SECURITY_1_REG register
 *  need des
 */
#define LP_TEE_HP_GPIO_SECURITY_1_REG (DR_REG_LP_TEE_BASE + 0x1c)
/** LP_TEE_HP_GPIO_LOCK_P1 : R/W; bitpos: [31:0]; default: 0;
 *  This field decides whether hp_gpio_config of PIN0~31 can be locked, or not. 0
 *  (default): unlocked. 1: locked.
 */
#define LP_TEE_HP_GPIO_LOCK_P1    0xFFFFFFFFU
#define LP_TEE_HP_GPIO_LOCK_P1_M  (LP_TEE_HP_GPIO_LOCK_P1_V << LP_TEE_HP_GPIO_LOCK_P1_S)
#define LP_TEE_HP_GPIO_LOCK_P1_V  0xFFFFFFFFU
#define LP_TEE_HP_GPIO_LOCK_P1_S  0

/** LP_TEE_HP_GPIO_SECURITY_2_REG register
 *  need des
 */
#define LP_TEE_HP_GPIO_SECURITY_2_REG (DR_REG_LP_TEE_BASE + 0x20)
/** LP_TEE_HP_GPIO_LOCK_P2 : R/W; bitpos: [30:0]; default: 0;
 *  This field decides whether hp_gpio_config of PIN32~62 can be locked, or not. 0
 *  (default): unlocked. 1: locked.
 */
#define LP_TEE_HP_GPIO_LOCK_P2    0x7FFFFFFFU
#define LP_TEE_HP_GPIO_LOCK_P2_M  (LP_TEE_HP_GPIO_LOCK_P2_V << LP_TEE_HP_GPIO_LOCK_P2_S)
#define LP_TEE_HP_GPIO_LOCK_P2_V  0x7FFFFFFFU
#define LP_TEE_HP_GPIO_LOCK_P2_S  0

/** LP_TEE_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define LP_TEE_CLOCK_GATE_REG (DR_REG_LP_TEE_BASE + 0x3f8)
/** LP_TEE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: enable automatic clock gating
 *  1: keep the clock always on
 */
#define LP_TEE_CLK_EN    (BIT(0))
#define LP_TEE_CLK_EN_M  (LP_TEE_CLK_EN_V << LP_TEE_CLK_EN_S)
#define LP_TEE_CLK_EN_V  0x00000001U
#define LP_TEE_CLK_EN_S  0

/** LP_TEE_DATE_REG register
 *  Version control register
 */
#define LP_TEE_DATE_REG (DR_REG_LP_TEE_BASE + 0x3fc)
/** LP_TEE_DATE : R/W; bitpos: [27:0]; default: 38822464;
 *  Version control register
 */
#define LP_TEE_DATE    0x0FFFFFFFU
#define LP_TEE_DATE_M  (LP_TEE_DATE_V << LP_TEE_DATE_S)
#define LP_TEE_DATE_V  0x0FFFFFFFU
#define LP_TEE_DATE_S  0

#ifdef __cplusplus
}
#endif
