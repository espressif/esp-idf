/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN YOUR APPLICATIONS
// The following APIs are for internal use, public to other IDF components, but not for users' applications.

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S controller type
 */
typedef enum {
    I2S_CTLR_HP,    ///< HP I2S
    I2S_CTLR_LP,    ///< LP I2S
} i2s_ctlr_t;

/**
 * @brief Hold the I2S port occupation
 *
 * @note This private API is used to avoid applications from using the same I2S instance for different purpose.
 * @note This function will help enable the peripheral APB clock as well.
 *
 * @param[in] type I2S controller type
 * @param id I2S port number
 * @param comp_name The name of compnant that occupied this i2s controller
 * @return
 *      - ESP_OK: The specific I2S port is free and register the new device object successfully
 *      - ESP_ERR_INVALID_ARG: Invalid argument, e.g. wrong port_id
 *      - ESP_ERR_NOT_FOUND Specific I2S port is not available
 */
esp_err_t i2s_platform_acquire_occupation(i2s_ctlr_t type, int id, const char *comp_name);

/**
 * @brief Release the I2S port occupation
 *
 * @note This function will help disable the peripheral APB clock as well.
 *
 * @param[in] type I2S controller type
 * @param id I2S port number
 * @return
 *      - ESP_OK: Deregister I2S port successfully (i.e. that I2S port can used used by other users after this function returns)
 *      - ESP_ERR_INVALID_ARG: Invalid argument, e.g. wrong port_id
 *      - ESP_ERR_INVALID_STATE: Specific I2S port is free already
 */
esp_err_t i2s_platform_release_occupation(i2s_ctlr_t type, int id);

/**
 * @brief This function is only used for getting DMA buffer offset in `test_i2s_iram.c`
 *
 * @return
 *      - The offset of DMA buffers in the `i2s_chan_handle_t` struct (unit: bytes)
 */
size_t i2s_platform_get_dma_buffer_offset(void);

#ifdef __cplusplus
}
#endif
