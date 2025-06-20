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
/* Type of test_conf register*/
typedef union {
    struct {
        /* clk_en : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_en: 1;
        /* modem_ant_force_sel_bt : R/W; bitpos: [1]; default: 0;*/
        uint32_t modem_ant_force_sel_bt: 1;
        /* modem_ant_force_sel_wifi : R/W; bitpos: [2]; default: 0;*/
        uint32_t modem_ant_force_sel_wifi: 1;
        /* fpga_debug_clkswitch : R/W; bitpos: [3]; default: 0;*/
        uint32_t fpga_debug_clkswitch: 1;
        /* fpga_debug_clk80 : R/W; bitpos: [4]; default: 0;*/
        uint32_t fpga_debug_clk80: 1;
        /* fpga_debug_clk40 : R/W; bitpos: [5]; default: 0;*/
        uint32_t fpga_debug_clk40: 1;
        /* fpga_debug_clk20 : R/W; bitpos: [6]; default: 0;*/
        uint32_t fpga_debug_clk20: 1;
        /* fpga_debug_clk10 : R/W; bitpos: [7]; default: 0;*/
        uint32_t fpga_debug_clk10: 1;
        /* modem_mem_mode_force : R/W; bitpos: [8]; default: 1;*/
        uint32_t modem_mem_mode_force: 1;
        uint32_t reserved_9: 23;
    };
    uint32_t val;
} modem_syscon_test_conf_reg_t;

/* Type of clk_conf register*/
typedef union {
    struct {
        /* pwdet_sar_clock_ena : R/W; bitpos: [0]; default: 0;*/
        uint32_t pwdet_sar_clock_ena: 1;
        /* pwdet_clk_div_num : R/W; bitpos: [8:1]; default: 1;*/
        uint32_t pwdet_clk_div_num: 8;
        /* clk_tx_dac_inv_ena : R/W; bitpos: [9]; default: 0;*/
        uint32_t clk_tx_dac_inv_ena: 1;
        /* clk_rx_adc_inv_ena : R/W; bitpos: [10]; default: 0;*/
        uint32_t clk_rx_adc_inv_ena: 1;
        /* clk_pwdet_adc_inv_ena : R/W; bitpos: [11]; default: 0;*/
        uint32_t clk_pwdet_adc_inv_ena: 1;
        /* clk_i2c_mst_sel_160m : R/W; bitpos: [12]; default: 0;*/
        uint32_t clk_i2c_mst_sel_160m: 1;
        uint32_t reserved_13: 8;
        /* clk_data_dump_mux : R/W; bitpos: [21]; default: 0;*/
        uint32_t clk_data_dump_mux: 1;
        /* clk_etm_en : R/W; bitpos: [22]; default: 0;*/
        uint32_t clk_etm_en: 1;
        /* clk_zb_apb_en : R/W; bitpos: [23]; default: 0;*/
        uint32_t clk_zb_apb_en: 1;
        /* clk_zbmac_en : R/W; bitpos: [24]; default: 0;*/
        uint32_t clk_zbmac_en: 1;
        /* clk_modem_sec_ecb_en : R/W; bitpos: [25]; default: 0;*/
        uint32_t clk_modem_sec_ecb_en: 1;
        /* clk_modem_sec_ccm_en : R/W; bitpos: [26]; default: 0;*/
        uint32_t clk_modem_sec_ccm_en: 1;
        /* clk_modem_sec_bah_en : R/W; bitpos: [27]; default: 0;*/
        uint32_t clk_modem_sec_bah_en: 1;
        /* clk_modem_sec_apb_en : R/W; bitpos: [28]; default: 0;*/
        uint32_t clk_modem_sec_apb_en: 1;
        /* clk_modem_sec_en : R/W; bitpos: [29]; default: 0;*/
        uint32_t clk_modem_sec_en: 1;
        /* clk_ble_timer_en : R/W; bitpos: [30]; default: 0;*/
        uint32_t clk_ble_timer_en: 1;
        /* clk_data_dump_en : R/W; bitpos: [31]; default: 0;*/
        uint32_t clk_data_dump_en: 1;
    };
    uint32_t val;
} modem_syscon_clk_conf_reg_t;

/* Type of clk_conf_force_on register*/
typedef union {
    struct {
        /* clk_aon_fo : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_aon_fo: 1;
        /* clk_pll_fo : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_pll_fo: 1;
        /* clk_modem_sec_fo : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_modem_sec_fo: 1;
        /* clk_modem_sec_ecb_fo : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_modem_sec_ecb_fo: 1;
        /* clk_modem_sec_ccm_fo : R/W; bitpos: [4]; default: 0;*/
        uint32_t clk_modem_sec_ccm_fo: 1;
        /* clk_modem_sec_bah_fo : R/W; bitpos: [5]; default: 0;*/
        uint32_t clk_modem_sec_bah_fo: 1;
        /* clk_modem_sec_apb_fo : R/W; bitpos: [6]; default: 0;*/
        uint32_t clk_modem_sec_apb_fo: 1;
        /* clk_modem_sec_ahb_fo : R/W; bitpos: [7]; default: 0;*/
        uint32_t clk_modem_sec_ahb_fo: 1;
        /* clk_fe_pwdet_adc_inf_fo : R/W; bitpos: [8]; default: 0;*/
        uint32_t clk_fe_pwdet_adc_inf_fo: 1;
        /* clk_fe_dac_inf_fo : R/W; bitpos: [9]; default: 0;*/
        uint32_t clk_fe_dac_inf_fo: 1;
        /* clk_fe_apb_fo : R/W; bitpos: [10]; default: 0;*/
        uint32_t clk_fe_apb_fo: 1;
        /* clk_fe_adc_inf_fo : R/W; bitpos: [11]; default: 0;*/
        uint32_t clk_fe_adc_inf_fo: 1;
        /* clk_fe_80m_fo : R/W; bitpos: [12]; default: 0;*/
        uint32_t clk_fe_80m_fo: 1;
        /* clk_fe_40m_fo : R/W; bitpos: [13]; default: 0;*/
        uint32_t clk_fe_40m_fo: 1;
        /* clk_fe_20m_fo : R/W; bitpos: [14]; default: 0;*/
        uint32_t clk_fe_20m_fo: 1;
        /* clk_fe_160m_fo : R/W; bitpos: [15]; default: 0;*/
        uint32_t clk_fe_160m_fo: 1;
        /* clk_etm_fo : R/W; bitpos: [16]; default: 0;*/
        uint32_t clk_etm_fo: 1;
        /* clk_etm_apb_fo : R/W; bitpos: [17]; default: 0;*/
        uint32_t clk_etm_apb_fo: 1;
        /* clk_data_dump_mem_fo : R/W; bitpos: [18]; default: 0;*/
        uint32_t clk_data_dump_mem_fo: 1;
        /* clk_data_dump_fo : R/W; bitpos: [19]; default: 0;*/
        uint32_t clk_data_dump_fo: 1;
        /* clk_btmac_fo : R/W; bitpos: [20]; default: 0;*/
        uint32_t clk_btmac_fo: 1;
        /* clk_btmac_ahb_fo : R/W; bitpos: [21]; default: 0;*/
        uint32_t clk_btmac_ahb_fo: 1;
        /* clk_bt_apb_fo : R/W; bitpos: [22]; default: 0;*/
        uint32_t clk_bt_apb_fo: 1;
        /* clk_bt_apb_32m_fo : R/W; bitpos: [23]; default: 0;*/
        uint32_t clk_bt_apb_32m_fo: 1;
        /* clk_bt_8m_fo : R/W; bitpos: [24]; default: 0;*/
        uint32_t clk_bt_8m_fo: 1;
        /* clk_bt_80m_fo : R/W; bitpos: [25]; default: 0;*/
        uint32_t clk_bt_80m_fo: 1;
        /* clk_bt_40m_fo : R/W; bitpos: [26]; default: 0;*/
        uint32_t clk_bt_40m_fo: 1;
        /* clk_bt_32m_fo : R/W; bitpos: [27]; default: 0;*/
        uint32_t clk_bt_32m_fo: 1;
        /* clk_bt_16m_fo : R/W; bitpos: [28]; default: 0;*/
        uint32_t clk_bt_16m_fo: 1;
        /* clk_bt_160m_fo : R/W; bitpos: [29]; default: 0;*/
        uint32_t clk_bt_160m_fo: 1;
        /* clk_ble_timer_fo : R/W; bitpos: [30]; default: 0;*/
        uint32_t clk_ble_timer_fo: 1;
        /* clk_ble_timer_apb_fo : R/W; bitpos: [31]; default: 0;*/
        uint32_t clk_ble_timer_apb_fo: 1;
    };
    uint32_t val;
} modem_syscon_clk_conf_force_on_reg_t;

/* Type of clk_conf_power_st register*/
typedef union {
    struct {
        uint32_t reserved_0: 8;
        /* clk_zb_st_map : R/W; bitpos: [11:8]; default: 0;*/
        uint32_t clk_zb_st_map: 4;
        /* clk_fe_st_map : R/W; bitpos: [15:12]; default: 0;*/
        uint32_t clk_fe_st_map: 4;
        /* clk_bt_st_map : R/W; bitpos: [19:16]; default: 0;*/
        uint32_t clk_bt_st_map: 4;
        /* clk_wifi_st_map : R/W; bitpos: [23:20]; default: 0;*/
        uint32_t clk_wifi_st_map: 4;
        /* clk_modem_peri_st_map : R/W; bitpos: [27:24]; default: 0;*/
        uint32_t clk_modem_peri_st_map: 4;
        /* clk_modem_apb_st_map : R/W; bitpos: [31:28]; default: 0;*/
        uint32_t clk_modem_apb_st_map: 4;
    };
    uint32_t val;
} modem_syscon_clk_conf_power_st_reg_t;

/* Type of modem_rst_conf register*/
typedef union {
    struct {
        uint32_t reserved_0: 6;
        /* rst_fe_txlogain : R/W; bitpos: [6]; default: 0;*/
        uint32_t rst_fe_txlogain: 1;
        /* rst_fe_sdm : R/W; bitpos: [7]; default: 0;*/
        uint32_t rst_fe_sdm: 1;
        /* rst_wifibb : R/W; bitpos: [8]; default: 0;*/
        uint32_t rst_wifibb: 1;
        /* rst_wifimac : R/W; bitpos: [9]; default: 0;*/
        uint32_t rst_wifimac: 1;
        /* rst_fe_pwdet_adc : R/W; bitpos: [10]; default: 0;*/
        uint32_t rst_fe_pwdet_adc: 1;
        /* rst_fe_dac : R/W; bitpos: [11]; default: 0;*/
        uint32_t rst_fe_dac: 1;
        /* rst_fe_adc : R/W; bitpos: [12]; default: 0;*/
        uint32_t rst_fe_adc: 1;
        /* rst_fe_ahb : R/W; bitpos: [13]; default: 0;*/
        uint32_t rst_fe_ahb: 1;
        /* rst_fe : R/W; bitpos: [14]; default: 0;*/
        uint32_t rst_fe: 1;
        /* rst_btmac_apb : R/W; bitpos: [15]; default: 0;*/
        uint32_t rst_btmac_apb: 1;
        /* rst_btmac : R/W; bitpos: [16]; default: 0;*/
        uint32_t rst_btmac: 1;
        /* rst_btbb_apb : R/W; bitpos: [17]; default: 0;*/
        uint32_t rst_btbb_apb: 1;
        /* rst_btbb : R/W; bitpos: [18]; default: 0;*/
        uint32_t rst_btbb: 1;
        /* rst_coex_btmac_apb : R/W; bitpos: [19]; default: 0;*/
        uint32_t rst_coex_btmac_apb: 1;
        /* rst_coex_btmac : R/W; bitpos: [20]; default: 0;*/
        uint32_t rst_coex_btmac: 1;
        uint32_t reserved_21: 1;
        /* rst_etm : R/W; bitpos: [22]; default: 0;*/
        uint32_t rst_etm: 1;
        /* rst_zbmac_apb : R/W; bitpos: [23]; default: 0;*/
        uint32_t rst_zbmac_apb: 1;
        /* rst_zbmac : R/W; bitpos: [24]; default: 0;*/
        uint32_t rst_zbmac: 1;
        /* rst_modem_ecb : R/W; bitpos: [25]; default: 0;*/
        uint32_t rst_modem_ecb: 1;
        /* rst_modem_ccm : R/W; bitpos: [26]; default: 0;*/
        uint32_t rst_modem_ccm: 1;
        /* rst_modem_bah : R/W; bitpos: [27]; default: 0;*/
        uint32_t rst_modem_bah: 1;
        uint32_t reserved_28: 1;
        /* rst_modem_sec : R/W; bitpos: [29]; default: 0;*/
        uint32_t rst_modem_sec: 1;
        /* rst_ble_timer : R/W; bitpos: [30]; default: 0;*/
        uint32_t rst_ble_timer: 1;
        /* rst_data_dump : R/W; bitpos: [31]; default: 0;*/
        uint32_t rst_data_dump: 1;
    };
    uint32_t val;
} modem_syscon_modem_rst_conf_reg_t;

