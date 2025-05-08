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

/** LP_TEE_TRNG_CTRL_REG register
 *  trng read/write control register
 */
#define LP_TEE_TRNG_CTRL_REG (DR_REG_LP_BASE + 0x0)
/** LP_TEE_READ_TEE_TRNG : R/W; bitpos: [0]; default: 1;
 *  Configures trng registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_TRNG    (BIT(0))
#define LP_TEE_READ_TEE_TRNG_M  (LP_TEE_READ_TEE_TRNG_V << LP_TEE_READ_TEE_TRNG_S)
#define LP_TEE_READ_TEE_TRNG_V  0x00000001U
#define LP_TEE_READ_TEE_TRNG_S  0
/** LP_TEE_READ_REE0_TRNG : R/W; bitpos: [1]; default: 0;
 *  Configures trng registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_TRNG    (BIT(1))
#define LP_TEE_READ_REE0_TRNG_M  (LP_TEE_READ_REE0_TRNG_V << LP_TEE_READ_REE0_TRNG_S)
#define LP_TEE_READ_REE0_TRNG_V  0x00000001U
#define LP_TEE_READ_REE0_TRNG_S  1
/** LP_TEE_READ_REE1_TRNG : R/W; bitpos: [2]; default: 0;
 *  Configures trng registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_TRNG    (BIT(2))
#define LP_TEE_READ_REE1_TRNG_M  (LP_TEE_READ_REE1_TRNG_V << LP_TEE_READ_REE1_TRNG_S)
#define LP_TEE_READ_REE1_TRNG_V  0x00000001U
#define LP_TEE_READ_REE1_TRNG_S  2
/** LP_TEE_READ_REE2_TRNG : R/W; bitpos: [3]; default: 0;
 *  Configures trng registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_TRNG    (BIT(3))
#define LP_TEE_READ_REE2_TRNG_M  (LP_TEE_READ_REE2_TRNG_V << LP_TEE_READ_REE2_TRNG_S)
#define LP_TEE_READ_REE2_TRNG_V  0x00000001U
#define LP_TEE_READ_REE2_TRNG_S  3
/** LP_TEE_WRITE_TEE_TRNG : R/W; bitpos: [4]; default: 1;
 *  Configures trng registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_TRNG    (BIT(4))
#define LP_TEE_WRITE_TEE_TRNG_M  (LP_TEE_WRITE_TEE_TRNG_V << LP_TEE_WRITE_TEE_TRNG_S)
#define LP_TEE_WRITE_TEE_TRNG_V  0x00000001U
#define LP_TEE_WRITE_TEE_TRNG_S  4
/** LP_TEE_WRITE_REE0_TRNG : R/W; bitpos: [5]; default: 0;
 *  Configures trng registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_TRNG    (BIT(5))
#define LP_TEE_WRITE_REE0_TRNG_M  (LP_TEE_WRITE_REE0_TRNG_V << LP_TEE_WRITE_REE0_TRNG_S)
#define LP_TEE_WRITE_REE0_TRNG_V  0x00000001U
#define LP_TEE_WRITE_REE0_TRNG_S  5
/** LP_TEE_WRITE_REE1_TRNG : R/W; bitpos: [6]; default: 0;
 *  Configures trng registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_TRNG    (BIT(6))
#define LP_TEE_WRITE_REE1_TRNG_M  (LP_TEE_WRITE_REE1_TRNG_V << LP_TEE_WRITE_REE1_TRNG_S)
#define LP_TEE_WRITE_REE1_TRNG_V  0x00000001U
#define LP_TEE_WRITE_REE1_TRNG_S  6
/** LP_TEE_WRITE_REE2_TRNG : R/W; bitpos: [7]; default: 0;
 *  Configures trng registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_TRNG    (BIT(7))
#define LP_TEE_WRITE_REE2_TRNG_M  (LP_TEE_WRITE_REE2_TRNG_V << LP_TEE_WRITE_REE2_TRNG_S)
#define LP_TEE_WRITE_REE2_TRNG_V  0x00000001U
#define LP_TEE_WRITE_REE2_TRNG_S  7

/** LP_TEE_EFUSE_CTRL_REG register
 *  efuse read/write control register
 */
