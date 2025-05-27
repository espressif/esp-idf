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

/** LP_TEE_M0_MODE_CTRL_REG register
 *  Security mode configuration register
 */
#define LP_TEE_M0_MODE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x0)
/** LP_TEE_M0_MODE : R/W; bitpos: [1:0]; default: 3;
 *  Configures the security mode for LP CPU.
 *  0: TEE
 *  1: REE0
 *  2: REE1
 *  3: REE2
 */
#define LP_TEE_M0_MODE    0x00000003U
#define LP_TEE_M0_MODE_M  (LP_TEE_M0_MODE_V << LP_TEE_M0_MODE_S)
#define LP_TEE_M0_MODE_V  0x00000003U
#define LP_TEE_M0_MODE_S  0
/** LP_TEE_M0_LOCK : R/W; bitpos: [2]; default: 0;
 *  Configures to lock the value of LP_TEE_M0_MODE.
 *  0: Do not lock
 *  1: Lock
 */
#define LP_TEE_M0_LOCK    (BIT(2))
#define LP_TEE_M0_LOCK_M  (LP_TEE_M0_LOCK_V << LP_TEE_M0_LOCK_S)
#define LP_TEE_M0_LOCK_V  0x00000001U
#define LP_TEE_M0_LOCK_S  2

/** LP_TEE_EFUSE_CTRL_REG register
 *  eFuse read/write control register
 */
#define LP_TEE_EFUSE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x4)
/** LP_TEE_READ_TEE_EFUSE : R/W; bitpos: [0]; default: 1;
 *  Configures efuse registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_EFUSE    (BIT(0))
#define LP_TEE_READ_TEE_EFUSE_M  (LP_TEE_READ_TEE_EFUSE_V << LP_TEE_READ_TEE_EFUSE_S)
#define LP_TEE_READ_TEE_EFUSE_V  0x00000001U
#define LP_TEE_READ_TEE_EFUSE_S  0
/** LP_TEE_READ_REE0_EFUSE : R/W; bitpos: [1]; default: 0;
 *  Configures efuse registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_EFUSE    (BIT(1))
#define LP_TEE_READ_REE0_EFUSE_M  (LP_TEE_READ_REE0_EFUSE_V << LP_TEE_READ_REE0_EFUSE_S)
#define LP_TEE_READ_REE0_EFUSE_V  0x00000001U
#define LP_TEE_READ_REE0_EFUSE_S  1
/** LP_TEE_READ_REE1_EFUSE : R/W; bitpos: [2]; default: 0;
 *  Configures efuse registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_EFUSE    (BIT(2))
#define LP_TEE_READ_REE1_EFUSE_M  (LP_TEE_READ_REE1_EFUSE_V << LP_TEE_READ_REE1_EFUSE_S)
#define LP_TEE_READ_REE1_EFUSE_V  0x00000001U
#define LP_TEE_READ_REE1_EFUSE_S  2
/** LP_TEE_READ_REE2_EFUSE : R/W; bitpos: [3]; default: 0;
 *  Configures efuse registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_EFUSE    (BIT(3))
#define LP_TEE_READ_REE2_EFUSE_M  (LP_TEE_READ_REE2_EFUSE_V << LP_TEE_READ_REE2_EFUSE_S)
#define LP_TEE_READ_REE2_EFUSE_V  0x00000001U
#define LP_TEE_READ_REE2_EFUSE_S  3
/** LP_TEE_WRITE_TEE_EFUSE : R/W; bitpos: [4]; default: 1;
 *  Configures efuse registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_EFUSE    (BIT(4))
#define LP_TEE_WRITE_TEE_EFUSE_M  (LP_TEE_WRITE_TEE_EFUSE_V << LP_TEE_WRITE_TEE_EFUSE_S)
#define LP_TEE_WRITE_TEE_EFUSE_V  0x00000001U
#define LP_TEE_WRITE_TEE_EFUSE_S  4
/** LP_TEE_WRITE_REE0_EFUSE : R/W; bitpos: [5]; default: 0;
 *  Configures efuse registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_EFUSE    (BIT(5))
#define LP_TEE_WRITE_REE0_EFUSE_M  (LP_TEE_WRITE_REE0_EFUSE_V << LP_TEE_WRITE_REE0_EFUSE_S)
#define LP_TEE_WRITE_REE0_EFUSE_V  0x00000001U
#define LP_TEE_WRITE_REE0_EFUSE_S  5
/** LP_TEE_WRITE_REE1_EFUSE : R/W; bitpos: [6]; default: 0;
 *  Configures efuse registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_EFUSE    (BIT(6))
#define LP_TEE_WRITE_REE1_EFUSE_M  (LP_TEE_WRITE_REE1_EFUSE_V << LP_TEE_WRITE_REE1_EFUSE_S)
#define LP_TEE_WRITE_REE1_EFUSE_V  0x00000001U
#define LP_TEE_WRITE_REE1_EFUSE_S  6
/** LP_TEE_WRITE_REE2_EFUSE : R/W; bitpos: [7]; default: 0;
 *  Configures efuse registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_EFUSE    (BIT(7))
#define LP_TEE_WRITE_REE2_EFUSE_M  (LP_TEE_WRITE_REE2_EFUSE_V << LP_TEE_WRITE_REE2_EFUSE_S)
#define LP_TEE_WRITE_REE2_EFUSE_V  0x00000001U
#define LP_TEE_WRITE_REE2_EFUSE_S  7

/** LP_TEE_PMU_CTRL_REG register
 *  PMU read/write control register
 */
#define LP_TEE_PMU_CTRL_REG (DR_REG_LP_TEE_BASE + 0x8)
/** LP_TEE_READ_TEE_PMU : R/W; bitpos: [0]; default: 1;
 *  Configures pmu registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_PMU    (BIT(0))
#define LP_TEE_READ_TEE_PMU_M  (LP_TEE_READ_TEE_PMU_V << LP_TEE_READ_TEE_PMU_S)
#define LP_TEE_READ_TEE_PMU_V  0x00000001U
#define LP_TEE_READ_TEE_PMU_S  0
/** LP_TEE_READ_REE0_PMU : R/W; bitpos: [1]; default: 0;
 *  Configures pmu registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_PMU    (BIT(1))
#define LP_TEE_READ_REE0_PMU_M  (LP_TEE_READ_REE0_PMU_V << LP_TEE_READ_REE0_PMU_S)
#define LP_TEE_READ_REE0_PMU_V  0x00000001U
#define LP_TEE_READ_REE0_PMU_S  1
/** LP_TEE_READ_REE1_PMU : R/W; bitpos: [2]; default: 0;
 *  Configures pmu registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_PMU    (BIT(2))
#define LP_TEE_READ_REE1_PMU_M  (LP_TEE_READ_REE1_PMU_V << LP_TEE_READ_REE1_PMU_S)
#define LP_TEE_READ_REE1_PMU_V  0x00000001U
#define LP_TEE_READ_REE1_PMU_S  2
/** LP_TEE_READ_REE2_PMU : R/W; bitpos: [3]; default: 0;
 *  Configures pmu registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_PMU    (BIT(3))
#define LP_TEE_READ_REE2_PMU_M  (LP_TEE_READ_REE2_PMU_V << LP_TEE_READ_REE2_PMU_S)
#define LP_TEE_READ_REE2_PMU_V  0x00000001U
#define LP_TEE_READ_REE2_PMU_S  3
/** LP_TEE_WRITE_TEE_PMU : R/W; bitpos: [4]; default: 1;
 *  Configures pmu registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_PMU    (BIT(4))
#define LP_TEE_WRITE_TEE_PMU_M  (LP_TEE_WRITE_TEE_PMU_V << LP_TEE_WRITE_TEE_PMU_S)
#define LP_TEE_WRITE_TEE_PMU_V  0x00000001U
#define LP_TEE_WRITE_TEE_PMU_S  4
/** LP_TEE_WRITE_REE0_PMU : R/W; bitpos: [5]; default: 0;
 *  Configures pmu registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_PMU    (BIT(5))
#define LP_TEE_WRITE_REE0_PMU_M  (LP_TEE_WRITE_REE0_PMU_V << LP_TEE_WRITE_REE0_PMU_S)
#define LP_TEE_WRITE_REE0_PMU_V  0x00000001U
#define LP_TEE_WRITE_REE0_PMU_S  5
/** LP_TEE_WRITE_REE1_PMU : R/W; bitpos: [6]; default: 0;
 *  Configures pmu registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_PMU    (BIT(6))
#define LP_TEE_WRITE_REE1_PMU_M  (LP_TEE_WRITE_REE1_PMU_V << LP_TEE_WRITE_REE1_PMU_S)
#define LP_TEE_WRITE_REE1_PMU_V  0x00000001U
#define LP_TEE_WRITE_REE1_PMU_S  6
/** LP_TEE_WRITE_REE2_PMU : R/W; bitpos: [7]; default: 0;
 *  Configures pmu registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_PMU    (BIT(7))
#define LP_TEE_WRITE_REE2_PMU_M  (LP_TEE_WRITE_REE2_PMU_V << LP_TEE_WRITE_REE2_PMU_S)
#define LP_TEE_WRITE_REE2_PMU_V  0x00000001U
#define LP_TEE_WRITE_REE2_PMU_S  7

/** LP_TEE_CLKRST_CTRL_REG register
 *  LP_CLKRST read/write control register
 */
