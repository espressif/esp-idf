/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include "sdkconfig.h"
#include "sys/lock.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "hal/adc_types.h"

#include "esp_private/adc_lock.h"
#include "esp_private/adc2_wifi.h"


static const char *TAG = "adc_lock";

static _lock_t adc1_lock;
static _lock_t adc2_lock;

esp_err_t adc_lock_acquire(adc_unit_t unit_mask)
{
    if (unit_mask & ADC_UNIT_1) {
        _lock_acquire(&adc1_lock);
    }

    if (unit_mask & ADC_UNIT_2) {
        _lock_acquire(&adc2_lock);
    }

    return ESP_OK;
}

esp_err_t adc_lock_release(adc_unit_t unit_mask)
{
    if (unit_mask & ADC_UNIT_2) {
        ESP_RETURN_ON_FALSE(((uint32_t *)adc2_lock != NULL), ESP_ERR_INVALID_STATE, TAG, "adc2 lock release without acquiring");
        _lock_release(&adc2_lock);
    }

    if (unit_mask & ADC_UNIT_1) {
        ESP_RETURN_ON_FALSE(((uint32_t *)adc1_lock != NULL), ESP_ERR_INVALID_STATE, TAG, "adc1 lock release without acquiring");
        _lock_release(&adc1_lock);
    }

    return ESP_OK;
}

esp_err_t adc_lock_try_acquire(adc_unit_t unit_mask)
{
    if (unit_mask & ADC_UNIT_1) {
        if (_lock_try_acquire(&adc1_lock) == -1) {
            return ESP_ERR_TIMEOUT;
        }
    }

    if (unit_mask & ADC_UNIT_2) {
        if (_lock_try_acquire(&adc2_lock) == -1) {
            return ESP_ERR_TIMEOUT;
        }
    }

    return ESP_OK;
}

esp_err_t adc2_wifi_acquire(void)
{
#if CONFIG_IDF_TARGET_ESP32
    /* Wi-Fi module will use adc2. Use locks to avoid conflicts. */
    adc_lock_acquire(ADC_UNIT_2);
    ESP_LOGD(TAG, "Wi-Fi takes adc2 lock.");
#endif

    return ESP_OK;
}

esp_err_t adc2_wifi_release(void)
{
#if CONFIG_IDF_TARGET_ESP32
    return adc_lock_release(ADC_UNIT_2);
#endif

    return ESP_OK;
}
