/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: read write control register */
/** Type of lp_sysreg_ctrl register
 *  lp_sysreg read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_sysreg : R/W; bitpos: [0]; default: 1;
         *  Configures lp_sysreg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_sysreg:1;
        /** read_ree0_lp_sysreg : R/W; bitpos: [1]; default: 0;
         *  Configures lp_sysreg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_sysreg:1;
        /** read_ree1_lp_sysreg : R/W; bitpos: [2]; default: 0;
         *  Configures lp_sysreg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_sysreg:1;
        /** read_ree2_lp_sysreg : R/W; bitpos: [3]; default: 0;
         *  Configures lp_sysreg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_sysreg:1;
        /** write_tee_lp_sysreg : R/W; bitpos: [4]; default: 1;
         *  Configures lp_sysreg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_sysreg:1;
        /** write_ree0_lp_sysreg : R/W; bitpos: [5]; default: 0;
         *  Configures lp_sysreg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_sysreg:1;
        /** write_ree1_lp_sysreg : R/W; bitpos: [6]; default: 0;
         *  Configures lp_sysreg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_sysreg:1;
        /** write_ree2_lp_sysreg : R/W; bitpos: [7]; default: 0;
         *  Configures lp_sysreg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_sysreg:1;
        /** lp_sysreg_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_sysreg peri_apm configuration
         */
        uint32_t lp_sysreg_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_sysreg_ctrl_reg_t;

/** Type of lp_aonclkrst_ctrl register
 *  lp_aonclkrst read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_aonclkrst : R/W; bitpos: [0]; default: 1;
         *  Configures lp_aonclkrst registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_aonclkrst:1;
        /** read_ree0_lp_aonclkrst : R/W; bitpos: [1]; default: 0;
         *  Configures lp_aonclkrst registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_aonclkrst:1;
        /** read_ree1_lp_aonclkrst : R/W; bitpos: [2]; default: 0;
         *  Configures lp_aonclkrst registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_aonclkrst:1;
        /** read_ree2_lp_aonclkrst : R/W; bitpos: [3]; default: 0;
         *  Configures lp_aonclkrst registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_aonclkrst:1;
        /** write_tee_lp_aonclkrst : R/W; bitpos: [4]; default: 1;
         *  Configures lp_aonclkrst registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_aonclkrst:1;
        /** write_ree0_lp_aonclkrst : R/W; bitpos: [5]; default: 0;
         *  Configures lp_aonclkrst registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_aonclkrst:1;
        /** write_ree1_lp_aonclkrst : R/W; bitpos: [6]; default: 0;
         *  Configures lp_aonclkrst registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_aonclkrst:1;
        /** write_ree2_lp_aonclkrst : R/W; bitpos: [7]; default: 0;
         *  Configures lp_aonclkrst registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_aonclkrst:1;
        /** lp_aonclkrst_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_aonclkrst peri_apm configuration
         */
        uint32_t lp_aonclkrst_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_aonclkrst_ctrl_reg_t;

/** Type of lp_anaperi_ctrl register
 *  lp_anaperi read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_anaperi : R/W; bitpos: [0]; default: 1;
         *  Configures lp_anaperi registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_anaperi:1;
        /** read_ree0_lp_anaperi : R/W; bitpos: [1]; default: 0;
         *  Configures lp_anaperi registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_anaperi:1;
        /** read_ree1_lp_anaperi : R/W; bitpos: [2]; default: 0;
         *  Configures lp_anaperi registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_anaperi:1;
        /** read_ree2_lp_anaperi : R/W; bitpos: [3]; default: 0;
         *  Configures lp_anaperi registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_anaperi:1;
        /** write_tee_lp_anaperi : R/W; bitpos: [4]; default: 1;
         *  Configures lp_anaperi registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_anaperi:1;
        /** write_ree0_lp_anaperi : R/W; bitpos: [5]; default: 0;
         *  Configures lp_anaperi registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_anaperi:1;
        /** write_ree1_lp_anaperi : R/W; bitpos: [6]; default: 0;
         *  Configures lp_anaperi registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_anaperi:1;
        /** write_ree2_lp_anaperi : R/W; bitpos: [7]; default: 0;
         *  Configures lp_anaperi registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_anaperi:1;
        /** lp_anaperi_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_anaperi peri_apm configuration
         */
        uint32_t lp_anaperi_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_anaperi_ctrl_reg_t;

/** Type of lp_huk_ctrl register
 *  lp_huk read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_huk : R/W; bitpos: [0]; default: 1;
         *  Configures lp_huk registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_huk:1;
        /** read_ree0_lp_huk : R/W; bitpos: [1]; default: 0;
         *  Configures lp_huk registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_huk:1;
        /** read_ree1_lp_huk : R/W; bitpos: [2]; default: 0;
         *  Configures lp_huk registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_huk:1;
        /** read_ree2_lp_huk : R/W; bitpos: [3]; default: 0;
         *  Configures lp_huk registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_huk:1;
        /** write_tee_lp_huk : R/W; bitpos: [4]; default: 1;
         *  Configures lp_huk registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_huk:1;
        /** write_ree0_lp_huk : R/W; bitpos: [5]; default: 0;
         *  Configures lp_huk registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_huk:1;
        /** write_ree1_lp_huk : R/W; bitpos: [6]; default: 0;
         *  Configures lp_huk registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_huk:1;
        /** write_ree2_lp_huk : R/W; bitpos: [7]; default: 0;
         *  Configures lp_huk registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_huk:1;
        /** lp_huk_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_huk peri_apm configuration
         */
        uint32_t lp_huk_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_huk_ctrl_reg_t;

