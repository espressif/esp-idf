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

typedef union {
    struct {
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} modem_syscon_test_conf_reg_t;

typedef union {
    struct {
        uint32_t reserved_0:21;
        uint32_t clk_etm_en:1;
        uint32_t clk_zb_apb_en:1;
        uint32_t clk_zb_mac_en:1;
        uint32_t clk_modem_sec_ecb_en:1;
        uint32_t clk_modem_sec_ccm_en:1;
        uint32_t clk_modem_sec_bah_en:1;
        uint32_t clk_modem_sec_apb_en:1;
        uint32_t clk_modem_sec_en:1;
        uint32_t clk_ble_timer_apb_en:1;
        uint32_t clk_ble_timer_en:1;
        uint32_t clk_data_dump_en:1;
    };
    uint32_t val;
} modem_syscon_clk_conf_reg_t;

typedef union {
    struct {
        uint32_t reserved_0:22;
        uint32_t clk_etm_fo:1;
        uint32_t reserved_23:1;
        uint32_t clk_zb_fo:1;
        uint32_t reserved_25:4;
        uint32_t clk_modem_sec_fo:1;
        uint32_t clk_ble_timer_fo:1;
        uint32_t clk_data_dump_fo:1;
    };
    uint32_t val;
} modem_syscon_clk_conf_force_on_reg_t;

typedef union {
    struct {
        uint32_t reserved_0:14;
        uint32_t rst_fe:1;
        uint32_t rst_btmac_apb:1;
        uint32_t rst_btmac:1;
        uint32_t rst_btbb_apb:1;
        uint32_t rst_btbb:1;
        uint32_t reserved_19:3;
        uint32_t rst_etm:1;
        uint32_t reserved_23:1;
        uint32_t rst_zbmac:1;
        uint32_t rst_modem_ecb:1;
        uint32_t rst_modem_ccm:1;
        uint32_t rst_modem_bah:1;
        uint32_t reserved_28:1;
        uint32_t rst_modem_sec:1;
        uint32_t rst_ble_timer:1;
        uint32_t rst_data_dump:1;
    };
    uint32_t val;
} modem_syscon_modem_rst_conf_reg_t;

typedef union {
    struct {
        uint32_t reserved_0:12;
        uint32_t clk_fe_16m_en:1;
        uint32_t clk_fe_32m_en:1;
        uint32_t clk_fe_sdm_en:1;
        uint32_t clk_fe_adc_en:1;
        uint32_t clk_fe_apb_en:1;
        uint32_t clk_bt_apb_en:1;
        uint32_t clk_bt_en:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} modem_syscon_clk_conf1_reg_t;

typedef union {
    struct {
        uint32_t reserved_0:16;
        uint32_t clk_fe_fo:1;
        uint32_t reserved_17:1;
        uint32_t clk_bt_fo:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} modem_syscon_clk_conf1_force_on_reg_t;

typedef union {
    struct {
        uint32_t modem_mem_wp:3;
        uint32_t modem_mem_wa:3;
        uint32_t modem_mem_ra:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} modem_syscon_mem_conf_reg_t;

typedef union {
    struct {
        uint32_t date:28;
        uint32_t reserved_28:4;
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
