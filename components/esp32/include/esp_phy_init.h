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
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file PHY init parameters and API
 */


/**
 * @brief Structure holding PHY init parameters
 */
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

/**
 * @brief Opaque PHY calibration data
 */
typedef struct {
    uint8_t opaque[1904];                   /*!< calibration data */
} esp_phy_calibration_data_t;

typedef enum {
    PHY_RF_CAL_PARTIAL = 0x00000000,        /*!< Do part of RF calibration. This should be used after power-on reset. */
    PHY_RF_CAL_NONE    = 0x00000001,        /*!< Don't do any RF calibration. This mode is only suggested to be used after deep sleep reset. */
    PHY_RF_CAL_FULL    = 0x00000002         /*!< Do full RF calibration. Produces best results, but also consumes a lot of time and current. Suggested to be used once. */
} esp_phy_calibration_mode_t;

/**
 * @brief Get PHY init data
 *
 * If "Use a partition to store PHY init data" option is set in menuconfig,
 * This function will load PHY init data from a partition. Otherwise,
 * PHY init data will be compiled into the application itself, and this function
 * will return a pointer to PHY init data located in read-only memory (DROM).
 *
 * If "Use a partition to store PHY init data" option is enabled, this function
 * may return NULL if the data loaded from flash is not valid.
 *
 * @note Call esp_phy_release_init_data to release the pointer obtained using
 * this function after the call to esp_wifi_init.
 *
 * @return pointer to PHY init data structure
 */
const esp_phy_init_data_t* esp_phy_get_init_data();

/**
 * @brief Release PHY init data
 * @param data  pointer to PHY init data structure obtained from
 *              esp_phy_get_init_data function
 */
void esp_phy_release_init_data(const esp_phy_init_data_t* data);

/**
 * @brief Function called by esp_phy_init to load PHY calibration data
 *
 * This is a convenience function which can be used to load PHY calibration
 * data from NVS. Data can be stored to NVS using esp_phy_store_cal_data_to_nvs
 * function.
 *
 * If calibration data is not present in the NVS, or
 * data is not valid (was obtained for a chip with a different MAC address,
 * or obtained for a different version of software), this function will
 * return an error.
 *
 * If "Initialize PHY in startup code" option is set in menuconfig, this
 * function will be used to load calibration data. To provide a different
 * mechanism for loading calibration data, disable
 * "Initialize PHY in startup code" option in menuconfig and call esp_phy_init
 * function from the application. For an example usage of esp_phy_init and
 * this function, see esp_phy_store_cal_data_to_nvs function in cpu_start.c
 *
 * @param out_cal_data pointer to calibration data structure to be filled with
 *                     loaded data.
 * @return ESP_OK on success
 */
esp_err_t esp_phy_load_cal_data_from_nvs(esp_phy_calibration_data_t* out_cal_data);

/**
 * @brief Function called by esp_phy_init to store PHY calibration data
 *
 * This is a convenience function which can be used to store PHY calibration
 * data to the NVS. Calibration data is returned by esp_phy_init function.
 * Data saved using this function to the NVS can later be loaded using
 * esp_phy_store_cal_data_to_nvs function.
 *
 * If "Initialize PHY in startup code" option is set in menuconfig, this
 * function will be used to store calibration data. To provide a different
 * mechanism for storing calibration data, disable
 * "Initialize PHY in startup code" option in menuconfig and call esp_phy_init
 * function from the application.
 *
 * @param cal_data pointer to calibration data which has to be saved.
 * @return ESP_OK on success
 */
esp_err_t esp_phy_store_cal_data_to_nvs(const esp_phy_calibration_data_t* cal_data);

/**
 * @brief Initialize PHY and RF module
 *
 * PHY and RF module should be initialized in order to use WiFi or BT.
 * Now PHY and RF initializing job is done automatically when start WiFi or BT. Users should not
 * call this API in their application.
 *
 * @param init_data  PHY parameters. Default set of parameters can
 *                   be obtained by calling esp_phy_get_default_init_data
 *                   function.
 * @param mode  Calibration mode (Full, partial, or no calibration)
 * @param[inout] calibration_data
 * @param is_sleep WiFi wakes up from sleep or not
 * @return ESP_OK on success.
 * @return ESP_FAIL on fail.
 */
esp_err_t esp_phy_rf_init(const esp_phy_init_data_t* init_data,
        esp_phy_calibration_mode_t mode, esp_phy_calibration_data_t* calibration_data, bool is_sleep);

/**
 * @brief De-initialize PHY and RF module
 *
 * PHY module should be de-initialized in order to shutdown WiFi or BT.
 * Now PHY and RF de-initializing job is done automatically when stop WiFi or BT. Users should not
 * call this API in their application.
 *
 * @return ESP_OK on success.
 */
esp_err_t esp_phy_rf_deinit(void);

/**
 * @brief Load calibration data from NVS and initialize PHY and RF module
 */
void esp_phy_load_cal_and_init(void);

#ifdef __cplusplus
}
#endif