/** Type of lp_pmu_ctrl register
 *  lp_pmu read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_pmu : R/W; bitpos: [0]; default: 1;
         *  Configures lp_pmu registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_pmu:1;
        /** read_ree0_lp_pmu : R/W; bitpos: [1]; default: 0;
         *  Configures lp_pmu registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_pmu:1;
        /** read_ree1_lp_pmu : R/W; bitpos: [2]; default: 0;
         *  Configures lp_pmu registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_pmu:1;
        /** read_ree2_lp_pmu : R/W; bitpos: [3]; default: 0;
         *  Configures lp_pmu registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_pmu:1;
        /** write_tee_lp_pmu : R/W; bitpos: [4]; default: 1;
         *  Configures lp_pmu registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_pmu:1;
        /** write_ree0_lp_pmu : R/W; bitpos: [5]; default: 0;
         *  Configures lp_pmu registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_pmu:1;
        /** write_ree1_lp_pmu : R/W; bitpos: [6]; default: 0;
         *  Configures lp_pmu registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_pmu:1;
        /** write_ree2_lp_pmu : R/W; bitpos: [7]; default: 0;
         *  Configures lp_pmu registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_pmu:1;
        /** lp_pmu_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_pmu peri_apm configuration
         */
        uint32_t lp_pmu_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_pmu_ctrl_reg_t;

/** Type of lp_touch_aon_ctrl register
 *  lp_touch_aon read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_touch_aon : R/W; bitpos: [0]; default: 1;
         *  Configures lp_touch_aon registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_touch_aon:1;
        /** read_ree0_lp_touch_aon : R/W; bitpos: [1]; default: 0;
         *  Configures lp_touch_aon registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_touch_aon:1;
        /** read_ree1_lp_touch_aon : R/W; bitpos: [2]; default: 0;
         *  Configures lp_touch_aon registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_touch_aon:1;
        /** read_ree2_lp_touch_aon : R/W; bitpos: [3]; default: 0;
         *  Configures lp_touch_aon registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_touch_aon:1;
        /** write_tee_lp_touch_aon : R/W; bitpos: [4]; default: 1;
         *  Configures lp_touch_aon registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_touch_aon:1;
        /** write_ree0_lp_touch_aon : R/W; bitpos: [5]; default: 0;
         *  Configures lp_touch_aon registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_touch_aon:1;
        /** write_ree1_lp_touch_aon : R/W; bitpos: [6]; default: 0;
         *  Configures lp_touch_aon registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_touch_aon:1;
        /** write_ree2_lp_touch_aon : R/W; bitpos: [7]; default: 0;
         *  Configures lp_touch_aon registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_touch_aon:1;
        /** lp_touch_aon_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_touch_aon peri_apm configuration
         */
        uint32_t lp_touch_aon_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_touch_aon_ctrl_reg_t;

/** Type of lp_peri_pms_ctrl register
 *  lp_peri_pms read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_peri_pms : R/W; bitpos: [0]; default: 1;
         *  Configures lp_peri_pms registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_peri_pms:1;
        /** read_ree0_lp_peri_pms : HRO; bitpos: [1]; default: 0;
         *  Configures lp_peri_pms registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_peri_pms:1;
        /** read_ree1_lp_peri_pms : HRO; bitpos: [2]; default: 0;
         *  Configures lp_peri_pms registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_peri_pms:1;
        /** read_ree2_lp_peri_pms : HRO; bitpos: [3]; default: 0;
         *  Configures lp_peri_pms registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_peri_pms:1;
        /** write_tee_lp_peri_pms : R/W; bitpos: [4]; default: 1;
         *  Configures lp_peri_pms registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_peri_pms:1;
        /** write_ree0_lp_peri_pms : HRO; bitpos: [5]; default: 0;
         *  Configures lp_peri_pms registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_peri_pms:1;
        /** write_ree1_lp_peri_pms : HRO; bitpos: [6]; default: 0;
         *  Configures lp_peri_pms registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_peri_pms:1;
        /** write_ree2_lp_peri_pms : HRO; bitpos: [7]; default: 0;
         *  Configures lp_peri_pms registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_peri_pms:1;
        /** lp_peri_pms_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_peri_pms peri_apm configuration
         */
        uint32_t lp_peri_pms_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_peri_pms_ctrl_reg_t;

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
        /** lp_tee_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_tee peri_apm configuration
         */
        uint32_t lp_tee_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_tee_ctrl_reg_t;

/** Type of lp_apm_ctrl register
 *  lp_apm read/write control register
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
        /** lp_apm_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_apm peri_apm configuration
         */
        uint32_t lp_apm_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_apm_ctrl_reg_t;

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
        /** lp_timer_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_timer peri_apm configuration
         */
        uint32_t lp_timer_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_timer_ctrl_reg_t;

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
        /** lp_wdt_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_wdt peri_apm configuration
         */
        uint32_t lp_wdt_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_wdt_ctrl_reg_t;

