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
/** Type of trng_ctrl register
 *  trng read/write control register
 */
typedef union {
    struct {
        /** read_tee_trng : R/W; bitpos: [0]; default: 1;
         *  Configures trng registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_trng:1;
        /** read_ree0_trng : R/W; bitpos: [1]; default: 0;
         *  Configures trng registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_trng:1;
        /** read_ree1_trng : R/W; bitpos: [2]; default: 0;
         *  Configures trng registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_trng:1;
        /** read_ree2_trng : R/W; bitpos: [3]; default: 0;
         *  Configures trng registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_trng:1;
        /** write_tee_trng : R/W; bitpos: [4]; default: 1;
         *  Configures trng registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_trng:1;
        /** write_ree0_trng : R/W; bitpos: [5]; default: 0;
         *  Configures trng registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_trng:1;
        /** write_ree1_trng : R/W; bitpos: [6]; default: 0;
         *  Configures trng registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_trng:1;
        /** write_ree2_trng : R/W; bitpos: [7]; default: 0;
         *  Configures trng registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_trng:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_trng_ctrl_reg_t;

/** Type of efuse_ctrl register
 *  efuse read/write control register
 */
typedef union {
    struct {
        /** read_tee_efuse : R/W; bitpos: [0]; default: 1;
         *  Configures efuse registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_efuse:1;
        /** read_ree0_efuse : R/W; bitpos: [1]; default: 0;
         *  Configures efuse registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_efuse:1;
        /** read_ree1_efuse : R/W; bitpos: [2]; default: 0;
         *  Configures efuse registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_efuse:1;
        /** read_ree2_efuse : R/W; bitpos: [3]; default: 0;
         *  Configures efuse registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_efuse:1;
        /** write_tee_efuse : R/W; bitpos: [4]; default: 1;
         *  Configures efuse registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_efuse:1;
        /** write_ree0_efuse : R/W; bitpos: [5]; default: 0;
         *  Configures efuse registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_efuse:1;
        /** write_ree1_efuse : R/W; bitpos: [6]; default: 0;
         *  Configures efuse registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_efuse:1;
        /** write_ree2_efuse : R/W; bitpos: [7]; default: 0;
         *  Configures efuse registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_efuse:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_efuse_ctrl_reg_t;

/** Type of pmu_ctrl register
 *  pmu read/write control register
 */
typedef union {
    struct {
        /** read_tee_pmu : R/W; bitpos: [0]; default: 1;
         *  Configures pmu registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_pmu:1;
        /** read_ree0_pmu : R/W; bitpos: [1]; default: 0;
         *  Configures pmu registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_pmu:1;
        /** read_ree1_pmu : R/W; bitpos: [2]; default: 0;
         *  Configures pmu registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_pmu:1;
        /** read_ree2_pmu : R/W; bitpos: [3]; default: 0;
         *  Configures pmu registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_pmu:1;
        /** write_tee_pmu : R/W; bitpos: [4]; default: 1;
         *  Configures pmu registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_pmu:1;
        /** write_ree0_pmu : R/W; bitpos: [5]; default: 0;
         *  Configures pmu registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_pmu:1;
        /** write_ree1_pmu : R/W; bitpos: [6]; default: 0;
         *  Configures pmu registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_pmu:1;
        /** write_ree2_pmu : R/W; bitpos: [7]; default: 0;
         *  Configures pmu registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_pmu:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_pmu_ctrl_reg_t;

/** Type of clkrst_ctrl register
 *  clkrst read/write control register
 */
typedef union {
    struct {
        /** read_tee_clkrst : R/W; bitpos: [0]; default: 1;
         *  Configures clkrst registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_clkrst:1;
        /** read_ree0_clkrst : R/W; bitpos: [1]; default: 0;
         *  Configures clkrst registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_clkrst:1;
        /** read_ree1_clkrst : R/W; bitpos: [2]; default: 0;
         *  Configures clkrst registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_clkrst:1;
        /** read_ree2_clkrst : R/W; bitpos: [3]; default: 0;
         *  Configures clkrst registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_clkrst:1;
        /** write_tee_clkrst : R/W; bitpos: [4]; default: 1;
         *  Configures clkrst registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_clkrst:1;
        /** write_ree0_clkrst : R/W; bitpos: [5]; default: 0;
         *  Configures clkrst registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_clkrst:1;
        /** write_ree1_clkrst : R/W; bitpos: [6]; default: 0;
         *  Configures clkrst registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_clkrst:1;
        /** write_ree2_clkrst : R/W; bitpos: [7]; default: 0;
         *  Configures clkrst registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_clkrst:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_clkrst_ctrl_reg_t;

/** Type of lp_aon_ctrl_ctrl register
 *  lp_aon_ctrl read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_aon_ctrl : R/W; bitpos: [0]; default: 1;
         *  Configures lp_aon_ctrl registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_aon_ctrl:1;
        /** read_ree0_lp_aon_ctrl : R/W; bitpos: [1]; default: 0;
         *  Configures lp_aon_ctrl registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_aon_ctrl:1;
        /** read_ree1_lp_aon_ctrl : R/W; bitpos: [2]; default: 0;
         *  Configures lp_aon_ctrl registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_aon_ctrl:1;
        /** read_ree2_lp_aon_ctrl : R/W; bitpos: [3]; default: 0;
         *  Configures lp_aon_ctrl registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_aon_ctrl:1;
        /** write_tee_lp_aon_ctrl : R/W; bitpos: [4]; default: 1;
         *  Configures lp_aon_ctrl registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_aon_ctrl:1;
        /** write_ree0_lp_aon_ctrl : R/W; bitpos: [5]; default: 0;
         *  Configures lp_aon_ctrl registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_aon_ctrl:1;
        /** write_ree1_lp_aon_ctrl : R/W; bitpos: [6]; default: 0;
         *  Configures lp_aon_ctrl registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_aon_ctrl:1;
        /** write_ree2_lp_aon_ctrl : R/W; bitpos: [7]; default: 0;
         *  Configures lp_aon_ctrl registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_aon_ctrl:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_aon_ctrl_ctrl_reg_t;

/** Type of lp_timer_ctrl register
 *  lp_timer read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_timer : R/W; bitpos: [0]; default: 1;
         *  Configures lp_timer registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_timer:1;
        /** read_ree0_lp_timer : R/W; bitpos: [1]; default: 0;
         *  Configures lp_timer registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_timer:1;
        /** read_ree1_lp_timer : R/W; bitpos: [2]; default: 0;
         *  Configures lp_timer registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_timer:1;
        /** read_ree2_lp_timer : R/W; bitpos: [3]; default: 0;
         *  Configures lp_timer registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_timer:1;
        /** write_tee_lp_timer : R/W; bitpos: [4]; default: 1;
         *  Configures lp_timer registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_timer:1;
        /** write_ree0_lp_timer : R/W; bitpos: [5]; default: 0;
         *  Configures lp_timer registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_timer:1;
        /** write_ree1_lp_timer : R/W; bitpos: [6]; default: 0;
         *  Configures lp_timer registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_timer:1;
        /** write_ree2_lp_timer : R/W; bitpos: [7]; default: 0;
         *  Configures lp_timer registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_timer:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_timer_ctrl_reg_t;

/** Type of lp_wdt_ctrl register
 *  lp_wdt read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_wdt : R/W; bitpos: [0]; default: 1;
         *  Configures lp_wdt registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_wdt:1;
        /** read_ree0_lp_wdt : R/W; bitpos: [1]; default: 0;
         *  Configures lp_wdt registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_wdt:1;
        /** read_ree1_lp_wdt : R/W; bitpos: [2]; default: 0;
         *  Configures lp_wdt registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_wdt:1;
        /** read_ree2_lp_wdt : R/W; bitpos: [3]; default: 0;
         *  Configures lp_wdt registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_wdt:1;
        /** write_tee_lp_wdt : R/W; bitpos: [4]; default: 1;
         *  Configures lp_wdt registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_wdt:1;
        /** write_ree0_lp_wdt : R/W; bitpos: [5]; default: 0;
         *  Configures lp_wdt registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_wdt:1;
        /** write_ree1_lp_wdt : R/W; bitpos: [6]; default: 0;
         *  Configures lp_wdt registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_wdt:1;
        /** write_ree2_lp_wdt : R/W; bitpos: [7]; default: 0;
         *  Configures lp_wdt registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_wdt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_wdt_ctrl_reg_t;

/** Type of lpperi_ctrl register
 *  lpperi read/write control register
 */
typedef union {
    struct {
        /** read_tee_lpperi : R/W; bitpos: [0]; default: 1;
         *  Configures lpperi registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lpperi:1;
        /** read_ree0_lpperi : R/W; bitpos: [1]; default: 0;
         *  Configures lpperi registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lpperi:1;
        /** read_ree1_lpperi : R/W; bitpos: [2]; default: 0;
         *  Configures lpperi registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lpperi:1;
        /** read_ree2_lpperi : R/W; bitpos: [3]; default: 0;
         *  Configures lpperi registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lpperi:1;
        /** write_tee_lpperi : R/W; bitpos: [4]; default: 1;
         *  Configures lpperi registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lpperi:1;
        /** write_ree0_lpperi : R/W; bitpos: [5]; default: 0;
         *  Configures lpperi registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lpperi:1;
        /** write_ree1_lpperi : R/W; bitpos: [6]; default: 0;
         *  Configures lpperi registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lpperi:1;
        /** write_ree2_lpperi : R/W; bitpos: [7]; default: 0;
         *  Configures lpperi registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lpperi:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lpperi_ctrl_reg_t;

/** Type of lp_ana_peri_ctrl register
 *  lp_ana_peri read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_ana_peri : R/W; bitpos: [0]; default: 1;
         *  Configures lp_ana_peri registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_ana_peri:1;
        /** read_ree0_lp_ana_peri : R/W; bitpos: [1]; default: 0;
         *  Configures lp_ana_peri registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_ana_peri:1;
        /** read_ree1_lp_ana_peri : R/W; bitpos: [2]; default: 0;
         *  Configures lp_ana_peri registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_ana_peri:1;
        /** read_ree2_lp_ana_peri : R/W; bitpos: [3]; default: 0;
         *  Configures lp_ana_peri registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_ana_peri:1;
        /** write_tee_lp_ana_peri : R/W; bitpos: [4]; default: 1;
         *  Configures lp_ana_peri registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_ana_peri:1;
        /** write_ree0_lp_ana_peri : R/W; bitpos: [5]; default: 0;
         *  Configures lp_ana_peri registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_ana_peri:1;
        /** write_ree1_lp_ana_peri : R/W; bitpos: [6]; default: 0;
         *  Configures lp_ana_peri registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_ana_peri:1;
        /** write_ree2_lp_ana_peri : R/W; bitpos: [7]; default: 0;
         *  Configures lp_ana_peri registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_ana_peri:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_ana_peri_ctrl_reg_t;

/** Type of lp_touch_ctrl register
 *  lp_touch read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_touch : R/W; bitpos: [0]; default: 1;
         *  Configures lp_touch registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_touch:1;
        /** read_ree0_lp_touch : R/W; bitpos: [1]; default: 0;
         *  Configures lp_touch registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_touch:1;
        /** read_ree1_lp_touch : R/W; bitpos: [2]; default: 0;
         *  Configures lp_touch registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_touch:1;
        /** read_ree2_lp_touch : R/W; bitpos: [3]; default: 0;
         *  Configures lp_touch registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_touch:1;
        /** write_tee_lp_touch : R/W; bitpos: [4]; default: 1;
         *  Configures lp_touch registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_touch:1;
        /** write_ree0_lp_touch : R/W; bitpos: [5]; default: 0;
         *  Configures lp_touch registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_touch:1;
        /** write_ree1_lp_touch : R/W; bitpos: [6]; default: 0;
         *  Configures lp_touch registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_touch:1;
        /** write_ree2_lp_touch : R/W; bitpos: [7]; default: 0;
         *  Configures lp_touch registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_touch:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_touch_ctrl_reg_t;

/** Type of touch_aon_ctrl register
 *  touch_aon read/write control register
 */
typedef union {
    struct {
        /** read_tee_touch_aon : R/W; bitpos: [0]; default: 1;
         *  Configures touch_aon registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_touch_aon:1;
        /** read_ree0_touch_aon : R/W; bitpos: [1]; default: 0;
         *  Configures touch_aon registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_touch_aon:1;
        /** read_ree1_touch_aon : R/W; bitpos: [2]; default: 0;
         *  Configures touch_aon registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_touch_aon:1;
        /** read_ree2_touch_aon : R/W; bitpos: [3]; default: 0;
         *  Configures touch_aon registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_touch_aon:1;
        /** write_tee_touch_aon : R/W; bitpos: [4]; default: 1;
         *  Configures touch_aon registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_touch_aon:1;
        /** write_ree0_touch_aon : R/W; bitpos: [5]; default: 0;
         *  Configures touch_aon registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_touch_aon:1;
        /** write_ree1_touch_aon : R/W; bitpos: [6]; default: 0;
         *  Configures touch_aon registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_touch_aon:1;
        /** write_ree2_touch_aon : R/W; bitpos: [7]; default: 0;
         *  Configures touch_aon registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_touch_aon:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_touch_aon_ctrl_reg_t;

/** Type of lp_io_ctrl register
 *  lp_io read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_io : R/W; bitpos: [0]; default: 1;
         *  Configures lp_io registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_io:1;
        /** read_ree0_lp_io : R/W; bitpos: [1]; default: 0;
         *  Configures lp_io registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_io:1;
        /** read_ree1_lp_io : R/W; bitpos: [2]; default: 0;
         *  Configures lp_io registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_io:1;
        /** read_ree2_lp_io : R/W; bitpos: [3]; default: 0;
         *  Configures lp_io registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_io:1;
        /** write_tee_lp_io : R/W; bitpos: [4]; default: 1;
         *  Configures lp_io registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_io:1;
        /** write_ree0_lp_io : R/W; bitpos: [5]; default: 0;
         *  Configures lp_io registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_io:1;
        /** write_ree1_lp_io : R/W; bitpos: [6]; default: 0;
         *  Configures lp_io registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_io:1;
        /** write_ree2_lp_io : R/W; bitpos: [7]; default: 0;
         *  Configures lp_io registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_io:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_io_ctrl_reg_t;

/** Type of lp_ble_timer_ctrl register
 *  lp_ble_timer read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_ble_timer : R/W; bitpos: [0]; default: 1;
         *  Configures lp_ble_timer registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_ble_timer:1;
        /** read_ree0_lp_ble_timer : R/W; bitpos: [1]; default: 0;
         *  Configures lp_ble_timer registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_ble_timer:1;
        /** read_ree1_lp_ble_timer : R/W; bitpos: [2]; default: 0;
         *  Configures lp_ble_timer registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_ble_timer:1;
        /** read_ree2_lp_ble_timer : R/W; bitpos: [3]; default: 0;
         *  Configures lp_ble_timer registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_ble_timer:1;
        /** write_tee_lp_ble_timer : R/W; bitpos: [4]; default: 1;
         *  Configures lp_ble_timer registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_ble_timer:1;
        /** write_ree0_lp_ble_timer : R/W; bitpos: [5]; default: 0;
         *  Configures lp_ble_timer registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_ble_timer:1;
        /** write_ree1_lp_ble_timer : R/W; bitpos: [6]; default: 0;
         *  Configures lp_ble_timer registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_ble_timer:1;
        /** write_ree2_lp_ble_timer : R/W; bitpos: [7]; default: 0;
         *  Configures lp_ble_timer registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_ble_timer:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_ble_timer_ctrl_reg_t;

/** Type of lp_tee_ctrl register
 *  lp_tee read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_tee : R/W; bitpos: [0]; default: 1;
         *  Configures lp_tee registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_tee:1;
        /** read_ree0_lp_tee : HRO; bitpos: [1]; default: 0;
         *  Configures lp_tee registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_tee:1;
        /** read_ree1_lp_tee : HRO; bitpos: [2]; default: 0;
         *  Configures lp_tee registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_tee:1;
        /** read_ree2_lp_tee : HRO; bitpos: [3]; default: 0;
         *  Configures lp_tee registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_tee:1;
        /** write_tee_lp_tee : R/W; bitpos: [4]; default: 1;
         *  Configures lp_tee registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_tee:1;
        /** write_ree0_lp_tee : HRO; bitpos: [5]; default: 0;
         *  Configures lp_tee registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_tee:1;
        /** write_ree1_lp_tee : HRO; bitpos: [6]; default: 0;
         *  Configures lp_tee registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_tee:1;
        /** write_ree2_lp_tee : HRO; bitpos: [7]; default: 0;
         *  Configures lp_tee registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_tee:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_tee_ctrl_reg_t;

/** Type of huk_ctrl register
 *  lp_tee read/write control register
 */
typedef union {
    struct {
        /** read_tee_huk : R/W; bitpos: [0]; default: 1;
         *  Configures huk registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_huk:1;
        /** read_ree0_huk : R/W; bitpos: [1]; default: 0;
         *  Configures huk registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_huk:1;
        /** read_ree1_huk : R/W; bitpos: [2]; default: 0;
         *  Configures huk registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_huk:1;
        /** read_ree2_huk : R/W; bitpos: [3]; default: 0;
         *  Configures huk registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_huk:1;
        /** write_tee_huk : R/W; bitpos: [4]; default: 1;
         *  Configures huk registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_huk:1;
        /** write_ree0_huk : R/W; bitpos: [5]; default: 0;
         *  Configures huk registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_huk:1;
        /** write_ree1_huk : R/W; bitpos: [6]; default: 0;
         *  Configures huk registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_huk:1;
        /** write_ree2_huk : R/W; bitpos: [7]; default: 0;
         *  Configures huk registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_huk:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_huk_ctrl_reg_t;


/** Group: configure_register */
/** Type of lp_gpio_security register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_lock : R/W; bitpos: [5:0]; default: 0;
         *  This field decides whether lp_gpio_config can be locked, or not. 0 (default):
         *  unlocked. 1: locked.
         */
        uint32_t lp_gpio_lock:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_tee_lp_gpio_security_reg_t;

/** Type of hp_gpio_security_1 register
 *  need des
 */
typedef union {
    struct {
        /** hp_gpio_lock_p1 : R/W; bitpos: [31:0]; default: 0;
         *  This field decides whether hp_gpio_config of PIN0~31 can be locked, or not. 0
         *  (default): unlocked. 1: locked.
         */
        uint32_t hp_gpio_lock_p1:32;
    };
    uint32_t val;
} lp_tee_hp_gpio_security_1_reg_t;

/** Type of hp_gpio_security_2 register
 *  need des
 */
typedef union {
    struct {
        /** hp_gpio_lock_p2 : R/W; bitpos: [7:0]; default: 0;
         *  This field decides whether hp_gpio_config of PIN32~39 can be locked, or not. 0
         *  (default): unlocked. 1: locked.
         */
        uint32_t hp_gpio_lock_p2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_hp_gpio_security_2_reg_t;


/** Group: config register */
/** Type of bus_err_conf register
 *  Clock gating register
 */
typedef union {
    struct {
        /** bus_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether return error response to cpu when access blocked
         *  0: disable error response
         *  1: enable error response
         */
        uint32_t bus_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_bus_err_conf_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: enable automatic clock gating
         *  1: keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38813840;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_tee_date_reg_t;


typedef struct {
    volatile lp_tee_trng_ctrl_reg_t trng_ctrl;
    volatile lp_tee_efuse_ctrl_reg_t efuse_ctrl;
    volatile lp_tee_pmu_ctrl_reg_t pmu_ctrl;
    volatile lp_tee_clkrst_ctrl_reg_t clkrst_ctrl;
    volatile lp_tee_lp_aon_ctrl_ctrl_reg_t lp_aon_ctrl_ctrl;
    volatile lp_tee_lp_timer_ctrl_reg_t lp_timer_ctrl;
    volatile lp_tee_lp_wdt_ctrl_reg_t lp_wdt_ctrl;
    volatile lp_tee_lpperi_ctrl_reg_t lpperi_ctrl;
    volatile lp_tee_lp_ana_peri_ctrl_reg_t lp_ana_peri_ctrl;
    volatile lp_tee_lp_touch_ctrl_reg_t lp_touch_ctrl;
    volatile lp_tee_touch_aon_ctrl_reg_t touch_aon_ctrl;
    volatile lp_tee_lp_io_ctrl_reg_t lp_io_ctrl;
    volatile lp_tee_lp_ble_timer_ctrl_reg_t lp_ble_timer_ctrl;
    volatile lp_tee_lp_tee_ctrl_reg_t lp_tee_ctrl;
    volatile lp_tee_huk_ctrl_reg_t huk_ctrl;
    uint32_t reserved_03c[42];
    volatile lp_tee_lp_gpio_security_reg_t lp_gpio_security;
    volatile lp_tee_hp_gpio_security_1_reg_t hp_gpio_security_1;
    volatile lp_tee_hp_gpio_security_2_reg_t hp_gpio_security_2;
    volatile lp_tee_bus_err_conf_reg_t bus_err_conf;
    uint32_t reserved_0f4;
    volatile lp_tee_clock_gate_reg_t clock_gate;
    volatile lp_tee_date_reg_t date;
} lp_tee_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_tee_dev_t) == 0x100, "Invalid size of lp_tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
