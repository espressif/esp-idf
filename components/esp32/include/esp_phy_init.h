// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint8_t param_ver_id;                   /*!< init_data structure version */
	uint8_t crystal_select;                 /*!< 0: 40MHz, 1: 26 MHz, 2: 24 MHz, 3: auto */
	uint8_t wifi_rx_gain_swp_step_1;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_2;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_3;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_4;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_5;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_6;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_7;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_8;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_9;        /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_10;       /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_11;       /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_12;       /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_13;       /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_14;       /*!< do not change */
	uint8_t wifi_rx_gain_swp_step_15;       /*!< do not change */
	uint8_t bt_rx_gain_swp_step_1;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_2;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_3;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_4;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_5;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_6;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_7;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_8;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_9;          /*!< do not change */
	uint8_t bt_rx_gain_swp_step_10;         /*!< do not change */
	uint8_t bt_rx_gain_swp_step_11;         /*!< do not change */
	uint8_t bt_rx_gain_swp_step_12;         /*!< do not change */
	uint8_t bt_rx_gain_swp_step_13;         /*!< do not change */
	uint8_t bt_rx_gain_swp_step_14;         /*!< do not change */
	uint8_t bt_rx_gain_swp_step_15;         /*!< do not change */
	uint8_t gain_cmp_1;                     /*!< do not change */
	uint8_t gain_cmp_6;                     /*!< do not change */
	uint8_t gain_cmp_11;                    /*!< do not change */
	uint8_t gain_cmp_ext2_1;                /*!< do not change */
	uint8_t gain_cmp_ext2_6;                /*!< do not change */
	uint8_t gain_cmp_ext2_11;               /*!< do not change */
	uint8_t gain_cmp_ext3_1;                /*!< do not change */
	uint8_t gain_cmp_ext3_6;                /*!< do not change */
	uint8_t gain_cmp_ext3_11;               /*!< do not change */
	uint8_t gain_cmp_bt_ofs_1;              /*!< do not change */
	uint8_t gain_cmp_bt_ofs_6;              /*!< do not change */
	uint8_t gain_cmp_bt_ofs_11;             /*!< do not change */
	uint8_t target_power_qdb_0;             /*!< 78 means target power is 78/4=19.5dbm */
	uint8_t target_power_qdb_1;             /*!< 76 means target power is 76/4=19dbm */
	uint8_t target_power_qdb_2;             /*!< 74 means target power is 74/4=18.5dbm */
	uint8_t target_power_qdb_3;             /*!< 68 means target power is 68/4=17dbm */
	uint8_t target_power_qdb_4;             /*!< 64 means target power is 64/4=16dbm */
	uint8_t target_power_qdb_5;             /*!< 52 means target power is 52/4=13dbm */
	uint8_t target_power_index_mcs0;        /*!< target power index is 0, means target power is target_power_qdb_0 19.5dbm;  (1m,2m,5.5m,11m,6m,9m) */
	uint8_t target_power_index_mcs1;        /*!< target power index is 0, means target power is target_power_qdb_0 19.5dbm;  (12m) */
	uint8_t target_power_index_mcs2;        /*!< target power index is 1, means target power is target_power_qdb_1 19dbm;    (18m) */
	uint8_t target_power_index_mcs3;        /*!< target power index is 1, means target power is target_power_qdb_1 19dbm;    (24m) */
	uint8_t target_power_index_mcs4;        /*!< target power index is 2, means target power is target_power_qdb_2 18.5dbm;  (36m) */
	uint8_t target_power_index_mcs5;        /*!< target power index is 3, means target power is target_power_qdb_3 17dbm;    (48m) */
	uint8_t target_power_index_mcs6;        /*!< target power index is 4, means target power is target_power_qdb_4 16dbm;    (54m) */
	uint8_t target_power_index_mcs7;        /*!< target power index is 5, means target power is target_power_qdb_5 13dbm */
	uint8_t pwr_ind_11b_en;                 /*!< 0: 11b power is same as mcs0 and 6m, 1: 11b power different with OFDM */
	uint8_t pwr_ind_11b_0;                  /*!< 1m, 2m power index [0~5] */
	uint8_t pwr_ind_11b_1;                  /*!< 5.5m, 11m power index [0~5] */
	uint8_t chan_backoff_en;                /*!< 0: channel backoff disable, 1:channel backoff enable */
	uint8_t chan1_power_backoff_qdb;        /*!< 4 means backoff is 1db */
	uint8_t chan2_power_backoff_qdb;        /*!< see chan1_power_backoff_qdb */
	uint8_t chan3_power_backoff_qdb;        /*!< chan1_power_backoff_qdb */
	uint8_t chan4_power_backoff_qdb;        /*!< chan1_power_backoff_qdb */
	uint8_t chan5_power_backoff_qdb;        /*!< chan1_power_backoff_qdb */
	uint8_t chan6_power_backoff_qdb;        /*!< chan1_power_backoff_qdb */
	uint8_t chan7_power_backoff_qdb;        /*!< chan1_power_backoff_qdb */
	uint8_t chan8_power_backoff_qdb;        /*!< chan1_power_backoff_qdb */
	uint8_t chan9_power_backoff_qdb;        /*!< chan1_power_backoff_qdb */
	uint8_t chan10_power_backoff_qdb;       /*!< chan1_power_backoff_qdb */
	uint8_t chan11_power_backoff_qdb;       /*!< chan1_power_backoff_qdb */
	uint8_t chan12_power_backoff_qdb;       /*!< chan1_power_backoff_qdb */
	uint8_t chan13_power_backoff_qdb;       /*!< chan1_power_backoff_qdb */
	uint8_t chan14_power_backoff_qdb;       /*!< chan1_power_backoff_qdb */
	uint8_t chan1_rate_backoff_index;       /*!< if bit i is set, backoff data rate is target_power_qdb_i */
	uint8_t chan2_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan3_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan4_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan5_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan6_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan7_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan8_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan9_rate_backoff_index;       /*!< see chan1_rate_backoff_index */
	uint8_t chan10_rate_backoff_index;      /*!< see chan1_rate_backoff_index */
	uint8_t chan11_rate_backoff_index;      /*!< see chan1_rate_backoff_index */
	uint8_t chan12_rate_backoff_index;      /*!< see chan1_rate_backoff_index */
	uint8_t chan13_rate_backoff_index;      /*!< see chan1_rate_backoff_index */
	uint8_t chan14_rate_backoff_index;      /*!< see chan1_rate_backoff_index */
	uint8_t spur_freq_cfg_msb_1;            /*!< first spur: */
	uint8_t spur_freq_cfg_1;                /*!< spur_freq_cfg = (spur_freq_cfg_msb_1 <<8) | spur_freq_cfg_1 */
	uint8_t spur_freq_cfg_div_1;            /*!< spur_freq=spur_freq_cfg/spur_freq_cfg_div_1 */
	uint8_t spur_freq_en_h_1;               /*!< the seventh bit for total enable */
	uint8_t spur_freq_en_l_1;               /*!< each bit for 1 channel, and use [spur_freq_en_h, spur_freq_en_l] to select the spur's channel priority */
	uint8_t spur_freq_cfg_msb_2;            /*!< second spur: */
	uint8_t spur_freq_cfg_2;                /*!< spur_freq_cfg = (spur_freq_cfg_msb_2 <<8) | spur_freq_cfg_2 */
	uint8_t spur_freq_cfg_div_2;            /*!< spur_freq=spur_freq_cfg/spur_freq_cfg_div_2 */
	uint8_t spur_freq_en_h_2;               /*!< the seventh bit for total enable */
	uint8_t spur_freq_en_l_2;               /*!< each bit for 1 channel, and use [spur_freq_en_h, spur_freq_en_l] to select the spur's channel priority */
	uint8_t spur_freq_cfg_msb_3;            /*!< third spur: */
	uint8_t spur_freq_cfg_3;                /*!< spur_freq_cfg = (spur_freq_cfg_msb_3 <<8) | spur_freq_cfg_3 */
	uint8_t spur_freq_cfg_div_3;            /*!< spur_freq=spur_freq_cfg/spur_freq_cfg_div_3 */
	uint8_t spur_freq_en_h_3;               /*!< the seventh bit for total enable */
	uint8_t spur_freq_en_l_3;               /*!< each bit for 1 channel, and use [spur_freq_en_h, spur_freq_en_l] to select the spur's channel priority, */
	uint8_t reserved[23];                   /*!< reserved for future expansion */
} esp_phy_init_data_t;