/** Type of lp_clk_cali_sosc_ctrl register
 *  lp_clk_cali_sosc read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_clk_cali_sosc : R/W; bitpos: [0]; default: 1;
         *  Configures lp_clk_cali_sosc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_clk_cali_sosc:1;
        /** read_ree0_lp_clk_cali_sosc : R/W; bitpos: [1]; default: 0;
         *  Configures lp_clk_cali_sosc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_clk_cali_sosc:1;
        /** read_ree1_lp_clk_cali_sosc : R/W; bitpos: [2]; default: 0;
         *  Configures lp_clk_cali_sosc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_clk_cali_sosc:1;
        /** read_ree2_lp_clk_cali_sosc : R/W; bitpos: [3]; default: 0;
         *  Configures lp_clk_cali_sosc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_clk_cali_sosc:1;
        /** write_tee_lp_clk_cali_sosc : R/W; bitpos: [4]; default: 1;
         *  Configures lp_clk_cali_sosc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_clk_cali_sosc:1;
        /** write_ree0_lp_clk_cali_sosc : R/W; bitpos: [5]; default: 0;
         *  Configures lp_clk_cali_sosc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_clk_cali_sosc:1;
        /** write_ree1_lp_clk_cali_sosc : R/W; bitpos: [6]; default: 0;
         *  Configures lp_clk_cali_sosc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_clk_cali_sosc:1;
        /** write_ree2_lp_clk_cali_sosc : R/W; bitpos: [7]; default: 0;
         *  Configures lp_clk_cali_sosc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_clk_cali_sosc:1;
        /** lp_clk_cali_sosc_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_clk_cali_sosc peri_apm configuration
         */
        uint32_t lp_clk_cali_sosc_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_clk_cali_sosc_ctrl_reg_t;

/** Type of lp_clk_cali_fosc_ctrl register
 *  lp_clk_cali_fosc read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_clk_cali_fosc : R/W; bitpos: [0]; default: 1;
         *  Configures lp_clk_cali_fosc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_clk_cali_fosc:1;
        /** read_ree0_lp_clk_cali_fosc : R/W; bitpos: [1]; default: 0;
         *  Configures lp_clk_cali_fosc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_clk_cali_fosc:1;
        /** read_ree1_lp_clk_cali_fosc : R/W; bitpos: [2]; default: 0;
         *  Configures lp_clk_cali_fosc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_clk_cali_fosc:1;
        /** read_ree2_lp_clk_cali_fosc : R/W; bitpos: [3]; default: 0;
         *  Configures lp_clk_cali_fosc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_clk_cali_fosc:1;
        /** write_tee_lp_clk_cali_fosc : R/W; bitpos: [4]; default: 1;
         *  Configures lp_clk_cali_fosc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_clk_cali_fosc:1;
        /** write_ree0_lp_clk_cali_fosc : R/W; bitpos: [5]; default: 0;
         *  Configures lp_clk_cali_fosc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_clk_cali_fosc:1;
        /** write_ree1_lp_clk_cali_fosc : R/W; bitpos: [6]; default: 0;
         *  Configures lp_clk_cali_fosc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_clk_cali_fosc:1;
        /** write_ree2_lp_clk_cali_fosc : R/W; bitpos: [7]; default: 0;
         *  Configures lp_clk_cali_fosc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_clk_cali_fosc:1;
        /** lp_clk_cali_fosc_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_clk_cali_fosc peri_apm configuration
         */
        uint32_t lp_clk_cali_fosc_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_clk_cali_fosc_ctrl_reg_t;

/** Type of lp_pwr_reg_ctrl register
 *  lp_pwr_reg read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_pwr_reg : R/W; bitpos: [0]; default: 1;
         *  Configures lp_pwr_reg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_pwr_reg:1;
        /** read_ree0_lp_pwr_reg : R/W; bitpos: [1]; default: 0;
         *  Configures lp_pwr_reg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_pwr_reg:1;
        /** read_ree1_lp_pwr_reg : R/W; bitpos: [2]; default: 0;
         *  Configures lp_pwr_reg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_pwr_reg:1;
        /** read_ree2_lp_pwr_reg : R/W; bitpos: [3]; default: 0;
         *  Configures lp_pwr_reg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_pwr_reg:1;
        /** write_tee_lp_pwr_reg : R/W; bitpos: [4]; default: 1;
         *  Configures lp_pwr_reg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_pwr_reg:1;
        /** write_ree0_lp_pwr_reg : R/W; bitpos: [5]; default: 0;
         *  Configures lp_pwr_reg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_pwr_reg:1;
        /** write_ree1_lp_pwr_reg : R/W; bitpos: [6]; default: 0;
         *  Configures lp_pwr_reg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_pwr_reg:1;
        /** write_ree2_lp_pwr_reg : R/W; bitpos: [7]; default: 0;
         *  Configures lp_pwr_reg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_pwr_reg:1;
        /** lp_pwr_reg_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_pwr_reg peri_apm configuration
         */
        uint32_t lp_pwr_reg_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_pwr_reg_ctrl_reg_t;

