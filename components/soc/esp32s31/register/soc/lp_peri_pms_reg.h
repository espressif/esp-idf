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

/** LP_PERI_PMS_LP_SYSREG_CTRL_REG register
 *  lp_sysreg read/write control register
 */
#define LP_PERI_PMS_LP_SYSREG_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x0)
/** LP_PERI_PMS_READ_TEE_LP_SYSREG : R/W; bitpos: [0]; default: 1;
 *  Configures lp_sysreg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_SYSREG    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_SYSREG_M  (LP_PERI_PMS_READ_TEE_LP_SYSREG_V << LP_PERI_PMS_READ_TEE_LP_SYSREG_S)
#define LP_PERI_PMS_READ_TEE_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_SYSREG_S  0
/** LP_PERI_PMS_READ_REE0_LP_SYSREG : R/W; bitpos: [1]; default: 0;
 *  Configures lp_sysreg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_SYSREG    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_SYSREG_M  (LP_PERI_PMS_READ_REE0_LP_SYSREG_V << LP_PERI_PMS_READ_REE0_LP_SYSREG_S)
#define LP_PERI_PMS_READ_REE0_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_SYSREG_S  1
/** LP_PERI_PMS_READ_REE1_LP_SYSREG : R/W; bitpos: [2]; default: 0;
 *  Configures lp_sysreg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_SYSREG    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_SYSREG_M  (LP_PERI_PMS_READ_REE1_LP_SYSREG_V << LP_PERI_PMS_READ_REE1_LP_SYSREG_S)
#define LP_PERI_PMS_READ_REE1_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_SYSREG_S  2
/** LP_PERI_PMS_READ_REE2_LP_SYSREG : R/W; bitpos: [3]; default: 0;
 *  Configures lp_sysreg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_SYSREG    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_SYSREG_M  (LP_PERI_PMS_READ_REE2_LP_SYSREG_V << LP_PERI_PMS_READ_REE2_LP_SYSREG_S)
#define LP_PERI_PMS_READ_REE2_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_SYSREG_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_SYSREG : R/W; bitpos: [4]; default: 1;
 *  Configures lp_sysreg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_SYSREG    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_SYSREG_M  (LP_PERI_PMS_WRITE_TEE_LP_SYSREG_V << LP_PERI_PMS_WRITE_TEE_LP_SYSREG_S)
#define LP_PERI_PMS_WRITE_TEE_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_SYSREG_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_SYSREG : R/W; bitpos: [5]; default: 0;
 *  Configures lp_sysreg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_SYSREG    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_SYSREG_M  (LP_PERI_PMS_WRITE_REE0_LP_SYSREG_V << LP_PERI_PMS_WRITE_REE0_LP_SYSREG_S)
#define LP_PERI_PMS_WRITE_REE0_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_SYSREG_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_SYSREG : R/W; bitpos: [6]; default: 0;
 *  Configures lp_sysreg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_SYSREG    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_SYSREG_M  (LP_PERI_PMS_WRITE_REE1_LP_SYSREG_V << LP_PERI_PMS_WRITE_REE1_LP_SYSREG_S)
#define LP_PERI_PMS_WRITE_REE1_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_SYSREG_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_SYSREG : R/W; bitpos: [7]; default: 0;
 *  Configures lp_sysreg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_SYSREG    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_SYSREG_M  (LP_PERI_PMS_WRITE_REE2_LP_SYSREG_V << LP_PERI_PMS_WRITE_REE2_LP_SYSREG_S)
#define LP_PERI_PMS_WRITE_REE2_LP_SYSREG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_SYSREG_S  7
/** LP_PERI_PMS_LP_SYSREG_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_sysreg peri_apm configuration
 */
#define LP_PERI_PMS_LP_SYSREG_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_SYSREG_LOCK_M  (LP_PERI_PMS_LP_SYSREG_LOCK_V << LP_PERI_PMS_LP_SYSREG_LOCK_S)
#define LP_PERI_PMS_LP_SYSREG_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_SYSREG_LOCK_S  8

/** LP_PERI_PMS_LP_AONCLKRST_CTRL_REG register
 *  lp_aonclkrst read/write control register
 */
#define LP_PERI_PMS_LP_AONCLKRST_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x4)
/** LP_PERI_PMS_READ_TEE_LP_AONCLKRST : R/W; bitpos: [0]; default: 1;
 *  Configures lp_aonclkrst registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_AONCLKRST    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_AONCLKRST_M  (LP_PERI_PMS_READ_TEE_LP_AONCLKRST_V << LP_PERI_PMS_READ_TEE_LP_AONCLKRST_S)
#define LP_PERI_PMS_READ_TEE_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_AONCLKRST_S  0
/** LP_PERI_PMS_READ_REE0_LP_AONCLKRST : R/W; bitpos: [1]; default: 0;
 *  Configures lp_aonclkrst registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_AONCLKRST    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_AONCLKRST_M  (LP_PERI_PMS_READ_REE0_LP_AONCLKRST_V << LP_PERI_PMS_READ_REE0_LP_AONCLKRST_S)
#define LP_PERI_PMS_READ_REE0_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_AONCLKRST_S  1
/** LP_PERI_PMS_READ_REE1_LP_AONCLKRST : R/W; bitpos: [2]; default: 0;
 *  Configures lp_aonclkrst registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_AONCLKRST    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_AONCLKRST_M  (LP_PERI_PMS_READ_REE1_LP_AONCLKRST_V << LP_PERI_PMS_READ_REE1_LP_AONCLKRST_S)
#define LP_PERI_PMS_READ_REE1_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_AONCLKRST_S  2
/** LP_PERI_PMS_READ_REE2_LP_AONCLKRST : R/W; bitpos: [3]; default: 0;
 *  Configures lp_aonclkrst registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_AONCLKRST    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_AONCLKRST_M  (LP_PERI_PMS_READ_REE2_LP_AONCLKRST_V << LP_PERI_PMS_READ_REE2_LP_AONCLKRST_S)
#define LP_PERI_PMS_READ_REE2_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_AONCLKRST_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_AONCLKRST : R/W; bitpos: [4]; default: 1;
 *  Configures lp_aonclkrst registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_AONCLKRST    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_AONCLKRST_M  (LP_PERI_PMS_WRITE_TEE_LP_AONCLKRST_V << LP_PERI_PMS_WRITE_TEE_LP_AONCLKRST_S)
#define LP_PERI_PMS_WRITE_TEE_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_AONCLKRST_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_AONCLKRST : R/W; bitpos: [5]; default: 0;
 *  Configures lp_aonclkrst registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_AONCLKRST    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_AONCLKRST_M  (LP_PERI_PMS_WRITE_REE0_LP_AONCLKRST_V << LP_PERI_PMS_WRITE_REE0_LP_AONCLKRST_S)
#define LP_PERI_PMS_WRITE_REE0_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_AONCLKRST_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_AONCLKRST : R/W; bitpos: [6]; default: 0;
 *  Configures lp_aonclkrst registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_AONCLKRST    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_AONCLKRST_M  (LP_PERI_PMS_WRITE_REE1_LP_AONCLKRST_V << LP_PERI_PMS_WRITE_REE1_LP_AONCLKRST_S)
#define LP_PERI_PMS_WRITE_REE1_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_AONCLKRST_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_AONCLKRST : R/W; bitpos: [7]; default: 0;
 *  Configures lp_aonclkrst registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_AONCLKRST    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_AONCLKRST_M  (LP_PERI_PMS_WRITE_REE2_LP_AONCLKRST_V << LP_PERI_PMS_WRITE_REE2_LP_AONCLKRST_S)
#define LP_PERI_PMS_WRITE_REE2_LP_AONCLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_AONCLKRST_S  7
/** LP_PERI_PMS_LP_AONCLKRST_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_aonclkrst peri_apm configuration
 */
#define LP_PERI_PMS_LP_AONCLKRST_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_AONCLKRST_LOCK_M  (LP_PERI_PMS_LP_AONCLKRST_LOCK_V << LP_PERI_PMS_LP_AONCLKRST_LOCK_S)
#define LP_PERI_PMS_LP_AONCLKRST_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_AONCLKRST_LOCK_S  8

/** LP_PERI_PMS_LP_ANAPERI_CTRL_REG register
 *  lp_anaperi read/write control register
 */
#define LP_PERI_PMS_LP_ANAPERI_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x8)
/** LP_PERI_PMS_READ_TEE_LP_ANAPERI : R/W; bitpos: [0]; default: 1;
 *  Configures lp_anaperi registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_ANAPERI    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_ANAPERI_M  (LP_PERI_PMS_READ_TEE_LP_ANAPERI_V << LP_PERI_PMS_READ_TEE_LP_ANAPERI_S)
#define LP_PERI_PMS_READ_TEE_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_ANAPERI_S  0
/** LP_PERI_PMS_READ_REE0_LP_ANAPERI : R/W; bitpos: [1]; default: 0;
 *  Configures lp_anaperi registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_ANAPERI    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_ANAPERI_M  (LP_PERI_PMS_READ_REE0_LP_ANAPERI_V << LP_PERI_PMS_READ_REE0_LP_ANAPERI_S)
#define LP_PERI_PMS_READ_REE0_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_ANAPERI_S  1
/** LP_PERI_PMS_READ_REE1_LP_ANAPERI : R/W; bitpos: [2]; default: 0;
 *  Configures lp_anaperi registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_ANAPERI    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_ANAPERI_M  (LP_PERI_PMS_READ_REE1_LP_ANAPERI_V << LP_PERI_PMS_READ_REE1_LP_ANAPERI_S)
#define LP_PERI_PMS_READ_REE1_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_ANAPERI_S  2
/** LP_PERI_PMS_READ_REE2_LP_ANAPERI : R/W; bitpos: [3]; default: 0;
 *  Configures lp_anaperi registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_ANAPERI    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_ANAPERI_M  (LP_PERI_PMS_READ_REE2_LP_ANAPERI_V << LP_PERI_PMS_READ_REE2_LP_ANAPERI_S)
#define LP_PERI_PMS_READ_REE2_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_ANAPERI_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_ANAPERI : R/W; bitpos: [4]; default: 1;
 *  Configures lp_anaperi registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_ANAPERI    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_ANAPERI_M  (LP_PERI_PMS_WRITE_TEE_LP_ANAPERI_V << LP_PERI_PMS_WRITE_TEE_LP_ANAPERI_S)
#define LP_PERI_PMS_WRITE_TEE_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_ANAPERI_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_ANAPERI : R/W; bitpos: [5]; default: 0;
 *  Configures lp_anaperi registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_ANAPERI    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_ANAPERI_M  (LP_PERI_PMS_WRITE_REE0_LP_ANAPERI_V << LP_PERI_PMS_WRITE_REE0_LP_ANAPERI_S)
#define LP_PERI_PMS_WRITE_REE0_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_ANAPERI_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_ANAPERI : R/W; bitpos: [6]; default: 0;
 *  Configures lp_anaperi registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_ANAPERI    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_ANAPERI_M  (LP_PERI_PMS_WRITE_REE1_LP_ANAPERI_V << LP_PERI_PMS_WRITE_REE1_LP_ANAPERI_S)
#define LP_PERI_PMS_WRITE_REE1_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_ANAPERI_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_ANAPERI : R/W; bitpos: [7]; default: 0;
 *  Configures lp_anaperi registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_ANAPERI    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_ANAPERI_M  (LP_PERI_PMS_WRITE_REE2_LP_ANAPERI_V << LP_PERI_PMS_WRITE_REE2_LP_ANAPERI_S)
#define LP_PERI_PMS_WRITE_REE2_LP_ANAPERI_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_ANAPERI_S  7
/** LP_PERI_PMS_LP_ANAPERI_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_anaperi peri_apm configuration
 */
#define LP_PERI_PMS_LP_ANAPERI_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_ANAPERI_LOCK_M  (LP_PERI_PMS_LP_ANAPERI_LOCK_V << LP_PERI_PMS_LP_ANAPERI_LOCK_S)
#define LP_PERI_PMS_LP_ANAPERI_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_ANAPERI_LOCK_S  8

/** LP_PERI_PMS_LP_HUK_CTRL_REG register
 *  lp_huk read/write control register
 */
#define LP_PERI_PMS_LP_HUK_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0xc)
/** LP_PERI_PMS_READ_TEE_LP_HUK : R/W; bitpos: [0]; default: 1;
 *  Configures lp_huk registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_HUK    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_HUK_M  (LP_PERI_PMS_READ_TEE_LP_HUK_V << LP_PERI_PMS_READ_TEE_LP_HUK_S)
#define LP_PERI_PMS_READ_TEE_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_HUK_S  0
/** LP_PERI_PMS_READ_REE0_LP_HUK : R/W; bitpos: [1]; default: 0;
 *  Configures lp_huk registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_HUK    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_HUK_M  (LP_PERI_PMS_READ_REE0_LP_HUK_V << LP_PERI_PMS_READ_REE0_LP_HUK_S)
#define LP_PERI_PMS_READ_REE0_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_HUK_S  1
/** LP_PERI_PMS_READ_REE1_LP_HUK : R/W; bitpos: [2]; default: 0;
 *  Configures lp_huk registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_HUK    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_HUK_M  (LP_PERI_PMS_READ_REE1_LP_HUK_V << LP_PERI_PMS_READ_REE1_LP_HUK_S)
#define LP_PERI_PMS_READ_REE1_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_HUK_S  2
/** LP_PERI_PMS_READ_REE2_LP_HUK : R/W; bitpos: [3]; default: 0;
 *  Configures lp_huk registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_HUK    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_HUK_M  (LP_PERI_PMS_READ_REE2_LP_HUK_V << LP_PERI_PMS_READ_REE2_LP_HUK_S)
#define LP_PERI_PMS_READ_REE2_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_HUK_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_HUK : R/W; bitpos: [4]; default: 1;
 *  Configures lp_huk registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_HUK    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_HUK_M  (LP_PERI_PMS_WRITE_TEE_LP_HUK_V << LP_PERI_PMS_WRITE_TEE_LP_HUK_S)
#define LP_PERI_PMS_WRITE_TEE_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_HUK_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_HUK : R/W; bitpos: [5]; default: 0;
 *  Configures lp_huk registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_HUK    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_HUK_M  (LP_PERI_PMS_WRITE_REE0_LP_HUK_V << LP_PERI_PMS_WRITE_REE0_LP_HUK_S)
#define LP_PERI_PMS_WRITE_REE0_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_HUK_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_HUK : R/W; bitpos: [6]; default: 0;
 *  Configures lp_huk registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_HUK    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_HUK_M  (LP_PERI_PMS_WRITE_REE1_LP_HUK_V << LP_PERI_PMS_WRITE_REE1_LP_HUK_S)
#define LP_PERI_PMS_WRITE_REE1_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_HUK_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_HUK : R/W; bitpos: [7]; default: 0;
 *  Configures lp_huk registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_HUK    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_HUK_M  (LP_PERI_PMS_WRITE_REE2_LP_HUK_V << LP_PERI_PMS_WRITE_REE2_LP_HUK_S)
#define LP_PERI_PMS_WRITE_REE2_LP_HUK_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_HUK_S  7
/** LP_PERI_PMS_LP_HUK_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_huk peri_apm configuration
 */