typedef struct {
    uint8_t opaque[1904];                   /*!< opaque calibration data */
} esp_phy_calibration_data_t;

typedef enum {
    PHY_RF_CAL_PARTIAL = 0x00000000,        /*!< Do part of RF calibration. This should be used after power-on reset. */
    PHY_RF_CAL_NONE    = 0x00000001,        /*!< Don't do any RF calibration. This mode is only suggested to be used after deep sleep reset. */
    PHY_RF_CAL_FULL    = 0x00000002         /*!< Do full RF calibration. Produces best results, but also consumes a lot of time and current. Suggested to be used once. */
} esp_phy_calibration_mode_t;

/**
 *
 * @param mode
 * @return
 */
esp_err_t esp_phy_init(esp_phy_calibration_mode_t mode);

#ifndef CONFIG_ESP32_STORE_PHY_CALIBRATION_DATA_IN_NVS

/**
 *
 * @param cal_data
 * @return
 */
esp_err_t esp_phy_store_cal_data(const esp_phy_calibration_data_t* cal_data);

/**
 *
 * @param out_cal_data
 * @return
 */
esp_err_t esp_phy_load_cal_data(esp_phy_calibration_data_t* out_cal_data);

#endif // CONFIG_ESP32_STORE_PHY_CALIBRATION_DATA_IN_NVS

#ifdef __cplusplus
}
#endif