/** Type of lp_periclkrst_ctrl register
 *  lp_periclkrst read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_periclkrst : R/W; bitpos: [0]; default: 1;
         *  Configures lp_periclkrst registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_periclkrst:1;
        /** read_ree0_lp_periclkrst : R/W; bitpos: [1]; default: 0;
         *  Configures lp_periclkrst registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_periclkrst:1;
        /** read_ree1_lp_periclkrst : R/W; bitpos: [2]; default: 0;
         *  Configures lp_periclkrst registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_periclkrst:1;
        /** read_ree2_lp_periclkrst : R/W; bitpos: [3]; default: 0;
         *  Configures lp_periclkrst registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_periclkrst:1;
        /** write_tee_lp_periclkrst : R/W; bitpos: [4]; default: 1;
         *  Configures lp_periclkrst registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_periclkrst:1;
        /** write_ree0_lp_periclkrst : R/W; bitpos: [5]; default: 0;
         *  Configures lp_periclkrst registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_periclkrst:1;
        /** write_ree1_lp_periclkrst : R/W; bitpos: [6]; default: 0;
         *  Configures lp_periclkrst registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_periclkrst:1;
        /** write_ree2_lp_periclkrst : R/W; bitpos: [7]; default: 0;
         *  Configures lp_periclkrst registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_periclkrst:1;
        /** lp_periclkrst_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_periclkrst peri_apm configuration
         */
        uint32_t lp_periclkrst_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_periclkrst_ctrl_reg_t;

/** Type of lp_iomux_ctrl register
 *  lp_iomux read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_iomux : R/W; bitpos: [0]; default: 1;
         *  Configures lp_iomux registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_iomux:1;
        /** read_ree0_lp_iomux : R/W; bitpos: [1]; default: 0;
         *  Configures lp_iomux registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_iomux:1;
        /** read_ree1_lp_iomux : R/W; bitpos: [2]; default: 0;
         *  Configures lp_iomux registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_iomux:1;
        /** read_ree2_lp_iomux : R/W; bitpos: [3]; default: 0;
         *  Configures lp_iomux registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_iomux:1;
        /** write_tee_lp_iomux : R/W; bitpos: [4]; default: 1;
         *  Configures lp_iomux registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_iomux:1;
        /** write_ree0_lp_iomux : R/W; bitpos: [5]; default: 0;
         *  Configures lp_iomux registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_iomux:1;
        /** write_ree1_lp_iomux : R/W; bitpos: [6]; default: 0;
         *  Configures lp_iomux registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_iomux:1;
        /** write_ree2_lp_iomux : R/W; bitpos: [7]; default: 0;
         *  Configures lp_iomux registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_iomux:1;
        /** lp_iomux_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_iomux peri_apm configuration
         */
        uint32_t lp_iomux_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_iomux_ctrl_reg_t;

/** Type of lp_intr_ctrl register
 *  lp_intr read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_intr : R/W; bitpos: [0]; default: 1;
         *  Configures lp_intr registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_intr:1;
        /** read_ree0_lp_intr : R/W; bitpos: [1]; default: 0;
         *  Configures lp_intr registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_intr:1;
        /** read_ree1_lp_intr : R/W; bitpos: [2]; default: 0;
         *  Configures lp_intr registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_intr:1;
        /** read_ree2_lp_intr : R/W; bitpos: [3]; default: 0;
         *  Configures lp_intr registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_intr:1;
        /** write_tee_lp_intr : R/W; bitpos: [4]; default: 1;
         *  Configures lp_intr registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_intr:1;
        /** write_ree0_lp_intr : R/W; bitpos: [5]; default: 0;
         *  Configures lp_intr registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_intr:1;
        /** write_ree1_lp_intr : R/W; bitpos: [6]; default: 0;
         *  Configures lp_intr registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_intr:1;
        /** write_ree2_lp_intr : R/W; bitpos: [7]; default: 0;
         *  Configures lp_intr registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_intr:1;
        /** lp_intr_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_intr peri_apm configuration
         */
        uint32_t lp_intr_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_intr_ctrl_reg_t;

/** Type of lp_efuse_ctrl register
 *  lp_efuse read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_efuse : R/W; bitpos: [0]; default: 1;
         *  Configures lp_efuse registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_efuse:1;
        /** read_ree0_lp_efuse : R/W; bitpos: [1]; default: 0;
         *  Configures lp_efuse registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_efuse:1;
        /** read_ree1_lp_efuse : R/W; bitpos: [2]; default: 0;
         *  Configures lp_efuse registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_efuse:1;
        /** read_ree2_lp_efuse : R/W; bitpos: [3]; default: 0;
         *  Configures lp_efuse registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_efuse:1;
        /** write_tee_lp_efuse : R/W; bitpos: [4]; default: 1;
         *  Configures lp_efuse registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_efuse:1;
        /** write_ree0_lp_efuse : R/W; bitpos: [5]; default: 0;
         *  Configures lp_efuse registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_efuse:1;
        /** write_ree1_lp_efuse : R/W; bitpos: [6]; default: 0;
         *  Configures lp_efuse registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_efuse:1;
        /** write_ree2_lp_efuse : R/W; bitpos: [7]; default: 0;
         *  Configures lp_efuse registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_efuse:1;
        /** lp_efuse_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_efuse peri_apm configuration
         */
        uint32_t lp_efuse_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_efuse_ctrl_reg_t;