#define LP_PERI_PMS_LP_HUK_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_HUK_LOCK_M  (LP_PERI_PMS_LP_HUK_LOCK_V << LP_PERI_PMS_LP_HUK_LOCK_S)
#define LP_PERI_PMS_LP_HUK_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_HUK_LOCK_S  8

/** LP_PERI_PMS_LP_PMU_CTRL_REG register
 *  lp_pmu read/write control register
 */
#define LP_PERI_PMS_LP_PMU_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x10)
/** LP_PERI_PMS_READ_TEE_LP_PMU : R/W; bitpos: [0]; default: 1;
 *  Configures lp_pmu registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_PMU    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_PMU_M  (LP_PERI_PMS_READ_TEE_LP_PMU_V << LP_PERI_PMS_READ_TEE_LP_PMU_S)
#define LP_PERI_PMS_READ_TEE_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_PMU_S  0
/** LP_PERI_PMS_READ_REE0_LP_PMU : R/W; bitpos: [1]; default: 0;
 *  Configures lp_pmu registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_PMU    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_PMU_M  (LP_PERI_PMS_READ_REE0_LP_PMU_V << LP_PERI_PMS_READ_REE0_LP_PMU_S)
#define LP_PERI_PMS_READ_REE0_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_PMU_S  1
/** LP_PERI_PMS_READ_REE1_LP_PMU : R/W; bitpos: [2]; default: 0;
 *  Configures lp_pmu registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_PMU    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_PMU_M  (LP_PERI_PMS_READ_REE1_LP_PMU_V << LP_PERI_PMS_READ_REE1_LP_PMU_S)
#define LP_PERI_PMS_READ_REE1_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_PMU_S  2
/** LP_PERI_PMS_READ_REE2_LP_PMU : R/W; bitpos: [3]; default: 0;
 *  Configures lp_pmu registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_PMU    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_PMU_M  (LP_PERI_PMS_READ_REE2_LP_PMU_V << LP_PERI_PMS_READ_REE2_LP_PMU_S)
#define LP_PERI_PMS_READ_REE2_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_PMU_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_PMU : R/W; bitpos: [4]; default: 1;
 *  Configures lp_pmu registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_PMU    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_PMU_M  (LP_PERI_PMS_WRITE_TEE_LP_PMU_V << LP_PERI_PMS_WRITE_TEE_LP_PMU_S)
#define LP_PERI_PMS_WRITE_TEE_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_PMU_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_PMU : R/W; bitpos: [5]; default: 0;
 *  Configures lp_pmu registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_PMU    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_PMU_M  (LP_PERI_PMS_WRITE_REE0_LP_PMU_V << LP_PERI_PMS_WRITE_REE0_LP_PMU_S)
#define LP_PERI_PMS_WRITE_REE0_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_PMU_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_PMU : R/W; bitpos: [6]; default: 0;
 *  Configures lp_pmu registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_PMU    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_PMU_M  (LP_PERI_PMS_WRITE_REE1_LP_PMU_V << LP_PERI_PMS_WRITE_REE1_LP_PMU_S)
#define LP_PERI_PMS_WRITE_REE1_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_PMU_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_PMU : R/W; bitpos: [7]; default: 0;
 *  Configures lp_pmu registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_PMU    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_PMU_M  (LP_PERI_PMS_WRITE_REE2_LP_PMU_V << LP_PERI_PMS_WRITE_REE2_LP_PMU_S)
#define LP_PERI_PMS_WRITE_REE2_LP_PMU_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_PMU_S  7
/** LP_PERI_PMS_LP_PMU_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_pmu peri_apm configuration
 */
#define LP_PERI_PMS_LP_PMU_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_PMU_LOCK_M  (LP_PERI_PMS_LP_PMU_LOCK_V << LP_PERI_PMS_LP_PMU_LOCK_S)
#define LP_PERI_PMS_LP_PMU_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_PMU_LOCK_S  8

/** LP_PERI_PMS_LP_TOUCH_AON_CTRL_REG register
 *  lp_touch_aon read/write control register
 */
#define LP_PERI_PMS_LP_TOUCH_AON_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x14)
/** LP_PERI_PMS_READ_TEE_LP_TOUCH_AON : R/W; bitpos: [0]; default: 1;
 *  Configures lp_touch_aon registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_TOUCH_AON    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_TOUCH_AON_M  (LP_PERI_PMS_READ_TEE_LP_TOUCH_AON_V << LP_PERI_PMS_READ_TEE_LP_TOUCH_AON_S)
#define LP_PERI_PMS_READ_TEE_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_TOUCH_AON_S  0
/** LP_PERI_PMS_READ_REE0_LP_TOUCH_AON : R/W; bitpos: [1]; default: 0;
 *  Configures lp_touch_aon registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_TOUCH_AON    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_TOUCH_AON_M  (LP_PERI_PMS_READ_REE0_LP_TOUCH_AON_V << LP_PERI_PMS_READ_REE0_LP_TOUCH_AON_S)
#define LP_PERI_PMS_READ_REE0_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_TOUCH_AON_S  1
/** LP_PERI_PMS_READ_REE1_LP_TOUCH_AON : R/W; bitpos: [2]; default: 0;
 *  Configures lp_touch_aon registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_TOUCH_AON    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_TOUCH_AON_M  (LP_PERI_PMS_READ_REE1_LP_TOUCH_AON_V << LP_PERI_PMS_READ_REE1_LP_TOUCH_AON_S)
#define LP_PERI_PMS_READ_REE1_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_TOUCH_AON_S  2
/** LP_PERI_PMS_READ_REE2_LP_TOUCH_AON : R/W; bitpos: [3]; default: 0;
 *  Configures lp_touch_aon registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_TOUCH_AON    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_TOUCH_AON_M  (LP_PERI_PMS_READ_REE2_LP_TOUCH_AON_V << LP_PERI_PMS_READ_REE2_LP_TOUCH_AON_S)
#define LP_PERI_PMS_READ_REE2_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_TOUCH_AON_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_TOUCH_AON : R/W; bitpos: [4]; default: 1;
 *  Configures lp_touch_aon registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH_AON    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH_AON_M  (LP_PERI_PMS_WRITE_TEE_LP_TOUCH_AON_V << LP_PERI_PMS_WRITE_TEE_LP_TOUCH_AON_S)
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH_AON_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_TOUCH_AON : R/W; bitpos: [5]; default: 0;
 *  Configures lp_touch_aon registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH_AON    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH_AON_M  (LP_PERI_PMS_WRITE_REE0_LP_TOUCH_AON_V << LP_PERI_PMS_WRITE_REE0_LP_TOUCH_AON_S)
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH_AON_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_TOUCH_AON : R/W; bitpos: [6]; default: 0;
 *  Configures lp_touch_aon registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH_AON    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH_AON_M  (LP_PERI_PMS_WRITE_REE1_LP_TOUCH_AON_V << LP_PERI_PMS_WRITE_REE1_LP_TOUCH_AON_S)
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH_AON_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_TOUCH_AON : R/W; bitpos: [7]; default: 0;
 *  Configures lp_touch_aon registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH_AON    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH_AON_M  (LP_PERI_PMS_WRITE_REE2_LP_TOUCH_AON_V << LP_PERI_PMS_WRITE_REE2_LP_TOUCH_AON_S)
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH_AON_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH_AON_S  7
/** LP_PERI_PMS_LP_TOUCH_AON_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_touch_aon peri_apm configuration
 */
#define LP_PERI_PMS_LP_TOUCH_AON_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_TOUCH_AON_LOCK_M  (LP_PERI_PMS_LP_TOUCH_AON_LOCK_V << LP_PERI_PMS_LP_TOUCH_AON_LOCK_S)
#define LP_PERI_PMS_LP_TOUCH_AON_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_TOUCH_AON_LOCK_S  8

/** LP_PERI_PMS_LP_PERI_PMS_CTRL_REG register
 *  lp_peri_pms read/write control register
 */
#define LP_PERI_PMS_LP_PERI_PMS_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x18)
/** LP_PERI_PMS_READ_TEE_LP_PERI_PMS : R/W; bitpos: [0]; default: 1;
 *  Configures lp_peri_pms registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_PERI_PMS    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_PERI_PMS_M  (LP_PERI_PMS_READ_TEE_LP_PERI_PMS_V << LP_PERI_PMS_READ_TEE_LP_PERI_PMS_S)
#define LP_PERI_PMS_READ_TEE_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_PERI_PMS_S  0
/** LP_PERI_PMS_READ_REE0_LP_PERI_PMS : HRO; bitpos: [1]; default: 0;
 *  Configures lp_peri_pms registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_PERI_PMS    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_PERI_PMS_M  (LP_PERI_PMS_READ_REE0_LP_PERI_PMS_V << LP_PERI_PMS_READ_REE0_LP_PERI_PMS_S)
#define LP_PERI_PMS_READ_REE0_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_PERI_PMS_S  1
/** LP_PERI_PMS_READ_REE1_LP_PERI_PMS : HRO; bitpos: [2]; default: 0;
 *  Configures lp_peri_pms registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_PERI_PMS    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_PERI_PMS_M  (LP_PERI_PMS_READ_REE1_LP_PERI_PMS_V << LP_PERI_PMS_READ_REE1_LP_PERI_PMS_S)
#define LP_PERI_PMS_READ_REE1_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_PERI_PMS_S  2
/** LP_PERI_PMS_READ_REE2_LP_PERI_PMS : HRO; bitpos: [3]; default: 0;
 *  Configures lp_peri_pms registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_PERI_PMS    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_PERI_PMS_M  (LP_PERI_PMS_READ_REE2_LP_PERI_PMS_V << LP_PERI_PMS_READ_REE2_LP_PERI_PMS_S)
#define LP_PERI_PMS_READ_REE2_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_PERI_PMS_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_PERI_PMS : R/W; bitpos: [4]; default: 1;
 *  Configures lp_peri_pms registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_PERI_PMS    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_PERI_PMS_M  (LP_PERI_PMS_WRITE_TEE_LP_PERI_PMS_V << LP_PERI_PMS_WRITE_TEE_LP_PERI_PMS_S)
#define LP_PERI_PMS_WRITE_TEE_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_PERI_PMS_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_PERI_PMS : HRO; bitpos: [5]; default: 0;
 *  Configures lp_peri_pms registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_PERI_PMS    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_PERI_PMS_M  (LP_PERI_PMS_WRITE_REE0_LP_PERI_PMS_V << LP_PERI_PMS_WRITE_REE0_LP_PERI_PMS_S)
#define LP_PERI_PMS_WRITE_REE0_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_PERI_PMS_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_PERI_PMS : HRO; bitpos: [6]; default: 0;
 *  Configures lp_peri_pms registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_PERI_PMS    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_PERI_PMS_M  (LP_PERI_PMS_WRITE_REE1_LP_PERI_PMS_V << LP_PERI_PMS_WRITE_REE1_LP_PERI_PMS_S)
#define LP_PERI_PMS_WRITE_REE1_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_PERI_PMS_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_PERI_PMS : HRO; bitpos: [7]; default: 0;
 *  Configures lp_peri_pms registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_PERI_PMS    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_PERI_PMS_M  (LP_PERI_PMS_WRITE_REE2_LP_PERI_PMS_V << LP_PERI_PMS_WRITE_REE2_LP_PERI_PMS_S)
#define LP_PERI_PMS_WRITE_REE2_LP_PERI_PMS_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_PERI_PMS_S  7
/** LP_PERI_PMS_LP_PERI_PMS_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_peri_pms peri_apm configuration
 */
#define LP_PERI_PMS_LP_PERI_PMS_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_PERI_PMS_LOCK_M  (LP_PERI_PMS_LP_PERI_PMS_LOCK_V << LP_PERI_PMS_LP_PERI_PMS_LOCK_S)
#define LP_PERI_PMS_LP_PERI_PMS_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_PERI_PMS_LOCK_S  8

/** LP_PERI_PMS_LP_TEE_CTRL_REG register
 *  lp_tee read/write control register
 */
#define LP_PERI_PMS_LP_TEE_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x1c)
/** LP_PERI_PMS_READ_TEE_LP_TEE : R/W; bitpos: [0]; default: 1;
 *  Configures lp_tee registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_TEE    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_TEE_M  (LP_PERI_PMS_READ_TEE_LP_TEE_V << LP_PERI_PMS_READ_TEE_LP_TEE_S)
#define LP_PERI_PMS_READ_TEE_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_TEE_S  0
/** LP_PERI_PMS_READ_REE0_LP_TEE : HRO; bitpos: [1]; default: 0;
 *  Configures lp_tee registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_TEE    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_TEE_M  (LP_PERI_PMS_READ_REE0_LP_TEE_V << LP_PERI_PMS_READ_REE0_LP_TEE_S)
#define LP_PERI_PMS_READ_REE0_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_TEE_S  1
/** LP_PERI_PMS_READ_REE1_LP_TEE : HRO; bitpos: [2]; default: 0;
 *  Configures lp_tee registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_TEE    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_TEE_M  (LP_PERI_PMS_READ_REE1_LP_TEE_V << LP_PERI_PMS_READ_REE1_LP_TEE_S)
#define LP_PERI_PMS_READ_REE1_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_TEE_S  2
/** LP_PERI_PMS_READ_REE2_LP_TEE : HRO; bitpos: [3]; default: 0;
 *  Configures lp_tee registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_TEE    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_TEE_M  (LP_PERI_PMS_READ_REE2_LP_TEE_V << LP_PERI_PMS_READ_REE2_LP_TEE_S)
#define LP_PERI_PMS_READ_REE2_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_TEE_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_TEE : R/W; bitpos: [4]; default: 1;
 *  Configures lp_tee registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_TEE    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_TEE_M  (LP_PERI_PMS_WRITE_TEE_LP_TEE_V << LP_PERI_PMS_WRITE_TEE_LP_TEE_S)
#define LP_PERI_PMS_WRITE_TEE_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_TEE_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_TEE : HRO; bitpos: [5]; default: 0;
 *  Configures lp_tee registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_TEE    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_TEE_M  (LP_PERI_PMS_WRITE_REE0_LP_TEE_V << LP_PERI_PMS_WRITE_REE0_LP_TEE_S)
#define LP_PERI_PMS_WRITE_REE0_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_TEE_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_TEE : HRO; bitpos: [6]; default: 0;
 *  Configures lp_tee registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_TEE    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_TEE_M  (LP_PERI_PMS_WRITE_REE1_LP_TEE_V << LP_PERI_PMS_WRITE_REE1_LP_TEE_S)
#define LP_PERI_PMS_WRITE_REE1_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_TEE_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_TEE : HRO; bitpos: [7]; default: 0;
 *  Configures lp_tee registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_TEE    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_TEE_M  (LP_PERI_PMS_WRITE_REE2_LP_TEE_V << LP_PERI_PMS_WRITE_REE2_LP_TEE_S)
#define LP_PERI_PMS_WRITE_REE2_LP_TEE_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_TEE_S  7
/** LP_PERI_PMS_LP_TEE_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_tee peri_apm configuration
 */
