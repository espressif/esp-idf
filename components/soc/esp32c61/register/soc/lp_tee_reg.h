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

/** LP_TEE_M0_MODE_CTRL_REG register
 *  TEE mode control register
 */
#define LP_TEE_M0_MODE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x0)
/** LP_TEE_M0_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures M0 security level mode.\\
 *  0: tee_mode \\
 *  1: ree_mode0 \\
 *  2: ree_mode1 \\
 *  3: ree_mode2  \\
 */
#define LP_TEE_M0_MODE    0x00000003U
#define LP_TEE_M0_MODE_M  (LP_TEE_M0_MODE_V << LP_TEE_M0_MODE_S)
#define LP_TEE_M0_MODE_V  0x00000003U
#define LP_TEE_M0_MODE_S  0
/** LP_TEE_M0_LOCK : R/W; bitpos: [2]; default: 0;
 *  Set 1 to lock m0 tee configuration
 */
#define LP_TEE_M0_LOCK    (BIT(2))
#define LP_TEE_M0_LOCK_M  (LP_TEE_M0_LOCK_V << LP_TEE_M0_LOCK_S)
#define LP_TEE_M0_LOCK_V  0x00000001U
#define LP_TEE_M0_LOCK_S  2

/** LP_TEE_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define LP_TEE_CLOCK_GATE_REG (DR_REG_LP_TEE_BASE + 0x4)
/** LP_TEE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.\\
 *  0: enable automatic clock gating \\
 *  1: keep the clock always on  \\
 */
#define LP_TEE_CLK_EN    (BIT(0))
#define LP_TEE_CLK_EN_M  (LP_TEE_CLK_EN_V << LP_TEE_CLK_EN_S)
#define LP_TEE_CLK_EN_V  0x00000001U
#define LP_TEE_CLK_EN_S  0

/** LP_TEE_FORCE_ACC_HP_REG register
 *  Force access to hpmem configuration register
 */
#define LP_TEE_FORCE_ACC_HP_REG (DR_REG_LP_TEE_BASE + 0x90)
/** LP_TEE_FORCE_ACC_HPMEM_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether to allow LP CPU to force access to HP_MEM regardless of
 *  permission management.\\
 *  0: disable force access HP_MEM \\
 *  1: enable force access HP_MEM \\
 */
#define LP_TEE_FORCE_ACC_HPMEM_EN    (BIT(0))
#define LP_TEE_FORCE_ACC_HPMEM_EN_M  (LP_TEE_FORCE_ACC_HPMEM_EN_V << LP_TEE_FORCE_ACC_HPMEM_EN_S)
#define LP_TEE_FORCE_ACC_HPMEM_EN_V  0x00000001U
#define LP_TEE_FORCE_ACC_HPMEM_EN_S  0

/** LP_TEE_DATE_REG register
 *  Version control register
 */
#define LP_TEE_DATE_REG (DR_REG_LP_TEE_BASE + 0xfc)
/** LP_TEE_DATE_REG : R/W; bitpos: [27:0]; default: 35725664;
 *  Version control register
 */
#define LP_TEE_DATE_REG    0x0FFFFFFFU
#define LP_TEE_DATE_REG_M  (LP_TEE_DATE_REG_V << LP_TEE_DATE_REG_S)
#define LP_TEE_DATE_REG_V  0x0FFFFFFFU
#define LP_TEE_DATE_REG_S  0

#ifdef __cplusplus
}
#endif