#define LP_TEE_CLKRST_CTRL_REG (DR_REG_LP_TEE_BASE + 0xc)
/** LP_TEE_READ_TEE_CLKRST : R/W; bitpos: [0]; default: 1;
 *  Configures clkrst registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_CLKRST    (BIT(0))
#define LP_TEE_READ_TEE_CLKRST_M  (LP_TEE_READ_TEE_CLKRST_V << LP_TEE_READ_TEE_CLKRST_S)
#define LP_TEE_READ_TEE_CLKRST_V  0x00000001U
#define LP_TEE_READ_TEE_CLKRST_S  0
/** LP_TEE_READ_REE0_CLKRST : R/W; bitpos: [1]; default: 0;
 *  Configures clkrst registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_CLKRST    (BIT(1))
#define LP_TEE_READ_REE0_CLKRST_M  (LP_TEE_READ_REE0_CLKRST_V << LP_TEE_READ_REE0_CLKRST_S)
#define LP_TEE_READ_REE0_CLKRST_V  0x00000001U
#define LP_TEE_READ_REE0_CLKRST_S  1
/** LP_TEE_READ_REE1_CLKRST : R/W; bitpos: [2]; default: 0;
 *  Configures clkrst registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_CLKRST    (BIT(2))
#define LP_TEE_READ_REE1_CLKRST_M  (LP_TEE_READ_REE1_CLKRST_V << LP_TEE_READ_REE1_CLKRST_S)
#define LP_TEE_READ_REE1_CLKRST_V  0x00000001U
#define LP_TEE_READ_REE1_CLKRST_S  2
/** LP_TEE_READ_REE2_CLKRST : R/W; bitpos: [3]; default: 0;
 *  Configures clkrst registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_CLKRST    (BIT(3))
#define LP_TEE_READ_REE2_CLKRST_M  (LP_TEE_READ_REE2_CLKRST_V << LP_TEE_READ_REE2_CLKRST_S)
#define LP_TEE_READ_REE2_CLKRST_V  0x00000001U
#define LP_TEE_READ_REE2_CLKRST_S  3
/** LP_TEE_WRITE_TEE_CLKRST : R/W; bitpos: [4]; default: 1;
 *  Configures clkrst registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_CLKRST    (BIT(4))
#define LP_TEE_WRITE_TEE_CLKRST_M  (LP_TEE_WRITE_TEE_CLKRST_V << LP_TEE_WRITE_TEE_CLKRST_S)
#define LP_TEE_WRITE_TEE_CLKRST_V  0x00000001U
#define LP_TEE_WRITE_TEE_CLKRST_S  4
/** LP_TEE_WRITE_REE0_CLKRST : R/W; bitpos: [5]; default: 0;
 *  Configures clkrst registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_CLKRST    (BIT(5))
#define LP_TEE_WRITE_REE0_CLKRST_M  (LP_TEE_WRITE_REE0_CLKRST_V << LP_TEE_WRITE_REE0_CLKRST_S)
#define LP_TEE_WRITE_REE0_CLKRST_V  0x00000001U
#define LP_TEE_WRITE_REE0_CLKRST_S  5
/** LP_TEE_WRITE_REE1_CLKRST : R/W; bitpos: [6]; default: 0;
 *  Configures clkrst registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_CLKRST    (BIT(6))
#define LP_TEE_WRITE_REE1_CLKRST_M  (LP_TEE_WRITE_REE1_CLKRST_V << LP_TEE_WRITE_REE1_CLKRST_S)
#define LP_TEE_WRITE_REE1_CLKRST_V  0x00000001U
#define LP_TEE_WRITE_REE1_CLKRST_S  6
/** LP_TEE_WRITE_REE2_CLKRST : R/W; bitpos: [7]; default: 0;
 *  Configures clkrst registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_CLKRST    (BIT(7))
#define LP_TEE_WRITE_REE2_CLKRST_M  (LP_TEE_WRITE_REE2_CLKRST_V << LP_TEE_WRITE_REE2_CLKRST_S)
#define LP_TEE_WRITE_REE2_CLKRST_V  0x00000001U
#define LP_TEE_WRITE_REE2_CLKRST_S  7

/** LP_TEE_LP_AON_CTRL_CTRL_REG register
 *  LP_AON read/write control register
 */
