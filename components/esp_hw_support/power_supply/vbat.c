/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "hal/vbat_ll.h"
#include "hal/brownout_ll.h"
#include "hal/vbat_hal.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "esp_private/startup_internal.h"
#include "esp_check.h"
#include "soc/power_supply_periph.h"

#define VBAT_BROWNOUT_DET_LVL       CONFIG_ESP_VBAT_BROWNOUT_DET_LVL

#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
#define VBAT_CHARGE_DET_LVL_LOW        CONFIG_ESP_VBAT_DET_LVL_LOW
#define VBAT_CHARGE_DET_LVL_HIGH       CONFIG_ESP_VBAT_DET_LVL_HIGH
#define VBAT_CHARGER_RESISTOR_VALUE CONFIG_ESP_VBAT_CHARGER_CIRCUIT_RESISTOR_VAL

#if (VBAT_CHARGER_RESISTOR_VALUE < 1000 || VBAT_CHARGER_RESISTOR_VALUE > 4500 || VBAT_CHARGER_RESISTOR_VALUE % 500 != 0)
#error "vbat charger resistor (ESP_VBAT_CHARGER_CIRCUIT_RESISTOR_VAL) must be between 1000 and 4500 ohms and must be a multiple of 500."
#endif

#if (VBAT_BROWNOUT_DET_LVL >= VBAT_CHARGE_DET_LVL_LOW)
#error "vbat charger low threshold is equal or lower than vbat brownout threshold, please put vbat brownout threshold lower than vbat charger low threshold"
#endif

#endif

static const char TAG[] = "VBAT";

IRAM_ATTR static void vbat_isr_handler(void *arg)
{
    uint32_t int_status;
    vbat_ll_get_interrupt_status(&int_status);
    vbat_ll_clear_intr_mask(int_status);

    if (int_status & VBAT_LL_CHARGER_UNDERVOLTAGE_INTR) {
        ESP_DRAM_LOGW(TAG, "RTC battery voltage low, start charging...");
        vbat_ll_start_battery_charge(true);
    }

    if (int_status & VBAT_LL_CHARGER_UPVOLTAGE_INTR) {
        ESP_DRAM_LOGW(TAG, "RTC battery voltage reaches high limit , stop charging...");
        vbat_ll_start_battery_charge(false);
    }

    if (int_status & VBAT_LL_BROWNOUT_INTR) {
        // TODO: A callback may needed here to inform an under voltage event.
        ESP_DRAM_LOGW(TAG, "RTC battery voltage low, please change battery...");
    }

}

esp_err_t esp_vbat_init(void)
{
    intr_handle_t vbat_intr;
#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
    vbat_hal_config_t vbat_cfg = {
        .enable_vbat_charger = true,
        .charger_resistor_value = VBAT_CHARGER_RESISTOR_VALUE,
        .low_threshold = VBAT_CHARGE_DET_LVL_LOW,
        .high_threshold = VBAT_CHARGE_DET_LVL_HIGH,
        .brownout_threshold = VBAT_BROWNOUT_DET_LVL,
        .undervoltage_filter_time = 20,
        .upvoltage_filter_time = 10,
        .interrupt_mask = (VBAT_LL_CHARGER_MASK | VBAT_LL_DETECT_MASK),
    };

#else
    vbat_hal_config_t vbat_cfg = {
        .enable_vbat_charger = false,
        .brownout_threshold = VBAT_BROWNOUT_DET_LVL,
        .interrupt_mask = VBAT_LL_DETECT_MASK,
    };
#endif

    vbat_hal_config(&vbat_cfg);

    ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(power_supply_periph_signal.irq, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED, (uint32_t)brownout_ll_intr_get_status_reg(), VBAT_LL_CHARGER_MASK | VBAT_LL_DETECT_MASK, &vbat_isr_handler, NULL, &vbat_intr), TAG, "Allocate vbat isr failed");

    return ESP_OK;
}
