/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of i2c0_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** i2c0_ctrl : R/W; bitpos: [24:0]; default: 0;
         *  need_des
         */
        uint32_t i2c0_ctrl:25;
        /** i2c0_busy : RO; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t i2c0_busy:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} lp_i2c_ana_mst_i2c0_ctrl_reg_t;

/** Type of i2c0_conf register
 *  need_des
 */
typedef union {
    struct {
        /** i2c0_conf : R/W; bitpos: [23:0]; default: 0;
         *  need_des
         */
        uint32_t i2c0_conf:24;
        /** i2c0_status : RO; bitpos: [31:24]; default: 7;
         *  reserved
         */
        uint32_t i2c0_status:8;
    };
    uint32_t val;
} lp_i2c_ana_mst_i2c0_conf_reg_t;

/** Type of i2c0_data register
 *  need_des
 */
typedef union {
    struct {
        /** i2c0_rdata : RO; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t i2c0_rdata:8;
        /** i2c0_clk_sel : R/W; bitpos: [10:8]; default: 1;
         *  need_des
         */
        uint32_t i2c0_clk_sel:3;
        /** i2c_mst_sel : R/W; bitpos: [11]; default: 1;
         *  need  des
         */
        uint32_t i2c_mst_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_i2c_ana_mst_i2c0_data_reg_t;

/** Type of ana_conf1 register
 *  need_des
 */
typedef union {
    struct {
        /** ana_conf1 : R/W; bitpos: [23:0]; default: 0;
         *  need_des
         */
        uint32_t ana_conf1:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} lp_i2c_ana_mst_ana_conf1_reg_t;

/** Type of nouse register
 *  need_des
 */
typedef union {
    struct {
        /** i2c_mst_nouse : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t i2c_mst_nouse:32;
    };
    uint32_t val;
} lp_i2c_ana_mst_nouse_reg_t;

/** Type of device_en register
 *  need_des
 */
typedef union {
    struct {
        /** i2c_device_en : R/W; bitpos: [11:0]; default: 0;
         *  need_des
         */
        uint32_t i2c_device_en:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_i2c_ana_mst_device_en_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** i2c_mat_date : R/W; bitpos: [27:0]; default: 33583873;
         *  need_des
         */
        uint32_t i2c_mat_date:28;
        /** i2c_mat_clk_en : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t i2c_mat_clk_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} lp_i2c_ana_mst_date_reg_t;


typedef struct lp_i2c_ana_mst_dev_t {
    volatile lp_i2c_ana_mst_i2c0_ctrl_reg_t i2c0_ctrl;
    volatile lp_i2c_ana_mst_i2c0_conf_reg_t i2c0_conf;
    volatile lp_i2c_ana_mst_i2c0_data_reg_t i2c0_data;
    volatile lp_i2c_ana_mst_ana_conf1_reg_t ana_conf1;
    volatile lp_i2c_ana_mst_nouse_reg_t nouse;
    volatile lp_i2c_ana_mst_device_en_reg_t device_en;
    uint32_t reserved_018[249];
    volatile lp_i2c_ana_mst_date_reg_t date;
} lp_i2c_ana_mst_dev_t;

extern lp_i2c_ana_mst_dev_t LP_I2C_ANA_MST;

#ifndef __cplusplus
_Static_assert(sizeof(lp_i2c_ana_mst_dev_t) == 0x400, "Invalid size of lp_i2c_ana_mst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