#define LP_TEE_LP_AON_CTRL_CTRL_REG (DR_REG_LP_TEE_BASE + 0x10)
/** LP_TEE_READ_TEE_LP_AON_CTRL : R/W; bitpos: [0]; default: 1;
 *  Configures lp_aon_ctrl registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_AON_CTRL    (BIT(0))
#define LP_TEE_READ_TEE_LP_AON_CTRL_M  (LP_TEE_READ_TEE_LP_AON_CTRL_V << LP_TEE_READ_TEE_LP_AON_CTRL_S)
#define LP_TEE_READ_TEE_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_READ_TEE_LP_AON_CTRL_S  0
/** LP_TEE_READ_REE0_LP_AON_CTRL : R/W; bitpos: [1]; default: 0;
 *  Configures lp_aon_ctrl registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_AON_CTRL    (BIT(1))
#define LP_TEE_READ_REE0_LP_AON_CTRL_M  (LP_TEE_READ_REE0_LP_AON_CTRL_V << LP_TEE_READ_REE0_LP_AON_CTRL_S)
#define LP_TEE_READ_REE0_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_READ_REE0_LP_AON_CTRL_S  1
/** LP_TEE_READ_REE1_LP_AON_CTRL : R/W; bitpos: [2]; default: 0;
 *  Configures lp_aon_ctrl registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_AON_CTRL    (BIT(2))
#define LP_TEE_READ_REE1_LP_AON_CTRL_M  (LP_TEE_READ_REE1_LP_AON_CTRL_V << LP_TEE_READ_REE1_LP_AON_CTRL_S)
#define LP_TEE_READ_REE1_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_READ_REE1_LP_AON_CTRL_S  2
/** LP_TEE_READ_REE2_LP_AON_CTRL : R/W; bitpos: [3]; default: 0;
 *  Configures lp_aon_ctrl registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_AON_CTRL    (BIT(3))
#define LP_TEE_READ_REE2_LP_AON_CTRL_M  (LP_TEE_READ_REE2_LP_AON_CTRL_V << LP_TEE_READ_REE2_LP_AON_CTRL_S)
#define LP_TEE_READ_REE2_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_READ_REE2_LP_AON_CTRL_S  3
/** LP_TEE_WRITE_TEE_LP_AON_CTRL : R/W; bitpos: [4]; default: 1;
 *  Configures lp_aon_ctrl registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_AON_CTRL    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_AON_CTRL_M  (LP_TEE_WRITE_TEE_LP_AON_CTRL_V << LP_TEE_WRITE_TEE_LP_AON_CTRL_S)
#define LP_TEE_WRITE_TEE_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_AON_CTRL_S  4
/** LP_TEE_WRITE_REE0_LP_AON_CTRL : R/W; bitpos: [5]; default: 0;
 *  Configures lp_aon_ctrl registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_AON_CTRL    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_AON_CTRL_M  (LP_TEE_WRITE_REE0_LP_AON_CTRL_V << LP_TEE_WRITE_REE0_LP_AON_CTRL_S)
#define LP_TEE_WRITE_REE0_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_AON_CTRL_S  5
/** LP_TEE_WRITE_REE1_LP_AON_CTRL : R/W; bitpos: [6]; default: 0;
 *  Configures lp_aon_ctrl registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_AON_CTRL    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_AON_CTRL_M  (LP_TEE_WRITE_REE1_LP_AON_CTRL_V << LP_TEE_WRITE_REE1_LP_AON_CTRL_S)
#define LP_TEE_WRITE_REE1_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_AON_CTRL_S  6
/** LP_TEE_WRITE_REE2_LP_AON_CTRL : R/W; bitpos: [7]; default: 0;
 *  Configures lp_aon_ctrl registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_AON_CTRL    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_AON_CTRL_M  (LP_TEE_WRITE_REE2_LP_AON_CTRL_V << LP_TEE_WRITE_REE2_LP_AON_CTRL_S)
#define LP_TEE_WRITE_REE2_LP_AON_CTRL_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_AON_CTRL_S  7

/** LP_TEE_LP_TIMER_CTRL_REG register
 *  LP_TIMER read/write control register
 */
#define LP_TEE_LP_TIMER_CTRL_REG (DR_REG_LP_TEE_BASE + 0x14)
/** LP_TEE_READ_TEE_LP_TIMER : R/W; bitpos: [0]; default: 1;
 *  Configures lp_timer registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_TIMER    (BIT(0))
#define LP_TEE_READ_TEE_LP_TIMER_M  (LP_TEE_READ_TEE_LP_TIMER_V << LP_TEE_READ_TEE_LP_TIMER_S)
#define LP_TEE_READ_TEE_LP_TIMER_V  0x00000001U
#define LP_TEE_READ_TEE_LP_TIMER_S  0
/** LP_TEE_READ_REE0_LP_TIMER : R/W; bitpos: [1]; default: 0;
 *  Configures lp_timer registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_TIMER    (BIT(1))
#define LP_TEE_READ_REE0_LP_TIMER_M  (LP_TEE_READ_REE0_LP_TIMER_V << LP_TEE_READ_REE0_LP_TIMER_S)
#define LP_TEE_READ_REE0_LP_TIMER_V  0x00000001U
#define LP_TEE_READ_REE0_LP_TIMER_S  1
/** LP_TEE_READ_REE1_LP_TIMER : R/W; bitpos: [2]; default: 0;
 *  Configures lp_timer registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_TIMER    (BIT(2))
#define LP_TEE_READ_REE1_LP_TIMER_M  (LP_TEE_READ_REE1_LP_TIMER_V << LP_TEE_READ_REE1_LP_TIMER_S)
#define LP_TEE_READ_REE1_LP_TIMER_V  0x00000001U
#define LP_TEE_READ_REE1_LP_TIMER_S  2
/** LP_TEE_READ_REE2_LP_TIMER : R/W; bitpos: [3]; default: 0;
 *  Configures lp_timer registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_TIMER    (BIT(3))
#define LP_TEE_READ_REE2_LP_TIMER_M  (LP_TEE_READ_REE2_LP_TIMER_V << LP_TEE_READ_REE2_LP_TIMER_S)
#define LP_TEE_READ_REE2_LP_TIMER_V  0x00000001U
#define LP_TEE_READ_REE2_LP_TIMER_S  3
/** LP_TEE_WRITE_TEE_LP_TIMER : R/W; bitpos: [4]; default: 1;
 *  Configures lp_timer registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_TIMER    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_TIMER_M  (LP_TEE_WRITE_TEE_LP_TIMER_V << LP_TEE_WRITE_TEE_LP_TIMER_S)
#define LP_TEE_WRITE_TEE_LP_TIMER_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_TIMER_S  4
/** LP_TEE_WRITE_REE0_LP_TIMER : R/W; bitpos: [5]; default: 0;
 *  Configures lp_timer registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_TIMER    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_TIMER_M  (LP_TEE_WRITE_REE0_LP_TIMER_V << LP_TEE_WRITE_REE0_LP_TIMER_S)
#define LP_TEE_WRITE_REE0_LP_TIMER_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_TIMER_S  5
/** LP_TEE_WRITE_REE1_LP_TIMER : R/W; bitpos: [6]; default: 0;
 *  Configures lp_timer registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_TIMER    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_TIMER_M  (LP_TEE_WRITE_REE1_LP_TIMER_V << LP_TEE_WRITE_REE1_LP_TIMER_S)
#define LP_TEE_WRITE_REE1_LP_TIMER_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_TIMER_S  6
/** LP_TEE_WRITE_REE2_LP_TIMER : R/W; bitpos: [7]; default: 0;
 *  Configures lp_timer registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_TIMER    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_TIMER_M  (LP_TEE_WRITE_REE2_LP_TIMER_V << LP_TEE_WRITE_REE2_LP_TIMER_S)
#define LP_TEE_WRITE_REE2_LP_TIMER_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_TIMER_S  7

/** LP_TEE_LP_WDT_CTRL_REG register
 *  LP_WDT read/write control register
 */
