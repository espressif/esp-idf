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

/** Group: configure_register */
typedef union {
    struct {
        uint32_t clk_en: 1;
        uint32_t modem_ant_force_sel_bt: 1;
        uint32_t modem_ant_force_sel_wifi: 1;
        uint32_t fpga_debug_clkswitch: 1;
        uint32_t fpga_debug_clk80: 1;
        uint32_t fpga_debug_clk40: 1;
        uint32_t fpga_debug_clk20: 1;
        uint32_t fpga_debug_clk10: 1;
        uint32_t modem_mem_mode_force: 1;
        uint32_t modem_dis_wifi6_force: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} modem_syscon_test_conf_reg_t;

typedef union {
    struct {
        uint32_t pwdet_sar_clock_ena: 1;
        uint32_t pwdet_clk_div_num: 8;
        uint32_t clk_tx_dac_inv_ena: 1;
        uint32_t clk_rx_adc_inv_ena: 1;
        uint32_t clk_pwdet_adc_inv_ena: 1;
        uint32_t clk_i2c_mst_sel_160m: 1;
        uint32_t reserved_13: 8;
        uint32_t clk_data_dump_mux: 1;
        uint32_t clk_etm_en: 1;
        uint32_t clk_zb_apb_en: 1;
        uint32_t clk_zbmac_en: 1;
        uint32_t clk_modem_sec_ecb_en: 1;
        uint32_t clk_modem_sec_ccm_en: 1;
        uint32_t clk_modem_sec_bah_en: 1;
        uint32_t clk_modem_sec_apb_en: 1;
        uint32_t clk_modem_sec_en: 1;
        uint32_t clk_ble_timer_en: 1;
        uint32_t clk_data_dump_en: 1;
    };
    uint32_t val;
} modem_syscon_clk_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_wifibb_fo: 1;
        uint32_t clk_wifimac_fo: 1;
        uint32_t clk_wifi_apb_fo: 1;
        uint32_t clk_fe_fo: 1;
        uint32_t clk_fe_apb_fo: 1;
        uint32_t clk_btbb_fo: 1;
        uint32_t clk_btmac_fo: 1;
        uint32_t clk_bt_apb_fo: 1;
        uint32_t clk_zbmac_fo: 1;
        uint32_t clk_zbmac_apb_fo: 1;
        uint32_t reserved_10: 18;
        uint32_t clk_etm_fo: 1;
        uint32_t clk_modem_sec_fo: 1;
        uint32_t clk_ble_timer_fo: 1;
        uint32_t clk_data_dump_fo: 1;
    };
    uint32_t val;
} modem_syscon_clk_conf_force_on_reg_t;

typedef union {
    struct {
        uint32_t reserved_0: 8;
        uint32_t clk_zb_st_map: 4;
        uint32_t clk_fe_st_map: 4;
        uint32_t clk_bt_st_map: 4;
        uint32_t clk_wifi_st_map: 4;
        uint32_t clk_modem_peri_st_map: 4;
        uint32_t clk_modem_apb_st_map: 4;
    };
    uint32_t val;
} modem_syscon_clk_conf_power_st_reg_t;

typedef union {
    struct {
        uint32_t reserved_0: 8;
        uint32_t rst_wifibb: 1;
        uint32_t rst_wifimac: 1;
        uint32_t rst_fe_pwdet_adc: 1;
        uint32_t rst_fe_dac: 1;
        uint32_t rst_fe_adc: 1;
        uint32_t rst_fe_ahb: 1;
        uint32_t rst_fe: 1;
        uint32_t rst_btmac_apb: 1;
        uint32_t rst_btmac: 1;
        uint32_t rst_btbb_apb: 1;
        uint32_t rst_btbb: 1;
        uint32_t reserved_19: 3;
        uint32_t rst_etm: 1;
        uint32_t rst_zbmac_apb: 1;
        uint32_t rst_zbmac: 1;
        uint32_t rst_modem_ecb: 1;
        uint32_t rst_modem_ccm: 1;
        uint32_t rst_modem_bah: 1;
        uint32_t reserved_28: 1;
        uint32_t rst_modem_sec: 1;
        uint32_t rst_ble_timer: 1;
        uint32_t rst_data_dump: 1;
    };
    uint32_t val;
} modem_syscon_modem_rst_conf_reg_t;

typedef union {
    struct {
        uint32_t clk_wifibb_22m_en: 1;
        uint32_t clk_wifibb_40m_en: 1;
        uint32_t clk_wifibb_44m_en: 1;
        uint32_t clk_wifibb_80m_en: 1;
        uint32_t clk_wifibb_40x_en: 1;
        uint32_t clk_wifibb_80x_en: 1;
        uint32_t clk_wifibb_40x1_en: 1;
        uint32_t clk_wifibb_80x1_en: 1;
        uint32_t clk_wifibb_160x1_en: 1;
        uint32_t clk_wifimac_en: 1;
        uint32_t clk_wifi_apb_en: 1;
        uint32_t clk_fe_20m_en: 1;
        uint32_t clk_fe_40m_en: 1;
        uint32_t clk_fe_80m_en: 1;
        uint32_t clk_fe_160m_en: 1;
        uint32_t clk_fe_apb_en: 1;
        uint32_t clk_bt_apb_en: 1;
        uint32_t clk_btbb_en: 1;
        uint32_t clk_btmac_en: 1;
        uint32_t clk_fe_pwdet_adc_en: 1;
        uint32_t clk_fe_adc_en: 1;
        uint32_t clk_fe_dac_en: 1;
        uint32_t reserved_22: 10;
    };
    uint32_t val;
} modem_syscon_clk_conf1_reg_t;

typedef union {
    struct {
        uint32_t wifi_bb_cfg: 32;
    };
    uint32_t val;
} modem_syscon_wifi_bb_cfg_reg_t;

typedef union {
    struct {
        uint32_t fe_cfg: 32;
    };
    uint32_t val;
} modem_syscon_fe_cfg_reg_t;

typedef union {
    struct {
        uint32_t modem_rf1_mem_aux_ctrl: 32;
    };
    uint32_t val;
} modem_syscon_mem_rf1_conf_reg_t;

typedef union {
    struct {
        uint32_t modem_rf2_mem_aux_ctrl: 32;
    };
    uint32_t val;
} modem_syscon_mem_rf2_conf_reg_t;

typedef union {
    struct {
        uint32_t date: 28;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} modem_syscon_date_reg_t;

typedef struct {
    volatile modem_syscon_test_conf_reg_t test_conf;
    volatile modem_syscon_clk_conf_reg_t clk_conf;
    volatile modem_syscon_clk_conf_force_on_reg_t clk_conf_force_on;
    volatile modem_syscon_clk_conf_power_st_reg_t clk_conf_power_st;
    volatile modem_syscon_modem_rst_conf_reg_t modem_rst_conf;
    volatile modem_syscon_clk_conf1_reg_t clk_conf1;
    volatile modem_syscon_wifi_bb_cfg_reg_t wifi_bb_cfg;
    volatile modem_syscon_fe_cfg_reg_t fe_cfg;
    volatile modem_syscon_mem_rf1_conf_reg_t mem_rf1_conf;
    volatile modem_syscon_mem_rf2_conf_reg_t mem_rf2_conf;
    volatile modem_syscon_date_reg_t date;
} modem_syscon_dev_t;

extern modem_syscon_dev_t MODEM_SYSCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_syscon_dev_t) == 0x2c, "Invalid size of modem_syscon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