/* Type of clk_conf1 register*/
typedef union {
    struct {
        /* clk_wifibb_22m_en : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_wifibb_22m_en: 1;
        /* clk_wifibb_40m_en : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_wifibb_40m_en: 1;
        /* clk_wifibb_44m_en : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_wifibb_44m_en: 1;
        /* clk_wifibb_80m_en : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_wifibb_80m_en: 1;
        /* clk_wifibb_40x_en : R/W; bitpos: [4]; default: 0;*/
        uint32_t clk_wifibb_40x_en: 1;
        /* clk_wifibb_80x_en : R/W; bitpos: [5]; default: 0;*/
        uint32_t clk_wifibb_80x_en: 1;
        /* clk_wifibb_40x1_en : R/W; bitpos: [6]; default: 0;*/
        uint32_t clk_wifibb_40x1_en: 1;
        /* clk_wifibb_80x1_en : R/W; bitpos: [7]; default: 0;*/
        uint32_t clk_wifibb_80x1_en: 1;
        /* clk_wifibb_160x1_en : R/W; bitpos: [8]; default: 0;*/
        uint32_t clk_wifibb_160x1_en: 1;
        /* clk_wifimac_en : R/W; bitpos: [9]; default: 0;*/
        uint32_t clk_wifimac_en: 1;
        /* clk_wifi_apb_en : R/W; bitpos: [10]; default: 0;*/
        uint32_t clk_wifi_apb_en: 1;
        /* clk_fe_20m_en : R/W; bitpos: [11]; default: 0;*/
        uint32_t clk_fe_20m_en: 1;
        /* clk_fe_40m_en : R/W; bitpos: [12]; default: 0;*/
        uint32_t clk_fe_40m_en: 1;
        /* clk_fe_80m_en : R/W; bitpos: [13]; default: 0;*/
        uint32_t clk_fe_80m_en: 1;
        /* clk_fe_160m_en : R/W; bitpos: [14]; default: 0;*/
        uint32_t clk_fe_160m_en: 1;
        /* clk_fe_apb_en : R/W; bitpos: [15]; default: 0;*/
        uint32_t clk_fe_apb_en: 1;
        /* clk_bt_apb_en : R/W; bitpos: [16]; default: 0;*/
        uint32_t clk_bt_apb_en: 1;
        /* clk_btbb_en : R/W; bitpos: [17]; default: 0;*/
        uint32_t clk_btbb_en: 1;
        /* clk_btmac_en : R/W; bitpos: [18]; default: 0;*/
        uint32_t clk_btmac_en: 1;
        /* clk_fe_pwdet_adc_en : R/W; bitpos: [19]; default: 0;*/
        uint32_t clk_fe_pwdet_adc_en: 1;
        /* clk_fe_adc_en : R/W; bitpos: [20]; default: 0;*/
        uint32_t clk_fe_adc_en: 1;
        /* clk_fe_dac_en : R/W; bitpos: [21]; default: 0;*/
        uint32_t clk_fe_dac_en: 1;
        /* clk_fe_sdm_en : R/W; bitpos: [22]; default: 0;*/
        uint32_t clk_fe_sdm_en: 1;
        /* clk_fe_txlogain_en : R/W; bitpos: [23]; default: 0;*/
        uint32_t clk_fe_txlogain_en: 1;
        /* clk_fe_32m_en : R/W; bitpos: [24]; default: 0;*/
        uint32_t clk_fe_32m_en: 1;
        /* clk_fe_16m_en : R/W; bitpos: [25]; default: 0;*/
        uint32_t clk_fe_16m_en: 1;
        uint32_t reserved_26: 6;
    };
    uint32_t val;
} modem_syscon_clk_conf1_reg_t;