#define LP_TEE_LP_WDT_CTRL_REG (DR_REG_LP_TEE_BASE + 0x18)
/** LP_TEE_READ_TEE_LP_WDT : R/W; bitpos: [0]; default: 1;
 *  Configures lp_wdt registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_WDT    (BIT(0))
#define LP_TEE_READ_TEE_LP_WDT_M  (LP_TEE_READ_TEE_LP_WDT_V << LP_TEE_READ_TEE_LP_WDT_S)
#define LP_TEE_READ_TEE_LP_WDT_V  0x00000001U
#define LP_TEE_READ_TEE_LP_WDT_S  0
/** LP_TEE_READ_REE0_LP_WDT : R/W; bitpos: [1]; default: 0;
 *  Configures lp_wdt registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_WDT    (BIT(1))
#define LP_TEE_READ_REE0_LP_WDT_M  (LP_TEE_READ_REE0_LP_WDT_V << LP_TEE_READ_REE0_LP_WDT_S)
#define LP_TEE_READ_REE0_LP_WDT_V  0x00000001U
#define LP_TEE_READ_REE0_LP_WDT_S  1
/** LP_TEE_READ_REE1_LP_WDT : R/W; bitpos: [2]; default: 0;
 *  Configures lp_wdt registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_WDT    (BIT(2))
#define LP_TEE_READ_REE1_LP_WDT_M  (LP_TEE_READ_REE1_LP_WDT_V << LP_TEE_READ_REE1_LP_WDT_S)
#define LP_TEE_READ_REE1_LP_WDT_V  0x00000001U
#define LP_TEE_READ_REE1_LP_WDT_S  2
/** LP_TEE_READ_REE2_LP_WDT : R/W; bitpos: [3]; default: 0;
 *  Configures lp_wdt registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_WDT    (BIT(3))
#define LP_TEE_READ_REE2_LP_WDT_M  (LP_TEE_READ_REE2_LP_WDT_V << LP_TEE_READ_REE2_LP_WDT_S)
#define LP_TEE_READ_REE2_LP_WDT_V  0x00000001U
#define LP_TEE_READ_REE2_LP_WDT_S  3
/** LP_TEE_WRITE_TEE_LP_WDT : R/W; bitpos: [4]; default: 1;
 *  Configures lp_wdt registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_WDT    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_WDT_M  (LP_TEE_WRITE_TEE_LP_WDT_V << LP_TEE_WRITE_TEE_LP_WDT_S)
#define LP_TEE_WRITE_TEE_LP_WDT_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_WDT_S  4
/** LP_TEE_WRITE_REE0_LP_WDT : R/W; bitpos: [5]; default: 0;
 *  Configures lp_wdt registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_WDT    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_WDT_M  (LP_TEE_WRITE_REE0_LP_WDT_V << LP_TEE_WRITE_REE0_LP_WDT_S)
#define LP_TEE_WRITE_REE0_LP_WDT_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_WDT_S  5
/** LP_TEE_WRITE_REE1_LP_WDT : R/W; bitpos: [6]; default: 0;
 *  Configures lp_wdt registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_WDT    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_WDT_M  (LP_TEE_WRITE_REE1_LP_WDT_V << LP_TEE_WRITE_REE1_LP_WDT_S)
#define LP_TEE_WRITE_REE1_LP_WDT_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_WDT_S  6
/** LP_TEE_WRITE_REE2_LP_WDT : R/W; bitpos: [7]; default: 0;
 *  Configures lp_wdt registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_WDT    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_WDT_M  (LP_TEE_WRITE_REE2_LP_WDT_V << LP_TEE_WRITE_REE2_LP_WDT_S)
#define LP_TEE_WRITE_REE2_LP_WDT_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_WDT_S  7

/** LP_TEE_LP_PERI_CTRL_REG register
 *  LPPERI read/write control register
 */
#define LP_TEE_LP_PERI_CTRL_REG (DR_REG_LP_TEE_BASE + 0x1c)
/** LP_TEE_READ_TEE_LP_PERI : R/W; bitpos: [0]; default: 1;
 *  Configures lp_peri registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_PERI    (BIT(0))
#define LP_TEE_READ_TEE_LP_PERI_M  (LP_TEE_READ_TEE_LP_PERI_V << LP_TEE_READ_TEE_LP_PERI_S)
#define LP_TEE_READ_TEE_LP_PERI_V  0x00000001U
#define LP_TEE_READ_TEE_LP_PERI_S  0
/** LP_TEE_READ_REE0_LP_PERI : R/W; bitpos: [1]; default: 0;
 *  Configures lp_peri registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_PERI    (BIT(1))
#define LP_TEE_READ_REE0_LP_PERI_M  (LP_TEE_READ_REE0_LP_PERI_V << LP_TEE_READ_REE0_LP_PERI_S)
#define LP_TEE_READ_REE0_LP_PERI_V  0x00000001U
#define LP_TEE_READ_REE0_LP_PERI_S  1
/** LP_TEE_READ_REE1_LP_PERI : R/W; bitpos: [2]; default: 0;
 *  Configures lp_peri registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_PERI    (BIT(2))
#define LP_TEE_READ_REE1_LP_PERI_M  (LP_TEE_READ_REE1_LP_PERI_V << LP_TEE_READ_REE1_LP_PERI_S)
#define LP_TEE_READ_REE1_LP_PERI_V  0x00000001U
#define LP_TEE_READ_REE1_LP_PERI_S  2
/** LP_TEE_READ_REE2_LP_PERI : R/W; bitpos: [3]; default: 0;
 *  Configures lp_peri registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_PERI    (BIT(3))
#define LP_TEE_READ_REE2_LP_PERI_M  (LP_TEE_READ_REE2_LP_PERI_V << LP_TEE_READ_REE2_LP_PERI_S)
#define LP_TEE_READ_REE2_LP_PERI_V  0x00000001U
#define LP_TEE_READ_REE2_LP_PERI_S  3
/** LP_TEE_WRITE_TEE_LP_PERI : R/W; bitpos: [4]; default: 1;
 *  Configures lp_peri registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_PERI    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_PERI_M  (LP_TEE_WRITE_TEE_LP_PERI_V << LP_TEE_WRITE_TEE_LP_PERI_S)
#define LP_TEE_WRITE_TEE_LP_PERI_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_PERI_S  4
/** LP_TEE_WRITE_REE0_LP_PERI : R/W; bitpos: [5]; default: 0;
 *  Configures lp_peri registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_PERI    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_PERI_M  (LP_TEE_WRITE_REE0_LP_PERI_V << LP_TEE_WRITE_REE0_LP_PERI_S)
#define LP_TEE_WRITE_REE0_LP_PERI_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_PERI_S  5
/** LP_TEE_WRITE_REE1_LP_PERI : R/W; bitpos: [6]; default: 0;
 *  Configures lp_peri registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_PERI    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_PERI_M  (LP_TEE_WRITE_REE1_LP_PERI_V << LP_TEE_WRITE_REE1_LP_PERI_S)
#define LP_TEE_WRITE_REE1_LP_PERI_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_PERI_S  6
/** LP_TEE_WRITE_REE2_LP_PERI : R/W; bitpos: [7]; default: 0;
 *  Configures lp_peri registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_PERI    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_PERI_M  (LP_TEE_WRITE_REE2_LP_PERI_V << LP_TEE_WRITE_REE2_LP_PERI_S)
#define LP_TEE_WRITE_REE2_LP_PERI_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_PERI_S  7

/** LP_TEE_LP_ANA_PERI_CTRL_REG register
 *  LP_ANA_PERI read/write control register
 */
