/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t clk_en                    :    1;
            uint32_t modem_ant_force_sel_bt    :    1;
            uint32_t modem_ant_force_sel_wifi  :    1;
            uint32_t fpga_debug_clkswitch      :    1;
            uint32_t fpga_debug_clk80          :    1;
            uint32_t fpga_debug_clk40          :    1;
            uint32_t fpga_debug_clk20          :    1;
            uint32_t fpga_debug_clk10          :    1;
            uint32_t modem_mem_mode_force      :    1;
            uint32_t reserved9                 :    23;
        };
        uint32_t val;
    } test_conf;
    union {
        struct {
            uint32_t pwdet_sar_clock_ena       :    1;
            uint32_t pwdet_clk_div_num         :    8;
            uint32_t clk_tx_dac_inv_ena        :    1;
            uint32_t clk_rx_adc_inv_ena        :    1;
            uint32_t clk_pwdet_adc_inv_ena     :    1;
            uint32_t clk_i2c_mst_sel_160m      :    1;
            uint32_t reserved13                :    8;
            uint32_t clk_data_dump_mux         :    1;
            uint32_t clk_etm_en                :    1;
            uint32_t clk_zb_apb_en             :    1;
            uint32_t clk_zbmac_en              :    1;
            uint32_t clk_modem_sec_ecb_en      :    1;
            uint32_t clk_modem_sec_ccm_en      :    1;
            uint32_t clk_modem_sec_bah_en      :    1;
            uint32_t clk_modem_sec_apb_en      :    1;
            uint32_t clk_modem_sec_en          :    1;
            uint32_t clk_ble_timer_en          :    1;
            uint32_t clk_data_dump_en          :    1;
        };
        uint32_t val;
    } clk_conf;
    union {
        struct {
            uint32_t clk_wifibb_fo             :    1;
            uint32_t clk_wifimac_fo            :    1;
            uint32_t clk_wifi_apb_fo           :    1;
            uint32_t clk_fe_fo                 :    1;
            uint32_t clk_fe_apb_fo             :    1;
            uint32_t clk_btbb_fo               :    1;
            uint32_t clk_btmac_fo              :    1;
            uint32_t clk_bt_apb_fo             :    1;
            uint32_t clk_zbmac_fo              :    1;
            uint32_t clk_zbmac_apb_fo          :    1;
            uint32_t reserved10                    :    13;
            uint32_t reserved23                    :    1;
            uint32_t reserved24                    :    1;
            uint32_t reserved25                    :    1;
            uint32_t reserved26                    :    1;
            uint32_t reserved27                    :    1;
            uint32_t clk_etm_fo                :    1;
            uint32_t clk_modem_sec_fo          :    1;
            uint32_t clk_ble_timer_fo          :    1;
            uint32_t clk_data_dump_fo          :    1;
        };
        uint32_t val;
    } clk_conf_force_on;
    union {
        struct {
            uint32_t reserved0                     :    8;
            uint32_t clk_zb_st_map             :    4;
            uint32_t clk_fe_st_map             :    4;
            uint32_t clk_bt_st_map             :    4;
            uint32_t clk_wifi_st_map           :    4;
            uint32_t clk_modem_peri_st_map     :    4;
            uint32_t clk_modem_apb_st_map      :    4;
        };
        uint32_t val;
    } clk_conf_power_st;
    union {
        struct {
            uint32_t reserved0                     :    1;
            uint32_t reserved1                     :    1;
            uint32_t reserved2                     :    1;
            uint32_t reserved3                     :    1;
            uint32_t reserved4                     :    1;
            uint32_t reserved5                     :    1;
            uint32_t reserved6                     :    1;
            uint32_t reserved7                     :    1;
            uint32_t rst_wifibb                :    1;
            uint32_t rst_wifimac               :    1;
            uint32_t rst_fe_pwdet_adc          :    1;
            uint32_t rst_fe_dac                :    1;
            uint32_t rst_fe_adc                :    1;
            uint32_t rst_fe_ahb                :    1;
            uint32_t rst_fe                    :    1;
            uint32_t rst_btmac_apb             :    1;
            uint32_t rst_btmac                 :    1;
            uint32_t rst_btbb_apb              :    1;
            uint32_t rst_btbb                  :    1;
            uint32_t reserved19                    :    3;
            uint32_t rst_etm                   :    1;
            uint32_t rst_zbmac_apb             :    1;
            uint32_t rst_zbmac                 :    1;
            uint32_t rst_modem_ecb             :    1;
            uint32_t rst_modem_ccm             :    1;
            uint32_t rst_modem_bah             :    1;
            uint32_t reserved28                    :    1;
            uint32_t rst_modem_sec             :    1;
            uint32_t rst_ble_timer             :    1;
            uint32_t rst_data_dump             :    1;
        };
        uint32_t val;
    } modem_rst_conf;
    union {
        struct {
            uint32_t clk_wifibb_22m_en         :    1;
            uint32_t clk_wifibb_40m_en         :    1;
            uint32_t clk_wifibb_44m_en         :    1;
            uint32_t clk_wifibb_80m_en         :    1;
            uint32_t clk_wifibb_40x_en         :    1;
            uint32_t clk_wifibb_80x_en         :    1;
            uint32_t clk_wifibb_40x1_en        :    1;
            uint32_t clk_wifibb_80x1_en        :    1;
            uint32_t clk_wifibb_160x1_en       :    1;
            uint32_t clk_wifimac_en            :    1;
            uint32_t clk_wifi_apb_en           :    1;
            uint32_t clk_fe_20m_en             :    1;
            uint32_t clk_fe_40m_en             :    1;
            uint32_t clk_fe_80m_en             :    1;
            uint32_t clk_fe_160m_en            :    1;
            uint32_t clk_fe_apb_en             :    1;
            uint32_t clk_bt_apb_en             :    1;
            uint32_t clk_btbb_en               :    1;
            uint32_t clk_btmac_en              :    1;
            uint32_t clk_fe_pwdet_adc_en       :    1;
            uint32_t clk_fe_adc_en             :    1;
            uint32_t clk_fe_dac_en             :    1;
            uint32_t reserved22                :    1;
            uint32_t reserved23                :    1;
            uint32_t reserved24                :    8;
        };
        uint32_t val;
    } clk_conf1;
    uint32_t wifi_bb_cfg;
    uint32_t mem_rf1_conf;
    uint32_t mem_rf2_conf;
    union {
        struct {
            uint32_t date                      :    28;
            uint32_t reserved28                    :    4;
        };
        uint32_t val;
    } date;
} modem_syscon_dev_t;

extern modem_syscon_dev_t MODEM_SYSCON;

#ifndef __cplusplus
_Static_assert(sizeof(modem_syscon_dev_t) == 0x28, "Invalid size of modem_syscon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
