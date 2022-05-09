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