#define LP_PERI_PMS_LP_TEE_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_TEE_LOCK_M  (LP_PERI_PMS_LP_TEE_LOCK_V << LP_PERI_PMS_LP_TEE_LOCK_S)
#define LP_PERI_PMS_LP_TEE_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_TEE_LOCK_S  8

/** LP_PERI_PMS_LP_APM_CTRL_REG register
 *  lp_apm read/write control register
 */
#define LP_PERI_PMS_LP_APM_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x20)
/** LP_PERI_PMS_READ_TEE_LP_APM : R/W; bitpos: [0]; default: 1;
 *  Configures lp_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_APM    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_APM_M  (LP_PERI_PMS_READ_TEE_LP_APM_V << LP_PERI_PMS_READ_TEE_LP_APM_S)
#define LP_PERI_PMS_READ_TEE_LP_APM_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_APM_S  0
/** LP_PERI_PMS_READ_REE0_LP_APM : HRO; bitpos: [1]; default: 0;
 *  Configures lp_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_APM    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_APM_M  (LP_PERI_PMS_READ_REE0_LP_APM_V << LP_PERI_PMS_READ_REE0_LP_APM_S)
#define LP_PERI_PMS_READ_REE0_LP_APM_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_APM_S  1
/** LP_PERI_PMS_READ_REE1_LP_APM : HRO; bitpos: [2]; default: 0;
 *  Configures lp_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_APM    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_APM_M  (LP_PERI_PMS_READ_REE1_LP_APM_V << LP_PERI_PMS_READ_REE1_LP_APM_S)
#define LP_PERI_PMS_READ_REE1_LP_APM_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_APM_S  2
/** LP_PERI_PMS_READ_REE2_LP_APM : HRO; bitpos: [3]; default: 0;
 *  Configures lp_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_APM    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_APM_M  (LP_PERI_PMS_READ_REE2_LP_APM_V << LP_PERI_PMS_READ_REE2_LP_APM_S)
#define LP_PERI_PMS_READ_REE2_LP_APM_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_APM_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_APM : R/W; bitpos: [4]; default: 1;
 *  Configures lp_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_APM    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_APM_M  (LP_PERI_PMS_WRITE_TEE_LP_APM_V << LP_PERI_PMS_WRITE_TEE_LP_APM_S)
#define LP_PERI_PMS_WRITE_TEE_LP_APM_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_APM_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_APM : HRO; bitpos: [5]; default: 0;
 *  Configures lp_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_APM    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_APM_M  (LP_PERI_PMS_WRITE_REE0_LP_APM_V << LP_PERI_PMS_WRITE_REE0_LP_APM_S)
#define LP_PERI_PMS_WRITE_REE0_LP_APM_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_APM_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_APM : HRO; bitpos: [6]; default: 0;
 *  Configures lp_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_APM    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_APM_M  (LP_PERI_PMS_WRITE_REE1_LP_APM_V << LP_PERI_PMS_WRITE_REE1_LP_APM_S)
#define LP_PERI_PMS_WRITE_REE1_LP_APM_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_APM_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_APM : HRO; bitpos: [7]; default: 0;
 *  Configures lp_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_APM    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_APM_M  (LP_PERI_PMS_WRITE_REE2_LP_APM_V << LP_PERI_PMS_WRITE_REE2_LP_APM_S)
#define LP_PERI_PMS_WRITE_REE2_LP_APM_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_APM_S  7
/** LP_PERI_PMS_LP_APM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_apm peri_apm configuration
 */
#define LP_PERI_PMS_LP_APM_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_APM_LOCK_M  (LP_PERI_PMS_LP_APM_LOCK_V << LP_PERI_PMS_LP_APM_LOCK_S)
#define LP_PERI_PMS_LP_APM_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_APM_LOCK_S  8

/** LP_PERI_PMS_LP_TIMER_CTRL_REG register
 *  lp_timer read/write control register
 */
#define LP_PERI_PMS_LP_TIMER_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x24)
/** LP_PERI_PMS_READ_TEE_LP_TIMER : R/W; bitpos: [0]; default: 1;
 *  Configures lp_timer registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_TIMER    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_TIMER_M  (LP_PERI_PMS_READ_TEE_LP_TIMER_V << LP_PERI_PMS_READ_TEE_LP_TIMER_S)
#define LP_PERI_PMS_READ_TEE_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_TIMER_S  0
/** LP_PERI_PMS_READ_REE0_LP_TIMER : R/W; bitpos: [1]; default: 0;
 *  Configures lp_timer registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_TIMER    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_TIMER_M  (LP_PERI_PMS_READ_REE0_LP_TIMER_V << LP_PERI_PMS_READ_REE0_LP_TIMER_S)
#define LP_PERI_PMS_READ_REE0_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_TIMER_S  1
/** LP_PERI_PMS_READ_REE1_LP_TIMER : R/W; bitpos: [2]; default: 0;
 *  Configures lp_timer registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_TIMER    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_TIMER_M  (LP_PERI_PMS_READ_REE1_LP_TIMER_V << LP_PERI_PMS_READ_REE1_LP_TIMER_S)
#define LP_PERI_PMS_READ_REE1_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_TIMER_S  2
/** LP_PERI_PMS_READ_REE2_LP_TIMER : R/W; bitpos: [3]; default: 0;
 *  Configures lp_timer registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_TIMER    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_TIMER_M  (LP_PERI_PMS_READ_REE2_LP_TIMER_V << LP_PERI_PMS_READ_REE2_LP_TIMER_S)
#define LP_PERI_PMS_READ_REE2_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_TIMER_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_TIMER : R/W; bitpos: [4]; default: 1;
 *  Configures lp_timer registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_TIMER    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_TIMER_M  (LP_PERI_PMS_WRITE_TEE_LP_TIMER_V << LP_PERI_PMS_WRITE_TEE_LP_TIMER_S)
#define LP_PERI_PMS_WRITE_TEE_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_TIMER_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_TIMER : R/W; bitpos: [5]; default: 0;
 *  Configures lp_timer registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_TIMER    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_TIMER_M  (LP_PERI_PMS_WRITE_REE0_LP_TIMER_V << LP_PERI_PMS_WRITE_REE0_LP_TIMER_S)
#define LP_PERI_PMS_WRITE_REE0_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_TIMER_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_TIMER : R/W; bitpos: [6]; default: 0;
 *  Configures lp_timer registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_TIMER    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_TIMER_M  (LP_PERI_PMS_WRITE_REE1_LP_TIMER_V << LP_PERI_PMS_WRITE_REE1_LP_TIMER_S)
#define LP_PERI_PMS_WRITE_REE1_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_TIMER_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_TIMER : R/W; bitpos: [7]; default: 0;
 *  Configures lp_timer registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_TIMER    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_TIMER_M  (LP_PERI_PMS_WRITE_REE2_LP_TIMER_V << LP_PERI_PMS_WRITE_REE2_LP_TIMER_S)
#define LP_PERI_PMS_WRITE_REE2_LP_TIMER_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_TIMER_S  7
/** LP_PERI_PMS_LP_TIMER_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_timer peri_apm configuration
 */
#define LP_PERI_PMS_LP_TIMER_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_TIMER_LOCK_M  (LP_PERI_PMS_LP_TIMER_LOCK_V << LP_PERI_PMS_LP_TIMER_LOCK_S)
#define LP_PERI_PMS_LP_TIMER_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_TIMER_LOCK_S  8

/** LP_PERI_PMS_LP_WDT_CTRL_REG register
 *  lp_wdt read/write control register
 */
#define LP_PERI_PMS_LP_WDT_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x28)
/** LP_PERI_PMS_READ_TEE_LP_WDT : R/W; bitpos: [0]; default: 1;
 *  Configures lp_wdt registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_WDT    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_WDT_M  (LP_PERI_PMS_READ_TEE_LP_WDT_V << LP_PERI_PMS_READ_TEE_LP_WDT_S)
#define LP_PERI_PMS_READ_TEE_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_WDT_S  0
/** LP_PERI_PMS_READ_REE0_LP_WDT : R/W; bitpos: [1]; default: 0;
 *  Configures lp_wdt registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_WDT    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_WDT_M  (LP_PERI_PMS_READ_REE0_LP_WDT_V << LP_PERI_PMS_READ_REE0_LP_WDT_S)
#define LP_PERI_PMS_READ_REE0_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_WDT_S  1
/** LP_PERI_PMS_READ_REE1_LP_WDT : R/W; bitpos: [2]; default: 0;
 *  Configures lp_wdt registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_WDT    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_WDT_M  (LP_PERI_PMS_READ_REE1_LP_WDT_V << LP_PERI_PMS_READ_REE1_LP_WDT_S)
#define LP_PERI_PMS_READ_REE1_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_WDT_S  2
/** LP_PERI_PMS_READ_REE2_LP_WDT : R/W; bitpos: [3]; default: 0;
 *  Configures lp_wdt registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_WDT    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_WDT_M  (LP_PERI_PMS_READ_REE2_LP_WDT_V << LP_PERI_PMS_READ_REE2_LP_WDT_S)
#define LP_PERI_PMS_READ_REE2_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_WDT_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_WDT : R/W; bitpos: [4]; default: 1;
 *  Configures lp_wdt registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_WDT    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_WDT_M  (LP_PERI_PMS_WRITE_TEE_LP_WDT_V << LP_PERI_PMS_WRITE_TEE_LP_WDT_S)
#define LP_PERI_PMS_WRITE_TEE_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_WDT_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_WDT : R/W; bitpos: [5]; default: 0;
 *  Configures lp_wdt registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_WDT    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_WDT_M  (LP_PERI_PMS_WRITE_REE0_LP_WDT_V << LP_PERI_PMS_WRITE_REE0_LP_WDT_S)
#define LP_PERI_PMS_WRITE_REE0_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_WDT_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_WDT : R/W; bitpos: [6]; default: 0;
 *  Configures lp_wdt registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_WDT    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_WDT_M  (LP_PERI_PMS_WRITE_REE1_LP_WDT_V << LP_PERI_PMS_WRITE_REE1_LP_WDT_S)
#define LP_PERI_PMS_WRITE_REE1_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_WDT_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_WDT : R/W; bitpos: [7]; default: 0;
 *  Configures lp_wdt registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_WDT    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_WDT_M  (LP_PERI_PMS_WRITE_REE2_LP_WDT_V << LP_PERI_PMS_WRITE_REE2_LP_WDT_S)
#define LP_PERI_PMS_WRITE_REE2_LP_WDT_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_WDT_S  7
/** LP_PERI_PMS_LP_WDT_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_wdt peri_apm configuration
 */
#define LP_PERI_PMS_LP_WDT_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_WDT_LOCK_M  (LP_PERI_PMS_LP_WDT_LOCK_V << LP_PERI_PMS_LP_WDT_LOCK_S)
#define LP_PERI_PMS_LP_WDT_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_WDT_LOCK_S  8

/** LP_PERI_PMS_LP_CLK_CALI_SOSC_CTRL_REG register
 *  lp_clk_cali_sosc read/write control register
 */
#define LP_PERI_PMS_LP_CLK_CALI_SOSC_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x2c)
/** LP_PERI_PMS_READ_TEE_LP_CLK_CALI_SOSC : R/W; bitpos: [0]; default: 1;
 *  Configures lp_clk_cali_sosc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_SOSC    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_READ_TEE_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_READ_TEE_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_SOSC_S  0
/** LP_PERI_PMS_READ_REE0_LP_CLK_CALI_SOSC : R/W; bitpos: [1]; default: 0;
 *  Configures lp_clk_cali_sosc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_SOSC    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_READ_REE0_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_READ_REE0_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_SOSC_S  1
/** LP_PERI_PMS_READ_REE1_LP_CLK_CALI_SOSC : R/W; bitpos: [2]; default: 0;
 *  Configures lp_clk_cali_sosc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_SOSC    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_READ_REE1_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_READ_REE1_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_SOSC_S  2
/** LP_PERI_PMS_READ_REE2_LP_CLK_CALI_SOSC : R/W; bitpos: [3]; default: 0;
 *  Configures lp_clk_cali_sosc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_SOSC    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_READ_REE2_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_READ_REE2_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_SOSC_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_SOSC : R/W; bitpos: [4]; default: 1;
 *  Configures lp_clk_cali_sosc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_SOSC    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_SOSC_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_SOSC : R/W; bitpos: [5]; default: 0;
 *  Configures lp_clk_cali_sosc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_SOSC    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_SOSC_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_SOSC : R/W; bitpos: [6]; default: 0;
 *  Configures lp_clk_cali_sosc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_SOSC    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_SOSC_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_SOSC : R/W; bitpos: [7]; default: 0;
 *  Configures lp_clk_cali_sosc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_SOSC    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_SOSC_M  (LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_SOSC_V << LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_SOSC_S)
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_SOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_SOSC_S  7
/** LP_PERI_PMS_LP_CLK_CALI_SOSC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_clk_cali_sosc peri_apm configuration
 */
