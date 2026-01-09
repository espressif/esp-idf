/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_PSRAM_MSPI_MB_WORKAROUND    (CONFIG_IDF_TARGET_ESP32C5 && CONFIG_ESP32C5_REV_MIN_FULL < 102) || (CONFIG_IDF_TARGET_ESP32C61 && CONFIG_ESP32C61_REV_MIN_FULL < 101)

/**
 * @brief Register MSPI PSRAM interrupt
 *
 * This ISR mainly:
 * - Report MSPI bus errors, e.g. fifo overflow, underflow. For example, when data:
 *   - peripheral -> dma -> mspi -> psram, if the mspi fifo is empty, there will be a fifo underflow error.)
 *   - peripheral <- dma <- mspi <- psram, if the mspi fifo is full, there will be a fifo overflow error.)
 * - etc.
 *
 * @return ESP_OK on success, otherwise an error code
 */
esp_err_t esp_psram_mspi_register_isr(void);

/**
 * @brief Unregister MSPI PSRAM interrupt
 *
 * @return ESP_OK on success, otherwise an error code
 */
esp_err_t esp_psram_mspi_unregister_isr(void);

/**
 * @brief Initialize PSRAM MSPI memory barrier
 *
 * @return ESP_OK on success, otherwise an error code
 */
esp_err_t esp_psram_mspi_mb_init(void);

/**
 * @brief PSRAM MSPI memory barrier
 */
void esp_psram_mspi_mb(void);

#ifdef __cplusplus
}
#endif
