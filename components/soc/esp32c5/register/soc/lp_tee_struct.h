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

/** Group: Configuration Registers */
/** Type of m0_mode_ctrl register
 *  Security mode configuration register
 */
typedef union {
    struct {
        /** m0_mode : R/W; bitpos: [1:0]; default: 3;
         *  Configures the security mode for LP CPU.
         *  0: TEE
         *  1: REE0
         *  2: REE1
         *  3: REE2
         */
        uint32_t m0_mode:2;
        /** m0_lock : R/W; bitpos: [2]; default: 0;
         *  Configures to lock the value of LP_TEE_M0_MODE.
         *  0: Do not lock
         *  1: Lock
         */
        uint32_t m0_lock:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_tee_m0_mode_ctrl_reg_t;


/** Group: Peripheral Read/Write Control Register */
/** Type of efuse_ctrl register
 *  eFuse read/write control register
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
 *  PMU read/write control register
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
 *  LP_CLKRST read/write control register
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
 *  LP_AON read/write control register
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
 *  LP_TIMER read/write control register
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
 *  LP_WDT read/write control register
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

/** Type of lp_peri_ctrl register
 *  LPPERI read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_peri : R/W; bitpos: [0]; default: 1;
         *  Configures lp_peri registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_peri:1;
        /** read_ree0_lp_peri : R/W; bitpos: [1]; default: 0;
         *  Configures lp_peri registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_peri:1;
        /** read_ree1_lp_peri : R/W; bitpos: [2]; default: 0;
         *  Configures lp_peri registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_peri:1;
        /** read_ree2_lp_peri : R/W; bitpos: [3]; default: 0;
         *  Configures lp_peri registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_peri:1;
        /** write_tee_lp_peri : R/W; bitpos: [4]; default: 1;
         *  Configures lp_peri registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_peri:1;
        /** write_ree0_lp_peri : R/W; bitpos: [5]; default: 0;
         *  Configures lp_peri registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_peri:1;
        /** write_ree1_lp_peri : R/W; bitpos: [6]; default: 0;
         *  Configures lp_peri registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_peri:1;
        /** write_ree2_lp_peri : R/W; bitpos: [7]; default: 0;
         *  Configures lp_peri registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_peri:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_peri_ctrl_reg_t;

/** Type of lp_ana_peri_ctrl register
 *  LP_ANA_PERI read/write control register
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

/** Type of lp_io_ctrl register
 *  LP_GPIO and LP_IO_MUX read/write control register
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

/** Type of lp_tee_ctrl register
 *  LP_TEE read/write control register
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

/** Type of uart_ctrl register
 *  LP_UART read/write control register
 */
typedef union {
    struct {
        /** read_tee_uart : R/W; bitpos: [0]; default: 1;
         *  Configures uart registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_uart:1;
        /** read_ree0_uart : R/W; bitpos: [1]; default: 0;
         *  Configures uart registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_uart:1;
        /** read_ree1_uart : R/W; bitpos: [2]; default: 0;
         *  Configures uart registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_uart:1;
        /** read_ree2_uart : R/W; bitpos: [3]; default: 0;
         *  Configures uart registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_uart:1;
        /** write_tee_uart : R/W; bitpos: [4]; default: 1;
         *  Configures uart registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_uart:1;
        /** write_ree0_uart : R/W; bitpos: [5]; default: 0;
         *  Configures uart registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_uart:1;
        /** write_ree1_uart : R/W; bitpos: [6]; default: 0;
         *  Configures uart registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_uart:1;
        /** write_ree2_uart : R/W; bitpos: [7]; default: 0;
         *  Configures uart registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_uart:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_uart_ctrl_reg_t;

/** Type of i2c_ext_ctrl register
 *  LP_I2C read/write control register
 */
typedef union {
    struct {
        /** read_tee_i2c_ext : R/W; bitpos: [0]; default: 1;
         *  Configures i2c_ext registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_i2c_ext:1;
        /** read_ree0_i2c_ext : R/W; bitpos: [1]; default: 0;
         *  Configures i2c_ext registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_i2c_ext:1;
        /** read_ree1_i2c_ext : R/W; bitpos: [2]; default: 0;
         *  Configures i2c_ext registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_i2c_ext:1;
        /** read_ree2_i2c_ext : R/W; bitpos: [3]; default: 0;
         *  Configures i2c_ext registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_i2c_ext:1;
        /** write_tee_i2c_ext : R/W; bitpos: [4]; default: 1;
         *  Configures i2c_ext registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_i2c_ext:1;
        /** write_ree0_i2c_ext : R/W; bitpos: [5]; default: 0;
         *  Configures i2c_ext registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_i2c_ext:1;
        /** write_ree1_i2c_ext : R/W; bitpos: [6]; default: 0;
         *  Configures i2c_ext registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_i2c_ext:1;
        /** write_ree2_i2c_ext : R/W; bitpos: [7]; default: 0;
         *  Configures i2c_ext registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_i2c_ext:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_i2c_ext_ctrl_reg_t;

/** Type of i2c_ana_mst_ctrl register
 *  I2C_ANA_MST read/write control register
 */
typedef union {
    struct {
        /** read_tee_i2c_ana_mst : R/W; bitpos: [0]; default: 1;
         *  Configures i2c_ana_mst registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_i2c_ana_mst:1;
        /** read_ree0_i2c_ana_mst : R/W; bitpos: [1]; default: 0;
         *  Configures i2c_ana_mst registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_i2c_ana_mst:1;
        /** read_ree1_i2c_ana_mst : R/W; bitpos: [2]; default: 0;
         *  Configures i2c_ana_mst registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_i2c_ana_mst:1;
        /** read_ree2_i2c_ana_mst : R/W; bitpos: [3]; default: 0;
         *  Configures i2c_ana_mst registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_i2c_ana_mst:1;
        /** write_tee_i2c_ana_mst : R/W; bitpos: [4]; default: 1;
         *  Configures i2c_ana_mst registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_i2c_ana_mst:1;
        /** write_ree0_i2c_ana_mst : R/W; bitpos: [5]; default: 0;
         *  Configures i2c_ana_mst registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_i2c_ana_mst:1;
        /** write_ree1_i2c_ana_mst : R/W; bitpos: [6]; default: 0;
         *  Configures i2c_ana_mst registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_i2c_ana_mst:1;
        /** write_ree2_i2c_ana_mst : R/W; bitpos: [7]; default: 0;
         *  Configures i2c_ana_mst registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_i2c_ana_mst:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_i2c_ana_mst_ctrl_reg_t;

/** Type of lp_apm_ctrl register
 *  LP_APM read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_apm : R/W; bitpos: [0]; default: 1;
         *  Configures lp_apm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_apm:1;
        /** read_ree0_lp_apm : HRO; bitpos: [1]; default: 0;
         *  Configures lp_apm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_apm:1;
        /** read_ree1_lp_apm : HRO; bitpos: [2]; default: 0;
         *  Configures lp_apm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_apm:1;
        /** read_ree2_lp_apm : HRO; bitpos: [3]; default: 0;
         *  Configures lp_apm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_apm:1;
        /** write_tee_lp_apm : R/W; bitpos: [4]; default: 1;
         *  Configures lp_apm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_apm:1;
        /** write_ree0_lp_apm : HRO; bitpos: [5]; default: 0;
         *  Configures lp_apm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_apm:1;
        /** write_ree1_lp_apm : HRO; bitpos: [6]; default: 0;
         *  Configures lp_apm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_apm:1;
        /** write_ree2_lp_apm : HRO; bitpos: [7]; default: 0;
         *  Configures lp_apm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_apm:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_tee_lp_apm_ctrl_reg_t;

/** Type of force_acc_hp register
 *  Force access to HP SRAM configuration register
 */
typedef union {
    struct {
        /** force_acc_hpmem_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to allow LP CPU to forcibly access HP SRAM regardless of
         *  permission management.
         *  0: Disable force access to HP SRAM
         *  1: Enable force access to HP SRAM
         */
        uint32_t force_acc_hpmem_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_force_acc_hp_reg_t;

/** Type of bus_err_conf register
 *  Error message return configuration register
 */
typedef union {
    struct {
        /** bus_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to return error message to CPU when access is blocked.
         *  0: Disable
         *  1: Enable
         */
        uint32_t bus_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_bus_err_conf_reg_t;


/** Group: Clock Gating Registers */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: Enable automatic clock gating
         *  1: Keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_tee_clock_gate_reg_t;


/** Group: Version Control Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 2363416;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_tee_date_reg_t;


typedef struct {
    volatile lp_tee_m0_mode_ctrl_reg_t m0_mode_ctrl;
    volatile lp_tee_efuse_ctrl_reg_t efuse_ctrl;
    volatile lp_tee_pmu_ctrl_reg_t pmu_ctrl;
    volatile lp_tee_clkrst_ctrl_reg_t clkrst_ctrl;
    volatile lp_tee_lp_aon_ctrl_ctrl_reg_t lp_aon_ctrl_ctrl;
    volatile lp_tee_lp_timer_ctrl_reg_t lp_timer_ctrl;
    volatile lp_tee_lp_wdt_ctrl_reg_t lp_wdt_ctrl;
    volatile lp_tee_lp_peri_ctrl_reg_t lp_peri_ctrl;
    volatile lp_tee_lp_ana_peri_ctrl_reg_t lp_ana_peri_ctrl;
    uint32_t reserved_024[2];
    volatile lp_tee_lp_io_ctrl_reg_t lp_io_ctrl;
    uint32_t reserved_030;
    volatile lp_tee_lp_tee_ctrl_reg_t lp_tee_ctrl;
    volatile lp_tee_uart_ctrl_reg_t uart_ctrl;
    uint32_t reserved_03c;
    volatile lp_tee_i2c_ext_ctrl_reg_t i2c_ext_ctrl;
    volatile lp_tee_i2c_ana_mst_ctrl_reg_t i2c_ana_mst_ctrl;
    uint32_t reserved_048;
    volatile lp_tee_lp_apm_ctrl_reg_t lp_apm_ctrl;
    uint32_t reserved_050[16];
    volatile lp_tee_force_acc_hp_reg_t force_acc_hp;
    uint32_t reserved_094[23];
    volatile lp_tee_bus_err_conf_reg_t bus_err_conf;
    uint32_t reserved_0f4;
    volatile lp_tee_clock_gate_reg_t clock_gate;
    volatile lp_tee_date_reg_t date;
} lp_tee_dev_t;

extern lp_tee_dev_t LP_TEE;

#ifndef __cplusplus
_Static_assert(sizeof(lp_tee_dev_t) == 0x100, "Invalid size of lp_tee_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