#define LP_TEE_EFUSE_CTRL_REG (DR_REG_LP_BASE + 0x4)
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
 *  pmu read/write control register
 */
#define LP_TEE_PMU_CTRL_REG (DR_REG_LP_BASE + 0x8)
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
 *  clkrst read/write control register
 */
#define LP_TEE_CLKRST_CTRL_REG (DR_REG_LP_BASE + 0xc)
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
 *  lp_aon_ctrl read/write control register
 */
#define LP_TEE_LP_AON_CTRL_CTRL_REG (DR_REG_LP_BASE + 0x10)
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
 *  lp_timer read/write control register
 */
#define LP_TEE_LP_TIMER_CTRL_REG (DR_REG_LP_BASE + 0x14)
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
 *  lp_wdt read/write control register
 */
#define LP_TEE_LP_WDT_CTRL_REG (DR_REG_LP_BASE + 0x18)
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

/** LP_TEE_LPPERI_CTRL_REG register
 *  lpperi read/write control register
 */
#define LP_TEE_LPPERI_CTRL_REG (DR_REG_LP_BASE + 0x1c)
/** LP_TEE_READ_TEE_LPPERI : R/W; bitpos: [0]; default: 1;
 *  Configures lpperi registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LPPERI    (BIT(0))
#define LP_TEE_READ_TEE_LPPERI_M  (LP_TEE_READ_TEE_LPPERI_V << LP_TEE_READ_TEE_LPPERI_S)
#define LP_TEE_READ_TEE_LPPERI_V  0x00000001U
#define LP_TEE_READ_TEE_LPPERI_S  0
/** LP_TEE_READ_REE0_LPPERI : R/W; bitpos: [1]; default: 0;
 *  Configures lpperi registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LPPERI    (BIT(1))
#define LP_TEE_READ_REE0_LPPERI_M  (LP_TEE_READ_REE0_LPPERI_V << LP_TEE_READ_REE0_LPPERI_S)
#define LP_TEE_READ_REE0_LPPERI_V  0x00000001U
#define LP_TEE_READ_REE0_LPPERI_S  1
/** LP_TEE_READ_REE1_LPPERI : R/W; bitpos: [2]; default: 0;
 *  Configures lpperi registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LPPERI    (BIT(2))
#define LP_TEE_READ_REE1_LPPERI_M  (LP_TEE_READ_REE1_LPPERI_V << LP_TEE_READ_REE1_LPPERI_S)
#define LP_TEE_READ_REE1_LPPERI_V  0x00000001U
#define LP_TEE_READ_REE1_LPPERI_S  2
/** LP_TEE_READ_REE2_LPPERI : R/W; bitpos: [3]; default: 0;
 *  Configures lpperi registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LPPERI    (BIT(3))
#define LP_TEE_READ_REE2_LPPERI_M  (LP_TEE_READ_REE2_LPPERI_V << LP_TEE_READ_REE2_LPPERI_S)
#define LP_TEE_READ_REE2_LPPERI_V  0x00000001U
#define LP_TEE_READ_REE2_LPPERI_S  3
/** LP_TEE_WRITE_TEE_LPPERI : R/W; bitpos: [4]; default: 1;
 *  Configures lpperi registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LPPERI    (BIT(4))
#define LP_TEE_WRITE_TEE_LPPERI_M  (LP_TEE_WRITE_TEE_LPPERI_V << LP_TEE_WRITE_TEE_LPPERI_S)
#define LP_TEE_WRITE_TEE_LPPERI_V  0x00000001U
#define LP_TEE_WRITE_TEE_LPPERI_S  4
/** LP_TEE_WRITE_REE0_LPPERI : R/W; bitpos: [5]; default: 0;
 *  Configures lpperi registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LPPERI    (BIT(5))
#define LP_TEE_WRITE_REE0_LPPERI_M  (LP_TEE_WRITE_REE0_LPPERI_V << LP_TEE_WRITE_REE0_LPPERI_S)
#define LP_TEE_WRITE_REE0_LPPERI_V  0x00000001U
#define LP_TEE_WRITE_REE0_LPPERI_S  5
/** LP_TEE_WRITE_REE1_LPPERI : R/W; bitpos: [6]; default: 0;
 *  Configures lpperi registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LPPERI    (BIT(6))
#define LP_TEE_WRITE_REE1_LPPERI_M  (LP_TEE_WRITE_REE1_LPPERI_V << LP_TEE_WRITE_REE1_LPPERI_S)
#define LP_TEE_WRITE_REE1_LPPERI_V  0x00000001U
#define LP_TEE_WRITE_REE1_LPPERI_S  6
/** LP_TEE_WRITE_REE2_LPPERI : R/W; bitpos: [7]; default: 0;
 *  Configures lpperi registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LPPERI    (BIT(7))
#define LP_TEE_WRITE_REE2_LPPERI_M  (LP_TEE_WRITE_REE2_LPPERI_V << LP_TEE_WRITE_REE2_LPPERI_S)
#define LP_TEE_WRITE_REE2_LPPERI_V  0x00000001U
#define LP_TEE_WRITE_REE2_LPPERI_S  7

/** LP_TEE_LP_ANA_PERI_CTRL_REG register
 *  lp_ana_peri read/write control register
 */
