/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_flash.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_SPI_FLASH_FREQ_LIMIT_C5_240MHZ
/**
 * @brief Callback to limit CPU frequency for flash encryption writes (ESP32-C5 only, 240MHz)
 *
 * This function limits CPU frequency to <= 160MHz during encrypted flash writes.
 * It should be called before starting an encrypted flash write operation.
 *
 * @note This is an internal function, not exposed to users.
 * @note This function is placed in IRAM (implementation uses IRAM_ATTR).
 * @note This function is called from spi_flash_os_func_app.c::spi1_start() after cache
 *       and scheduler locks are acquired. As a result, there is no concurrency concern
 *       and no need for internal locking or reference counting.
 */
void esp_flash_freq_limit_cb(void);

/**
 * @brief Callback to unlimit CPU frequency after flash encryption writes (ESP32-C5 only, 240MHz)
 *
 * This function restores the CPU frequency after an encrypted flash write operation.
 * It should be called after completing an encrypted flash write operation.
 *
 * @note This is an internal function, not exposed to users.
 * @note This function is placed in IRAM (implementation uses IRAM_ATTR).
 * @note This function is called from spi_flash_os_func_app.c::spi1_end() before cache
 *       and scheduler locks are released. As a result, there is no concurrency concern
 *       and no need for internal locking or reference counting.
 */
void esp_flash_freq_unlimit_cb(void);
#endif // CONFIG_SPI_FLASH_FREQ_LIMIT_C5_240MHZ

#ifdef __cplusplus
}
#endif