/** Type of lp_uart_ctrl register
 *  lp_uart read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_uart : R/W; bitpos: [0]; default: 1;
         *  Configures lp_uart registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_uart:1;
        /** read_ree0_lp_uart : R/W; bitpos: [1]; default: 0;
         *  Configures lp_uart registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_uart:1;
        /** read_ree1_lp_uart : R/W; bitpos: [2]; default: 0;
         *  Configures lp_uart registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_uart:1;
        /** read_ree2_lp_uart : R/W; bitpos: [3]; default: 0;
         *  Configures lp_uart registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_uart:1;
        /** write_tee_lp_uart : R/W; bitpos: [4]; default: 1;
         *  Configures lp_uart registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_uart:1;
        /** write_ree0_lp_uart : R/W; bitpos: [5]; default: 0;
         *  Configures lp_uart registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_uart:1;
        /** write_ree1_lp_uart : R/W; bitpos: [6]; default: 0;
         *  Configures lp_uart registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_uart:1;
        /** write_ree2_lp_uart : R/W; bitpos: [7]; default: 0;
         *  Configures lp_uart registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_uart:1;
        /** lp_uart_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_uart peri_apm configuration
         */
        uint32_t lp_uart_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_uart_ctrl_reg_t;

/** Type of lp_i2c_ctrl register
 *  lp_i2c read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_i2c : R/W; bitpos: [0]; default: 1;
         *  Configures lp_i2c registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_i2c:1;
        /** read_ree0_lp_i2c : R/W; bitpos: [1]; default: 0;
         *  Configures lp_i2c registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_i2c:1;
        /** read_ree1_lp_i2c : R/W; bitpos: [2]; default: 0;
         *  Configures lp_i2c registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_i2c:1;
        /** read_ree2_lp_i2c : R/W; bitpos: [3]; default: 0;
         *  Configures lp_i2c registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_i2c:1;
        /** write_tee_lp_i2c : R/W; bitpos: [4]; default: 1;
         *  Configures lp_i2c registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_i2c:1;
        /** write_ree0_lp_i2c : R/W; bitpos: [5]; default: 0;
         *  Configures lp_i2c registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_i2c:1;
        /** write_ree1_lp_i2c : R/W; bitpos: [6]; default: 0;
         *  Configures lp_i2c registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_i2c:1;
        /** write_ree2_lp_i2c : R/W; bitpos: [7]; default: 0;
         *  Configures lp_i2c registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_i2c:1;
        /** lp_i2c_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_i2c peri_apm configuration
         */
        uint32_t lp_i2c_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_i2c_ctrl_reg_t;

/** Type of lp_spi_ctrl register
 *  lp_spi read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_spi : R/W; bitpos: [0]; default: 1;
         *  Configures lp_spi registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_spi:1;
        /** read_ree0_lp_spi : R/W; bitpos: [1]; default: 0;
         *  Configures lp_spi registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_spi:1;
        /** read_ree1_lp_spi : R/W; bitpos: [2]; default: 0;
         *  Configures lp_spi registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_spi:1;
        /** read_ree2_lp_spi : R/W; bitpos: [3]; default: 0;
         *  Configures lp_spi registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_spi:1;
        /** write_tee_lp_spi : R/W; bitpos: [4]; default: 1;
         *  Configures lp_spi registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_spi:1;
        /** write_ree0_lp_spi : R/W; bitpos: [5]; default: 0;
         *  Configures lp_spi registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_spi:1;
        /** write_ree1_lp_spi : R/W; bitpos: [6]; default: 0;
         *  Configures lp_spi registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_spi:1;
        /** write_ree2_lp_spi : R/W; bitpos: [7]; default: 0;
         *  Configures lp_spi registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_spi:1;
        /** lp_spi_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_spi peri_apm configuration
         */
        uint32_t lp_spi_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_spi_ctrl_reg_t;

/** Type of lp_i2cmst_ctrl register
 *  lp_i2cmst read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_i2cmst : R/W; bitpos: [0]; default: 1;
         *  Configures lp_i2cmst registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_i2cmst:1;
        /** read_ree0_lp_i2cmst : R/W; bitpos: [1]; default: 0;
         *  Configures lp_i2cmst registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_i2cmst:1;
        /** read_ree1_lp_i2cmst : R/W; bitpos: [2]; default: 0;
         *  Configures lp_i2cmst registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_i2cmst:1;
        /** read_ree2_lp_i2cmst : R/W; bitpos: [3]; default: 0;
         *  Configures lp_i2cmst registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_i2cmst:1;
        /** write_tee_lp_i2cmst : R/W; bitpos: [4]; default: 1;
         *  Configures lp_i2cmst registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_i2cmst:1;
        /** write_ree0_lp_i2cmst : R/W; bitpos: [5]; default: 0;
         *  Configures lp_i2cmst registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_i2cmst:1;
        /** write_ree1_lp_i2cmst : R/W; bitpos: [6]; default: 0;
         *  Configures lp_i2cmst registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_i2cmst:1;
        /** write_ree2_lp_i2cmst : R/W; bitpos: [7]; default: 0;
         *  Configures lp_i2cmst registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_i2cmst:1;
        /** lp_i2cmst_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_i2cmst peri_apm configuration
         */
        uint32_t lp_i2cmst_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_i2cmst_ctrl_reg_t;