#define LP_TEE_LP_ANA_PERI_CTRL_REG (DR_REG_LP_TEE_BASE + 0x20)
/** LP_TEE_READ_TEE_LP_ANA_PERI : R/W; bitpos: [0]; default: 1;
 *  Configures lp_ana_peri registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_ANA_PERI    (BIT(0))
#define LP_TEE_READ_TEE_LP_ANA_PERI_M  (LP_TEE_READ_TEE_LP_ANA_PERI_V << LP_TEE_READ_TEE_LP_ANA_PERI_S)
#define LP_TEE_READ_TEE_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_READ_TEE_LP_ANA_PERI_S  0
/** LP_TEE_READ_REE0_LP_ANA_PERI : R/W; bitpos: [1]; default: 0;
 *  Configures lp_ana_peri registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_ANA_PERI    (BIT(1))
#define LP_TEE_READ_REE0_LP_ANA_PERI_M  (LP_TEE_READ_REE0_LP_ANA_PERI_V << LP_TEE_READ_REE0_LP_ANA_PERI_S)
#define LP_TEE_READ_REE0_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_READ_REE0_LP_ANA_PERI_S  1
/** LP_TEE_READ_REE1_LP_ANA_PERI : R/W; bitpos: [2]; default: 0;
 *  Configures lp_ana_peri registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_ANA_PERI    (BIT(2))
#define LP_TEE_READ_REE1_LP_ANA_PERI_M  (LP_TEE_READ_REE1_LP_ANA_PERI_V << LP_TEE_READ_REE1_LP_ANA_PERI_S)
#define LP_TEE_READ_REE1_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_READ_REE1_LP_ANA_PERI_S  2
/** LP_TEE_READ_REE2_LP_ANA_PERI : R/W; bitpos: [3]; default: 0;
 *  Configures lp_ana_peri registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_ANA_PERI    (BIT(3))
#define LP_TEE_READ_REE2_LP_ANA_PERI_M  (LP_TEE_READ_REE2_LP_ANA_PERI_V << LP_TEE_READ_REE2_LP_ANA_PERI_S)
#define LP_TEE_READ_REE2_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_READ_REE2_LP_ANA_PERI_S  3
/** LP_TEE_WRITE_TEE_LP_ANA_PERI : R/W; bitpos: [4]; default: 1;
 *  Configures lp_ana_peri registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_ANA_PERI    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_ANA_PERI_M  (LP_TEE_WRITE_TEE_LP_ANA_PERI_V << LP_TEE_WRITE_TEE_LP_ANA_PERI_S)
#define LP_TEE_WRITE_TEE_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_ANA_PERI_S  4
/** LP_TEE_WRITE_REE0_LP_ANA_PERI : R/W; bitpos: [5]; default: 0;
 *  Configures lp_ana_peri registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_ANA_PERI    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_ANA_PERI_M  (LP_TEE_WRITE_REE0_LP_ANA_PERI_V << LP_TEE_WRITE_REE0_LP_ANA_PERI_S)
#define LP_TEE_WRITE_REE0_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_ANA_PERI_S  5
/** LP_TEE_WRITE_REE1_LP_ANA_PERI : R/W; bitpos: [6]; default: 0;
 *  Configures lp_ana_peri registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_ANA_PERI    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_ANA_PERI_M  (LP_TEE_WRITE_REE1_LP_ANA_PERI_V << LP_TEE_WRITE_REE1_LP_ANA_PERI_S)
#define LP_TEE_WRITE_REE1_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_ANA_PERI_S  6
/** LP_TEE_WRITE_REE2_LP_ANA_PERI : R/W; bitpos: [7]; default: 0;
 *  Configures lp_ana_peri registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_ANA_PERI    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_ANA_PERI_M  (LP_TEE_WRITE_REE2_LP_ANA_PERI_V << LP_TEE_WRITE_REE2_LP_ANA_PERI_S)
#define LP_TEE_WRITE_REE2_LP_ANA_PERI_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_ANA_PERI_S  7

/** LP_TEE_LP_IO_CTRL_REG register
 *  LP_GPIO and LP_IO_MUX read/write control register
 */
#define LP_TEE_LP_IO_CTRL_REG (DR_REG_LP_TEE_BASE + 0x2c)
/** LP_TEE_READ_TEE_LP_IO : R/W; bitpos: [0]; default: 1;
 *  Configures lp_io registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_IO    (BIT(0))
#define LP_TEE_READ_TEE_LP_IO_M  (LP_TEE_READ_TEE_LP_IO_V << LP_TEE_READ_TEE_LP_IO_S)
#define LP_TEE_READ_TEE_LP_IO_V  0x00000001U
#define LP_TEE_READ_TEE_LP_IO_S  0
/** LP_TEE_READ_REE0_LP_IO : R/W; bitpos: [1]; default: 0;
 *  Configures lp_io registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_IO    (BIT(1))
#define LP_TEE_READ_REE0_LP_IO_M  (LP_TEE_READ_REE0_LP_IO_V << LP_TEE_READ_REE0_LP_IO_S)
#define LP_TEE_READ_REE0_LP_IO_V  0x00000001U
#define LP_TEE_READ_REE0_LP_IO_S  1
/** LP_TEE_READ_REE1_LP_IO : R/W; bitpos: [2]; default: 0;
 *  Configures lp_io registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_IO    (BIT(2))
#define LP_TEE_READ_REE1_LP_IO_M  (LP_TEE_READ_REE1_LP_IO_V << LP_TEE_READ_REE1_LP_IO_S)
#define LP_TEE_READ_REE1_LP_IO_V  0x00000001U
#define LP_TEE_READ_REE1_LP_IO_S  2
/** LP_TEE_READ_REE2_LP_IO : R/W; bitpos: [3]; default: 0;
 *  Configures lp_io registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_IO    (BIT(3))
#define LP_TEE_READ_REE2_LP_IO_M  (LP_TEE_READ_REE2_LP_IO_V << LP_TEE_READ_REE2_LP_IO_S)
#define LP_TEE_READ_REE2_LP_IO_V  0x00000001U
#define LP_TEE_READ_REE2_LP_IO_S  3
/** LP_TEE_WRITE_TEE_LP_IO : R/W; bitpos: [4]; default: 1;
 *  Configures lp_io registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_IO    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_IO_M  (LP_TEE_WRITE_TEE_LP_IO_V << LP_TEE_WRITE_TEE_LP_IO_S)
#define LP_TEE_WRITE_TEE_LP_IO_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_IO_S  4
/** LP_TEE_WRITE_REE0_LP_IO : R/W; bitpos: [5]; default: 0;
 *  Configures lp_io registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_IO    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_IO_M  (LP_TEE_WRITE_REE0_LP_IO_V << LP_TEE_WRITE_REE0_LP_IO_S)
#define LP_TEE_WRITE_REE0_LP_IO_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_IO_S  5
/** LP_TEE_WRITE_REE1_LP_IO : R/W; bitpos: [6]; default: 0;
 *  Configures lp_io registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_IO    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_IO_M  (LP_TEE_WRITE_REE1_LP_IO_V << LP_TEE_WRITE_REE1_LP_IO_S)
#define LP_TEE_WRITE_REE1_LP_IO_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_IO_S  6
/** LP_TEE_WRITE_REE2_LP_IO : R/W; bitpos: [7]; default: 0;
 *  Configures lp_io registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_IO    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_IO_M  (LP_TEE_WRITE_REE2_LP_IO_V << LP_TEE_WRITE_REE2_LP_IO_S)
#define LP_TEE_WRITE_REE2_LP_IO_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_IO_S  7

/** LP_TEE_LP_TEE_CTRL_REG register
 *  LP_TEE read/write control register
 */
