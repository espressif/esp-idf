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
#include "esp_phy_init.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file phy.h
 * @brief Declarations for functions provided by libphy.a
 */

/**
 * @brief Return ROM function pointer table from PHY library.
 */
void phy_get_romfunc_addr(void);

/**
 * @brief Initialize PHY module and do RF calibration
 * @param[in] init_data Initialization parameters to be used by the PHY
 * @param[inout] cal_data As input, calibration data previously obtained. As output, will contain new calibration data.
 * @param[in] cal_mode  RF calibration mode
 * @return reserved for future use
 */
int register_chipv7_phy(const esp_phy_init_data_t* init_data, esp_phy_calibration_data_t *cal_data, esp_phy_calibration_mode_t cal_mode);

/**
 * @brief Get the format version of calibration data used by PHY library.
 * @return Format version number, OR'ed with BIT(16) if PHY is in WIFI only mode.
 */
uint32_t phy_get_rf_cal_version();

/**
 * @brief Set RF/BB for only WIFI mode or coexist(WIFI & BT) mode
 * @param[in] true is for only WIFI mode, false is for coexist mode. default is 0.
 * @return NULL
 */
void phy_set_wifi_mode_only(bool wifi_only);

/**
 * @brief Set BT the highest priority in coexist mode.
 * @return NULL
 */
void coex_bt_high_prio(void);

#ifdef __cplusplus
}
#endif