#define LP_PERI_PMS_LP_CLK_CALI_SOSC_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_CLK_CALI_SOSC_LOCK_M  (LP_PERI_PMS_LP_CLK_CALI_SOSC_LOCK_V << LP_PERI_PMS_LP_CLK_CALI_SOSC_LOCK_S)
#define LP_PERI_PMS_LP_CLK_CALI_SOSC_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_CLK_CALI_SOSC_LOCK_S  8

/** LP_PERI_PMS_LP_CLK_CALI_FOSC_CTRL_REG register
 *  lp_clk_cali_fosc read/write control register
 */
#define LP_PERI_PMS_LP_CLK_CALI_FOSC_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x30)
/** LP_PERI_PMS_READ_TEE_LP_CLK_CALI_FOSC : R/W; bitpos: [0]; default: 1;
 *  Configures lp_clk_cali_fosc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_FOSC    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_READ_TEE_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_READ_TEE_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_CLK_CALI_FOSC_S  0
/** LP_PERI_PMS_READ_REE0_LP_CLK_CALI_FOSC : R/W; bitpos: [1]; default: 0;
 *  Configures lp_clk_cali_fosc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_FOSC    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_READ_REE0_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_READ_REE0_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_CLK_CALI_FOSC_S  1
/** LP_PERI_PMS_READ_REE1_LP_CLK_CALI_FOSC : R/W; bitpos: [2]; default: 0;
 *  Configures lp_clk_cali_fosc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_FOSC    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_READ_REE1_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_READ_REE1_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_CLK_CALI_FOSC_S  2
/** LP_PERI_PMS_READ_REE2_LP_CLK_CALI_FOSC : R/W; bitpos: [3]; default: 0;
 *  Configures lp_clk_cali_fosc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_FOSC    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_READ_REE2_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_READ_REE2_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_CLK_CALI_FOSC_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_FOSC : R/W; bitpos: [4]; default: 1;
 *  Configures lp_clk_cali_fosc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_FOSC    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_CLK_CALI_FOSC_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_FOSC : R/W; bitpos: [5]; default: 0;
 *  Configures lp_clk_cali_fosc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_FOSC    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_CLK_CALI_FOSC_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_FOSC : R/W; bitpos: [6]; default: 0;
 *  Configures lp_clk_cali_fosc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_FOSC    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_CLK_CALI_FOSC_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_FOSC : R/W; bitpos: [7]; default: 0;
 *  Configures lp_clk_cali_fosc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_FOSC    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_FOSC_M  (LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_FOSC_V << LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_FOSC_S)
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_FOSC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_CLK_CALI_FOSC_S  7
/** LP_PERI_PMS_LP_CLK_CALI_FOSC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_clk_cali_fosc peri_apm configuration
 */
#define LP_PERI_PMS_LP_CLK_CALI_FOSC_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_CLK_CALI_FOSC_LOCK_M  (LP_PERI_PMS_LP_CLK_CALI_FOSC_LOCK_V << LP_PERI_PMS_LP_CLK_CALI_FOSC_LOCK_S)
#define LP_PERI_PMS_LP_CLK_CALI_FOSC_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_CLK_CALI_FOSC_LOCK_S  8

/** LP_PERI_PMS_LP_PWR_REG_CTRL_REG register
 *  lp_pwr_reg read/write control register
 */
#define LP_PERI_PMS_LP_PWR_REG_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x34)
/** LP_PERI_PMS_READ_TEE_LP_PWR_REG : R/W; bitpos: [0]; default: 1;
 *  Configures lp_pwr_reg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_PWR_REG    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_PWR_REG_M  (LP_PERI_PMS_READ_TEE_LP_PWR_REG_V << LP_PERI_PMS_READ_TEE_LP_PWR_REG_S)
#define LP_PERI_PMS_READ_TEE_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_PWR_REG_S  0
/** LP_PERI_PMS_READ_REE0_LP_PWR_REG : R/W; bitpos: [1]; default: 0;
 *  Configures lp_pwr_reg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_PWR_REG    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_PWR_REG_M  (LP_PERI_PMS_READ_REE0_LP_PWR_REG_V << LP_PERI_PMS_READ_REE0_LP_PWR_REG_S)
#define LP_PERI_PMS_READ_REE0_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_PWR_REG_S  1
/** LP_PERI_PMS_READ_REE1_LP_PWR_REG : R/W; bitpos: [2]; default: 0;
 *  Configures lp_pwr_reg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_PWR_REG    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_PWR_REG_M  (LP_PERI_PMS_READ_REE1_LP_PWR_REG_V << LP_PERI_PMS_READ_REE1_LP_PWR_REG_S)
#define LP_PERI_PMS_READ_REE1_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_PWR_REG_S  2
/** LP_PERI_PMS_READ_REE2_LP_PWR_REG : R/W; bitpos: [3]; default: 0;
 *  Configures lp_pwr_reg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_PWR_REG    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_PWR_REG_M  (LP_PERI_PMS_READ_REE2_LP_PWR_REG_V << LP_PERI_PMS_READ_REE2_LP_PWR_REG_S)
#define LP_PERI_PMS_READ_REE2_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_PWR_REG_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_PWR_REG : R/W; bitpos: [4]; default: 1;
 *  Configures lp_pwr_reg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_PWR_REG    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_PWR_REG_M  (LP_PERI_PMS_WRITE_TEE_LP_PWR_REG_V << LP_PERI_PMS_WRITE_TEE_LP_PWR_REG_S)
#define LP_PERI_PMS_WRITE_TEE_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_PWR_REG_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_PWR_REG : R/W; bitpos: [5]; default: 0;
 *  Configures lp_pwr_reg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_PWR_REG    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_PWR_REG_M  (LP_PERI_PMS_WRITE_REE0_LP_PWR_REG_V << LP_PERI_PMS_WRITE_REE0_LP_PWR_REG_S)
#define LP_PERI_PMS_WRITE_REE0_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_PWR_REG_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_PWR_REG : R/W; bitpos: [6]; default: 0;
 *  Configures lp_pwr_reg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_PWR_REG    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_PWR_REG_M  (LP_PERI_PMS_WRITE_REE1_LP_PWR_REG_V << LP_PERI_PMS_WRITE_REE1_LP_PWR_REG_S)
#define LP_PERI_PMS_WRITE_REE1_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_PWR_REG_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_PWR_REG : R/W; bitpos: [7]; default: 0;
 *  Configures lp_pwr_reg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_PWR_REG    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_PWR_REG_M  (LP_PERI_PMS_WRITE_REE2_LP_PWR_REG_V << LP_PERI_PMS_WRITE_REE2_LP_PWR_REG_S)
#define LP_PERI_PMS_WRITE_REE2_LP_PWR_REG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_PWR_REG_S  7
/** LP_PERI_PMS_LP_PWR_REG_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_pwr_reg peri_apm configuration
 */
#define LP_PERI_PMS_LP_PWR_REG_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_PWR_REG_LOCK_M  (LP_PERI_PMS_LP_PWR_REG_LOCK_V << LP_PERI_PMS_LP_PWR_REG_LOCK_S)
#define LP_PERI_PMS_LP_PWR_REG_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_PWR_REG_LOCK_S  8

/** LP_PERI_PMS_LP_PERICLKRST_CTRL_REG register
 *  lp_periclkrst read/write control register
 */
#define LP_PERI_PMS_LP_PERICLKRST_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x38)
/** LP_PERI_PMS_READ_TEE_LP_PERICLKRST : R/W; bitpos: [0]; default: 1;
 *  Configures lp_periclkrst registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_PERICLKRST    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_PERICLKRST_M  (LP_PERI_PMS_READ_TEE_LP_PERICLKRST_V << LP_PERI_PMS_READ_TEE_LP_PERICLKRST_S)
#define LP_PERI_PMS_READ_TEE_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_PERICLKRST_S  0
/** LP_PERI_PMS_READ_REE0_LP_PERICLKRST : R/W; bitpos: [1]; default: 0;
 *  Configures lp_periclkrst registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_PERICLKRST    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_PERICLKRST_M  (LP_PERI_PMS_READ_REE0_LP_PERICLKRST_V << LP_PERI_PMS_READ_REE0_LP_PERICLKRST_S)
#define LP_PERI_PMS_READ_REE0_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_PERICLKRST_S  1
/** LP_PERI_PMS_READ_REE1_LP_PERICLKRST : R/W; bitpos: [2]; default: 0;
 *  Configures lp_periclkrst registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_PERICLKRST    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_PERICLKRST_M  (LP_PERI_PMS_READ_REE1_LP_PERICLKRST_V << LP_PERI_PMS_READ_REE1_LP_PERICLKRST_S)
#define LP_PERI_PMS_READ_REE1_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_PERICLKRST_S  2
/** LP_PERI_PMS_READ_REE2_LP_PERICLKRST : R/W; bitpos: [3]; default: 0;
 *  Configures lp_periclkrst registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_PERICLKRST    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_PERICLKRST_M  (LP_PERI_PMS_READ_REE2_LP_PERICLKRST_V << LP_PERI_PMS_READ_REE2_LP_PERICLKRST_S)
#define LP_PERI_PMS_READ_REE2_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_PERICLKRST_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_PERICLKRST : R/W; bitpos: [4]; default: 1;
 *  Configures lp_periclkrst registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_PERICLKRST    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_PERICLKRST_M  (LP_PERI_PMS_WRITE_TEE_LP_PERICLKRST_V << LP_PERI_PMS_WRITE_TEE_LP_PERICLKRST_S)
#define LP_PERI_PMS_WRITE_TEE_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_PERICLKRST_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_PERICLKRST : R/W; bitpos: [5]; default: 0;
 *  Configures lp_periclkrst registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_PERICLKRST    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_PERICLKRST_M  (LP_PERI_PMS_WRITE_REE0_LP_PERICLKRST_V << LP_PERI_PMS_WRITE_REE0_LP_PERICLKRST_S)
#define LP_PERI_PMS_WRITE_REE0_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_PERICLKRST_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_PERICLKRST : R/W; bitpos: [6]; default: 0;
 *  Configures lp_periclkrst registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_PERICLKRST    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_PERICLKRST_M  (LP_PERI_PMS_WRITE_REE1_LP_PERICLKRST_V << LP_PERI_PMS_WRITE_REE1_LP_PERICLKRST_S)
#define LP_PERI_PMS_WRITE_REE1_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_PERICLKRST_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_PERICLKRST : R/W; bitpos: [7]; default: 0;
 *  Configures lp_periclkrst registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_PERICLKRST    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_PERICLKRST_M  (LP_PERI_PMS_WRITE_REE2_LP_PERICLKRST_V << LP_PERI_PMS_WRITE_REE2_LP_PERICLKRST_S)
#define LP_PERI_PMS_WRITE_REE2_LP_PERICLKRST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_PERICLKRST_S  7
/** LP_PERI_PMS_LP_PERICLKRST_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_periclkrst peri_apm configuration
 */
#define LP_PERI_PMS_LP_PERICLKRST_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_PERICLKRST_LOCK_M  (LP_PERI_PMS_LP_PERICLKRST_LOCK_V << LP_PERI_PMS_LP_PERICLKRST_LOCK_S)
#define LP_PERI_PMS_LP_PERICLKRST_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_PERICLKRST_LOCK_S  8

/** LP_PERI_PMS_LP_IOMUX_CTRL_REG register
 *  lp_iomux read/write control register
 */
#define LP_PERI_PMS_LP_IOMUX_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x3c)
/** LP_PERI_PMS_READ_TEE_LP_IOMUX : R/W; bitpos: [0]; default: 1;
 *  Configures lp_iomux registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_IOMUX    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_IOMUX_M  (LP_PERI_PMS_READ_TEE_LP_IOMUX_V << LP_PERI_PMS_READ_TEE_LP_IOMUX_S)
#define LP_PERI_PMS_READ_TEE_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_IOMUX_S  0
/** LP_PERI_PMS_READ_REE0_LP_IOMUX : R/W; bitpos: [1]; default: 0;
 *  Configures lp_iomux registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_IOMUX    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_IOMUX_M  (LP_PERI_PMS_READ_REE0_LP_IOMUX_V << LP_PERI_PMS_READ_REE0_LP_IOMUX_S)
#define LP_PERI_PMS_READ_REE0_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_IOMUX_S  1
/** LP_PERI_PMS_READ_REE1_LP_IOMUX : R/W; bitpos: [2]; default: 0;
 *  Configures lp_iomux registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_IOMUX    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_IOMUX_M  (LP_PERI_PMS_READ_REE1_LP_IOMUX_V << LP_PERI_PMS_READ_REE1_LP_IOMUX_S)
#define LP_PERI_PMS_READ_REE1_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_IOMUX_S  2
/** LP_PERI_PMS_READ_REE2_LP_IOMUX : R/W; bitpos: [3]; default: 0;
 *  Configures lp_iomux registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_IOMUX    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_IOMUX_M  (LP_PERI_PMS_READ_REE2_LP_IOMUX_V << LP_PERI_PMS_READ_REE2_LP_IOMUX_S)
#define LP_PERI_PMS_READ_REE2_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_IOMUX_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_IOMUX : R/W; bitpos: [4]; default: 1;
 *  Configures lp_iomux registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_IOMUX    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_IOMUX_M  (LP_PERI_PMS_WRITE_TEE_LP_IOMUX_V << LP_PERI_PMS_WRITE_TEE_LP_IOMUX_S)
#define LP_PERI_PMS_WRITE_TEE_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_IOMUX_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_IOMUX : R/W; bitpos: [5]; default: 0;
 *  Configures lp_iomux registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_IOMUX    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_IOMUX_M  (LP_PERI_PMS_WRITE_REE0_LP_IOMUX_V << LP_PERI_PMS_WRITE_REE0_LP_IOMUX_S)
#define LP_PERI_PMS_WRITE_REE0_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_IOMUX_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_IOMUX : R/W; bitpos: [6]; default: 0;
 *  Configures lp_iomux registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_IOMUX    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_IOMUX_M  (LP_PERI_PMS_WRITE_REE1_LP_IOMUX_V << LP_PERI_PMS_WRITE_REE1_LP_IOMUX_S)
#define LP_PERI_PMS_WRITE_REE1_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_IOMUX_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_IOMUX : R/W; bitpos: [7]; default: 0;
 *  Configures lp_iomux registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_IOMUX    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_IOMUX_M  (LP_PERI_PMS_WRITE_REE2_LP_IOMUX_V << LP_PERI_PMS_WRITE_REE2_LP_IOMUX_S)
#define LP_PERI_PMS_WRITE_REE2_LP_IOMUX_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_IOMUX_S  7
/** LP_PERI_PMS_LP_IOMUX_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_iomux peri_apm configuration
 */
#define LP_PERI_PMS_LP_IOMUX_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_IOMUX_LOCK_M  (LP_PERI_PMS_LP_IOMUX_LOCK_V << LP_PERI_PMS_LP_IOMUX_LOCK_S)
#define LP_PERI_PMS_LP_IOMUX_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_IOMUX_LOCK_S  8

/** LP_PERI_PMS_LP_INTR_CTRL_REG register
 *  lp_intr read/write control register
 */
#define LP_PERI_PMS_LP_INTR_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x40)
/** LP_PERI_PMS_READ_TEE_LP_INTR : R/W; bitpos: [0]; default: 1;
 *  Configures lp_intr registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_INTR    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_INTR_M  (LP_PERI_PMS_READ_TEE_LP_INTR_V << LP_PERI_PMS_READ_TEE_LP_INTR_S)
#define LP_PERI_PMS_READ_TEE_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_INTR_S  0
/** LP_PERI_PMS_READ_REE0_LP_INTR : R/W; bitpos: [1]; default: 0;
 *  Configures lp_intr registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_INTR    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_INTR_M  (LP_PERI_PMS_READ_REE0_LP_INTR_V << LP_PERI_PMS_READ_REE0_LP_INTR_S)
#define LP_PERI_PMS_READ_REE0_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_INTR_S  1
/** LP_PERI_PMS_READ_REE1_LP_INTR : R/W; bitpos: [2]; default: 0;
 *  Configures lp_intr registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_INTR    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_INTR_M  (LP_PERI_PMS_READ_REE1_LP_INTR_V << LP_PERI_PMS_READ_REE1_LP_INTR_S)
#define LP_PERI_PMS_READ_REE1_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_INTR_S  2
/** LP_PERI_PMS_READ_REE2_LP_INTR : R/W; bitpos: [3]; default: 0;
 *  Configures lp_intr registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_INTR    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_INTR_M  (LP_PERI_PMS_READ_REE2_LP_INTR_V << LP_PERI_PMS_READ_REE2_LP_INTR_S)
#define LP_PERI_PMS_READ_REE2_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_INTR_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_INTR : R/W; bitpos: [4]; default: 1;
 *  Configures lp_intr registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_INTR    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_INTR_M  (LP_PERI_PMS_WRITE_TEE_LP_INTR_V << LP_PERI_PMS_WRITE_TEE_LP_INTR_S)
#define LP_PERI_PMS_WRITE_TEE_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_INTR_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_INTR : R/W; bitpos: [5]; default: 0;
 *  Configures lp_intr registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_INTR    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_INTR_M  (LP_PERI_PMS_WRITE_REE0_LP_INTR_V << LP_PERI_PMS_WRITE_REE0_LP_INTR_S)
#define LP_PERI_PMS_WRITE_REE0_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_INTR_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_INTR : R/W; bitpos: [6]; default: 0;
 *  Configures lp_intr registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_INTR    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_INTR_M  (LP_PERI_PMS_WRITE_REE1_LP_INTR_V << LP_PERI_PMS_WRITE_REE1_LP_INTR_S)
#define LP_PERI_PMS_WRITE_REE1_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_INTR_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_INTR : R/W; bitpos: [7]; default: 0;
 *  Configures lp_intr registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_INTR    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_INTR_M  (LP_PERI_PMS_WRITE_REE2_LP_INTR_V << LP_PERI_PMS_WRITE_REE2_LP_INTR_S)
#define LP_PERI_PMS_WRITE_REE2_LP_INTR_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_INTR_S  7
/** LP_PERI_PMS_LP_INTR_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_intr peri_apm configuration
 */
#define LP_PERI_PMS_LP_INTR_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_INTR_LOCK_M  (LP_PERI_PMS_LP_INTR_LOCK_V << LP_PERI_PMS_LP_INTR_LOCK_S)
#define LP_PERI_PMS_LP_INTR_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_INTR_LOCK_S  8

/** LP_PERI_PMS_LP_EFUSE_CTRL_REG register
 *  lp_efuse read/write control register
 */
#define LP_PERI_PMS_LP_EFUSE_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x44)
/** LP_PERI_PMS_READ_TEE_LP_EFUSE : R/W; bitpos: [0]; default: 1;
 *  Configures lp_efuse registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_EFUSE    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_EFUSE_M  (LP_PERI_PMS_READ_TEE_LP_EFUSE_V << LP_PERI_PMS_READ_TEE_LP_EFUSE_S)
#define LP_PERI_PMS_READ_TEE_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_EFUSE_S  0
/** LP_PERI_PMS_READ_REE0_LP_EFUSE : R/W; bitpos: [1]; default: 0;
 *  Configures lp_efuse registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_EFUSE    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_EFUSE_M  (LP_PERI_PMS_READ_REE0_LP_EFUSE_V << LP_PERI_PMS_READ_REE0_LP_EFUSE_S)
#define LP_PERI_PMS_READ_REE0_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_EFUSE_S  1
/** LP_PERI_PMS_READ_REE1_LP_EFUSE : R/W; bitpos: [2]; default: 0;
 *  Configures lp_efuse registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_EFUSE    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_EFUSE_M  (LP_PERI_PMS_READ_REE1_LP_EFUSE_V << LP_PERI_PMS_READ_REE1_LP_EFUSE_S)
#define LP_PERI_PMS_READ_REE1_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_EFUSE_S  2
/** LP_PERI_PMS_READ_REE2_LP_EFUSE : R/W; bitpos: [3]; default: 0;
 *  Configures lp_efuse registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_EFUSE    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_EFUSE_M  (LP_PERI_PMS_READ_REE2_LP_EFUSE_V << LP_PERI_PMS_READ_REE2_LP_EFUSE_S)
#define LP_PERI_PMS_READ_REE2_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_EFUSE_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_EFUSE : R/W; bitpos: [4]; default: 1;
 *  Configures lp_efuse registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_EFUSE    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_EFUSE_M  (LP_PERI_PMS_WRITE_TEE_LP_EFUSE_V << LP_PERI_PMS_WRITE_TEE_LP_EFUSE_S)
#define LP_PERI_PMS_WRITE_TEE_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_EFUSE_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_EFUSE : R/W; bitpos: [5]; default: 0;
 *  Configures lp_efuse registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_EFUSE    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_EFUSE_M  (LP_PERI_PMS_WRITE_REE0_LP_EFUSE_V << LP_PERI_PMS_WRITE_REE0_LP_EFUSE_S)
#define LP_PERI_PMS_WRITE_REE0_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_EFUSE_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_EFUSE : R/W; bitpos: [6]; default: 0;
 *  Configures lp_efuse registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_EFUSE    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_EFUSE_M  (LP_PERI_PMS_WRITE_REE1_LP_EFUSE_V << LP_PERI_PMS_WRITE_REE1_LP_EFUSE_S)
#define LP_PERI_PMS_WRITE_REE1_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_EFUSE_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_EFUSE : R/W; bitpos: [7]; default: 0;
 *  Configures lp_efuse registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_EFUSE    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_EFUSE_M  (LP_PERI_PMS_WRITE_REE2_LP_EFUSE_V << LP_PERI_PMS_WRITE_REE2_LP_EFUSE_S)
#define LP_PERI_PMS_WRITE_REE2_LP_EFUSE_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_EFUSE_S  7
/** LP_PERI_PMS_LP_EFUSE_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_efuse peri_apm configuration
 */
#define LP_PERI_PMS_LP_EFUSE_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_EFUSE_LOCK_M  (LP_PERI_PMS_LP_EFUSE_LOCK_V << LP_PERI_PMS_LP_EFUSE_LOCK_S)
#define LP_PERI_PMS_LP_EFUSE_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_EFUSE_LOCK_S  8

/** LP_PERI_PMS_LP_UART_CTRL_REG register
 *  lp_uart read/write control register
 */
#define LP_PERI_PMS_LP_UART_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x48)
/** LP_PERI_PMS_READ_TEE_LP_UART : R/W; bitpos: [0]; default: 1;
 *  Configures lp_uart registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_UART    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_UART_M  (LP_PERI_PMS_READ_TEE_LP_UART_V << LP_PERI_PMS_READ_TEE_LP_UART_S)
#define LP_PERI_PMS_READ_TEE_LP_UART_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_UART_S  0
/** LP_PERI_PMS_READ_REE0_LP_UART : R/W; bitpos: [1]; default: 0;
 *  Configures lp_uart registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_UART    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_UART_M  (LP_PERI_PMS_READ_REE0_LP_UART_V << LP_PERI_PMS_READ_REE0_LP_UART_S)
#define LP_PERI_PMS_READ_REE0_LP_UART_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_UART_S  1
/** LP_PERI_PMS_READ_REE1_LP_UART : R/W; bitpos: [2]; default: 0;
 *  Configures lp_uart registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_UART    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_UART_M  (LP_PERI_PMS_READ_REE1_LP_UART_V << LP_PERI_PMS_READ_REE1_LP_UART_S)
#define LP_PERI_PMS_READ_REE1_LP_UART_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_UART_S  2
/** LP_PERI_PMS_READ_REE2_LP_UART : R/W; bitpos: [3]; default: 0;
 *  Configures lp_uart registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_UART    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_UART_M  (LP_PERI_PMS_READ_REE2_LP_UART_V << LP_PERI_PMS_READ_REE2_LP_UART_S)
#define LP_PERI_PMS_READ_REE2_LP_UART_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_UART_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_UART : R/W; bitpos: [4]; default: 1;
 *  Configures lp_uart registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_UART    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_UART_M  (LP_PERI_PMS_WRITE_TEE_LP_UART_V << LP_PERI_PMS_WRITE_TEE_LP_UART_S)
#define LP_PERI_PMS_WRITE_TEE_LP_UART_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_UART_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_UART : R/W; bitpos: [5]; default: 0;
 *  Configures lp_uart registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_UART    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_UART_M  (LP_PERI_PMS_WRITE_REE0_LP_UART_V << LP_PERI_PMS_WRITE_REE0_LP_UART_S)
#define LP_PERI_PMS_WRITE_REE0_LP_UART_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_UART_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_UART : R/W; bitpos: [6]; default: 0;
 *  Configures lp_uart registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_UART    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_UART_M  (LP_PERI_PMS_WRITE_REE1_LP_UART_V << LP_PERI_PMS_WRITE_REE1_LP_UART_S)
#define LP_PERI_PMS_WRITE_REE1_LP_UART_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_UART_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_UART : R/W; bitpos: [7]; default: 0;
 *  Configures lp_uart registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_UART    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_UART_M  (LP_PERI_PMS_WRITE_REE2_LP_UART_V << LP_PERI_PMS_WRITE_REE2_LP_UART_S)
#define LP_PERI_PMS_WRITE_REE2_LP_UART_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_UART_S  7
/** LP_PERI_PMS_LP_UART_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_uart peri_apm configuration
 */
#define LP_PERI_PMS_LP_UART_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_UART_LOCK_M  (LP_PERI_PMS_LP_UART_LOCK_V << LP_PERI_PMS_LP_UART_LOCK_S)
#define LP_PERI_PMS_LP_UART_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_UART_LOCK_S  8

/** LP_PERI_PMS_LP_I2C_CTRL_REG register
 *  lp_i2c read/write control register
 */
#define LP_PERI_PMS_LP_I2C_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x4c)
/** LP_PERI_PMS_READ_TEE_LP_I2C : R/W; bitpos: [0]; default: 1;
 *  Configures lp_i2c registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_I2C    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_I2C_M  (LP_PERI_PMS_READ_TEE_LP_I2C_V << LP_PERI_PMS_READ_TEE_LP_I2C_S)
#define LP_PERI_PMS_READ_TEE_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_I2C_S  0
/** LP_PERI_PMS_READ_REE0_LP_I2C : R/W; bitpos: [1]; default: 0;
 *  Configures lp_i2c registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_I2C    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_I2C_M  (LP_PERI_PMS_READ_REE0_LP_I2C_V << LP_PERI_PMS_READ_REE0_LP_I2C_S)
#define LP_PERI_PMS_READ_REE0_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_I2C_S  1
/** LP_PERI_PMS_READ_REE1_LP_I2C : R/W; bitpos: [2]; default: 0;
 *  Configures lp_i2c registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_I2C    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_I2C_M  (LP_PERI_PMS_READ_REE1_LP_I2C_V << LP_PERI_PMS_READ_REE1_LP_I2C_S)
#define LP_PERI_PMS_READ_REE1_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_I2C_S  2
/** LP_PERI_PMS_READ_REE2_LP_I2C : R/W; bitpos: [3]; default: 0;
 *  Configures lp_i2c registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_I2C    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_I2C_M  (LP_PERI_PMS_READ_REE2_LP_I2C_V << LP_PERI_PMS_READ_REE2_LP_I2C_S)
#define LP_PERI_PMS_READ_REE2_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_I2C_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_I2C : R/W; bitpos: [4]; default: 1;
 *  Configures lp_i2c registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_I2C    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_I2C_M  (LP_PERI_PMS_WRITE_TEE_LP_I2C_V << LP_PERI_PMS_WRITE_TEE_LP_I2C_S)
#define LP_PERI_PMS_WRITE_TEE_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_I2C_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_I2C : R/W; bitpos: [5]; default: 0;
 *  Configures lp_i2c registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_I2C    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_I2C_M  (LP_PERI_PMS_WRITE_REE0_LP_I2C_V << LP_PERI_PMS_WRITE_REE0_LP_I2C_S)
#define LP_PERI_PMS_WRITE_REE0_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_I2C_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_I2C : R/W; bitpos: [6]; default: 0;
 *  Configures lp_i2c registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_I2C    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_I2C_M  (LP_PERI_PMS_WRITE_REE1_LP_I2C_V << LP_PERI_PMS_WRITE_REE1_LP_I2C_S)
#define LP_PERI_PMS_WRITE_REE1_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_I2C_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_I2C : R/W; bitpos: [7]; default: 0;
 *  Configures lp_i2c registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_I2C    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_I2C_M  (LP_PERI_PMS_WRITE_REE2_LP_I2C_V << LP_PERI_PMS_WRITE_REE2_LP_I2C_S)
#define LP_PERI_PMS_WRITE_REE2_LP_I2C_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_I2C_S  7
/** LP_PERI_PMS_LP_I2C_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_i2c peri_apm configuration
 */
