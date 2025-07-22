/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_log.h"
#include "esp_check.h"
#include "esp_ieee802154.h"
#include "esp_pm.h"
static esp_pm_lock_handle_t s_pm_lock = NULL;
static const char* TAG = "esp openthread sleep";
static bool s_ot_sleep = false;

esp_err_t esp_openthread_sleep_init(void)
{
    esp_err_t err = ESP_OK;

    // Here we use APB_MAX because modem requires MODEM_REQUIRED_MIN_APB_CLK_FREQ.
    // No need for CPU_MAX to reduce current consumption during Rx window.
    err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "ot_sleep", &s_pm_lock);
    if (err == ESP_OK) {
        esp_pm_lock_acquire(s_pm_lock);
        ESP_LOGI(TAG, "Enable OpenThread light sleep, the wake up source is ESP timer");
    } else {
        if (s_pm_lock != NULL) {
            esp_pm_lock_delete(s_pm_lock);
            s_pm_lock = NULL;
        }
    }
    return err;
}

void esp_openthread_sleep_process(void)
{
    if (s_ot_sleep == false && esp_ieee802154_get_state() == ESP_IEEE802154_RADIO_SLEEP) {
        esp_pm_lock_release(s_pm_lock);
        s_ot_sleep = true;
    } else if (s_ot_sleep && esp_ieee802154_get_state() != ESP_IEEE802154_RADIO_SLEEP) {
        esp_pm_lock_acquire(s_pm_lock);
        s_ot_sleep = false;
    }
}
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE
