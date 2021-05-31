/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_common.h"

#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * @brief Enable DAC output data from I2S
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_i2s_enable(void);

/**
 * @brief Disable DAC output data from I2S
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_i2s_disable(void);

#ifdef __cplusplus
}
#endif
