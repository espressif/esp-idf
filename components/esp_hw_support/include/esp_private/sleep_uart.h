/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_sleep.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prepare all enabled UARTs for sleep based on their configured handling modes
 *
 * This function iterates through all UART ports and applies the appropriate handling
 * strategy (flush, suspend, or discard) based on each UART's configured mode and
 * the sleep parameters. Only enabled UARTs are processed.
 *
 * @param sleep_flags Sleep configuration flags (e.g., PMU_SLEEP_PD_TOP to indicate TOP domain power down)
 * @param deep_sleep  true if entering deep sleep, false for light sleep
 */
void sleep_uart_prepare(uint32_t sleep_flags, bool deep_sleep);

/**
 * @brief Resume UARTs that were suspended during sleep preparation
 *
 * This function restores transmission for UARTs that were suspended (via XOFF)
 * during sleep preparation. Only UARTs that were actually suspended are resumed
 * (tracked via internal bitmap). UARTs that were flushed or discarded are not affected.
 */
void sleep_uart_resume(void);

/**
 * @brief Set the UART handling mode for a specific UART port
 *
 * This function configures how a specific UART port should be handled during
 * sleep entry. The configured mode will be used (and possibly resolved if set
 * to ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART) when sleep_uart_prepare() is called.
 *
 * @param uart_num UART port number (0 to SOC_UART_HP_NUM-1)
 * @param handling_mode Handling mode to configure (see esp_sleep_uart_handling_mode_t)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if uart_num is out of range or handling_mode is invalid
 */
esp_err_t sleep_uart_set_handling_mode(int uart_num, esp_sleep_uart_handling_mode_t handling_mode);

#ifdef __cplusplus
}
#endif