#define LP_TEE_LP_TEE_CTRL_REG (DR_REG_LP_TEE_BASE + 0x34)
/** LP_TEE_READ_TEE_LP_TEE : R/W; bitpos: [0]; default: 1;
 *  Configures lp_tee registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_TEE    (BIT(0))
#define LP_TEE_READ_TEE_LP_TEE_M  (LP_TEE_READ_TEE_LP_TEE_V << LP_TEE_READ_TEE_LP_TEE_S)
#define LP_TEE_READ_TEE_LP_TEE_V  0x00000001U
#define LP_TEE_READ_TEE_LP_TEE_S  0
/** LP_TEE_READ_REE0_LP_TEE : HRO; bitpos: [1]; default: 0;
 *  Configures lp_tee registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_TEE    (BIT(1))
#define LP_TEE_READ_REE0_LP_TEE_M  (LP_TEE_READ_REE0_LP_TEE_V << LP_TEE_READ_REE0_LP_TEE_S)
#define LP_TEE_READ_REE0_LP_TEE_V  0x00000001U
#define LP_TEE_READ_REE0_LP_TEE_S  1
/** LP_TEE_READ_REE1_LP_TEE : HRO; bitpos: [2]; default: 0;
 *  Configures lp_tee registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_TEE    (BIT(2))
#define LP_TEE_READ_REE1_LP_TEE_M  (LP_TEE_READ_REE1_LP_TEE_V << LP_TEE_READ_REE1_LP_TEE_S)
#define LP_TEE_READ_REE1_LP_TEE_V  0x00000001U
#define LP_TEE_READ_REE1_LP_TEE_S  2
/** LP_TEE_READ_REE2_LP_TEE : HRO; bitpos: [3]; default: 0;
 *  Configures lp_tee registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_TEE    (BIT(3))
#define LP_TEE_READ_REE2_LP_TEE_M  (LP_TEE_READ_REE2_LP_TEE_V << LP_TEE_READ_REE2_LP_TEE_S)
#define LP_TEE_READ_REE2_LP_TEE_V  0x00000001U
#define LP_TEE_READ_REE2_LP_TEE_S  3
/** LP_TEE_WRITE_TEE_LP_TEE : R/W; bitpos: [4]; default: 1;
 *  Configures lp_tee registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_TEE    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_TEE_M  (LP_TEE_WRITE_TEE_LP_TEE_V << LP_TEE_WRITE_TEE_LP_TEE_S)
#define LP_TEE_WRITE_TEE_LP_TEE_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_TEE_S  4
/** LP_TEE_WRITE_REE0_LP_TEE : HRO; bitpos: [5]; default: 0;
 *  Configures lp_tee registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_TEE    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_TEE_M  (LP_TEE_WRITE_REE0_LP_TEE_V << LP_TEE_WRITE_REE0_LP_TEE_S)
#define LP_TEE_WRITE_REE0_LP_TEE_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_TEE_S  5
/** LP_TEE_WRITE_REE1_LP_TEE : HRO; bitpos: [6]; default: 0;
 *  Configures lp_tee registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_TEE    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_TEE_M  (LP_TEE_WRITE_REE1_LP_TEE_V << LP_TEE_WRITE_REE1_LP_TEE_S)
#define LP_TEE_WRITE_REE1_LP_TEE_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_TEE_S  6
/** LP_TEE_WRITE_REE2_LP_TEE : HRO; bitpos: [7]; default: 0;
 *  Configures lp_tee registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_TEE    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_TEE_M  (LP_TEE_WRITE_REE2_LP_TEE_V << LP_TEE_WRITE_REE2_LP_TEE_S)
#define LP_TEE_WRITE_REE2_LP_TEE_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_TEE_S  7

/** LP_TEE_UART_CTRL_REG register
 *  LP_UART read/write control register
 */
#define LP_TEE_UART_CTRL_REG (DR_REG_LP_TEE_BASE + 0x38)
/** LP_TEE_READ_TEE_UART : R/W; bitpos: [0]; default: 1;
 *  Configures uart registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_UART    (BIT(0))
#define LP_TEE_READ_TEE_UART_M  (LP_TEE_READ_TEE_UART_V << LP_TEE_READ_TEE_UART_S)
#define LP_TEE_READ_TEE_UART_V  0x00000001U
#define LP_TEE_READ_TEE_UART_S  0
/** LP_TEE_READ_REE0_UART : R/W; bitpos: [1]; default: 0;
 *  Configures uart registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_UART    (BIT(1))
#define LP_TEE_READ_REE0_UART_M  (LP_TEE_READ_REE0_UART_V << LP_TEE_READ_REE0_UART_S)
#define LP_TEE_READ_REE0_UART_V  0x00000001U
#define LP_TEE_READ_REE0_UART_S  1
/** LP_TEE_READ_REE1_UART : R/W; bitpos: [2]; default: 0;
 *  Configures uart registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_UART    (BIT(2))
#define LP_TEE_READ_REE1_UART_M  (LP_TEE_READ_REE1_UART_V << LP_TEE_READ_REE1_UART_S)
#define LP_TEE_READ_REE1_UART_V  0x00000001U
#define LP_TEE_READ_REE1_UART_S  2
/** LP_TEE_READ_REE2_UART : R/W; bitpos: [3]; default: 0;
 *  Configures uart registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_UART    (BIT(3))
#define LP_TEE_READ_REE2_UART_M  (LP_TEE_READ_REE2_UART_V << LP_TEE_READ_REE2_UART_S)
#define LP_TEE_READ_REE2_UART_V  0x00000001U
#define LP_TEE_READ_REE2_UART_S  3
/** LP_TEE_WRITE_TEE_UART : R/W; bitpos: [4]; default: 1;
 *  Configures uart registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_UART    (BIT(4))
#define LP_TEE_WRITE_TEE_UART_M  (LP_TEE_WRITE_TEE_UART_V << LP_TEE_WRITE_TEE_UART_S)
#define LP_TEE_WRITE_TEE_UART_V  0x00000001U
#define LP_TEE_WRITE_TEE_UART_S  4
/** LP_TEE_WRITE_REE0_UART : R/W; bitpos: [5]; default: 0;
 *  Configures uart registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_UART    (BIT(5))
#define LP_TEE_WRITE_REE0_UART_M  (LP_TEE_WRITE_REE0_UART_V << LP_TEE_WRITE_REE0_UART_S)
#define LP_TEE_WRITE_REE0_UART_V  0x00000001U
#define LP_TEE_WRITE_REE0_UART_S  5
/** LP_TEE_WRITE_REE1_UART : R/W; bitpos: [6]; default: 0;
 *  Configures uart registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_UART    (BIT(6))
#define LP_TEE_WRITE_REE1_UART_M  (LP_TEE_WRITE_REE1_UART_V << LP_TEE_WRITE_REE1_UART_S)
#define LP_TEE_WRITE_REE1_UART_V  0x00000001U
#define LP_TEE_WRITE_REE1_UART_S  6
/** LP_TEE_WRITE_REE2_UART : R/W; bitpos: [7]; default: 0;
 *  Configures uart registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_UART    (BIT(7))
#define LP_TEE_WRITE_REE2_UART_M  (LP_TEE_WRITE_REE2_UART_V << LP_TEE_WRITE_REE2_UART_S)
#define LP_TEE_WRITE_REE2_UART_V  0x00000001U
#define LP_TEE_WRITE_REE2_UART_S  7

/** LP_TEE_I2C_EXT_CTRL_REG register
 *  LP_I2C read/write control register
 */