/** Type of lp_trng_ctrl register
 *  lp_trng read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_trng : R/W; bitpos: [0]; default: 1;
         *  Configures lp_trng registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_trng:1;
        /** read_ree0_lp_trng : R/W; bitpos: [1]; default: 0;
         *  Configures lp_trng registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_trng:1;
        /** read_ree1_lp_trng : R/W; bitpos: [2]; default: 0;
         *  Configures lp_trng registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_trng:1;
        /** read_ree2_lp_trng : R/W; bitpos: [3]; default: 0;
         *  Configures lp_trng registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_trng:1;
        /** write_tee_lp_trng : R/W; bitpos: [4]; default: 1;
         *  Configures lp_trng registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_trng:1;
        /** write_ree0_lp_trng : R/W; bitpos: [5]; default: 0;
         *  Configures lp_trng registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_trng:1;
        /** write_ree1_lp_trng : R/W; bitpos: [6]; default: 0;
         *  Configures lp_trng registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_trng:1;
        /** write_ree2_lp_trng : R/W; bitpos: [7]; default: 0;
         *  Configures lp_trng registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_trng:1;
        /** lp_trng_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_trng peri_apm configuration
         */
        uint32_t lp_trng_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_trng_ctrl_reg_t;

/** Type of lp_adc_ctrl register
 *  lp_adc read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_adc : R/W; bitpos: [0]; default: 1;
         *  Configures lp_adc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_adc:1;
        /** read_ree0_lp_adc : R/W; bitpos: [1]; default: 0;
         *  Configures lp_adc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_adc:1;
        /** read_ree1_lp_adc : R/W; bitpos: [2]; default: 0;
         *  Configures lp_adc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_adc:1;
        /** read_ree2_lp_adc : R/W; bitpos: [3]; default: 0;
         *  Configures lp_adc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_adc:1;
        /** write_tee_lp_adc : R/W; bitpos: [4]; default: 1;
         *  Configures lp_adc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_adc:1;
        /** write_ree0_lp_adc : R/W; bitpos: [5]; default: 0;
         *  Configures lp_adc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_adc:1;
        /** write_ree1_lp_adc : R/W; bitpos: [6]; default: 0;
         *  Configures lp_adc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_adc:1;
        /** write_ree2_lp_adc : R/W; bitpos: [7]; default: 0;
         *  Configures lp_adc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_adc:1;
        /** lp_adc_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_adc peri_apm configuration
         */
        uint32_t lp_adc_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_adc_ctrl_reg_t;

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
        /** lp_touch_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_touch peri_apm configuration
         */
        uint32_t lp_touch_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_touch_ctrl_reg_t;

/** Type of lp_mailbox_ctrl register
 *  lp_mailbox read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_mailbox : R/W; bitpos: [0]; default: 1;
         *  Configures lp_mailbox registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_mailbox:1;
        /** read_ree0_lp_mailbox : R/W; bitpos: [1]; default: 0;
         *  Configures lp_mailbox registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_mailbox:1;
        /** read_ree1_lp_mailbox : R/W; bitpos: [2]; default: 0;
         *  Configures lp_mailbox registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_mailbox:1;
        /** read_ree2_lp_mailbox : R/W; bitpos: [3]; default: 0;
         *  Configures lp_mailbox registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_mailbox:1;
        /** write_tee_lp_mailbox : R/W; bitpos: [4]; default: 1;
         *  Configures lp_mailbox registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_mailbox:1;
        /** write_ree0_lp_mailbox : R/W; bitpos: [5]; default: 0;
         *  Configures lp_mailbox registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_mailbox:1;
        /** write_ree1_lp_mailbox : R/W; bitpos: [6]; default: 0;
         *  Configures lp_mailbox registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_mailbox:1;
        /** write_ree2_lp_mailbox : R/W; bitpos: [7]; default: 0;
         *  Configures lp_mailbox registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_mailbox:1;
        /** lp_mailbox_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_mailbox peri_apm configuration
         */
        uint32_t lp_mailbox_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_mailbox_ctrl_reg_t;

/** Type of lp_tsens_ctrl register
 *  lp_tsens read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_tsens : R/W; bitpos: [0]; default: 1;
         *  Configures lp_tsens registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_tsens:1;
        /** read_ree0_lp_tsens : R/W; bitpos: [1]; default: 0;
         *  Configures lp_tsens registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_tsens:1;
        /** read_ree1_lp_tsens : R/W; bitpos: [2]; default: 0;
         *  Configures lp_tsens registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_tsens:1;
        /** read_ree2_lp_tsens : R/W; bitpos: [3]; default: 0;
         *  Configures lp_tsens registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_tsens:1;
        /** write_tee_lp_tsens : R/W; bitpos: [4]; default: 1;
         *  Configures lp_tsens registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_tsens:1;
        /** write_ree0_lp_tsens : R/W; bitpos: [5]; default: 0;
         *  Configures lp_tsens registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_tsens:1;
        /** write_ree1_lp_tsens : R/W; bitpos: [6]; default: 0;
         *  Configures lp_tsens registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_tsens:1;
        /** write_ree2_lp_tsens : R/W; bitpos: [7]; default: 0;
         *  Configures lp_tsens registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_tsens:1;
        /** lp_tsens_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_tsens peri_apm configuration
         */
        uint32_t lp_tsens_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_tsens_ctrl_reg_t;

