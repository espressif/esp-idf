/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