#define LP_TEE_LP_ANA_PERI_CTRL_REG (DR_REG_LP_BASE + 0x20)
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

/** LP_TEE_LP_TOUCH_CTRL_REG register
 *  lp_touch read/write control register
 */
#define LP_TEE_LP_TOUCH_CTRL_REG (DR_REG_LP_BASE + 0x24)
/** LP_TEE_READ_TEE_LP_TOUCH : R/W; bitpos: [0]; default: 1;
 *  Configures lp_touch registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_TOUCH    (BIT(0))
#define LP_TEE_READ_TEE_LP_TOUCH_M  (LP_TEE_READ_TEE_LP_TOUCH_V << LP_TEE_READ_TEE_LP_TOUCH_S)
#define LP_TEE_READ_TEE_LP_TOUCH_V  0x00000001U
#define LP_TEE_READ_TEE_LP_TOUCH_S  0
/** LP_TEE_READ_REE0_LP_TOUCH : R/W; bitpos: [1]; default: 0;
 *  Configures lp_touch registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_TOUCH    (BIT(1))
#define LP_TEE_READ_REE0_LP_TOUCH_M  (LP_TEE_READ_REE0_LP_TOUCH_V << LP_TEE_READ_REE0_LP_TOUCH_S)
#define LP_TEE_READ_REE0_LP_TOUCH_V  0x00000001U
#define LP_TEE_READ_REE0_LP_TOUCH_S  1
/** LP_TEE_READ_REE1_LP_TOUCH : R/W; bitpos: [2]; default: 0;
 *  Configures lp_touch registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_TOUCH    (BIT(2))
#define LP_TEE_READ_REE1_LP_TOUCH_M  (LP_TEE_READ_REE1_LP_TOUCH_V << LP_TEE_READ_REE1_LP_TOUCH_S)
#define LP_TEE_READ_REE1_LP_TOUCH_V  0x00000001U
#define LP_TEE_READ_REE1_LP_TOUCH_S  2
/** LP_TEE_READ_REE2_LP_TOUCH : R/W; bitpos: [3]; default: 0;
 *  Configures lp_touch registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_TOUCH    (BIT(3))
#define LP_TEE_READ_REE2_LP_TOUCH_M  (LP_TEE_READ_REE2_LP_TOUCH_V << LP_TEE_READ_REE2_LP_TOUCH_S)
#define LP_TEE_READ_REE2_LP_TOUCH_V  0x00000001U
#define LP_TEE_READ_REE2_LP_TOUCH_S  3
/** LP_TEE_WRITE_TEE_LP_TOUCH : R/W; bitpos: [4]; default: 1;
 *  Configures lp_touch registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_TOUCH    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_TOUCH_M  (LP_TEE_WRITE_TEE_LP_TOUCH_V << LP_TEE_WRITE_TEE_LP_TOUCH_S)
#define LP_TEE_WRITE_TEE_LP_TOUCH_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_TOUCH_S  4
/** LP_TEE_WRITE_REE0_LP_TOUCH : R/W; bitpos: [5]; default: 0;
 *  Configures lp_touch registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_TOUCH    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_TOUCH_M  (LP_TEE_WRITE_REE0_LP_TOUCH_V << LP_TEE_WRITE_REE0_LP_TOUCH_S)
#define LP_TEE_WRITE_REE0_LP_TOUCH_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_TOUCH_S  5
/** LP_TEE_WRITE_REE1_LP_TOUCH : R/W; bitpos: [6]; default: 0;
 *  Configures lp_touch registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_TOUCH    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_TOUCH_M  (LP_TEE_WRITE_REE1_LP_TOUCH_V << LP_TEE_WRITE_REE1_LP_TOUCH_S)
#define LP_TEE_WRITE_REE1_LP_TOUCH_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_TOUCH_S  6
/** LP_TEE_WRITE_REE2_LP_TOUCH : R/W; bitpos: [7]; default: 0;
 *  Configures lp_touch registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_TOUCH    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_TOUCH_M  (LP_TEE_WRITE_REE2_LP_TOUCH_V << LP_TEE_WRITE_REE2_LP_TOUCH_S)
#define LP_TEE_WRITE_REE2_LP_TOUCH_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_TOUCH_S  7

/** LP_TEE_TOUCH_AON_CTRL_REG register
 *  touch_aon read/write control register
 */