/** Type of lp_ahb_pdma_ctrl register
 *  lp_ahb_pdma read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_ahb_pdma : R/W; bitpos: [0]; default: 1;
         *  Configures lp_ahb_pdma registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_ahb_pdma:1;
        /** read_ree0_lp_ahb_pdma : R/W; bitpos: [1]; default: 0;
         *  Configures lp_ahb_pdma registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_ahb_pdma:1;
        /** read_ree1_lp_ahb_pdma : R/W; bitpos: [2]; default: 0;
         *  Configures lp_ahb_pdma registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_ahb_pdma:1;
        /** read_ree2_lp_ahb_pdma : R/W; bitpos: [3]; default: 0;
         *  Configures lp_ahb_pdma registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_ahb_pdma:1;
        /** write_tee_lp_ahb_pdma : R/W; bitpos: [4]; default: 1;
         *  Configures lp_ahb_pdma registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_ahb_pdma:1;
        /** write_ree0_lp_ahb_pdma : R/W; bitpos: [5]; default: 0;
         *  Configures lp_ahb_pdma registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_ahb_pdma:1;
        /** write_ree1_lp_ahb_pdma : R/W; bitpos: [6]; default: 0;
         *  Configures lp_ahb_pdma registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_ahb_pdma:1;
        /** write_ree2_lp_ahb_pdma : R/W; bitpos: [7]; default: 0;
         *  Configures lp_ahb_pdma registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_ahb_pdma:1;
        /** lp_ahb_pdma_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_ahb_pdma peri_apm configuration
         */
        uint32_t lp_ahb_pdma_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_ahb_pdma_ctrl_reg_t;

/** Type of lp_dac_ctrl register
 *  lp_dac read/write control register
 */
