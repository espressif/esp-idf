/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: read write control register */
/** Type of tee_trng_ctrl register
 *  trng read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_trng : R/W; bitpos: [0]; default: 1;
         *  Configures trng registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_trng:1;
        /** tee_read_ree0_trng : R/W; bitpos: [1]; default: 0;
         *  Configures trng registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_trng:1;
        /** tee_read_ree1_trng : R/W; bitpos: [2]; default: 0;
         *  Configures trng registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_trng:1;
        /** tee_read_ree2_trng : R/W; bitpos: [3]; default: 0;
         *  Configures trng registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_trng:1;
        /** tee_write_tee_trng : R/W; bitpos: [4]; default: 1;
         *  Configures trng registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_trng:1;
        /** tee_write_ree0_trng : R/W; bitpos: [5]; default: 0;
         *  Configures trng registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_trng:1;
        /** tee_write_ree1_trng : R/W; bitpos: [6]; default: 0;
         *  Configures trng registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_trng:1;
        /** tee_write_ree2_trng : R/W; bitpos: [7]; default: 0;
         *  Configures trng registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_trng:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_trng_ctrl_reg_t;

/** Type of tee_efuse_ctrl register
 *  efuse read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_efuse : R/W; bitpos: [0]; default: 1;
         *  Configures efuse registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_efuse:1;
        /** tee_read_ree0_efuse : R/W; bitpos: [1]; default: 0;
         *  Configures efuse registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_efuse:1;
        /** tee_read_ree1_efuse : R/W; bitpos: [2]; default: 0;
         *  Configures efuse registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_efuse:1;
        /** tee_read_ree2_efuse : R/W; bitpos: [3]; default: 0;
         *  Configures efuse registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_efuse:1;
        /** tee_write_tee_efuse : R/W; bitpos: [4]; default: 1;
         *  Configures efuse registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_efuse:1;
        /** tee_write_ree0_efuse : R/W; bitpos: [5]; default: 0;
         *  Configures efuse registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_efuse:1;
        /** tee_write_ree1_efuse : R/W; bitpos: [6]; default: 0;
         *  Configures efuse registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_efuse:1;
        /** tee_write_ree2_efuse : R/W; bitpos: [7]; default: 0;
         *  Configures efuse registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_efuse:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_efuse_ctrl_reg_t;

/** Type of tee_pmu_ctrl register
 *  pmu read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_pmu : R/W; bitpos: [0]; default: 1;
         *  Configures pmu registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_pmu:1;
        /** tee_read_ree0_pmu : R/W; bitpos: [1]; default: 0;
         *  Configures pmu registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_pmu:1;
        /** tee_read_ree1_pmu : R/W; bitpos: [2]; default: 0;
         *  Configures pmu registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_pmu:1;
        /** tee_read_ree2_pmu : R/W; bitpos: [3]; default: 0;
         *  Configures pmu registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_pmu:1;
        /** tee_write_tee_pmu : R/W; bitpos: [4]; default: 1;
         *  Configures pmu registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_pmu:1;
        /** tee_write_ree0_pmu : R/W; bitpos: [5]; default: 0;
         *  Configures pmu registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_pmu:1;
        /** tee_write_ree1_pmu : R/W; bitpos: [6]; default: 0;
         *  Configures pmu registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_pmu:1;
        /** tee_write_ree2_pmu : R/W; bitpos: [7]; default: 0;
         *  Configures pmu registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_pmu:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_pmu_ctrl_reg_t;

/** Type of tee_clkrst_ctrl register
 *  clkrst read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_clkrst : R/W; bitpos: [0]; default: 1;
         *  Configures clkrst registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_clkrst:1;
        /** tee_read_ree0_clkrst : R/W; bitpos: [1]; default: 0;
         *  Configures clkrst registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_clkrst:1;
        /** tee_read_ree1_clkrst : R/W; bitpos: [2]; default: 0;
         *  Configures clkrst registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_clkrst:1;
        /** tee_read_ree2_clkrst : R/W; bitpos: [3]; default: 0;
         *  Configures clkrst registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_clkrst:1;
        /** tee_write_tee_clkrst : R/W; bitpos: [4]; default: 1;
         *  Configures clkrst registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_clkrst:1;
        /** tee_write_ree0_clkrst : R/W; bitpos: [5]; default: 0;
         *  Configures clkrst registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_clkrst:1;
        /** tee_write_ree1_clkrst : R/W; bitpos: [6]; default: 0;
         *  Configures clkrst registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_clkrst:1;
        /** tee_write_ree2_clkrst : R/W; bitpos: [7]; default: 0;
         *  Configures clkrst registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_clkrst:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_clkrst_ctrl_reg_t;

/** Type of tee_lp_aon_ctrl_ctrl register
 *  lp_aon_ctrl read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_aon_ctrl : R/W; bitpos: [0]; default: 1;
         *  Configures lp_aon_ctrl registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_aon_ctrl:1;
        /** tee_read_ree0_lp_aon_ctrl : R/W; bitpos: [1]; default: 0;
         *  Configures lp_aon_ctrl registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_aon_ctrl:1;
        /** tee_read_ree1_lp_aon_ctrl : R/W; bitpos: [2]; default: 0;
         *  Configures lp_aon_ctrl registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_aon_ctrl:1;
        /** tee_read_ree2_lp_aon_ctrl : R/W; bitpos: [3]; default: 0;
         *  Configures lp_aon_ctrl registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_aon_ctrl:1;
        /** tee_write_tee_lp_aon_ctrl : R/W; bitpos: [4]; default: 1;
         *  Configures lp_aon_ctrl registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_aon_ctrl:1;
        /** tee_write_ree0_lp_aon_ctrl : R/W; bitpos: [5]; default: 0;
         *  Configures lp_aon_ctrl registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_aon_ctrl:1;
        /** tee_write_ree1_lp_aon_ctrl : R/W; bitpos: [6]; default: 0;
         *  Configures lp_aon_ctrl registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_aon_ctrl:1;
        /** tee_write_ree2_lp_aon_ctrl : R/W; bitpos: [7]; default: 0;
         *  Configures lp_aon_ctrl registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_aon_ctrl:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_aon_ctrl_ctrl_reg_t;

/** Type of tee_lp_timer_ctrl register
 *  lp_timer read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_timer : R/W; bitpos: [0]; default: 1;
         *  Configures lp_timer registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_timer:1;
        /** tee_read_ree0_lp_timer : R/W; bitpos: [1]; default: 0;
         *  Configures lp_timer registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_timer:1;
        /** tee_read_ree1_lp_timer : R/W; bitpos: [2]; default: 0;
         *  Configures lp_timer registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_timer:1;
        /** tee_read_ree2_lp_timer : R/W; bitpos: [3]; default: 0;
         *  Configures lp_timer registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_timer:1;
        /** tee_write_tee_lp_timer : R/W; bitpos: [4]; default: 1;
         *  Configures lp_timer registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_timer:1;
        /** tee_write_ree0_lp_timer : R/W; bitpos: [5]; default: 0;
         *  Configures lp_timer registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_timer:1;
        /** tee_write_ree1_lp_timer : R/W; bitpos: [6]; default: 0;
         *  Configures lp_timer registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_timer:1;
        /** tee_write_ree2_lp_timer : R/W; bitpos: [7]; default: 0;
         *  Configures lp_timer registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_timer:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_timer_ctrl_reg_t;

/** Type of tee_lp_wdt_ctrl register
 *  lp_wdt read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_wdt : R/W; bitpos: [0]; default: 1;
         *  Configures lp_wdt registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_wdt:1;
        /** tee_read_ree0_lp_wdt : R/W; bitpos: [1]; default: 0;
         *  Configures lp_wdt registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_wdt:1;
        /** tee_read_ree1_lp_wdt : R/W; bitpos: [2]; default: 0;
         *  Configures lp_wdt registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_wdt:1;
        /** tee_read_ree2_lp_wdt : R/W; bitpos: [3]; default: 0;
         *  Configures lp_wdt registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_wdt:1;
        /** tee_write_tee_lp_wdt : R/W; bitpos: [4]; default: 1;
         *  Configures lp_wdt registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_wdt:1;
        /** tee_write_ree0_lp_wdt : R/W; bitpos: [5]; default: 0;
         *  Configures lp_wdt registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_wdt:1;
        /** tee_write_ree1_lp_wdt : R/W; bitpos: [6]; default: 0;
         *  Configures lp_wdt registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_wdt:1;
        /** tee_write_ree2_lp_wdt : R/W; bitpos: [7]; default: 0;
         *  Configures lp_wdt registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_wdt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_wdt_ctrl_reg_t;

/** Type of tee_lpperi_ctrl register
 *  lpperi read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lpperi : R/W; bitpos: [0]; default: 1;
         *  Configures lpperi registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lpperi:1;
        /** tee_read_ree0_lpperi : R/W; bitpos: [1]; default: 0;
         *  Configures lpperi registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lpperi:1;
        /** tee_read_ree1_lpperi : R/W; bitpos: [2]; default: 0;
         *  Configures lpperi registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lpperi:1;
        /** tee_read_ree2_lpperi : R/W; bitpos: [3]; default: 0;
         *  Configures lpperi registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lpperi:1;
        /** tee_write_tee_lpperi : R/W; bitpos: [4]; default: 1;
         *  Configures lpperi registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lpperi:1;
        /** tee_write_ree0_lpperi : R/W; bitpos: [5]; default: 0;
         *  Configures lpperi registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lpperi:1;
        /** tee_write_ree1_lpperi : R/W; bitpos: [6]; default: 0;
         *  Configures lpperi registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lpperi:1;
        /** tee_write_ree2_lpperi : R/W; bitpos: [7]; default: 0;
         *  Configures lpperi registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lpperi:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lpperi_ctrl_reg_t;

/** Type of tee_lp_ana_peri_ctrl register
 *  lp_ana_peri read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_ana_peri : R/W; bitpos: [0]; default: 1;
         *  Configures lp_ana_peri registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_ana_peri:1;
        /** tee_read_ree0_lp_ana_peri : R/W; bitpos: [1]; default: 0;
         *  Configures lp_ana_peri registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_ana_peri:1;
        /** tee_read_ree1_lp_ana_peri : R/W; bitpos: [2]; default: 0;
         *  Configures lp_ana_peri registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_ana_peri:1;
        /** tee_read_ree2_lp_ana_peri : R/W; bitpos: [3]; default: 0;
         *  Configures lp_ana_peri registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_ana_peri:1;
        /** tee_write_tee_lp_ana_peri : R/W; bitpos: [4]; default: 1;
         *  Configures lp_ana_peri registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_ana_peri:1;
        /** tee_write_ree0_lp_ana_peri : R/W; bitpos: [5]; default: 0;
         *  Configures lp_ana_peri registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_ana_peri:1;
        /** tee_write_ree1_lp_ana_peri : R/W; bitpos: [6]; default: 0;
         *  Configures lp_ana_peri registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_ana_peri:1;
        /** tee_write_ree2_lp_ana_peri : R/W; bitpos: [7]; default: 0;
         *  Configures lp_ana_peri registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_ana_peri:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_ana_peri_ctrl_reg_t;

/** Type of tee_lp_touch_ctrl register
 *  lp_touch read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_touch : R/W; bitpos: [0]; default: 1;
         *  Configures lp_touch registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_touch:1;
        /** tee_read_ree0_lp_touch : R/W; bitpos: [1]; default: 0;
         *  Configures lp_touch registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_touch:1;
        /** tee_read_ree1_lp_touch : R/W; bitpos: [2]; default: 0;
         *  Configures lp_touch registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_touch:1;
        /** tee_read_ree2_lp_touch : R/W; bitpos: [3]; default: 0;
         *  Configures lp_touch registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_touch:1;
        /** tee_write_tee_lp_touch : R/W; bitpos: [4]; default: 1;
         *  Configures lp_touch registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_touch:1;
        /** tee_write_ree0_lp_touch : R/W; bitpos: [5]; default: 0;
         *  Configures lp_touch registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_touch:1;
        /** tee_write_ree1_lp_touch : R/W; bitpos: [6]; default: 0;
         *  Configures lp_touch registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_touch:1;
        /** tee_write_ree2_lp_touch : R/W; bitpos: [7]; default: 0;
         *  Configures lp_touch registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_touch:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_touch_ctrl_reg_t;

/** Type of tee_touch_aon_ctrl register
 *  touch_aon read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_touch_aon : R/W; bitpos: [0]; default: 1;
         *  Configures touch_aon registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_touch_aon:1;
        /** tee_read_ree0_touch_aon : R/W; bitpos: [1]; default: 0;
         *  Configures touch_aon registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_touch_aon:1;
        /** tee_read_ree1_touch_aon : R/W; bitpos: [2]; default: 0;
         *  Configures touch_aon registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_touch_aon:1;
        /** tee_read_ree2_touch_aon : R/W; bitpos: [3]; default: 0;
         *  Configures touch_aon registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_touch_aon:1;
        /** tee_write_tee_touch_aon : R/W; bitpos: [4]; default: 1;
         *  Configures touch_aon registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_touch_aon:1;
        /** tee_write_ree0_touch_aon : R/W; bitpos: [5]; default: 0;
         *  Configures touch_aon registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_touch_aon:1;
        /** tee_write_ree1_touch_aon : R/W; bitpos: [6]; default: 0;
         *  Configures touch_aon registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_touch_aon:1;
        /** tee_write_ree2_touch_aon : R/W; bitpos: [7]; default: 0;
         *  Configures touch_aon registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_touch_aon:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_touch_aon_ctrl_reg_t;

/** Type of tee_lp_io_ctrl register
 *  lp_io read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_io : R/W; bitpos: [0]; default: 1;
         *  Configures lp_io registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_io:1;
        /** tee_read_ree0_lp_io : R/W; bitpos: [1]; default: 0;
         *  Configures lp_io registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_io:1;
        /** tee_read_ree1_lp_io : R/W; bitpos: [2]; default: 0;
         *  Configures lp_io registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_io:1;
        /** tee_read_ree2_lp_io : R/W; bitpos: [3]; default: 0;
         *  Configures lp_io registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_io:1;
        /** tee_write_tee_lp_io : R/W; bitpos: [4]; default: 1;
         *  Configures lp_io registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_io:1;
        /** tee_write_ree0_lp_io : R/W; bitpos: [5]; default: 0;
         *  Configures lp_io registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_io:1;
        /** tee_write_ree1_lp_io : R/W; bitpos: [6]; default: 0;
         *  Configures lp_io registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_io:1;
        /** tee_write_ree2_lp_io : R/W; bitpos: [7]; default: 0;
         *  Configures lp_io registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_io:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_io_ctrl_reg_t;

/** Type of tee_lp_ble_timer_ctrl register
 *  lp_ble_timer read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_ble_timer : R/W; bitpos: [0]; default: 1;
         *  Configures lp_ble_timer registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_ble_timer:1;
        /** tee_read_ree0_lp_ble_timer : R/W; bitpos: [1]; default: 0;
         *  Configures lp_ble_timer registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_ble_timer:1;
        /** tee_read_ree1_lp_ble_timer : R/W; bitpos: [2]; default: 0;
         *  Configures lp_ble_timer registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_ble_timer:1;
        /** tee_read_ree2_lp_ble_timer : R/W; bitpos: [3]; default: 0;
         *  Configures lp_ble_timer registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_ble_timer:1;
        /** tee_write_tee_lp_ble_timer : R/W; bitpos: [4]; default: 1;
         *  Configures lp_ble_timer registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_ble_timer:1;
        /** tee_write_ree0_lp_ble_timer : R/W; bitpos: [5]; default: 0;
         *  Configures lp_ble_timer registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_ble_timer:1;
        /** tee_write_ree1_lp_ble_timer : R/W; bitpos: [6]; default: 0;
         *  Configures lp_ble_timer registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_ble_timer:1;
        /** tee_write_ree2_lp_ble_timer : R/W; bitpos: [7]; default: 0;
         *  Configures lp_ble_timer registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_ble_timer:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_ble_timer_ctrl_reg_t;

/** Type of tee_lp_tee_ctrl register
 *  lp_tee read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_lp_tee : R/W; bitpos: [0]; default: 1;
         *  Configures lp_tee registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_lp_tee:1;
        /** tee_read_ree0_lp_tee : HRO; bitpos: [1]; default: 0;
         *  Configures lp_tee registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_lp_tee:1;
        /** tee_read_ree1_lp_tee : HRO; bitpos: [2]; default: 0;
         *  Configures lp_tee registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_lp_tee:1;
        /** tee_read_ree2_lp_tee : HRO; bitpos: [3]; default: 0;
         *  Configures lp_tee registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_lp_tee:1;
        /** tee_write_tee_lp_tee : R/W; bitpos: [4]; default: 1;
         *  Configures lp_tee registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_lp_tee:1;
        /** tee_write_ree0_lp_tee : HRO; bitpos: [5]; default: 0;
         *  Configures lp_tee registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_lp_tee:1;
        /** tee_write_ree1_lp_tee : HRO; bitpos: [6]; default: 0;
         *  Configures lp_tee registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_lp_tee:1;
        /** tee_write_ree2_lp_tee : HRO; bitpos: [7]; default: 0;
         *  Configures lp_tee registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_lp_tee:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_tee_ctrl_reg_t;

/** Type of tee_huk_ctrl register
 *  lp_tee read/write control register
 */
