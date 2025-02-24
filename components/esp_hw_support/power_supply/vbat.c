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
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/startup_internal.h"
#include "esp_private/rtc_ctrl.h"
#include "esp_sleep.h"
#include "esp_vbat.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "soc/clk_tree_defs.h"
#include "soc/power_supply_periph.h"

#if CONFIG_ESP_VBAT_INIT_AUTO
#if CONFIG_ESP_VBAT_ISR_CACHE_SAFE
#define VBAT_INTR_ALLOC_FLAG    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED)
#else
#define VBAT_INTR_ALLOC_FLAG    (ESP_INTR_FLAG_SHARED)
#endif // CONFIG_ESP_VBAT_ISR_CACHE_SAFE

#define VBAT_BROWNOUT_DET_LVL       CONFIG_ESP_VBAT_BROWNOUT_DET_LVL

#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
#define VBAT_CHARGE_DET_LVL_LOW         CONFIG_ESP_VBAT_DET_LVL_LOW
#define VBAT_CHARGE_DET_LVL_HIGH        CONFIG_ESP_VBAT_DET_LVL_HIGH
#define VBAT_CHARGER_RESISTOR_VALUE     CONFIG_ESP_VBAT_CHARGER_CIRCUIT_RESISTOR_VAL
#endif

#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY || CONFIG_ESP_VBAT_WAKEUP_CHIP_ON_VBAT_BROWNOUT
#define VBAT_CHARGER_FILTER_TIME_US             50
#define VBAT_CHARGER_HYSTERESIS_THRESHOLD_LOW   100
#define VBAT_CHARGER_HYSTERESIS_THRESHOLD_HIGH  (VBAT_CHARGER_HYSTERESIS_THRESHOLD_LOW + (VBAT_CHARGER_FILTER_TIME_US * SOC_CLK_RC_FAST_FREQ_APPROX) / MHZ)
#endif

#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
#if (VBAT_CHARGER_RESISTOR_VALUE < 1000 || VBAT_CHARGER_RESISTOR_VALUE > 4500 || VBAT_CHARGER_RESISTOR_VALUE % 500 != 0)
#error "vbat charger resistor (ESP_VBAT_CHARGER_CIRCUIT_RESISTOR_VAL) must be between 1000 and 4500 ohms and must be a multiple of 500."
#endif

#if (VBAT_BROWNOUT_DET_LVL >= VBAT_CHARGE_DET_LVL_LOW)
#error "vbat charger low threshold is equal or lower than vbat brownout threshold, please put vbat brownout threshold lower than vbat charger low threshold"
#endif
#endif

static const char TAG[] = "VBAT";
#endif

static struct {
    esp_vbat_state_t state;
#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
    SemaphoreHandle_t charging_done_sem;
#endif
} vbat_status = {
    .state = ESP_VBAT_STATE_NORMAL
};

#if CONFIG_ESP_VBAT_INIT_AUTO
#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
static StaticSemaphore_t charging_done_semphr_buffer;
#endif

IRAM_ATTR static void vbat_isr_handler(void *arg)
{
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    uint32_t int_status;
    vbat_ll_get_interrupt_status(&int_status);

#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
    if (int_status & VBAT_LL_CHARGER_UPVOLTAGE_INTR) {
        ESP_DRAM_LOGW(TAG, "RTC battery voltage reaches high limit , stop charging...");
        vbat_status.state = ESP_VBAT_STATE_NORMAL;
        vbat_ll_start_battery_charge(false);
        vbat_ll_enable_intr_mask(VBAT_LL_CHARGER_UNDERVOLTAGE_INTR | VBAT_LL_BROWNOUT_INTR, true);
        vbat_ll_enable_intr_mask(VBAT_LL_CHARGER_UPVOLTAGE_INTR, false);
        vbat_ll_clear_intr_mask(VBAT_LL_CHARGER_UPVOLTAGE_INTR);
        esp_sleep_enable_vbat_under_volt_wakeup();
        esp_sleep_sub_mode_config(ESP_SLEEP_VBAT_POWER_DEEPSLEEP_MODE, true);
        xSemaphoreGiveFromISR(vbat_status.charging_done_sem, &HPTaskAwoken);
    }
#endif

    if (int_status & (VBAT_LL_CHARGER_UNDERVOLTAGE_INTR)) {
#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
        vbat_status.state = ESP_VBAT_STATE_CHARGING;
        ESP_DRAM_LOGW(TAG, "RTC battery voltage low, start charging...");
        vbat_ll_start_battery_charge(true);
        vbat_ll_enable_intr_mask(VBAT_LL_CHARGER_UPVOLTAGE_INTR, true);
        vbat_ll_enable_intr_mask(VBAT_LL_CHARGER_UNDERVOLTAGE_INTR, false);
#endif
        vbat_ll_clear_intr_mask(VBAT_LL_CHARGER_UNDERVOLTAGE_INTR);
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_VBAT_UNDER_VOLT);
        esp_sleep_sub_mode_config(ESP_SLEEP_VBAT_POWER_DEEPSLEEP_MODE, false);
    }

    if (int_status & VBAT_LL_BROWNOUT_INTR) {
        // TODO: A callback may needed here to inform an under voltage event.
        vbat_status.state = ESP_VBAT_STATE_LOWBATTERY;
        esp_sleep_sub_mode_force_disable(ESP_SLEEP_VBAT_POWER_DEEPSLEEP_MODE);
        ESP_DRAM_LOGW(TAG, "RTC battery voltage low, please change battery...");
        vbat_ll_clear_intr_mask(VBAT_LL_BROWNOUT_INTR);
    }

    if (HPTaskAwoken) {
        portYIELD_FROM_ISR();
    }
}