#define LP_PERI_PMS_LP_I2C_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_I2C_LOCK_M  (LP_PERI_PMS_LP_I2C_LOCK_V << LP_PERI_PMS_LP_I2C_LOCK_S)
#define LP_PERI_PMS_LP_I2C_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_I2C_LOCK_S  8

/** LP_PERI_PMS_LP_SPI_CTRL_REG register
 *  lp_spi read/write control register
 */
#define LP_PERI_PMS_LP_SPI_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x50)
/** LP_PERI_PMS_READ_TEE_LP_SPI : R/W; bitpos: [0]; default: 1;
 *  Configures lp_spi registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_SPI    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_SPI_M  (LP_PERI_PMS_READ_TEE_LP_SPI_V << LP_PERI_PMS_READ_TEE_LP_SPI_S)
#define LP_PERI_PMS_READ_TEE_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_SPI_S  0
/** LP_PERI_PMS_READ_REE0_LP_SPI : R/W; bitpos: [1]; default: 0;
 *  Configures lp_spi registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_SPI    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_SPI_M  (LP_PERI_PMS_READ_REE0_LP_SPI_V << LP_PERI_PMS_READ_REE0_LP_SPI_S)
#define LP_PERI_PMS_READ_REE0_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_SPI_S  1
/** LP_PERI_PMS_READ_REE1_LP_SPI : R/W; bitpos: [2]; default: 0;
 *  Configures lp_spi registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_SPI    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_SPI_M  (LP_PERI_PMS_READ_REE1_LP_SPI_V << LP_PERI_PMS_READ_REE1_LP_SPI_S)
#define LP_PERI_PMS_READ_REE1_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_SPI_S  2
/** LP_PERI_PMS_READ_REE2_LP_SPI : R/W; bitpos: [3]; default: 0;
 *  Configures lp_spi registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_SPI    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_SPI_M  (LP_PERI_PMS_READ_REE2_LP_SPI_V << LP_PERI_PMS_READ_REE2_LP_SPI_S)
#define LP_PERI_PMS_READ_REE2_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_SPI_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_SPI : R/W; bitpos: [4]; default: 1;
 *  Configures lp_spi registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_SPI    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_SPI_M  (LP_PERI_PMS_WRITE_TEE_LP_SPI_V << LP_PERI_PMS_WRITE_TEE_LP_SPI_S)
#define LP_PERI_PMS_WRITE_TEE_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_SPI_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_SPI : R/W; bitpos: [5]; default: 0;
 *  Configures lp_spi registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_SPI    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_SPI_M  (LP_PERI_PMS_WRITE_REE0_LP_SPI_V << LP_PERI_PMS_WRITE_REE0_LP_SPI_S)
#define LP_PERI_PMS_WRITE_REE0_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_SPI_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_SPI : R/W; bitpos: [6]; default: 0;
 *  Configures lp_spi registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_SPI    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_SPI_M  (LP_PERI_PMS_WRITE_REE1_LP_SPI_V << LP_PERI_PMS_WRITE_REE1_LP_SPI_S)
#define LP_PERI_PMS_WRITE_REE1_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_SPI_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_SPI : R/W; bitpos: [7]; default: 0;
 *  Configures lp_spi registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_SPI    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_SPI_M  (LP_PERI_PMS_WRITE_REE2_LP_SPI_V << LP_PERI_PMS_WRITE_REE2_LP_SPI_S)
#define LP_PERI_PMS_WRITE_REE2_LP_SPI_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_SPI_S  7
/** LP_PERI_PMS_LP_SPI_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_spi peri_apm configuration
 */
#define LP_PERI_PMS_LP_SPI_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_SPI_LOCK_M  (LP_PERI_PMS_LP_SPI_LOCK_V << LP_PERI_PMS_LP_SPI_LOCK_S)
#define LP_PERI_PMS_LP_SPI_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_SPI_LOCK_S  8

/** LP_PERI_PMS_LP_I2CMST_CTRL_REG register
 *  lp_i2cmst read/write control register
 */
#define LP_PERI_PMS_LP_I2CMST_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x54)
/** LP_PERI_PMS_READ_TEE_LP_I2CMST : R/W; bitpos: [0]; default: 1;
 *  Configures lp_i2cmst registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_I2CMST    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_I2CMST_M  (LP_PERI_PMS_READ_TEE_LP_I2CMST_V << LP_PERI_PMS_READ_TEE_LP_I2CMST_S)
#define LP_PERI_PMS_READ_TEE_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_I2CMST_S  0
/** LP_PERI_PMS_READ_REE0_LP_I2CMST : R/W; bitpos: [1]; default: 0;
 *  Configures lp_i2cmst registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_I2CMST    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_I2CMST_M  (LP_PERI_PMS_READ_REE0_LP_I2CMST_V << LP_PERI_PMS_READ_REE0_LP_I2CMST_S)
#define LP_PERI_PMS_READ_REE0_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_I2CMST_S  1
/** LP_PERI_PMS_READ_REE1_LP_I2CMST : R/W; bitpos: [2]; default: 0;
 *  Configures lp_i2cmst registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_I2CMST    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_I2CMST_M  (LP_PERI_PMS_READ_REE1_LP_I2CMST_V << LP_PERI_PMS_READ_REE1_LP_I2CMST_S)
#define LP_PERI_PMS_READ_REE1_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_I2CMST_S  2
/** LP_PERI_PMS_READ_REE2_LP_I2CMST : R/W; bitpos: [3]; default: 0;
 *  Configures lp_i2cmst registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_I2CMST    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_I2CMST_M  (LP_PERI_PMS_READ_REE2_LP_I2CMST_V << LP_PERI_PMS_READ_REE2_LP_I2CMST_S)
#define LP_PERI_PMS_READ_REE2_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_I2CMST_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_I2CMST : R/W; bitpos: [4]; default: 1;
 *  Configures lp_i2cmst registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_I2CMST    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_I2CMST_M  (LP_PERI_PMS_WRITE_TEE_LP_I2CMST_V << LP_PERI_PMS_WRITE_TEE_LP_I2CMST_S)
#define LP_PERI_PMS_WRITE_TEE_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_I2CMST_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_I2CMST : R/W; bitpos: [5]; default: 0;
 *  Configures lp_i2cmst registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_I2CMST    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_I2CMST_M  (LP_PERI_PMS_WRITE_REE0_LP_I2CMST_V << LP_PERI_PMS_WRITE_REE0_LP_I2CMST_S)
#define LP_PERI_PMS_WRITE_REE0_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_I2CMST_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_I2CMST : R/W; bitpos: [6]; default: 0;
 *  Configures lp_i2cmst registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_I2CMST    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_I2CMST_M  (LP_PERI_PMS_WRITE_REE1_LP_I2CMST_V << LP_PERI_PMS_WRITE_REE1_LP_I2CMST_S)
#define LP_PERI_PMS_WRITE_REE1_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_I2CMST_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_I2CMST : R/W; bitpos: [7]; default: 0;
 *  Configures lp_i2cmst registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_I2CMST    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_I2CMST_M  (LP_PERI_PMS_WRITE_REE2_LP_I2CMST_V << LP_PERI_PMS_WRITE_REE2_LP_I2CMST_S)
#define LP_PERI_PMS_WRITE_REE2_LP_I2CMST_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_I2CMST_S  7
/** LP_PERI_PMS_LP_I2CMST_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_i2cmst peri_apm configuration
 */
#define LP_PERI_PMS_LP_I2CMST_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_I2CMST_LOCK_M  (LP_PERI_PMS_LP_I2CMST_LOCK_V << LP_PERI_PMS_LP_I2CMST_LOCK_S)
#define LP_PERI_PMS_LP_I2CMST_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_I2CMST_LOCK_S  8

/** LP_PERI_PMS_LP_TRNG_CTRL_REG register
 *  lp_trng read/write control register
 */
#define LP_PERI_PMS_LP_TRNG_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x58)
/** LP_PERI_PMS_READ_TEE_LP_TRNG : R/W; bitpos: [0]; default: 1;
 *  Configures lp_trng registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_TRNG    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_TRNG_M  (LP_PERI_PMS_READ_TEE_LP_TRNG_V << LP_PERI_PMS_READ_TEE_LP_TRNG_S)
#define LP_PERI_PMS_READ_TEE_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_TRNG_S  0
/** LP_PERI_PMS_READ_REE0_LP_TRNG : R/W; bitpos: [1]; default: 0;
 *  Configures lp_trng registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_TRNG    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_TRNG_M  (LP_PERI_PMS_READ_REE0_LP_TRNG_V << LP_PERI_PMS_READ_REE0_LP_TRNG_S)
#define LP_PERI_PMS_READ_REE0_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_TRNG_S  1
/** LP_PERI_PMS_READ_REE1_LP_TRNG : R/W; bitpos: [2]; default: 0;
 *  Configures lp_trng registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_TRNG    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_TRNG_M  (LP_PERI_PMS_READ_REE1_LP_TRNG_V << LP_PERI_PMS_READ_REE1_LP_TRNG_S)
#define LP_PERI_PMS_READ_REE1_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_TRNG_S  2
/** LP_PERI_PMS_READ_REE2_LP_TRNG : R/W; bitpos: [3]; default: 0;
 *  Configures lp_trng registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_TRNG    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_TRNG_M  (LP_PERI_PMS_READ_REE2_LP_TRNG_V << LP_PERI_PMS_READ_REE2_LP_TRNG_S)
#define LP_PERI_PMS_READ_REE2_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_TRNG_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_TRNG : R/W; bitpos: [4]; default: 1;
 *  Configures lp_trng registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_TRNG    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_TRNG_M  (LP_PERI_PMS_WRITE_TEE_LP_TRNG_V << LP_PERI_PMS_WRITE_TEE_LP_TRNG_S)
#define LP_PERI_PMS_WRITE_TEE_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_TRNG_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_TRNG : R/W; bitpos: [5]; default: 0;
 *  Configures lp_trng registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_TRNG    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_TRNG_M  (LP_PERI_PMS_WRITE_REE0_LP_TRNG_V << LP_PERI_PMS_WRITE_REE0_LP_TRNG_S)
#define LP_PERI_PMS_WRITE_REE0_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_TRNG_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_TRNG : R/W; bitpos: [6]; default: 0;
 *  Configures lp_trng registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_TRNG    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_TRNG_M  (LP_PERI_PMS_WRITE_REE1_LP_TRNG_V << LP_PERI_PMS_WRITE_REE1_LP_TRNG_S)
#define LP_PERI_PMS_WRITE_REE1_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_TRNG_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_TRNG : R/W; bitpos: [7]; default: 0;
 *  Configures lp_trng registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_TRNG    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_TRNG_M  (LP_PERI_PMS_WRITE_REE2_LP_TRNG_V << LP_PERI_PMS_WRITE_REE2_LP_TRNG_S)
#define LP_PERI_PMS_WRITE_REE2_LP_TRNG_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_TRNG_S  7
/** LP_PERI_PMS_LP_TRNG_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_trng peri_apm configuration
 */
#define LP_PERI_PMS_LP_TRNG_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_TRNG_LOCK_M  (LP_PERI_PMS_LP_TRNG_LOCK_V << LP_PERI_PMS_LP_TRNG_LOCK_S)
#define LP_PERI_PMS_LP_TRNG_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_TRNG_LOCK_S  8

/** LP_PERI_PMS_LP_ADC_CTRL_REG register
 *  lp_adc read/write control register
 */
#define LP_PERI_PMS_LP_ADC_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x5c)
/** LP_PERI_PMS_READ_TEE_LP_ADC : R/W; bitpos: [0]; default: 1;
 *  Configures lp_adc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_ADC    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_ADC_M  (LP_PERI_PMS_READ_TEE_LP_ADC_V << LP_PERI_PMS_READ_TEE_LP_ADC_S)
#define LP_PERI_PMS_READ_TEE_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_ADC_S  0
/** LP_PERI_PMS_READ_REE0_LP_ADC : R/W; bitpos: [1]; default: 0;
 *  Configures lp_adc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_ADC    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_ADC_M  (LP_PERI_PMS_READ_REE0_LP_ADC_V << LP_PERI_PMS_READ_REE0_LP_ADC_S)
#define LP_PERI_PMS_READ_REE0_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_ADC_S  1
/** LP_PERI_PMS_READ_REE1_LP_ADC : R/W; bitpos: [2]; default: 0;
 *  Configures lp_adc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_ADC    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_ADC_M  (LP_PERI_PMS_READ_REE1_LP_ADC_V << LP_PERI_PMS_READ_REE1_LP_ADC_S)
#define LP_PERI_PMS_READ_REE1_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_ADC_S  2
/** LP_PERI_PMS_READ_REE2_LP_ADC : R/W; bitpos: [3]; default: 0;
 *  Configures lp_adc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_ADC    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_ADC_M  (LP_PERI_PMS_READ_REE2_LP_ADC_V << LP_PERI_PMS_READ_REE2_LP_ADC_S)
#define LP_PERI_PMS_READ_REE2_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_ADC_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_ADC : R/W; bitpos: [4]; default: 1;
 *  Configures lp_adc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_ADC    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_ADC_M  (LP_PERI_PMS_WRITE_TEE_LP_ADC_V << LP_PERI_PMS_WRITE_TEE_LP_ADC_S)
#define LP_PERI_PMS_WRITE_TEE_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_ADC_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_ADC : R/W; bitpos: [5]; default: 0;
 *  Configures lp_adc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_ADC    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_ADC_M  (LP_PERI_PMS_WRITE_REE0_LP_ADC_V << LP_PERI_PMS_WRITE_REE0_LP_ADC_S)
#define LP_PERI_PMS_WRITE_REE0_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_ADC_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_ADC : R/W; bitpos: [6]; default: 0;
 *  Configures lp_adc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_ADC    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_ADC_M  (LP_PERI_PMS_WRITE_REE1_LP_ADC_V << LP_PERI_PMS_WRITE_REE1_LP_ADC_S)
#define LP_PERI_PMS_WRITE_REE1_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_ADC_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_ADC : R/W; bitpos: [7]; default: 0;
 *  Configures lp_adc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_ADC    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_ADC_M  (LP_PERI_PMS_WRITE_REE2_LP_ADC_V << LP_PERI_PMS_WRITE_REE2_LP_ADC_S)
#define LP_PERI_PMS_WRITE_REE2_LP_ADC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_ADC_S  7
/** LP_PERI_PMS_LP_ADC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_adc peri_apm configuration
 */
