/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize PSRAM interface/hardware.
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_FAIL:              PSRAM isn't initialized successfully, potential reason would be: wrong VDDSDIO, invalid chip ID, etc.
 *        - ESP_ERR_INVALID_STATE: PSRAM is initialized already
 */
esp_err_t esp_psram_init(void);

/**
 * @brief If PSRAM has been initialized
 *
 * @return
 *          - true:  PSRAM has been initialized successfully
 *          - false: PSRAM hasn't been initialized or initialized failed
 */
bool esp_psram_is_initialized(void);

/**
 * @brief Get the available size of the attached PSRAM chip
 *
 * @return Size in bytes, or 0 if PSRAM isn't successfully initialized
 */
size_t esp_psram_get_size(void);

#ifdef __cplusplus
}
#endif