typedef union {
    struct {
        /** read_tee_lp_dac : R/W; bitpos: [0]; default: 1;
         *  Configures lp_dac registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_lp_dac:1;
        /** read_ree0_lp_dac : R/W; bitpos: [1]; default: 0;
         *  Configures lp_dac registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_lp_dac:1;
        /** read_ree1_lp_dac : R/W; bitpos: [2]; default: 0;
         *  Configures lp_dac registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_lp_dac:1;
        /** read_ree2_lp_dac : R/W; bitpos: [3]; default: 0;
         *  Configures lp_dac registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_lp_dac:1;
        /** write_tee_lp_dac : R/W; bitpos: [4]; default: 1;
         *  Configures lp_dac registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_lp_dac:1;
        /** write_ree0_lp_dac : R/W; bitpos: [5]; default: 0;
         *  Configures lp_dac registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_lp_dac:1;
        /** write_ree1_lp_dac : R/W; bitpos: [6]; default: 0;
         *  Configures lp_dac registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_lp_dac:1;
        /** write_ree2_lp_dac : R/W; bitpos: [7]; default: 0;
         *  Configures lp_dac registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_lp_dac:1;
        /** lp_dac_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock lp_dac peri_apm configuration
         */
        uint32_t lp_dac_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_peri_pms_lp_dac_ctrl_reg_t;


/** Group: PMS Register */
/** Type of lp_peri0_0 register
 *  LP_PERI0 PMS configuration & info register
 */
typedef union {
    struct {
        /** lp_peri0_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear lp_peri0 peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t lp_peri0_pms_exception_clr:1;
        /** lp_peri0_pms_exception_det : RO; bitpos: [1]; default: 0;
         *  Represents whether the lp_peri0 pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t lp_peri0_pms_exception_det:1;
        /** lp_peri0_pms_exception_id : RO; bitpos: [7:2]; default: 0;
         *  Represents the master id when lp_peri0 pms has been triggered.
         */
        uint32_t lp_peri0_pms_exception_id:6;
        /** lp_peri0_pms_exception_mode : RO; bitpos: [9:8]; default: 0;
         *  Represents the security mode when lp_peri0 pms has been triggered.
         */
        uint32_t lp_peri0_pms_exception_mode:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} lp_peri_pms_lp_peri0_0_reg_t;

/** Type of lp_peri0_1 register
 *  LP_PERI0 PMS exception addr record register
 */
typedef union {
    struct {
        /** lp_peri0_pms_exception_addr : RO; bitpos: [23:0]; default: 0;
         *  Represents the access address (bit27~bit0) when lp_peri0 pms has been triggered.
         */
        uint32_t lp_peri0_pms_exception_addr:24;
        /** lp_peri0_pms_exception_addr_const : HRO; bitpos: [31:24]; default: 32;
         *  Represents the access address (bit31~bit28) when lp_peri0 pms has been triggered.
         */
        uint32_t lp_peri0_pms_exception_addr_const:8;
    };
    uint32_t val;
} lp_peri_pms_lp_peri0_1_reg_t;

/** Type of lp_peri1_0 register
 *  LP_PERI1 PMS configuration & info register
 */
typedef union {
    struct {
        /** lp_peri1_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear lp_peri1 peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t lp_peri1_pms_exception_clr:1;
        /** lp_peri1_pms_exception_det : RO; bitpos: [1]; default: 0;
         *  Represents whether the lp_peri1 pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t lp_peri1_pms_exception_det:1;
        /** lp_peri1_pms_exception_id : RO; bitpos: [7:2]; default: 0;
         *  Represents the master id when lp_peri1 pms has been triggered.
         */
        uint32_t lp_peri1_pms_exception_id:6;
        /** lp_peri1_pms_exception_mode : RO; bitpos: [9:8]; default: 0;
         *  Represents the security mode when lp_peri1 pms has been triggered.
         */
        uint32_t lp_peri1_pms_exception_mode:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} lp_peri_pms_lp_peri1_0_reg_t;

/** Type of lp_peri1_1 register
 *  LP_PERI1 PMS exception addr record register
 */
typedef union {
    struct {
        /** lp_peri1_pms_exception_addr : RO; bitpos: [23:0]; default: 0;
         *  Represents the access address (bit27~bit0) when lp_peri1 pms has been triggered.
         */
        uint32_t lp_peri1_pms_exception_addr:24;
        /** lp_peri1_pms_exception_addr_const : HRO; bitpos: [31:24]; default: 32;
         *  Represents the access address (bit31~bit28) when lp_peri1 pms has been triggered.
         */
        uint32_t lp_peri1_pms_exception_addr_const:8;
    };
    uint32_t val;
} lp_peri_pms_lp_peri1_1_reg_t;

/** Type of int_en register
 *  APM interrupt enable register
 */
typedef union {
    struct {
        /** lp_peri1_pms_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable lp_peri1 pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t lp_peri1_pms_int_en:1;
        /** lp_peri0_pms_int_en : R/W; bitpos: [1]; default: 0;
         *  Configures to enable lp_peri0 pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t lp_peri0_pms_int_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_peri_pms_int_en_reg_t;


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
} lp_peri_pms_bus_err_conf_reg_t;


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
} lp_peri_pms_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38834752;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_peri_pms_date_reg_t;


typedef struct {
    volatile lp_peri_pms_lp_sysreg_ctrl_reg_t lp_sysreg_ctrl;
    volatile lp_peri_pms_lp_aonclkrst_ctrl_reg_t lp_aonclkrst_ctrl;
    volatile lp_peri_pms_lp_anaperi_ctrl_reg_t lp_anaperi_ctrl;
    volatile lp_peri_pms_lp_huk_ctrl_reg_t lp_huk_ctrl;
    volatile lp_peri_pms_lp_pmu_ctrl_reg_t lp_pmu_ctrl;
    volatile lp_peri_pms_lp_touch_aon_ctrl_reg_t lp_touch_aon_ctrl;
    volatile lp_peri_pms_lp_peri_pms_ctrl_reg_t lp_peri_pms_ctrl;
    volatile lp_peri_pms_lp_tee_ctrl_reg_t lp_tee_ctrl;
    volatile lp_peri_pms_lp_apm_ctrl_reg_t lp_apm_ctrl;
    volatile lp_peri_pms_lp_timer_ctrl_reg_t lp_timer_ctrl;
    volatile lp_peri_pms_lp_wdt_ctrl_reg_t lp_wdt_ctrl;
    volatile lp_peri_pms_lp_clk_cali_sosc_ctrl_reg_t lp_clk_cali_sosc_ctrl;
    volatile lp_peri_pms_lp_clk_cali_fosc_ctrl_reg_t lp_clk_cali_fosc_ctrl;
    volatile lp_peri_pms_lp_pwr_reg_ctrl_reg_t lp_pwr_reg_ctrl;
    volatile lp_peri_pms_lp_periclkrst_ctrl_reg_t lp_periclkrst_ctrl;
    volatile lp_peri_pms_lp_iomux_ctrl_reg_t lp_iomux_ctrl;
    volatile lp_peri_pms_lp_intr_ctrl_reg_t lp_intr_ctrl;
    volatile lp_peri_pms_lp_efuse_ctrl_reg_t lp_efuse_ctrl;
    volatile lp_peri_pms_lp_uart_ctrl_reg_t lp_uart_ctrl;
    volatile lp_peri_pms_lp_i2c_ctrl_reg_t lp_i2c_ctrl;
    volatile lp_peri_pms_lp_spi_ctrl_reg_t lp_spi_ctrl;
    volatile lp_peri_pms_lp_i2cmst_ctrl_reg_t lp_i2cmst_ctrl;
    volatile lp_peri_pms_lp_trng_ctrl_reg_t lp_trng_ctrl;
    volatile lp_peri_pms_lp_adc_ctrl_reg_t lp_adc_ctrl;
    volatile lp_peri_pms_lp_touch_ctrl_reg_t lp_touch_ctrl;
    volatile lp_peri_pms_lp_mailbox_ctrl_reg_t lp_mailbox_ctrl;
    volatile lp_peri_pms_lp_tsens_ctrl_reg_t lp_tsens_ctrl;
    volatile lp_peri_pms_lp_ahb_pdma_ctrl_reg_t lp_ahb_pdma_ctrl;
    volatile lp_peri_pms_lp_dac_ctrl_reg_t lp_dac_ctrl;
    uint32_t reserved_074[99];
    volatile lp_peri_pms_lp_peri0_0_reg_t lp_peri0_0;
    volatile lp_peri_pms_lp_peri0_1_reg_t lp_peri0_1;
    volatile lp_peri_pms_lp_peri1_0_reg_t lp_peri1_0;
    volatile lp_peri_pms_lp_peri1_1_reg_t lp_peri1_1;
    uint32_t reserved_210[60];
    volatile lp_peri_pms_int_en_reg_t int_en;
    uint32_t reserved_304[315];
    volatile lp_peri_pms_bus_err_conf_reg_t bus_err_conf;
    uint32_t reserved_7f4;
    volatile lp_peri_pms_clock_gate_reg_t clock_gate;
    volatile lp_peri_pms_date_reg_t date;
} lp_peri_pms_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_peri_pms_dev_t) == 0x800, "Invalid size of lp_peri_pms_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
