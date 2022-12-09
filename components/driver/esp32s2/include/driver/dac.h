// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "driver/dac_common.h"

#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * @brief DAC digital controller initialization.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_init(void);

/**
 * @brief DAC digital controller deinitialization.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_deinit(void);

/**
 * @brief Setting the DAC digital controller.
 *
 * @param cfg Pointer to digital controller paramter. See ``dac_digi_config_t``.
 * 
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t dac_digi_controller_config(const dac_digi_config_t *cfg);

/**
 * @brief DAC digital controller start output voltage.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_start(void);

/**
 * @brief DAC digital controller stop output voltage.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_stop(void);

/**
 * @brief Reset DAC digital controller FIFO.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_fifo_reset(void);

/**
 * @brief Reset DAC digital controller.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_reset(void);

#ifdef __cplusplus
}
#endif