#define LP_TEE_TOUCH_AON_CTRL_REG (DR_REG_LP_BASE + 0x28)
/** LP_TEE_READ_TEE_TOUCH_AON : R/W; bitpos: [0]; default: 1;
 *  Configures touch_aon registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_TOUCH_AON    (BIT(0))
#define LP_TEE_READ_TEE_TOUCH_AON_M  (LP_TEE_READ_TEE_TOUCH_AON_V << LP_TEE_READ_TEE_TOUCH_AON_S)
#define LP_TEE_READ_TEE_TOUCH_AON_V  0x00000001U
#define LP_TEE_READ_TEE_TOUCH_AON_S  0
/** LP_TEE_READ_REE0_TOUCH_AON : R/W; bitpos: [1]; default: 0;
 *  Configures touch_aon registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_TOUCH_AON    (BIT(1))
#define LP_TEE_READ_REE0_TOUCH_AON_M  (LP_TEE_READ_REE0_TOUCH_AON_V << LP_TEE_READ_REE0_TOUCH_AON_S)
#define LP_TEE_READ_REE0_TOUCH_AON_V  0x00000001U
#define LP_TEE_READ_REE0_TOUCH_AON_S  1
/** LP_TEE_READ_REE1_TOUCH_AON : R/W; bitpos: [2]; default: 0;
 *  Configures touch_aon registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_TOUCH_AON    (BIT(2))
#define LP_TEE_READ_REE1_TOUCH_AON_M  (LP_TEE_READ_REE1_TOUCH_AON_V << LP_TEE_READ_REE1_TOUCH_AON_S)
#define LP_TEE_READ_REE1_TOUCH_AON_V  0x00000001U
#define LP_TEE_READ_REE1_TOUCH_AON_S  2
/** LP_TEE_READ_REE2_TOUCH_AON : R/W; bitpos: [3]; default: 0;
 *  Configures touch_aon registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_TOUCH_AON    (BIT(3))
#define LP_TEE_READ_REE2_TOUCH_AON_M  (LP_TEE_READ_REE2_TOUCH_AON_V << LP_TEE_READ_REE2_TOUCH_AON_S)
#define LP_TEE_READ_REE2_TOUCH_AON_V  0x00000001U
#define LP_TEE_READ_REE2_TOUCH_AON_S  3
/** LP_TEE_WRITE_TEE_TOUCH_AON : R/W; bitpos: [4]; default: 1;
 *  Configures touch_aon registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_TOUCH_AON    (BIT(4))
#define LP_TEE_WRITE_TEE_TOUCH_AON_M  (LP_TEE_WRITE_TEE_TOUCH_AON_V << LP_TEE_WRITE_TEE_TOUCH_AON_S)
#define LP_TEE_WRITE_TEE_TOUCH_AON_V  0x00000001U
#define LP_TEE_WRITE_TEE_TOUCH_AON_S  4
/** LP_TEE_WRITE_REE0_TOUCH_AON : R/W; bitpos: [5]; default: 0;
 *  Configures touch_aon registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_TOUCH_AON    (BIT(5))
#define LP_TEE_WRITE_REE0_TOUCH_AON_M  (LP_TEE_WRITE_REE0_TOUCH_AON_V << LP_TEE_WRITE_REE0_TOUCH_AON_S)
#define LP_TEE_WRITE_REE0_TOUCH_AON_V  0x00000001U
#define LP_TEE_WRITE_REE0_TOUCH_AON_S  5
/** LP_TEE_WRITE_REE1_TOUCH_AON : R/W; bitpos: [6]; default: 0;
 *  Configures touch_aon registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_TOUCH_AON    (BIT(6))
#define LP_TEE_WRITE_REE1_TOUCH_AON_M  (LP_TEE_WRITE_REE1_TOUCH_AON_V << LP_TEE_WRITE_REE1_TOUCH_AON_S)
#define LP_TEE_WRITE_REE1_TOUCH_AON_V  0x00000001U
#define LP_TEE_WRITE_REE1_TOUCH_AON_S  6
/** LP_TEE_WRITE_REE2_TOUCH_AON : R/W; bitpos: [7]; default: 0;
 *  Configures touch_aon registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_TOUCH_AON    (BIT(7))
#define LP_TEE_WRITE_REE2_TOUCH_AON_M  (LP_TEE_WRITE_REE2_TOUCH_AON_V << LP_TEE_WRITE_REE2_TOUCH_AON_S)
#define LP_TEE_WRITE_REE2_TOUCH_AON_V  0x00000001U
#define LP_TEE_WRITE_REE2_TOUCH_AON_S  7

/** LP_TEE_LP_IO_CTRL_REG register
 *  lp_io read/write control register
 */
