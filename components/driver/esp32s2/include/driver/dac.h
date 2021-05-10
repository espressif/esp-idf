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
esp_err_t __attribute__((unused)) dac_digi_init(void);

/**
 * @brief Initialize the Digital DAC.
 *
 * @param init_cfg Pointer to Digital DAC initilization config. Refer to ``dac_digi_config_t``.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NO_MEM        If out of memory
 *         - ESP_OK                On success
 */
esp_err_t dac_digi_initialize(const dac_digi_config_t *init_cfg);

/**
 * @brief DAC digital controller deinitialization.
 * @return
 *     - ESP_OK success
 */
esp_err_t __attribute__((unused)) dac_digi_deinit(void);

/**
 * @brief Deinitialize the Digital DAC.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 *         - ESP_OK                On success
 */
esp_err_t dac_digi_deinitialize(void);

/**
 * @brief Write bytes to Digital DAC through DMA.
 *
 * @param[in] buffer                 Buffer to write to DAC.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid. Usually it means the ADC sampling rate is faster than the task processing rate.
 *         - ESP_ERR_TIMEOUT       Operation timed out
 *         - ESP_OK                On success
 */
esp_err_t dac_digi_write_bytes(const void *buffer);

/**
 * @brief Setting the DAC digital controller.
 *
 * @param cfg Pointer to digital controller paramter. See ``dac_digi_config_t``.
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t __attribute__((unused)) dac_digi_controller_config(const dac_digi_config_t *cfg);

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
