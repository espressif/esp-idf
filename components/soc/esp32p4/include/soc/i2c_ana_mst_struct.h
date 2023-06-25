/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configure Register */
/** Type of i2c0_ctrl register
 *  need des
 */
typedef union {
    struct {
        /** i2c0_ctrl : R/W; bitpos: [24:0]; default: 0;
         *  need des
         */
        uint32_t i2c0_ctrl:25;
        /** i2c0_busy : RO; bitpos: [25]; default: 0;
         *  need des
         */
        uint32_t i2c0_busy:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} i2c_ana_mst_i2c0_ctrl_reg_t;

/** Type of i2c1_ctrl register
 *  need des
 */
typedef union {
    struct {
        /** i2c1_ctrl : R/W; bitpos: [24:0]; default: 0;
         *  need des
         */
        uint32_t i2c1_ctrl:25;
        /** i2c1_busy : RO; bitpos: [25]; default: 0;
         *  need des
         */
        uint32_t i2c1_busy:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} i2c_ana_mst_i2c1_ctrl_reg_t;

/** Type of i2c0_conf register
 *  need des
 */
typedef union {
    struct {
        /** i2c0_conf : R/W; bitpos: [23:0]; default: 0;
         *  need des
         */
        uint32_t i2c0_conf:24;
        /** i2c0_status : RO; bitpos: [31:24]; default: 0;
         *  need des
         */
        uint32_t i2c0_status:8;
    };
    uint32_t val;
} i2c_ana_mst_i2c0_conf_reg_t;

/** Type of i2c1_conf register
 *  need des
 */
typedef union {
    struct {
        /** i2c1_conf : R/W; bitpos: [23:0]; default: 0;
         *  need des
         */
        uint32_t i2c1_conf:24;
        /** i2c1_status : RO; bitpos: [31:24]; default: 0;
         *  need des
         */
        uint32_t i2c1_status:8;
    };
    uint32_t val;
} i2c_ana_mst_i2c1_conf_reg_t;

/** Type of i2c_burst_conf register
 *  need des
 */
typedef union {
    struct {
        /** i2c_mst_burst_ctrl : R/W; bitpos: [31:0]; default: 0;
         *  need des
         */
        uint32_t i2c_mst_burst_ctrl:32;
    };
    uint32_t val;
} i2c_ana_mst_i2c_burst_conf_reg_t;

/** Type of i2c_burst_status register
 *  need des
 */
typedef union {
    struct {
        /** i2c_mst_burst_done : RO; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t i2c_mst_burst_done:1;
        /** i2c_mst0_burst_err_flag : RO; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t i2c_mst0_burst_err_flag:1;
        /** i2c_mst1_burst_err_flag : RO; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t i2c_mst1_burst_err_flag:1;
        uint32_t reserved_3:17;
        /** i2c_mst_burst_timeout_cnt : R/W; bitpos: [31:20]; default: 1024;
         *  need des
         */
        uint32_t i2c_mst_burst_timeout_cnt:12;
    };
    uint32_t val;
} i2c_ana_mst_i2c_burst_status_reg_t;

/** Type of ana_conf0 register
 *  need des
 */
typedef union {
    struct {
        /** ana_conf0 : R/W; bitpos: [23:0]; default: 0;
         *  need des
         */
        uint32_t ana_conf0:24;
        /** ana_status0 : RO; bitpos: [31:24]; default: 0;
         *  need des
         */
        uint32_t ana_status0:8;
    };
    uint32_t val;
} i2c_ana_mst_ana_conf0_reg_t;

/** Type of ana_conf1 register
 *  need des
 */
typedef union {
    struct {
        /** ana_conf1 : R/W; bitpos: [23:0]; default: 0;
         *  need des
         */
        uint32_t ana_conf1:24;
        /** ana_status1 : RO; bitpos: [31:24]; default: 0;
         *  need des
         */
        uint32_t ana_status1:8;
    };
    uint32_t val;
} i2c_ana_mst_ana_conf1_reg_t;

/** Type of ana_conf2 register
 *  need des
 */
typedef union {
    struct {
        /** ana_conf2 : R/W; bitpos: [23:0]; default: 0;
         *  need des
         */
        uint32_t ana_conf2:24;
        /** ana_status2 : RO; bitpos: [31:24]; default: 0;
         *  need des
         */
        uint32_t ana_status2:8;
    };
    uint32_t val;
} i2c_ana_mst_ana_conf2_reg_t;

/** Type of i2c0_ctrl1 register
 *  need des
 */
typedef union {
    struct {
        /** i2c0_scl_pulse_dur : R/W; bitpos: [5:0]; default: 2;
         *  need des
         */
        uint32_t i2c0_scl_pulse_dur:6;
        /** i2c0_sda_side_guard : R/W; bitpos: [10:6]; default: 1;
         *  need des
         */
        uint32_t i2c0_sda_side_guard:5;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} i2c_ana_mst_i2c0_ctrl1_reg_t;

/** Type of i2c1_ctrl1 register
 *  need des
 */
typedef union {
    struct {
        /** i2c1_scl_pulse_dur : R/W; bitpos: [5:0]; default: 2;
         *  need des
         */
        uint32_t i2c1_scl_pulse_dur:6;
        /** i2c1_sda_side_guard : R/W; bitpos: [10:6]; default: 1;
         *  need des
         */
        uint32_t i2c1_sda_side_guard:5;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} i2c_ana_mst_i2c1_ctrl1_reg_t;

/** Type of hw_i2c_ctrl register
 *  need des
 */
typedef union {
    struct {
        /** hw_i2c_scl_pulse_dur : R/W; bitpos: [5:0]; default: 2;
         *  need des
         */
        uint32_t hw_i2c_scl_pulse_dur:6;
        /** hw_i2c_sda_side_guard : R/W; bitpos: [10:6]; default: 1;
         *  need des
         */
        uint32_t hw_i2c_sda_side_guard:5;
        /** arbiter_dis : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t arbiter_dis:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} i2c_ana_mst_hw_i2c_ctrl_reg_t;

/** Type of nouse register
 *  need des
 */
typedef union {
    struct {
        /** i2c_mst_nouse : R/W; bitpos: [31:0]; default: 0;
         *  need des
         */
        uint32_t i2c_mst_nouse:32;
    };
    uint32_t val;
} i2c_ana_mst_nouse_reg_t;

/** Type of clk160m register
 *  need des
 */
typedef union {
    struct {
        /** clk_i2c_mst_sel_160m : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t clk_i2c_mst_sel_160m:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} i2c_ana_mst_clk160m_reg_t;

/** Type of date register
 *  need des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35656448;
         *  need des
         */
        uint32_t date:28;
        /** i2c_mst_clk_en : R/W; bitpos: [28]; default: 0;
         *  need des
         */
        uint32_t i2c_mst_clk_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} i2c_ana_mst_date_reg_t;


typedef struct {
    volatile i2c_ana_mst_i2c0_ctrl_reg_t i2c0_ctrl;
    volatile i2c_ana_mst_i2c1_ctrl_reg_t i2c1_ctrl;
    volatile i2c_ana_mst_i2c0_conf_reg_t i2c0_conf;
    volatile i2c_ana_mst_i2c1_conf_reg_t i2c1_conf;
    volatile i2c_ana_mst_i2c_burst_conf_reg_t i2c_burst_conf;
    volatile i2c_ana_mst_i2c_burst_status_reg_t i2c_burst_status;
    volatile i2c_ana_mst_ana_conf0_reg_t ana_conf0;
    volatile i2c_ana_mst_ana_conf1_reg_t ana_conf1;
    volatile i2c_ana_mst_ana_conf2_reg_t ana_conf2;
    volatile i2c_ana_mst_i2c0_ctrl1_reg_t i2c0_ctrl1;
    volatile i2c_ana_mst_i2c1_ctrl1_reg_t i2c1_ctrl1;
    volatile i2c_ana_mst_hw_i2c_ctrl_reg_t hw_i2c_ctrl;
    volatile i2c_ana_mst_nouse_reg_t nouse;
    volatile i2c_ana_mst_clk160m_reg_t clk160m;
    volatile i2c_ana_mst_date_reg_t date;
} i2c_ana_mst_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(i2c_ana_mst_dev_t) == 0x3c, "Invalid size of i2c_ana_mst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