#define LP_TEE_LP_IO_CTRL_REG (DR_REG_LP_BASE + 0x2c)
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

/** LP_TEE_LP_BLE_TIMER_CTRL_REG register
 *  lp_ble_timer read/write control register
 */
#define LP_TEE_LP_BLE_TIMER_CTRL_REG (DR_REG_LP_BASE + 0x30)
/** LP_TEE_READ_TEE_LP_BLE_TIMER : R/W; bitpos: [0]; default: 1;
 *  Configures lp_ble_timer registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_LP_BLE_TIMER    (BIT(0))
#define LP_TEE_READ_TEE_LP_BLE_TIMER_M  (LP_TEE_READ_TEE_LP_BLE_TIMER_V << LP_TEE_READ_TEE_LP_BLE_TIMER_S)
#define LP_TEE_READ_TEE_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_READ_TEE_LP_BLE_TIMER_S  0
/** LP_TEE_READ_REE0_LP_BLE_TIMER : R/W; bitpos: [1]; default: 0;
 *  Configures lp_ble_timer registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_LP_BLE_TIMER    (BIT(1))
#define LP_TEE_READ_REE0_LP_BLE_TIMER_M  (LP_TEE_READ_REE0_LP_BLE_TIMER_V << LP_TEE_READ_REE0_LP_BLE_TIMER_S)
#define LP_TEE_READ_REE0_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_READ_REE0_LP_BLE_TIMER_S  1
/** LP_TEE_READ_REE1_LP_BLE_TIMER : R/W; bitpos: [2]; default: 0;
 *  Configures lp_ble_timer registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_LP_BLE_TIMER    (BIT(2))
#define LP_TEE_READ_REE1_LP_BLE_TIMER_M  (LP_TEE_READ_REE1_LP_BLE_TIMER_V << LP_TEE_READ_REE1_LP_BLE_TIMER_S)
#define LP_TEE_READ_REE1_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_READ_REE1_LP_BLE_TIMER_S  2
/** LP_TEE_READ_REE2_LP_BLE_TIMER : R/W; bitpos: [3]; default: 0;
 *  Configures lp_ble_timer registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_LP_BLE_TIMER    (BIT(3))
#define LP_TEE_READ_REE2_LP_BLE_TIMER_M  (LP_TEE_READ_REE2_LP_BLE_TIMER_V << LP_TEE_READ_REE2_LP_BLE_TIMER_S)
#define LP_TEE_READ_REE2_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_READ_REE2_LP_BLE_TIMER_S  3
/** LP_TEE_WRITE_TEE_LP_BLE_TIMER : R/W; bitpos: [4]; default: 1;
 *  Configures lp_ble_timer registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_LP_BLE_TIMER    (BIT(4))
#define LP_TEE_WRITE_TEE_LP_BLE_TIMER_M  (LP_TEE_WRITE_TEE_LP_BLE_TIMER_V << LP_TEE_WRITE_TEE_LP_BLE_TIMER_S)
#define LP_TEE_WRITE_TEE_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_WRITE_TEE_LP_BLE_TIMER_S  4
/** LP_TEE_WRITE_REE0_LP_BLE_TIMER : R/W; bitpos: [5]; default: 0;
 *  Configures lp_ble_timer registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_LP_BLE_TIMER    (BIT(5))
#define LP_TEE_WRITE_REE0_LP_BLE_TIMER_M  (LP_TEE_WRITE_REE0_LP_BLE_TIMER_V << LP_TEE_WRITE_REE0_LP_BLE_TIMER_S)
#define LP_TEE_WRITE_REE0_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_WRITE_REE0_LP_BLE_TIMER_S  5
/** LP_TEE_WRITE_REE1_LP_BLE_TIMER : R/W; bitpos: [6]; default: 0;
 *  Configures lp_ble_timer registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_LP_BLE_TIMER    (BIT(6))
#define LP_TEE_WRITE_REE1_LP_BLE_TIMER_M  (LP_TEE_WRITE_REE1_LP_BLE_TIMER_V << LP_TEE_WRITE_REE1_LP_BLE_TIMER_S)
#define LP_TEE_WRITE_REE1_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_WRITE_REE1_LP_BLE_TIMER_S  6
/** LP_TEE_WRITE_REE2_LP_BLE_TIMER : R/W; bitpos: [7]; default: 0;
 *  Configures lp_ble_timer registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_LP_BLE_TIMER    (BIT(7))
#define LP_TEE_WRITE_REE2_LP_BLE_TIMER_M  (LP_TEE_WRITE_REE2_LP_BLE_TIMER_V << LP_TEE_WRITE_REE2_LP_BLE_TIMER_S)
#define LP_TEE_WRITE_REE2_LP_BLE_TIMER_V  0x00000001U
#define LP_TEE_WRITE_REE2_LP_BLE_TIMER_S  7

/** LP_TEE_LP_TEE_CTRL_REG register
 *  lp_tee read/write control register
 */