#define LP_PERI_PMS_LP_ADC_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_ADC_LOCK_M  (LP_PERI_PMS_LP_ADC_LOCK_V << LP_PERI_PMS_LP_ADC_LOCK_S)
#define LP_PERI_PMS_LP_ADC_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_ADC_LOCK_S  8

/** LP_PERI_PMS_LP_TOUCH_CTRL_REG register
 *  lp_touch read/write control register
 */
#define LP_PERI_PMS_LP_TOUCH_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x60)
/** LP_PERI_PMS_READ_TEE_LP_TOUCH : R/W; bitpos: [0]; default: 1;
 *  Configures lp_touch registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_TOUCH    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_TOUCH_M  (LP_PERI_PMS_READ_TEE_LP_TOUCH_V << LP_PERI_PMS_READ_TEE_LP_TOUCH_S)
#define LP_PERI_PMS_READ_TEE_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_TOUCH_S  0
/** LP_PERI_PMS_READ_REE0_LP_TOUCH : R/W; bitpos: [1]; default: 0;
 *  Configures lp_touch registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_TOUCH    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_TOUCH_M  (LP_PERI_PMS_READ_REE0_LP_TOUCH_V << LP_PERI_PMS_READ_REE0_LP_TOUCH_S)
#define LP_PERI_PMS_READ_REE0_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_TOUCH_S  1
/** LP_PERI_PMS_READ_REE1_LP_TOUCH : R/W; bitpos: [2]; default: 0;
 *  Configures lp_touch registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_TOUCH    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_TOUCH_M  (LP_PERI_PMS_READ_REE1_LP_TOUCH_V << LP_PERI_PMS_READ_REE1_LP_TOUCH_S)
#define LP_PERI_PMS_READ_REE1_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_TOUCH_S  2
/** LP_PERI_PMS_READ_REE2_LP_TOUCH : R/W; bitpos: [3]; default: 0;
 *  Configures lp_touch registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_TOUCH    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_TOUCH_M  (LP_PERI_PMS_READ_REE2_LP_TOUCH_V << LP_PERI_PMS_READ_REE2_LP_TOUCH_S)
#define LP_PERI_PMS_READ_REE2_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_TOUCH_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_TOUCH : R/W; bitpos: [4]; default: 1;
 *  Configures lp_touch registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH_M  (LP_PERI_PMS_WRITE_TEE_LP_TOUCH_V << LP_PERI_PMS_WRITE_TEE_LP_TOUCH_S)
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_TOUCH_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_TOUCH : R/W; bitpos: [5]; default: 0;
 *  Configures lp_touch registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH_M  (LP_PERI_PMS_WRITE_REE0_LP_TOUCH_V << LP_PERI_PMS_WRITE_REE0_LP_TOUCH_S)
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_TOUCH_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_TOUCH : R/W; bitpos: [6]; default: 0;
 *  Configures lp_touch registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH_M  (LP_PERI_PMS_WRITE_REE1_LP_TOUCH_V << LP_PERI_PMS_WRITE_REE1_LP_TOUCH_S)
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_TOUCH_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_TOUCH : R/W; bitpos: [7]; default: 0;
 *  Configures lp_touch registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH_M  (LP_PERI_PMS_WRITE_REE2_LP_TOUCH_V << LP_PERI_PMS_WRITE_REE2_LP_TOUCH_S)
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_TOUCH_S  7
/** LP_PERI_PMS_LP_TOUCH_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_touch peri_apm configuration
 */
#define LP_PERI_PMS_LP_TOUCH_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_TOUCH_LOCK_M  (LP_PERI_PMS_LP_TOUCH_LOCK_V << LP_PERI_PMS_LP_TOUCH_LOCK_S)
#define LP_PERI_PMS_LP_TOUCH_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_TOUCH_LOCK_S  8

/** LP_PERI_PMS_LP_MAILBOX_CTRL_REG register
 *  lp_mailbox read/write control register
 */
#define LP_PERI_PMS_LP_MAILBOX_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x64)
/** LP_PERI_PMS_READ_TEE_LP_MAILBOX : R/W; bitpos: [0]; default: 1;
 *  Configures lp_mailbox registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_MAILBOX    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_MAILBOX_M  (LP_PERI_PMS_READ_TEE_LP_MAILBOX_V << LP_PERI_PMS_READ_TEE_LP_MAILBOX_S)
#define LP_PERI_PMS_READ_TEE_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_MAILBOX_S  0
/** LP_PERI_PMS_READ_REE0_LP_MAILBOX : R/W; bitpos: [1]; default: 0;
 *  Configures lp_mailbox registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_MAILBOX    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_MAILBOX_M  (LP_PERI_PMS_READ_REE0_LP_MAILBOX_V << LP_PERI_PMS_READ_REE0_LP_MAILBOX_S)
#define LP_PERI_PMS_READ_REE0_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_MAILBOX_S  1
/** LP_PERI_PMS_READ_REE1_LP_MAILBOX : R/W; bitpos: [2]; default: 0;
 *  Configures lp_mailbox registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_MAILBOX    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_MAILBOX_M  (LP_PERI_PMS_READ_REE1_LP_MAILBOX_V << LP_PERI_PMS_READ_REE1_LP_MAILBOX_S)
#define LP_PERI_PMS_READ_REE1_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_MAILBOX_S  2
/** LP_PERI_PMS_READ_REE2_LP_MAILBOX : R/W; bitpos: [3]; default: 0;
 *  Configures lp_mailbox registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_MAILBOX    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_MAILBOX_M  (LP_PERI_PMS_READ_REE2_LP_MAILBOX_V << LP_PERI_PMS_READ_REE2_LP_MAILBOX_S)
#define LP_PERI_PMS_READ_REE2_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_MAILBOX_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_MAILBOX : R/W; bitpos: [4]; default: 1;
 *  Configures lp_mailbox registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_MAILBOX    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_MAILBOX_M  (LP_PERI_PMS_WRITE_TEE_LP_MAILBOX_V << LP_PERI_PMS_WRITE_TEE_LP_MAILBOX_S)
#define LP_PERI_PMS_WRITE_TEE_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_MAILBOX_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_MAILBOX : R/W; bitpos: [5]; default: 0;
 *  Configures lp_mailbox registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_MAILBOX    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_MAILBOX_M  (LP_PERI_PMS_WRITE_REE0_LP_MAILBOX_V << LP_PERI_PMS_WRITE_REE0_LP_MAILBOX_S)
#define LP_PERI_PMS_WRITE_REE0_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_MAILBOX_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_MAILBOX : R/W; bitpos: [6]; default: 0;
 *  Configures lp_mailbox registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_MAILBOX    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_MAILBOX_M  (LP_PERI_PMS_WRITE_REE1_LP_MAILBOX_V << LP_PERI_PMS_WRITE_REE1_LP_MAILBOX_S)
#define LP_PERI_PMS_WRITE_REE1_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_MAILBOX_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_MAILBOX : R/W; bitpos: [7]; default: 0;
 *  Configures lp_mailbox registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_MAILBOX    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_MAILBOX_M  (LP_PERI_PMS_WRITE_REE2_LP_MAILBOX_V << LP_PERI_PMS_WRITE_REE2_LP_MAILBOX_S)
#define LP_PERI_PMS_WRITE_REE2_LP_MAILBOX_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_MAILBOX_S  7
/** LP_PERI_PMS_LP_MAILBOX_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_mailbox peri_apm configuration
 */
#define LP_PERI_PMS_LP_MAILBOX_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_MAILBOX_LOCK_M  (LP_PERI_PMS_LP_MAILBOX_LOCK_V << LP_PERI_PMS_LP_MAILBOX_LOCK_S)
#define LP_PERI_PMS_LP_MAILBOX_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_MAILBOX_LOCK_S  8

/** LP_PERI_PMS_LP_TSENS_CTRL_REG register
 *  lp_tsens read/write control register
 */
#define LP_PERI_PMS_LP_TSENS_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x68)
/** LP_PERI_PMS_READ_TEE_LP_TSENS : R/W; bitpos: [0]; default: 1;
 *  Configures lp_tsens registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_TSENS    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_TSENS_M  (LP_PERI_PMS_READ_TEE_LP_TSENS_V << LP_PERI_PMS_READ_TEE_LP_TSENS_S)
#define LP_PERI_PMS_READ_TEE_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_TSENS_S  0
/** LP_PERI_PMS_READ_REE0_LP_TSENS : R/W; bitpos: [1]; default: 0;
 *  Configures lp_tsens registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_TSENS    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_TSENS_M  (LP_PERI_PMS_READ_REE0_LP_TSENS_V << LP_PERI_PMS_READ_REE0_LP_TSENS_S)
#define LP_PERI_PMS_READ_REE0_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_TSENS_S  1
/** LP_PERI_PMS_READ_REE1_LP_TSENS : R/W; bitpos: [2]; default: 0;
 *  Configures lp_tsens registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_TSENS    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_TSENS_M  (LP_PERI_PMS_READ_REE1_LP_TSENS_V << LP_PERI_PMS_READ_REE1_LP_TSENS_S)
#define LP_PERI_PMS_READ_REE1_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_TSENS_S  2
/** LP_PERI_PMS_READ_REE2_LP_TSENS : R/W; bitpos: [3]; default: 0;
 *  Configures lp_tsens registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_TSENS    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_TSENS_M  (LP_PERI_PMS_READ_REE2_LP_TSENS_V << LP_PERI_PMS_READ_REE2_LP_TSENS_S)
#define LP_PERI_PMS_READ_REE2_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_TSENS_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_TSENS : R/W; bitpos: [4]; default: 1;
 *  Configures lp_tsens registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_TSENS    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_TSENS_M  (LP_PERI_PMS_WRITE_TEE_LP_TSENS_V << LP_PERI_PMS_WRITE_TEE_LP_TSENS_S)
#define LP_PERI_PMS_WRITE_TEE_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_TSENS_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_TSENS : R/W; bitpos: [5]; default: 0;
 *  Configures lp_tsens registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_TSENS    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_TSENS_M  (LP_PERI_PMS_WRITE_REE0_LP_TSENS_V << LP_PERI_PMS_WRITE_REE0_LP_TSENS_S)
#define LP_PERI_PMS_WRITE_REE0_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_TSENS_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_TSENS : R/W; bitpos: [6]; default: 0;
 *  Configures lp_tsens registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_TSENS    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_TSENS_M  (LP_PERI_PMS_WRITE_REE1_LP_TSENS_V << LP_PERI_PMS_WRITE_REE1_LP_TSENS_S)
#define LP_PERI_PMS_WRITE_REE1_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_TSENS_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_TSENS : R/W; bitpos: [7]; default: 0;
 *  Configures lp_tsens registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_TSENS    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_TSENS_M  (LP_PERI_PMS_WRITE_REE2_LP_TSENS_V << LP_PERI_PMS_WRITE_REE2_LP_TSENS_S)
#define LP_PERI_PMS_WRITE_REE2_LP_TSENS_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_TSENS_S  7
/** LP_PERI_PMS_LP_TSENS_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_tsens peri_apm configuration
 */
#define LP_PERI_PMS_LP_TSENS_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_TSENS_LOCK_M  (LP_PERI_PMS_LP_TSENS_LOCK_V << LP_PERI_PMS_LP_TSENS_LOCK_S)
#define LP_PERI_PMS_LP_TSENS_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_TSENS_LOCK_S  8

/** LP_PERI_PMS_LP_AHB_PDMA_CTRL_REG register
 *  lp_ahb_pdma read/write control register
 */
#define LP_PERI_PMS_LP_AHB_PDMA_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x6c)
/** LP_PERI_PMS_READ_TEE_LP_AHB_PDMA : R/W; bitpos: [0]; default: 1;
 *  Configures lp_ahb_pdma registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_AHB_PDMA    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_AHB_PDMA_M  (LP_PERI_PMS_READ_TEE_LP_AHB_PDMA_V << LP_PERI_PMS_READ_TEE_LP_AHB_PDMA_S)
#define LP_PERI_PMS_READ_TEE_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_AHB_PDMA_S  0
/** LP_PERI_PMS_READ_REE0_LP_AHB_PDMA : R/W; bitpos: [1]; default: 0;
 *  Configures lp_ahb_pdma registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_AHB_PDMA    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_AHB_PDMA_M  (LP_PERI_PMS_READ_REE0_LP_AHB_PDMA_V << LP_PERI_PMS_READ_REE0_LP_AHB_PDMA_S)
#define LP_PERI_PMS_READ_REE0_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_AHB_PDMA_S  1
/** LP_PERI_PMS_READ_REE1_LP_AHB_PDMA : R/W; bitpos: [2]; default: 0;
 *  Configures lp_ahb_pdma registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_AHB_PDMA    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_AHB_PDMA_M  (LP_PERI_PMS_READ_REE1_LP_AHB_PDMA_V << LP_PERI_PMS_READ_REE1_LP_AHB_PDMA_S)
#define LP_PERI_PMS_READ_REE1_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_AHB_PDMA_S  2
/** LP_PERI_PMS_READ_REE2_LP_AHB_PDMA : R/W; bitpos: [3]; default: 0;
 *  Configures lp_ahb_pdma registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_AHB_PDMA    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_AHB_PDMA_M  (LP_PERI_PMS_READ_REE2_LP_AHB_PDMA_V << LP_PERI_PMS_READ_REE2_LP_AHB_PDMA_S)
#define LP_PERI_PMS_READ_REE2_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_AHB_PDMA_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_AHB_PDMA : R/W; bitpos: [4]; default: 1;
 *  Configures lp_ahb_pdma registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_AHB_PDMA    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_AHB_PDMA_M  (LP_PERI_PMS_WRITE_TEE_LP_AHB_PDMA_V << LP_PERI_PMS_WRITE_TEE_LP_AHB_PDMA_S)
#define LP_PERI_PMS_WRITE_TEE_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_AHB_PDMA_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_AHB_PDMA : R/W; bitpos: [5]; default: 0;
 *  Configures lp_ahb_pdma registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_AHB_PDMA    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_AHB_PDMA_M  (LP_PERI_PMS_WRITE_REE0_LP_AHB_PDMA_V << LP_PERI_PMS_WRITE_REE0_LP_AHB_PDMA_S)
#define LP_PERI_PMS_WRITE_REE0_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_AHB_PDMA_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_AHB_PDMA : R/W; bitpos: [6]; default: 0;
 *  Configures lp_ahb_pdma registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_AHB_PDMA    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_AHB_PDMA_M  (LP_PERI_PMS_WRITE_REE1_LP_AHB_PDMA_V << LP_PERI_PMS_WRITE_REE1_LP_AHB_PDMA_S)
#define LP_PERI_PMS_WRITE_REE1_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_AHB_PDMA_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_AHB_PDMA : R/W; bitpos: [7]; default: 0;
 *  Configures lp_ahb_pdma registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_AHB_PDMA    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_AHB_PDMA_M  (LP_PERI_PMS_WRITE_REE2_LP_AHB_PDMA_V << LP_PERI_PMS_WRITE_REE2_LP_AHB_PDMA_S)
#define LP_PERI_PMS_WRITE_REE2_LP_AHB_PDMA_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_AHB_PDMA_S  7
/** LP_PERI_PMS_LP_AHB_PDMA_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_ahb_pdma peri_apm configuration
 */
