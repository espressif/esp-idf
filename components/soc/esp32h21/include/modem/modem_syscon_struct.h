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

/** Group: Configuration Register */
/** Type of test_conf register
 *  No description
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  No description
         */
        uint32_t clk_en: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} modem_syscon_test_conf_reg_t;

/** Type of clk_conf register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 21;
        /** clk_etm_en : R/W; bitpos: [21]; default: 0;
         *  No description
         */
        uint32_t clk_etm_en: 1;
        /** clk_zb_apb_en : R/W; bitpos: [22]; default: 0;
         *  No description
         */
        uint32_t clk_zb_apb_en: 1;
        /** clk_zb_mac_en : R/W; bitpos: [23]; default: 0;
         *  No description
         */
        uint32_t clk_zb_mac_en: 1;
        /** clk_modem_sec_ecb_en : R/W; bitpos: [24]; default: 0;
         *  No description
         */
        uint32_t clk_modem_sec_ecb_en: 1;
        /** clk_modem_sec_ccm_en : R/W; bitpos: [25]; default: 0;
         *  No description
         */
        uint32_t clk_modem_sec_ccm_en: 1;
        /** clk_modem_sec_bah_en : R/W; bitpos: [26]; default: 0;
         *  No description
         */
        uint32_t clk_modem_sec_bah_en: 1;
        /** clk_modem_sec_apb_en : R/W; bitpos: [27]; default: 0;
         *  No description
         */
        uint32_t clk_modem_sec_apb_en: 1;
        /** clk_modem_sec_en : R/W; bitpos: [28]; default: 0;
         *  No description
         */
        uint32_t clk_modem_sec_en: 1;
        /** clk_ble_timer_apb_en : R/W; bitpos: [29]; default: 0;
         *  No description
         */
        uint32_t clk_ble_timer_apb_en: 1;
        /** clk_ble_timer_en : R/W; bitpos: [30]; default: 0;
         *  No description
         */
        uint32_t clk_ble_timer_en: 1;
        /** clk_data_dump_en : R/W; bitpos: [31]; default: 0;
         *  No description
         */
        uint32_t clk_data_dump_en: 1;
    };
    uint32_t val;
} modem_syscon_clk_conf_reg_t;

/** Type of clk_conf_force_on register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 22;
        /** clk_etm_fo : R/W; bitpos: [22]; default: 0;
         *  No description
         */
        uint32_t clk_etm_fo: 1;
        uint32_t reserved_23: 1;
        /** clk_zb_fo : R/W; bitpos: [24]; default: 0;
         *  No description
         */
        uint32_t clk_zb_fo: 1;
        uint32_t reserved_25: 4;
        /** clk_modem_sec_fo : R/W; bitpos: [29]; default: 0;
         *  No description
         */
        uint32_t clk_modem_sec_fo: 1;
        /** clk_ble_timer_fo : R/W; bitpos: [30]; default: 0;
         *  No description
         */
        uint32_t clk_ble_timer_fo: 1;
        /** clk_data_dump_fo : R/W; bitpos: [31]; default: 0;
         *  No description
         */
        uint32_t clk_data_dump_fo: 1;
    };
    uint32_t val;
} modem_syscon_clk_conf_force_on_reg_t;

/** Type of modem_rst_conf register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 14;
        /** rst_fe : R/W; bitpos: [14]; default: 0;
         *  No description
         */
        uint32_t rst_fe: 1;
        /** rst_btmac_apb : R/W; bitpos: [15]; default: 0;
         *  No description
         */
        uint32_t rst_btmac_apb: 1;
        /** rst_btmac : R/W; bitpos: [16]; default: 0;
         *  No description
         */
        uint32_t rst_btmac: 1;
        /** rst_btbb_apb : R/W; bitpos: [17]; default: 0;
         *  No description
         */
        uint32_t rst_btbb_apb: 1;
        /** rst_btbb : R/W; bitpos: [18]; default: 0;
         *  No description
         */
        uint32_t rst_btbb: 1;
        uint32_t reserved_19: 3;
        /** rst_etm : R/W; bitpos: [22]; default: 0;
         *  No description
         */
        uint32_t rst_etm: 1;
        /** rst_zbmac_apb : R/W; bitpos: [23]; default: 0;
         *  No description
         */
        uint32_t rst_zbmac_apb: 1;
        /** rst_zbmac : R/W; bitpos: [24]; default: 0;
         *  No description
         */
        uint32_t rst_zbmac: 1;
        /** rst_modem_ecb : R/W; bitpos: [25]; default: 0;
         *  No description
         */
        uint32_t rst_modem_ecb: 1;
        /** rst_modem_ccm : R/W; bitpos: [26]; default: 0;
         *  No description
         */
        uint32_t rst_modem_ccm: 1;
        /** rst_modem_bah : R/W; bitpos: [27]; default: 0;
         *  No description
         */
        uint32_t rst_modem_bah: 1;
        uint32_t reserved_28: 1;
        /** rst_modem_sec : R/W; bitpos: [29]; default: 0;
         *  No description
         */
        uint32_t rst_modem_sec: 1;
        /** rst_ble_timer : R/W; bitpos: [30]; default: 0;
         *  No description
         */
        uint32_t rst_ble_timer: 1;
        /** rst_data_dump : R/W; bitpos: [31]; default: 0;
         *  No description
         */
        uint32_t rst_data_dump: 1;
    };
    uint32_t val;
} modem_syscon_modem_rst_conf_reg_t;