#define LP_TEE_I2C_EXT_CTRL_REG (DR_REG_LP_TEE_BASE + 0x40)
/** LP_TEE_READ_TEE_I2C_EXT : R/W; bitpos: [0]; default: 1;
 *  Configures i2c_ext registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_I2C_EXT    (BIT(0))
#define LP_TEE_READ_TEE_I2C_EXT_M  (LP_TEE_READ_TEE_I2C_EXT_V << LP_TEE_READ_TEE_I2C_EXT_S)
#define LP_TEE_READ_TEE_I2C_EXT_V  0x00000001U
#define LP_TEE_READ_TEE_I2C_EXT_S  0
/** LP_TEE_READ_REE0_I2C_EXT : R/W; bitpos: [1]; default: 0;
 *  Configures i2c_ext registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_I2C_EXT    (BIT(1))
#define LP_TEE_READ_REE0_I2C_EXT_M  (LP_TEE_READ_REE0_I2C_EXT_V << LP_TEE_READ_REE0_I2C_EXT_S)
#define LP_TEE_READ_REE0_I2C_EXT_V  0x00000001U
#define LP_TEE_READ_REE0_I2C_EXT_S  1
/** LP_TEE_READ_REE1_I2C_EXT : R/W; bitpos: [2]; default: 0;
 *  Configures i2c_ext registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_I2C_EXT    (BIT(2))
#define LP_TEE_READ_REE1_I2C_EXT_M  (LP_TEE_READ_REE1_I2C_EXT_V << LP_TEE_READ_REE1_I2C_EXT_S)
#define LP_TEE_READ_REE1_I2C_EXT_V  0x00000001U
#define LP_TEE_READ_REE1_I2C_EXT_S  2
/** LP_TEE_READ_REE2_I2C_EXT : R/W; bitpos: [3]; default: 0;
 *  Configures i2c_ext registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_I2C_EXT    (BIT(3))
#define LP_TEE_READ_REE2_I2C_EXT_M  (LP_TEE_READ_REE2_I2C_EXT_V << LP_TEE_READ_REE2_I2C_EXT_S)
#define LP_TEE_READ_REE2_I2C_EXT_V  0x00000001U
#define LP_TEE_READ_REE2_I2C_EXT_S  3
/** LP_TEE_WRITE_TEE_I2C_EXT : R/W; bitpos: [4]; default: 1;
 *  Configures i2c_ext registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_I2C_EXT    (BIT(4))
#define LP_TEE_WRITE_TEE_I2C_EXT_M  (LP_TEE_WRITE_TEE_I2C_EXT_V << LP_TEE_WRITE_TEE_I2C_EXT_S)
#define LP_TEE_WRITE_TEE_I2C_EXT_V  0x00000001U
#define LP_TEE_WRITE_TEE_I2C_EXT_S  4
/** LP_TEE_WRITE_REE0_I2C_EXT : R/W; bitpos: [5]; default: 0;
 *  Configures i2c_ext registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_I2C_EXT    (BIT(5))
#define LP_TEE_WRITE_REE0_I2C_EXT_M  (LP_TEE_WRITE_REE0_I2C_EXT_V << LP_TEE_WRITE_REE0_I2C_EXT_S)
#define LP_TEE_WRITE_REE0_I2C_EXT_V  0x00000001U
#define LP_TEE_WRITE_REE0_I2C_EXT_S  5
/** LP_TEE_WRITE_REE1_I2C_EXT : R/W; bitpos: [6]; default: 0;
 *  Configures i2c_ext registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_I2C_EXT    (BIT(6))
#define LP_TEE_WRITE_REE1_I2C_EXT_M  (LP_TEE_WRITE_REE1_I2C_EXT_V << LP_TEE_WRITE_REE1_I2C_EXT_S)
#define LP_TEE_WRITE_REE1_I2C_EXT_V  0x00000001U
#define LP_TEE_WRITE_REE1_I2C_EXT_S  6
/** LP_TEE_WRITE_REE2_I2C_EXT : R/W; bitpos: [7]; default: 0;
 *  Configures i2c_ext registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_I2C_EXT    (BIT(7))
#define LP_TEE_WRITE_REE2_I2C_EXT_M  (LP_TEE_WRITE_REE2_I2C_EXT_V << LP_TEE_WRITE_REE2_I2C_EXT_S)
#define LP_TEE_WRITE_REE2_I2C_EXT_V  0x00000001U
#define LP_TEE_WRITE_REE2_I2C_EXT_S  7

/** LP_TEE_I2C_ANA_MST_CTRL_REG register
 *  I2C_ANA_MST read/write control register
 */
#define LP_TEE_I2C_ANA_MST_CTRL_REG (DR_REG_LP_TEE_BASE + 0x44)
/** LP_TEE_READ_TEE_I2C_ANA_MST : R/W; bitpos: [0]; default: 1;
 *  Configures i2c_ana_mst registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_I2C_ANA_MST    (BIT(0))
#define LP_TEE_READ_TEE_I2C_ANA_MST_M  (LP_TEE_READ_TEE_I2C_ANA_MST_V << LP_TEE_READ_TEE_I2C_ANA_MST_S)
#define LP_TEE_READ_TEE_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_READ_TEE_I2C_ANA_MST_S  0
/** LP_TEE_READ_REE0_I2C_ANA_MST : R/W; bitpos: [1]; default: 0;
 *  Configures i2c_ana_mst registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_I2C_ANA_MST    (BIT(1))
#define LP_TEE_READ_REE0_I2C_ANA_MST_M  (LP_TEE_READ_REE0_I2C_ANA_MST_V << LP_TEE_READ_REE0_I2C_ANA_MST_S)
#define LP_TEE_READ_REE0_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_READ_REE0_I2C_ANA_MST_S  1
/** LP_TEE_READ_REE1_I2C_ANA_MST : R/W; bitpos: [2]; default: 0;
 *  Configures i2c_ana_mst registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_I2C_ANA_MST    (BIT(2))
#define LP_TEE_READ_REE1_I2C_ANA_MST_M  (LP_TEE_READ_REE1_I2C_ANA_MST_V << LP_TEE_READ_REE1_I2C_ANA_MST_S)
#define LP_TEE_READ_REE1_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_READ_REE1_I2C_ANA_MST_S  2
/** LP_TEE_READ_REE2_I2C_ANA_MST : R/W; bitpos: [3]; default: 0;
 *  Configures i2c_ana_mst registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_I2C_ANA_MST    (BIT(3))
#define LP_TEE_READ_REE2_I2C_ANA_MST_M  (LP_TEE_READ_REE2_I2C_ANA_MST_V << LP_TEE_READ_REE2_I2C_ANA_MST_S)
#define LP_TEE_READ_REE2_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_READ_REE2_I2C_ANA_MST_S  3
/** LP_TEE_WRITE_TEE_I2C_ANA_MST : R/W; bitpos: [4]; default: 1;
 *  Configures i2c_ana_mst registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_I2C_ANA_MST    (BIT(4))
#define LP_TEE_WRITE_TEE_I2C_ANA_MST_M  (LP_TEE_WRITE_TEE_I2C_ANA_MST_V << LP_TEE_WRITE_TEE_I2C_ANA_MST_S)
#define LP_TEE_WRITE_TEE_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_WRITE_TEE_I2C_ANA_MST_S  4
/** LP_TEE_WRITE_REE0_I2C_ANA_MST : R/W; bitpos: [5]; default: 0;
 *  Configures i2c_ana_mst registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_I2C_ANA_MST    (BIT(5))
#define LP_TEE_WRITE_REE0_I2C_ANA_MST_M  (LP_TEE_WRITE_REE0_I2C_ANA_MST_V << LP_TEE_WRITE_REE0_I2C_ANA_MST_S)
#define LP_TEE_WRITE_REE0_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_WRITE_REE0_I2C_ANA_MST_S  5
/** LP_TEE_WRITE_REE1_I2C_ANA_MST : R/W; bitpos: [6]; default: 0;
 *  Configures i2c_ana_mst registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_I2C_ANA_MST    (BIT(6))
#define LP_TEE_WRITE_REE1_I2C_ANA_MST_M  (LP_TEE_WRITE_REE1_I2C_ANA_MST_V << LP_TEE_WRITE_REE1_I2C_ANA_MST_S)
#define LP_TEE_WRITE_REE1_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_WRITE_REE1_I2C_ANA_MST_S  6
/** LP_TEE_WRITE_REE2_I2C_ANA_MST : R/W; bitpos: [7]; default: 0;
 *  Configures i2c_ana_mst registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_I2C_ANA_MST    (BIT(7))
#define LP_TEE_WRITE_REE2_I2C_ANA_MST_M  (LP_TEE_WRITE_REE2_I2C_ANA_MST_V << LP_TEE_WRITE_REE2_I2C_ANA_MST_S)
#define LP_TEE_WRITE_REE2_I2C_ANA_MST_V  0x00000001U
#define LP_TEE_WRITE_REE2_I2C_ANA_MST_S  7

/** LP_TEE_LP_APM_CTRL_REG register
 *  LP_APM read/write control register
 */