/* Type of wifi_bb_cfg register*/
typedef union {
    struct {
        /* wifi_bb_cfg : R/W; bitpos: [31:0]; default: 0;*/
        uint32_t wifi_bb_cfg: 32;
    };
    uint32_t val;
} modem_syscon_wifi_bb_cfg_reg_t;

/* Type of fe_cfg register*/
typedef union {
    struct {
        /* fe_cfg : R/W; bitpos: [31:0]; default: 0;*/
        uint32_t fe_cfg: 32;
    };
    uint32_t val;
} modem_syscon_fe_cfg_reg_t;

/* Type of mem_rf1_conf register*/
typedef union {
    struct {
        /* modem_rf1_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 10320;*/
        uint32_t modem_rf1_mem_aux_ctrl: 32;
    };
    uint32_t val;
} modem_syscon_mem_rf1_conf_reg_t;

/* Type of mem_rf2_conf register*/
typedef union {
    struct {
        /* modem_rf2_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 0;*/
        uint32_t modem_rf2_mem_aux_ctrl: 32;
    };
    uint32_t val;
} modem_syscon_mem_rf2_conf_reg_t;

/* Type of btmac_clk_cfg register*/
typedef union {
    struct {
        /* clk_btmac_low_rate : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_btmac_low_rate: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} modem_syscon_btmac_clk_cfg_reg_t;

/* Type of clk_conf_force_on_2 register*/
typedef union {
    struct {
        /* clk_wifi_apb_fo : R/W; bitpos: [0]; default: 0;*/
        uint32_t clk_wifi_apb_fo: 1;
        /* clk_wifibb_160m_x1_fo : R/W; bitpos: [1]; default: 0;*/
        uint32_t clk_wifibb_160m_x1_fo: 1;
        /* clk_wifibb_22m_fo : R/W; bitpos: [2]; default: 0;*/
        uint32_t clk_wifibb_22m_fo: 1;
        /* clk_wifibb_40m_fo : R/W; bitpos: [3]; default: 0;*/
        uint32_t clk_wifibb_40m_fo: 1;
        /* clk_wifibb_40m_x1_fo : R/W; bitpos: [4]; default: 0;*/
        uint32_t clk_wifibb_40m_x1_fo: 1;
        /* clk_wifibb_40m_x_int_fo : R/W; bitpos: [5]; default: 0;*/
        uint32_t clk_wifibb_40m_x_int_fo: 1;
        /* clk_wifibb_44m_fo : R/W; bitpos: [6]; default: 0;*/
        uint32_t clk_wifibb_44m_fo: 1;
        /* clk_wifibb_80m_fo : R/W; bitpos: [7]; default: 0;*/
        uint32_t clk_wifibb_80m_fo: 1;
        /* clk_wifibb_80m_x1_fo : R/W; bitpos: [8]; default: 0;*/
        uint32_t clk_wifibb_80m_x1_fo: 1;
        /* clk_wifibb_80m_x_int_fo : R/W; bitpos: [9]; default: 0;*/
        uint32_t clk_wifibb_80m_x_int_fo: 1;
        /* clk_wifibb_apb_44m_fo : R/W; bitpos: [10]; default: 0;*/
        uint32_t clk_wifibb_apb_44m_fo: 1;
        /* clk_wifimac_ahb_fo : R/W; bitpos: [11]; default: 0;*/
        uint32_t clk_wifimac_ahb_fo: 1;
        /* clk_wifimac_fo : R/W; bitpos: [12]; default: 0;*/
        uint32_t clk_wifimac_fo: 1;
        /* clk_zbmac_ahb_fo : R/W; bitpos: [13]; default: 0;*/
        uint32_t clk_zbmac_ahb_fo: 1;
        /* clk_zbmac_apb_16m_fo : R/W; bitpos: [14]; default: 0;*/
        uint32_t clk_zbmac_apb_16m_fo: 1;
        /* clk_zbmac_apb_fo : R/W; bitpos: [15]; default: 0;*/
        uint32_t clk_zbmac_apb_fo: 1;
        /* clk_zbmac_fo : R/W; bitpos: [16]; default: 0;*/
        uint32_t clk_zbmac_fo: 1;
        /* clk_wifibb_80m_x_fo : R/W; bitpos: [17]; default: 0;*/
        uint32_t clk_wifibb_80m_x_fo: 1;
        /* clk_wifibb_40m_x_fo : R/W; bitpos: [18]; default: 0;*/
        uint32_t clk_wifibb_40m_x_fo: 1;
        /* clk_fe_adc_fo : R/W; bitpos: [19]; default: 0;*/
        uint32_t clk_fe_adc_fo: 1;
        /* clk_fe_txlogain_inf_fo : R/W; bitpos: [20]; default: 0;*/
        uint32_t clk_fe_txlogain_inf_fo: 1;
        /* clk_fe_sdm_inf_fo : R/W; bitpos: [21]; default: 0;*/
        uint32_t clk_fe_sdm_inf_fo: 1;
        /* clk_fe_sdm_fo : R/W; bitpos: [22]; default: 0;*/
        uint32_t clk_fe_sdm_fo: 1;
        /* clk_fe_sdm_div4_fo : R/W; bitpos: [23]; default: 0;*/
        uint32_t clk_fe_sdm_div4_fo: 1;
        /* clk_fe_sdm_div2_fo : R/W; bitpos: [24]; default: 0;*/
        uint32_t clk_fe_sdm_div2_fo: 1;
        /* clk_fe_16m_fo : R/W; bitpos: [25]; default: 0;*/
        uint32_t clk_fe_16m_fo: 1;
        /* clk_fe_32m_fo : R/W; bitpos: [26]; default: 0;*/
        uint32_t clk_fe_32m_fo: 1;
        uint32_t reserved_27: 5;
    };
    uint32_t val;
} modem_syscon_clk_conf_force_on_2_reg_t;

/* Type of apb_timeout_conf register*/
typedef union {
    struct {
        /* apb_timeout_thres : R/W; bitpos: [15:0]; default: 65535;*/
        uint32_t apb_timeout_thres: 16;
        /* apb_timeout_protect_en : R/W; bitpos: [16]; default: 1;*/
        uint32_t apb_timeout_protect_en: 1;
        /* apb_timeout_int_clr : WO; bitpos: [17]; default: 0;*/
        uint32_t apb_timeout_int_clr: 1;
        /* modem_apb_timeout_exception_pid : RO; bitpos: [21:18]; default: 0;*/
        uint32_t modem_apb_timeout_exception_pid: 4;
        /* modem_apb_timeout_int : RO; bitpos: [22]; default: 0;*/
        uint32_t modem_apb_timeout_int: 1;
        uint32_t reserved_23: 9;
    };
    uint32_t val;
} modem_syscon_apb_timeout_conf_reg_t;

/* Type of apb_timeout_exception_addr register*/
typedef union {
    struct {
        /* modem_apb_timeout_exception_paddr : RO; bitpos: [31:0]; default: 0;*/
        uint32_t modem_apb_timeout_exception_paddr: 32;
    };
    uint32_t val;
} modem_syscon_apb_timeout_exception_addr_reg_t;

/* Type of date register*/
typedef union {
    struct {
        /* date : R/W; bitpos: [27:0]; default: 37823024;*/
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
    volatile modem_syscon_btmac_clk_cfg_reg_t btmac_clk_cfg;
    volatile modem_syscon_clk_conf_force_on_2_reg_t clk_conf_force_on_2;
    volatile modem_syscon_apb_timeout_conf_reg_t apb_timeout_conf;
    volatile modem_syscon_apb_timeout_exception_addr_reg_t apb_timeout_exception_addr;
    volatile modem_syscon_date_reg_t date;
} modem_syscon_dev_t;

extern modem_syscon_dev_t MODEM_SYSCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_syscon_dev_t) == 0x3c, "Invalid size of modem_syscon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
