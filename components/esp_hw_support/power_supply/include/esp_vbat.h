/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_VBAT_STATE_NORMAL,          /*!< Normal working state, not charging */
    ESP_VBAT_STATE_CHARGING,        /*!< Battery is charging by VDDA, only for chargeable battery. */
    ESP_VBAT_STATE_LOWBATTERY,      /*!< Battery is under voltage, battery replacement required */
} esp_vbat_state_t;

#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
/**
 * @brief Wait until battery charging done.
 *
 * @note This function is not allowed to be called in ISR  context.
 *
 * @param  checking_period Time in ticks to wait until timeout or succeed
 * @return
 *  - ESP_OK: Battery charging completed
 *  - ESP_FAIL: Called in ISR context
 *  - ESP_ERR_TIMEOUT: Timeout waiting for battery charging to complete
 */
esp_err_t esp_vbat_wait_battery_charge_done(TickType_t checking_period);
#endif

/**
 * @brief Get battery status.
 * @return
 *     - DISCHARGING Battery is discharging.
 *     - CHARGING    Battery is charing (ESP_VBAT_USE_RECHARGEABLE_BATTERY only).
 *     - LOWBATTERY  Battery is under voltage.
 */
esp_vbat_state_t esp_vbat_get_battery_state(void);

#ifdef __cplusplus
}
#endif
