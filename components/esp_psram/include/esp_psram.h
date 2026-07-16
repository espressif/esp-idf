/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
 * Initializes the PSRAM hardware and load the XIP segments or maps the PSRAM memory
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

/**
 * @brief Check if the pointer falls inside the unencrypted PSRAM carve-out region
 *
 * When @c CONFIG_SPIRAM_ENC_EXEMPT is enabled, esp_psram reserves a region of PSRAM
 * that is mapped without encryption and exposed through the @c MALLOC_CAP_SPIRAM_NO_ENC
 * heap capability. This function lets drivers verify whether a buffer returned by the
 * heap allocator actually lives in that unencrypted region — useful for example after
 * a @c heap_caps_malloc_prefer() call that may have fallen back to encrypted PSRAM.
 *
 * @param[in] p  The pointer to check
 *
 * @return
 *        - true:  the pointer is within the unencrypted PSRAM carve-out
 *        - false: the pointer is not in the carve-out, PSRAM is not initialized,
 *                 or @c CONFIG_SPIRAM_ENC_EXEMPT is disabled
 */
bool esp_psram_ptr_is_no_enc(const void *p);

#ifdef __cplusplus
}
#endif
