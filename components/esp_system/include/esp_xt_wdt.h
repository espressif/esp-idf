/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "esp_intr_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief esp_xt_wdt configuration struct
 *
 */
typedef struct {
    uint8_t timeout;                /*!< Watchdog timeout */
    bool auto_backup_clk_enable;    /*!< Enable automatic switch to backup clock at timeout */
} esp_xt_wdt_config_t;

/*  Callback function for WDT interrupt*/
typedef void (*esp_xt_callback_t)(void *arg);

/**
 * @brief Initializes the xtal32k watchdog timer
 *
 * @param cfg Pointer to configuration struct
 * @return esp_err_t
 *      - ESP_OK: XTWDT was successfully enabled
 *      - ESP_ERR_NO_MEM: Failed to allocate ISR
 */
esp_err_t esp_xt_wdt_init(const esp_xt_wdt_config_t *cfg);

/**
 * @brief Register a callback function that will be called when the watchdog
 *        times out.
 *
 * @note This function will be called from an interrupt context where the cache might be disabled.
 *       Thus the function should be placed in IRAM and must not perform any blocking operations.
 *
 *       Only one callback function can be registered, any call to esp_xt_wdt_register_callback
 *       will override the previous callback function.
 *
 * @param func The callback function to register
 * @param arg  Pointer to argument that will be passed to the callback function
 */
void esp_xt_wdt_register_callback(esp_xt_callback_t func, void *arg);

/**
 * @brief Restores the xtal32k clock and re-enables the WDT
 *
 */
void esp_xt_wdt_restore_clk(void);

#ifdef __cplusplus
}
#endif