#define LP_TEE_LP_APM_CTRL_REG (DR_REG_LP_TEE_BASE + 0x4c)
/** LP_TEE_READ_TEE_LP_APM : R/W; bitpos: [0]; default: 1;
 *  Configures lp_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_APM    (BIT(0))
#define LP_TEE_READ_TEE_LP_APM_M  (LP_TEE_READ_TEE_LP_APM_V << LP_TEE_READ_TEE_LP_APM_S)
#define LP_TEE_READ_TEE_LP_APM_V  0x00000001U
#define LP_TEE_READ_TEE_LP_APM_S  0
/** LP_TEE_READ_REE0_LP_APM : HRO; bitpos: [1]; default: 0;
 *  Configures lp_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_APM    (BIT(1))
#define LP_TEE_READ_REE0_LP_APM_M  (LP_TEE_READ_REE0_LP_APM_V << LP_TEE_READ_REE0_LP_APM_S)
#define LP_TEE_READ_REE0_LP_APM_V  0x00000001U
#define LP_TEE_READ_REE0_LP_APM_S  1
/** LP_TEE_READ_REE1_LP_APM : HRO; bitpos: [2]; default: 0;
 *  Configures lp_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_APM    (BIT(2))
#define LP_TEE_READ_REE1_LP_APM_M  (LP_TEE_READ_REE1_LP_APM_V << LP_TEE_READ_REE1_LP_APM_S)
#define LP_TEE_READ_REE1_LP_APM_V  0x00000001U
#define LP_TEE_READ_REE1_LP_APM_S  2
/** LP_TEE_READ_REE2_LP_APM : HRO; bitpos: [3]; default: 0;
 *  Configures lp_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_APM    (BIT(3))
#define LP_TEE_READ_REE2_LP_APM_M  (LP_TEE_READ_REE2_LP_APM_V << LP_TEE_READ_REE2_LP_APM_S)
#define LP_TEE_READ_REE2_LP_APM_V  0x00000001U
#define LP_TEE_READ_REE2_LP_APM_S  3
/** LP_TEE_WRITE_TEE_LP_APM : R/W; bitpos: [4]; default: 1;
 *  Configures lp_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_APM    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_APM_M  (LP_TEE_WRITE_TEE_LP_APM_V << LP_TEE_WRITE_TEE_LP_APM_S)
#define LP_TEE_WRITE_TEE_LP_APM_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_APM_S  4
/** LP_TEE_WRITE_REE0_LP_APM : HRO; bitpos: [5]; default: 0;
 *  Configures lp_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_APM    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_APM_M  (LP_TEE_WRITE_REE0_LP_APM_V << LP_TEE_WRITE_REE0_LP_APM_S)
#define LP_TEE_WRITE_REE0_LP_APM_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_APM_S  5
/** LP_TEE_WRITE_REE1_LP_APM : HRO; bitpos: [6]; default: 0;
 *  Configures lp_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_APM    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_APM_M  (LP_TEE_WRITE_REE1_LP_APM_V << LP_TEE_WRITE_REE1_LP_APM_S)
#define LP_TEE_WRITE_REE1_LP_APM_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_APM_S  6
/** LP_TEE_WRITE_REE2_LP_APM : HRO; bitpos: [7]; default: 0;
 *  Configures lp_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_APM    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_APM_M  (LP_TEE_WRITE_REE2_LP_APM_V << LP_TEE_WRITE_REE2_LP_APM_S)
#define LP_TEE_WRITE_REE2_LP_APM_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_APM_S  7

/** LP_TEE_FORCE_ACC_HP_REG register
 *  Force access to HP SRAM configuration register
 */
#define LP_TEE_FORCE_ACC_HP_REG (DR_REG_LP_TEE_BASE + 0x90)
/** LP_TEE_FORCE_ACC_HPMEM_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether to allow LP CPU to forcibly access HP SRAM regardless of
 *  permission management.
 *  0: Disable force access to HP SRAM
 *  1: Enable force access to HP SRAM
 */
#define LP_TEE_FORCE_ACC_HPMEM_EN    (BIT(0))
#define LP_TEE_FORCE_ACC_HPMEM_EN_M  (LP_TEE_FORCE_ACC_HPMEM_EN_V << LP_TEE_FORCE_ACC_HPMEM_EN_S)
#define LP_TEE_FORCE_ACC_HPMEM_EN_V  0x00000001U
#define LP_TEE_FORCE_ACC_HPMEM_EN_S  0

/** LP_TEE_BUS_ERR_CONF_REG register
 *  Error message return configuration register
 */
#define LP_TEE_BUS_ERR_CONF_REG (DR_REG_LP_TEE_BASE + 0xf0)
/** LP_TEE_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether to return error message to CPU when access is blocked.
 *  0: Disable
 *  1: Enable
 */
#define LP_TEE_BUS_ERR_RESP_EN    (BIT(0))
#define LP_TEE_BUS_ERR_RESP_EN_M  (LP_TEE_BUS_ERR_RESP_EN_V << LP_TEE_BUS_ERR_RESP_EN_S)
#define LP_TEE_BUS_ERR_RESP_EN_V  0x00000001U
#define LP_TEE_BUS_ERR_RESP_EN_S  0

/** LP_TEE_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define LP_TEE_CLOCK_GATE_REG (DR_REG_LP_TEE_BASE + 0xf8)
/** LP_TEE_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: Enable automatic clock gating
 *  1: Keep the clock always on
 */
#define LP_TEE_CLK_EN    (BIT(0))
#define LP_TEE_CLK_EN_M  (LP_TEE_CLK_EN_V << LP_TEE_CLK_EN_S)
#define LP_TEE_CLK_EN_V  0x00000001U
#define LP_TEE_CLK_EN_S  0

/** LP_TEE_DATE_REG register
 *  Version control register
 */
#define LP_TEE_DATE_REG (DR_REG_LP_TEE_BASE + 0xfc)
/** LP_TEE_DATE : R/W; bitpos: [27:0]; default: 2363416;
 *  Version control register.
 */
#define LP_TEE_DATE    0x0FFFFFFFU
#define LP_TEE_DATE_M  (LP_TEE_DATE_V << LP_TEE_DATE_S)
#define LP_TEE_DATE_V  0x0FFFFFFFU
#define LP_TEE_DATE_S  0

#ifdef __cplusplus
}
#endif