typedef union {
    struct {
        /** tee_read_tee_huk : R/W; bitpos: [0]; default: 1;
         *  Configures huk registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_tee_huk:1;
        /** tee_read_ree0_huk : R/W; bitpos: [1]; default: 0;
         *  Configures huk registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree0_huk:1;
        /** tee_read_ree1_huk : R/W; bitpos: [2]; default: 0;
         *  Configures huk registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree1_huk:1;
        /** tee_read_ree2_huk : R/W; bitpos: [3]; default: 0;
         *  Configures huk registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t tee_read_ree2_huk:1;
        /** tee_write_tee_huk : R/W; bitpos: [4]; default: 1;
         *  Configures huk registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_tee_huk:1;
        /** tee_write_ree0_huk : R/W; bitpos: [5]; default: 0;
         *  Configures huk registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree0_huk:1;
        /** tee_write_ree1_huk : R/W; bitpos: [6]; default: 0;
         *  Configures huk registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree1_huk:1;
        /** tee_write_ree2_huk : R/W; bitpos: [7]; default: 0;
         *  Configures huk registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t tee_write_ree2_huk:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_huk_ctrl_reg_t;


/** Group: config register */
/** Type of tee_bus_err_conf register
 *  Clock gating register
 */
typedef union {
    struct {
        /** tee_bus_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether return error response to cpu when access blocked
         *  0: disable error response
         *  1: enable error response
         */
        uint32_t tee_bus_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_bus_err_conf_reg_t;


/** Group: clock gating register */
/** Type of tee_clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** tee_clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: enable automatic clock gating
         *  1: keep the clock always on
         */
        uint32_t tee_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_clock_gate_reg_t;


/** Group: Version control register */
/** Type of tee_date register
 *  Version control register
 */
typedef union {
    struct {
        /** tee_date : R/W; bitpos: [27:0]; default: 37818640;
         *  Version control register
         */
        uint32_t tee_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_tee_date_reg_t;


typedef struct {
    volatile lp_tee_trng_ctrl_reg_t tee_trng_ctrl;
    volatile lp_tee_efuse_ctrl_reg_t tee_efuse_ctrl;
    volatile lp_tee_pmu_ctrl_reg_t tee_pmu_ctrl;
    volatile lp_tee_clkrst_ctrl_reg_t tee_clkrst_ctrl;
    volatile lp_tee_lp_aon_ctrl_ctrl_reg_t tee_lp_aon_ctrl_ctrl;
    volatile lp_tee_lp_timer_ctrl_reg_t tee_lp_timer_ctrl;
    volatile lp_tee_lp_wdt_ctrl_reg_t tee_lp_wdt_ctrl;
    volatile lp_tee_lpperi_ctrl_reg_t tee_lpperi_ctrl;
    volatile lp_tee_lp_ana_peri_ctrl_reg_t tee_lp_ana_peri_ctrl;
    volatile lp_tee_lp_touch_ctrl_reg_t tee_lp_touch_ctrl;
    volatile lp_tee_touch_aon_ctrl_reg_t tee_touch_aon_ctrl;
    volatile lp_tee_lp_io_ctrl_reg_t tee_lp_io_ctrl;
    volatile lp_tee_lp_ble_timer_ctrl_reg_t tee_lp_ble_timer_ctrl;
    volatile lp_tee_lp_tee_ctrl_reg_t tee_lp_tee_ctrl;
    volatile lp_tee_huk_ctrl_reg_t tee_huk_ctrl;
    uint32_t reserved_03c[45];
    volatile lp_tee_bus_err_conf_reg_t tee_bus_err_conf;
    uint32_t reserved_0f4;
    volatile lp_tee_clock_gate_reg_t tee_clock_gate;
    volatile lp_tee_date_reg_t tee_date;
} lp_tee_dev_t;

extern lp_tee_dev_t LP_TEE;

#ifndef __cplusplus
_Static_assert(sizeof(lp_tee_dev_t) == 0x100, "Invalid size of lp_tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
