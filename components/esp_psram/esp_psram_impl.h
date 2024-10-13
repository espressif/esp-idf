/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_SIZE_2MB                  (2 * 1024 * 1024)
#define PSRAM_SIZE_4MB                  (4 * 1024 * 1024)
#define PSRAM_SIZE_8MB                  (8 * 1024 * 1024)
#define PSRAM_SIZE_16MB                 (16 * 1024 * 1024)
#define PSRAM_SIZE_32MB                 (32 * 1024 * 1024)
#define PSRAM_SIZE_64MB                 (64 * 1024 * 1024)

/**
 * @brief To get the physical psram size in bytes.
 *
 * @param[out] out_size_bytes    physical psram size in bytes.
 */
esp_err_t esp_psram_impl_get_physical_size(uint32_t *out_size_bytes);

/**
 * @brief To get the available physical psram size in bytes.
 *
 * @param[out] out_size_bytes    availabe physical psram size in bytes.
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes);

/**
 * @brief Enable psram and configure it to a ready state
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_NOT_SUPPORTED: PSRAM ID / vendor ID check fail
 *        - ESP_ERR_INVALID_STATE: On esp32, when VSPI peripheral is needed but cannot be claimed
 */
esp_err_t esp_psram_impl_enable(void);

/**
 * @brief get psram CS IO
 *
 * @return psram CS IO
 */
uint8_t esp_psram_impl_get_cs_io(void);

#ifdef __cplusplus
}
#endif