#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
esp_err_t esp_vbat_wait_battery_charge_done(TickType_t checking_period)
{
    BaseType_t ret;
    if (!xPortInIsrContext()) {
        ret = xSemaphoreTake(vbat_status.charging_done_sem, checking_period);
    } else {
        return ESP_FAIL;
    }
    return (ret == pdPASS) ? ESP_OK : ESP_ERR_TIMEOUT;
}
#endif
#endif

esp_vbat_state_t esp_vbat_get_battery_state(void)
{
    return vbat_status.state;
}

#if CONFIG_ESP_VBAT_INIT_AUTO
esp_err_t esp_vbat_init(void)
{
    intr_handle_t vbat_intr;
#if CONFIG_ESP_VBAT_USE_RECHARGEABLE_BATTERY
    vbat_status.charging_done_sem = xSemaphoreCreateBinaryStatic(&charging_done_semphr_buffer);
    vbat_hal_config_t vbat_cfg = {
        .enable_vbat_charger = true,
        .charger_resistor_value = VBAT_CHARGER_RESISTOR_VALUE,
        .low_threshold = VBAT_CHARGE_DET_LVL_LOW,
        .high_threshold = VBAT_CHARGE_DET_LVL_HIGH,
        .brownout_threshold = VBAT_BROWNOUT_DET_LVL,
        .undervoltage_filter_time = VBAT_CHARGER_HYSTERESIS_THRESHOLD_HIGH,
        .upvoltage_filter_time = VBAT_CHARGER_HYSTERESIS_THRESHOLD_LOW,
        .interrupt_mask = (VBAT_LL_CHARGER_MASK | VBAT_LL_DETECT_MASK),
    };
#else
    vbat_hal_config_t vbat_cfg = {
#if CONFIG_ESP_VBAT_WAKEUP_CHIP_ON_VBAT_BROWNOUT
        .enable_vbat_charger = true,
        .low_threshold = VBAT_BROWNOUT_DET_LVL,
        .undervoltage_filter_time = VBAT_CHARGER_HYSTERESIS_THRESHOLD_HIGH,
        .upvoltage_filter_time = VBAT_CHARGER_HYSTERESIS_THRESHOLD_LOW,
#endif
        .brownout_threshold = VBAT_BROWNOUT_DET_LVL,
        .interrupt_mask = VBAT_LL_DETECT_MASK,
    };
#endif
    vbat_hal_config(&vbat_cfg);
    ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(power_supply_periph_signal.irq, VBAT_INTR_ALLOC_FLAG, (uint32_t)brownout_ll_intr_get_status_reg(), vbat_cfg.interrupt_mask, &vbat_isr_handler, NULL, &vbat_intr), TAG, "Allocate vbat isr failed");
    esp_sleep_sub_mode_force_disable(ESP_SLEEP_VBAT_POWER_DEEPSLEEP_MODE);
    esp_sleep_sub_mode_config(ESP_SLEEP_VBAT_POWER_DEEPSLEEP_MODE, true);
    esp_sleep_enable_vbat_under_volt_wakeup();
    return ESP_OK;
}
#endif