#define LP_TEE_LP_TEE_CTRL_REG (DR_REG_LP_BASE + 0x34)
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

/** LP_TEE_HUK_CTRL_REG register
 *  lp_tee read/write control register
 */
#define LP_TEE_HUK_CTRL_REG (DR_REG_LP_BASE + 0x38)
/** LP_TEE_READ_TEE_HUK : R/W; bitpos: [0]; default: 1;
 *  Configures huk registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_TEE_HUK    (BIT(0))
#define LP_TEE_READ_TEE_HUK_M  (LP_TEE_READ_TEE_HUK_V << LP_TEE_READ_TEE_HUK_S)
#define LP_TEE_READ_TEE_HUK_V  0x00000001U
#define LP_TEE_READ_TEE_HUK_S  0
/** LP_TEE_READ_REE0_HUK : R/W; bitpos: [1]; default: 0;
 *  Configures huk registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE0_HUK    (BIT(1))
#define LP_TEE_READ_REE0_HUK_M  (LP_TEE_READ_REE0_HUK_V << LP_TEE_READ_REE0_HUK_S)
#define LP_TEE_READ_REE0_HUK_V  0x00000001U
#define LP_TEE_READ_REE0_HUK_S  1
/** LP_TEE_READ_REE1_HUK : R/W; bitpos: [2]; default: 0;
 *  Configures huk registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE1_HUK    (BIT(2))
#define LP_TEE_READ_REE1_HUK_M  (LP_TEE_READ_REE1_HUK_V << LP_TEE_READ_REE1_HUK_S)
#define LP_TEE_READ_REE1_HUK_V  0x00000001U
#define LP_TEE_READ_REE1_HUK_S  2
/** LP_TEE_READ_REE2_HUK : R/W; bitpos: [3]; default: 0;
 *  Configures huk registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define LP_TEE_READ_REE2_HUK    (BIT(3))
#define LP_TEE_READ_REE2_HUK_M  (LP_TEE_READ_REE2_HUK_V << LP_TEE_READ_REE2_HUK_S)
#define LP_TEE_READ_REE2_HUK_V  0x00000001U
#define LP_TEE_READ_REE2_HUK_S  3
/** LP_TEE_WRITE_TEE_HUK : R/W; bitpos: [4]; default: 1;
 *  Configures huk registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_TEE_HUK    (BIT(4))
#define LP_TEE_WRITE_TEE_HUK_M  (LP_TEE_WRITE_TEE_HUK_V << LP_TEE_WRITE_TEE_HUK_S)
#define LP_TEE_WRITE_TEE_HUK_V  0x00000001U
#define LP_TEE_WRITE_TEE_HUK_S  4
/** LP_TEE_WRITE_REE0_HUK : R/W; bitpos: [5]; default: 0;
 *  Configures huk registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE0_HUK    (BIT(5))
#define LP_TEE_WRITE_REE0_HUK_M  (LP_TEE_WRITE_REE0_HUK_V << LP_TEE_WRITE_REE0_HUK_S)
#define LP_TEE_WRITE_REE0_HUK_V  0x00000001U
#define LP_TEE_WRITE_REE0_HUK_S  5
/** LP_TEE_WRITE_REE1_HUK : R/W; bitpos: [6]; default: 0;
 *  Configures huk registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE1_HUK    (BIT(6))
#define LP_TEE_WRITE_REE1_HUK_M  (LP_TEE_WRITE_REE1_HUK_V << LP_TEE_WRITE_REE1_HUK_S)
#define LP_TEE_WRITE_REE1_HUK_V  0x00000001U
#define LP_TEE_WRITE_REE1_HUK_S  6
/** LP_TEE_WRITE_REE2_HUK : R/W; bitpos: [7]; default: 0;
 *  Configures huk registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define LP_TEE_WRITE_REE2_HUK    (BIT(7))
#define LP_TEE_WRITE_REE2_HUK_M  (LP_TEE_WRITE_REE2_HUK_V << LP_TEE_WRITE_REE2_HUK_S)
#define LP_TEE_WRITE_REE2_HUK_V  0x00000001U
#define LP_TEE_WRITE_REE2_HUK_S  7

/** LP_TEE_BUS_ERR_CONF_REG register
 *  Clock gating register
 */