/** Type of clk_conf1 register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 11;
        /** clk_fe_txlogain_en : R/W; bitpos: [11]; default: 0;
         *  No description
         */
        uint32_t clk_fe_txlogain_en: 1;
        /** clk_fe_16m_en : R/W; bitpos: [12]; default: 0;
         *  No description
         */
        uint32_t clk_fe_16m_en: 1;
        /** clk_fe_32m_en : R/W; bitpos: [13]; default: 0;
         *  No description
         */
        uint32_t clk_fe_32m_en: 1;
        /** clk_fe_sdm_en : R/W; bitpos: [14]; default: 0;
         *  No description
         */
        uint32_t clk_fe_sdm_en: 1;
        /** clk_fe_adc_en : R/W; bitpos: [15]; default: 0;
         *  No description
         */
        uint32_t clk_fe_adc_en: 1;
        /** clk_fe_apb_en : R/W; bitpos: [16]; default: 0;
         *  No description
         */
        uint32_t clk_fe_apb_en: 1;
        /** clk_bt_apb_en : R/W; bitpos: [17]; default: 0;
         *  No description
         */
        uint32_t clk_bt_apb_en: 1;
        /** clk_bt_en : R/W; bitpos: [18]; default: 0;
         *  No description
         */
        uint32_t clk_bt_en: 1;
        uint32_t reserved_19: 13;
    };
    uint32_t val;
} modem_syscon_clk_conf1_reg_t;

/** Type of clk_conf1_force_on register
 *  No description
 */
typedef union {
    struct {
        uint32_t reserved_0: 16;
        /** clk_fe_fo : R/W; bitpos: [16]; default: 0;
         *  No description
         */
        uint32_t clk_fe_fo: 1;
        uint32_t reserved_17: 1;
        /** clk_bt_fo : R/W; bitpos: [18]; default: 0;
         *  No description
         */
        uint32_t clk_bt_fo: 1;
        uint32_t reserved_19: 13;
    };
    uint32_t val;
} modem_syscon_clk_conf1_force_on_reg_t;

/** Type of mem_conf register
 *  No description
 */
typedef union {
    struct {
        /** modem_mem_wp : R/W; bitpos: [2:0]; default: 0;
         *  No description
         */
        uint32_t modem_mem_wp: 3;
        /** modem_mem_wa : R/W; bitpos: [5:3]; default: 5;
         *  No description
         */
        uint32_t modem_mem_wa: 3;
        /** modem_mem_ra : R/W; bitpos: [7:6]; default: 0;
         *  No description
         */
        uint32_t modem_mem_ra: 2;
        /** modem_mem_rm : R/W; bitpos: [11:8]; default: 2;
         *  No description
         */
        uint32_t modem_mem_rm: 4;
        uint32_t reserved_12: 20;
    };
    uint32_t val;
} modem_syscon_mem_conf_reg_t;

/** Group: Version Register */
/** Type of date register
 *  No description
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37785904;
         *  No description
         */
        uint32_t date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} modem_syscon_date_reg_t;

typedef struct {
    volatile modem_syscon_test_conf_reg_t test_conf;
    volatile modem_syscon_clk_conf_reg_t clk_conf;
    volatile modem_syscon_clk_conf_force_on_reg_t clk_conf_force_on;
    volatile modem_syscon_modem_rst_conf_reg_t modem_rst_conf;
    volatile modem_syscon_clk_conf1_reg_t clk_conf1;
    volatile modem_syscon_clk_conf1_force_on_reg_t clk_conf1_force_on;
    volatile modem_syscon_mem_conf_reg_t mem_conf;
    volatile modem_syscon_date_reg_t date;
} modem_syscon_dev_t;

extern modem_syscon_dev_t MODEM_SYSCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_syscon_dev_t) == 0x20, "Invalid size of modem_syscon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