#define LP_PERI_PMS_LP_AHB_PDMA_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_AHB_PDMA_LOCK_M  (LP_PERI_PMS_LP_AHB_PDMA_LOCK_V << LP_PERI_PMS_LP_AHB_PDMA_LOCK_S)
#define LP_PERI_PMS_LP_AHB_PDMA_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_AHB_PDMA_LOCK_S  8

/** LP_PERI_PMS_LP_DAC_CTRL_REG register
 *  lp_dac read/write control register
 */
#define LP_PERI_PMS_LP_DAC_CTRL_REG (DR_REG_LP_PERI_PMS_BASE + 0x70)
/** LP_PERI_PMS_READ_TEE_LP_DAC : R/W; bitpos: [0]; default: 1;
 *  Configures lp_dac registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_TEE_LP_DAC    (BIT(0))
#define LP_PERI_PMS_READ_TEE_LP_DAC_M  (LP_PERI_PMS_READ_TEE_LP_DAC_V << LP_PERI_PMS_READ_TEE_LP_DAC_S)
#define LP_PERI_PMS_READ_TEE_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_READ_TEE_LP_DAC_S  0
/** LP_PERI_PMS_READ_REE0_LP_DAC : R/W; bitpos: [1]; default: 0;
 *  Configures lp_dac registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE0_LP_DAC    (BIT(1))
#define LP_PERI_PMS_READ_REE0_LP_DAC_M  (LP_PERI_PMS_READ_REE0_LP_DAC_V << LP_PERI_PMS_READ_REE0_LP_DAC_S)
#define LP_PERI_PMS_READ_REE0_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_READ_REE0_LP_DAC_S  1
/** LP_PERI_PMS_READ_REE1_LP_DAC : R/W; bitpos: [2]; default: 0;
 *  Configures lp_dac registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE1_LP_DAC    (BIT(2))
#define LP_PERI_PMS_READ_REE1_LP_DAC_M  (LP_PERI_PMS_READ_REE1_LP_DAC_V << LP_PERI_PMS_READ_REE1_LP_DAC_S)
#define LP_PERI_PMS_READ_REE1_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_READ_REE1_LP_DAC_S  2
/** LP_PERI_PMS_READ_REE2_LP_DAC : R/W; bitpos: [3]; default: 0;
 *  Configures lp_dac registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_PERI_PMS_READ_REE2_LP_DAC    (BIT(3))
#define LP_PERI_PMS_READ_REE2_LP_DAC_M  (LP_PERI_PMS_READ_REE2_LP_DAC_V << LP_PERI_PMS_READ_REE2_LP_DAC_S)
#define LP_PERI_PMS_READ_REE2_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_READ_REE2_LP_DAC_S  3
/** LP_PERI_PMS_WRITE_TEE_LP_DAC : R/W; bitpos: [4]; default: 1;
 *  Configures lp_dac registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_TEE_LP_DAC    (BIT(4))
#define LP_PERI_PMS_WRITE_TEE_LP_DAC_M  (LP_PERI_PMS_WRITE_TEE_LP_DAC_V << LP_PERI_PMS_WRITE_TEE_LP_DAC_S)
#define LP_PERI_PMS_WRITE_TEE_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_WRITE_TEE_LP_DAC_S  4
/** LP_PERI_PMS_WRITE_REE0_LP_DAC : R/W; bitpos: [5]; default: 0;
 *  Configures lp_dac registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE0_LP_DAC    (BIT(5))
#define LP_PERI_PMS_WRITE_REE0_LP_DAC_M  (LP_PERI_PMS_WRITE_REE0_LP_DAC_V << LP_PERI_PMS_WRITE_REE0_LP_DAC_S)
#define LP_PERI_PMS_WRITE_REE0_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE0_LP_DAC_S  5
/** LP_PERI_PMS_WRITE_REE1_LP_DAC : R/W; bitpos: [6]; default: 0;
 *  Configures lp_dac registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE1_LP_DAC    (BIT(6))
#define LP_PERI_PMS_WRITE_REE1_LP_DAC_M  (LP_PERI_PMS_WRITE_REE1_LP_DAC_V << LP_PERI_PMS_WRITE_REE1_LP_DAC_S)
#define LP_PERI_PMS_WRITE_REE1_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE1_LP_DAC_S  6
/** LP_PERI_PMS_WRITE_REE2_LP_DAC : R/W; bitpos: [7]; default: 0;
 *  Configures lp_dac registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_PERI_PMS_WRITE_REE2_LP_DAC    (BIT(7))
#define LP_PERI_PMS_WRITE_REE2_LP_DAC_M  (LP_PERI_PMS_WRITE_REE2_LP_DAC_V << LP_PERI_PMS_WRITE_REE2_LP_DAC_S)
#define LP_PERI_PMS_WRITE_REE2_LP_DAC_V  0x00000001U
#define LP_PERI_PMS_WRITE_REE2_LP_DAC_S  7
/** LP_PERI_PMS_LP_DAC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock lp_dac peri_apm configuration
 */
#define LP_PERI_PMS_LP_DAC_LOCK    (BIT(8))
#define LP_PERI_PMS_LP_DAC_LOCK_M  (LP_PERI_PMS_LP_DAC_LOCK_V << LP_PERI_PMS_LP_DAC_LOCK_S)
#define LP_PERI_PMS_LP_DAC_LOCK_V  0x00000001U
#define LP_PERI_PMS_LP_DAC_LOCK_S  8

/** LP_PERI_PMS_LP_PERI0_0_REG register
 *  LP_PERI0 PMS configuration & info register
 */
#define LP_PERI_PMS_LP_PERI0_0_REG (DR_REG_LP_PERI_PMS_BASE + 0x200)
/** LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear lp_peri0 peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_CLR    (BIT(0))
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_CLR_M  (LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_CLR_V << LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_CLR_S)
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_CLR_V  0x00000001U
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_CLR_S  0
/** LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_DET : RO; bitpos: [1]; default: 0;
 *  Represents whether the lp_peri0 pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_DET    (BIT(1))
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_DET_M  (LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_DET_V << LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_DET_S)
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_DET_V  0x00000001U
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_DET_S  1
/** LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ID : RO; bitpos: [7:2]; default: 0;
 *  Represents the master id when lp_peri0 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ID    0x0000003FU
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ID_M  (LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ID_V << LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ID_S)
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ID_V  0x0000003FU
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ID_S  2
/** LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_MODE : RO; bitpos: [9:8]; default: 0;
 *  Represents the security mode when lp_peri0 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_MODE    0x00000003U
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_MODE_M  (LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_MODE_V << LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_MODE_S)
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_MODE_V  0x00000003U
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_MODE_S  8

/** LP_PERI_PMS_LP_PERI0_1_REG register
 *  LP_PERI0 PMS exception addr record register
 */
#define LP_PERI_PMS_LP_PERI0_1_REG (DR_REG_LP_PERI_PMS_BASE + 0x204)
/** LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR : RO; bitpos: [23:0]; default: 0;
 *  Represents the access address (bit27~bit0) when lp_peri0 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR    0x00FFFFFFU
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_M  (LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_V << LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_S)
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_V  0x00FFFFFFU
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_S  0
/** LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_CONST : HRO; bitpos: [31:24]; default: 32;
 *  Represents the access address (bit31~bit28) when lp_peri0 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_CONST    0x000000FFU
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_CONST_M  (LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_CONST_V << LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_CONST_S)
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_CONST_V  0x000000FFU
#define LP_PERI_PMS_LP_PERI0_PMS_EXCEPTION_ADDR_CONST_S  24

/** LP_PERI_PMS_LP_PERI1_0_REG register
 *  LP_PERI1 PMS configuration & info register
 */
#define LP_PERI_PMS_LP_PERI1_0_REG (DR_REG_LP_PERI_PMS_BASE + 0x208)
/** LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear lp_peri1 peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_CLR    (BIT(0))
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_CLR_M  (LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_CLR_V << LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_CLR_S)
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_CLR_V  0x00000001U
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_CLR_S  0
/** LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_DET : RO; bitpos: [1]; default: 0;
 *  Represents whether the lp_peri1 pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_DET    (BIT(1))
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_DET_M  (LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_DET_V << LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_DET_S)
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_DET_V  0x00000001U
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_DET_S  1
/** LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ID : RO; bitpos: [7:2]; default: 0;
 *  Represents the master id when lp_peri1 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ID    0x0000003FU
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ID_M  (LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ID_V << LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ID_S)
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ID_V  0x0000003FU
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ID_S  2
/** LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_MODE : RO; bitpos: [9:8]; default: 0;
 *  Represents the security mode when lp_peri1 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_MODE    0x00000003U
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_MODE_M  (LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_MODE_V << LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_MODE_S)
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_MODE_V  0x00000003U
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_MODE_S  8

/** LP_PERI_PMS_LP_PERI1_1_REG register
 *  LP_PERI1 PMS exception addr record register
 */
#define LP_PERI_PMS_LP_PERI1_1_REG (DR_REG_LP_PERI_PMS_BASE + 0x20c)
/** LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR : RO; bitpos: [23:0]; default: 0;
 *  Represents the access address (bit27~bit0) when lp_peri1 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR    0x00FFFFFFU
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_M  (LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_V << LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_S)
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_V  0x00FFFFFFU
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_S  0
/** LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_CONST : HRO; bitpos: [31:24]; default: 32;
 *  Represents the access address (bit31~bit28) when lp_peri1 pms has been triggered.
 */
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_CONST    0x000000FFU
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_CONST_M  (LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_CONST_V << LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_CONST_S)
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_CONST_V  0x000000FFU
#define LP_PERI_PMS_LP_PERI1_PMS_EXCEPTION_ADDR_CONST_S  24

/** LP_PERI_PMS_INT_EN_REG register
 *  APM interrupt enable register
 */
#define LP_PERI_PMS_INT_EN_REG (DR_REG_LP_PERI_PMS_BASE + 0x300)
/** LP_PERI_PMS_LP_PERI1_PMS_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable lp_peri1 pms interrupt.
 *  0: disable
 *  1: enable
 */
#define LP_PERI_PMS_LP_PERI1_PMS_INT_EN    (BIT(0))
#define LP_PERI_PMS_LP_PERI1_PMS_INT_EN_M  (LP_PERI_PMS_LP_PERI1_PMS_INT_EN_V << LP_PERI_PMS_LP_PERI1_PMS_INT_EN_S)
#define LP_PERI_PMS_LP_PERI1_PMS_INT_EN_V  0x00000001U
#define LP_PERI_PMS_LP_PERI1_PMS_INT_EN_S  0
/** LP_PERI_PMS_LP_PERI0_PMS_INT_EN : R/W; bitpos: [1]; default: 0;
 *  Configures to enable lp_peri0 pms interrupt.
 *  0: disable
 *  1: enable
 */
#define LP_PERI_PMS_LP_PERI0_PMS_INT_EN    (BIT(1))
#define LP_PERI_PMS_LP_PERI0_PMS_INT_EN_M  (LP_PERI_PMS_LP_PERI0_PMS_INT_EN_V << LP_PERI_PMS_LP_PERI0_PMS_INT_EN_S)
#define LP_PERI_PMS_LP_PERI0_PMS_INT_EN_V  0x00000001U
#define LP_PERI_PMS_LP_PERI0_PMS_INT_EN_S  1

/** LP_PERI_PMS_BUS_ERR_CONF_REG register
 *  Clock gating register
 */
#define LP_PERI_PMS_BUS_ERR_CONF_REG (DR_REG_LP_PERI_PMS_BASE + 0x7f0)
/** LP_PERI_PMS_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether return error response to cpu when access blocked
 *  0: disable error response
 *  1: enable error response
 */
#define LP_PERI_PMS_BUS_ERR_RESP_EN    (BIT(0))
#define LP_PERI_PMS_BUS_ERR_RESP_EN_M  (LP_PERI_PMS_BUS_ERR_RESP_EN_V << LP_PERI_PMS_BUS_ERR_RESP_EN_S)
#define LP_PERI_PMS_BUS_ERR_RESP_EN_V  0x00000001U
#define LP_PERI_PMS_BUS_ERR_RESP_EN_S  0

/** LP_PERI_PMS_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define LP_PERI_PMS_CLOCK_GATE_REG (DR_REG_LP_PERI_PMS_BASE + 0x7f8)
/** LP_PERI_PMS_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: enable automatic clock gating
 *  1: keep the clock always on
 */
#define LP_PERI_PMS_CLK_EN    (BIT(0))
#define LP_PERI_PMS_CLK_EN_M  (LP_PERI_PMS_CLK_EN_V << LP_PERI_PMS_CLK_EN_S)
#define LP_PERI_PMS_CLK_EN_V  0x00000001U
#define LP_PERI_PMS_CLK_EN_S  0

/** LP_PERI_PMS_DATE_REG register
 *  Version control register
 */
#define LP_PERI_PMS_DATE_REG (DR_REG_LP_PERI_PMS_BASE + 0x7fc)
/** LP_PERI_PMS_DATE : R/W; bitpos: [27:0]; default: 38834752;
 *  Version control register
 */
#define LP_PERI_PMS_DATE    0x0FFFFFFFU
#define LP_PERI_PMS_DATE_M  (LP_PERI_PMS_DATE_V << LP_PERI_PMS_DATE_S)
#define LP_PERI_PMS_DATE_V  0x0FFFFFFFU
#define LP_PERI_PMS_DATE_S  0

#ifdef __cplusplus
}
#endif