#define LP_TEE_BUS_ERR_CONF_REG (DR_REG_LP_BASE + 0xf0)
/** LP_TEE_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether return error response to cpu when access blocked
 *  0: disable error response
 *  1: enable error response
 */
#define LP_TEE_BUS_ERR_RESP_EN    (BIT(0))
#define LP_TEE_BUS_ERR_RESP_EN_M  (LP_TEE_BUS_ERR_RESP_EN_V << LP_TEE_BUS_ERR_RESP_EN_S)
#define LP_TEE_BUS_ERR_RESP_EN_V  0x00000001U
#define LP_TEE_BUS_ERR_RESP_EN_S  0

/** LP_TEE_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define LP_TEE_CLOCK_GATE_REG (DR_REG_LP_BASE + 0xf8)
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
#define LP_TEE_DATE_REG (DR_REG_LP_BASE + 0xfc)
/** LP_TEE_DATE : R/W; bitpos: [27:0]; default: 37818640;
 *  Version control register
 */
#define LP_TEE_DATE    0x0FFFFFFFU
#define LP_TEE_DATE_M  (LP_TEE_DATE_V << LP_TEE_DATE_S)
#define LP_TEE_DATE_V  0x0FFFFFFFU
#define LP_TEE_DATE_S  0

#ifdef __cplusplus
}
#endif
