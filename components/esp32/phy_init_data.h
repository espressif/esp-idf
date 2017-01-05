// Copyright 2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "esp_phy_init.h"
#include "sdkconfig.h"

// constrain a value between 'low' and 'high', inclusive
#define LIMIT(val, low, high) ((val < low) ? low : (val > high) ? high : val)

#define PHY_INIT_MAGIC "PHYINIT"

static const char phy_init_magic_pre[] = PHY_INIT_MAGIC;

/**
 * @brief Structure containing default recommended PHY initialization parameters.
 */
static const esp_phy_init_data_t phy_init_data= {
        .param_ver_id = 1,
        .crystal_select = 3,
        .wifi_rx_gain_swp_step_1 = 0x05,
        .wifi_rx_gain_swp_step_2 = 0x04,
        .wifi_rx_gain_swp_step_3 = 0x06,
        .wifi_rx_gain_swp_step_4 = 0x05,
        .wifi_rx_gain_swp_step_5 = 0x01,
        .wifi_rx_gain_swp_step_6 = 0x06,
        .wifi_rx_gain_swp_step_7 = 0x05,
        .wifi_rx_gain_swp_step_8 = 0x04,
        .wifi_rx_gain_swp_step_9 = 0x06,
        .wifi_rx_gain_swp_step_10 = 0x04,
        .wifi_rx_gain_swp_step_11 = 0x05,
        .wifi_rx_gain_swp_step_12 = 0x00,
        .wifi_rx_gain_swp_step_13 = 0x00,
        .wifi_rx_gain_swp_step_14 = 0x00,
        .wifi_rx_gain_swp_step_15 = 0x00,
        .bt_rx_gain_swp_step_1 = 0x05,
        .bt_rx_gain_swp_step_2 = 0x04,
        .bt_rx_gain_swp_step_3 = 0x06,
        .bt_rx_gain_swp_step_4 = 0x05,
        .bt_rx_gain_swp_step_5 = 0x01,
        .bt_rx_gain_swp_step_6 = 0x06,
        .bt_rx_gain_swp_step_7 = 0x05,
        .bt_rx_gain_swp_step_8 = 0x00,
        .bt_rx_gain_swp_step_9 = 0x00,
        .bt_rx_gain_swp_step_10 = 0x00,
        .bt_rx_gain_swp_step_11 = 0x00,
        .bt_rx_gain_swp_step_12 = 0x00,
        .bt_rx_gain_swp_step_13 = 0x00,
        .bt_rx_gain_swp_step_14 = 0x00,
        .bt_rx_gain_swp_step_15 = 0x00,
        .gain_cmp_1 = 0x0a,
        .gain_cmp_6 = 0x0a,
        .gain_cmp_11 = 0x0c,
        .gain_cmp_ext2_1 = 0xf0,
        .gain_cmp_ext2_6 = 0xf0,
        .gain_cmp_ext2_11 = 0xf0,
        .gain_cmp_ext3_1 = 0xe0,
        .gain_cmp_ext3_6 = 0xe0,
        .gain_cmp_ext3_11 = 0xe0,
        .gain_cmp_bt_ofs_1 = 0x18,
        .gain_cmp_bt_ofs_6 = 0x18,
        .gain_cmp_bt_ofs_11 = 0x18,
        .target_power_qdb_0 = LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 78),
        .target_power_qdb_1 = LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 76),
        .target_power_qdb_2 = LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 74),
        .target_power_qdb_3 = LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 68),
        .target_power_qdb_4 = LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 60),
        .target_power_qdb_5 = LIMIT(CONFIG_ESP32_PHY_MAX_TX_POWER * 4, 0, 52),
        .target_power_index_mcs0 = 0,
        .target_power_index_mcs1 = 0,
        .target_power_index_mcs2 = 1,
        .target_power_index_mcs3 = 1,
        .target_power_index_mcs4 = 2,
        .target_power_index_mcs5 = 3,
        .target_power_index_mcs6 = 4,
        .target_power_index_mcs7 = 5,
        .pwr_ind_11b_en = 0,
        .pwr_ind_11b_0 = 0,
        .pwr_ind_11b_1 = 0,
        .chan_backoff_en = 0,
        .chan1_power_backoff_qdb = 0,
        .chan2_power_backoff_qdb = 0,
        .chan3_power_backoff_qdb = 0,
        .chan4_power_backoff_qdb = 0,
        .chan5_power_backoff_qdb = 0,
        .chan6_power_backoff_qdb = 0,
        .chan7_power_backoff_qdb = 0,
        .chan8_power_backoff_qdb = 0,
        .chan9_power_backoff_qdb = 0,
        .chan10_power_backoff_qdb = 0,
        .chan11_power_backoff_qdb = 0,
        .chan12_power_backoff_qdb = 0,
        .chan13_power_backoff_qdb = 0,
        .chan14_power_backoff_qdb = 0,
        .chan1_rate_backoff_index = 0,
        .chan2_rate_backoff_index = 0,
        .chan3_rate_backoff_index = 0,
        .chan4_rate_backoff_index = 0,
        .chan5_rate_backoff_index = 0,
        .chan6_rate_backoff_index = 0,
        .chan7_rate_backoff_index = 0,
        .chan8_rate_backoff_index = 0,
        .chan9_rate_backoff_index = 0,
        .chan10_rate_backoff_index = 0,
        .chan11_rate_backoff_index = 0,
        .chan12_rate_backoff_index = 0,
        .chan13_rate_backoff_index = 0,
        .chan14_rate_backoff_index = 0,
        .spur_freq_cfg_msb_1 = 0,
        .spur_freq_cfg_1 = 0,
        .spur_freq_cfg_div_1 = 0,
        .spur_freq_en_h_1 = 0,
        .spur_freq_en_l_1 = 0,
        .spur_freq_cfg_msb_2 = 0,
        .spur_freq_cfg_2 = 0,
        .spur_freq_cfg_div_2 = 0,
        .spur_freq_en_h_2 = 0,
        .spur_freq_en_l_2 = 0,
        .spur_freq_cfg_msb_3 = 0,
        .spur_freq_cfg_3 = 0,
        .spur_freq_cfg_div_3 = 0,
        .spur_freq_en_h_3 = 0,
        .spur_freq_en_l_3 = 0,
        .reserved = {0}
};

static const char phy_init_magic_post[] = PHY_INIT_MAGIC;

