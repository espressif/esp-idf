/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED

typedef struct dac_oneshot_s    *dac_oneshot_handle_t;  /*!< DAC oneshot channel handle */

/**
 * @brief DAC oneshot channel configuration
 *
 */
typedef struct {
    dac_channel_t               chan_id;    /*!< DAC channel id */
} dac_oneshot_config_t;

/**
 * @brief Allocate a new DAC oneshot channel
 * @note  The channel will be enabled as well when the channel allocated
 *
 * @param[in]  oneshot_cfg   The configuration for the oneshot channel
 * @param[out] ret_handle    The returned oneshot channel handle
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channel has been registered already
 *      - ESP_ERR_NO_MEM        No memory for the DAC oneshot channel resources
 *      - ESP_OK                Allocate the new DAC oneshot channel success
 */
esp_err_t dac_oneshot_new_channel(const dac_oneshot_config_t *oneshot_cfg, dac_oneshot_handle_t *ret_handle);

/**
 * @brief Delete the DAC oneshot channel
 * @note  The channel will be disabled as well when the channel deleted
 *
 * @param[in]  handle       The DAC oneshot channel handle
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel has already been de-registered
 *      - ESP_OK                Delete the oneshot channel success
 */
esp_err_t dac_oneshot_del_channel(dac_oneshot_handle_t handle);

/**
 * @brief Output the voltage
 * @note  Generally it'll take 7~11 us on ESP32 and 10~21 us on ESP32-S2
 *
 * @param[in]  handle       The DAC oneshot channel handle
 * @param[in]  digi_value   The digital value that need to be converted
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_OK                Convert the digital value success
 */
esp_err_t dac_oneshot_output_voltage(dac_oneshot_handle_t handle, uint8_t digi_value);

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
