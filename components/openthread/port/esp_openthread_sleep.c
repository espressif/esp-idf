/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_check.h"
#include "openthread/instance.h"
#include "openthread/thread.h"
#include "esp_openthread.h"
#include "esp_ieee802154.h"

#if CONFIG_IEEE802154_SLEEP_ENABLE
#include "esp_pm.h"
static esp_pm_lock_handle_t s_pm_lock = NULL;
static const char* TAG = "esp openthread sleep";
static bool s_ot_sleep = false;

esp_err_t esp_openthread_sleep_init(void)
{
    esp_err_t err = ESP_OK;

    err = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "ieee802154", &s_pm_lock);
    if (err != ESP_OK) {
        goto error;
    }
    esp_pm_lock_acquire(s_pm_lock);

    ESP_LOGI(TAG, "Enable ieee802154 light sleep, the wake up source is ESP timer");

    return err;

error:
    /*lock should release first and then delete*/
    if (s_pm_lock != NULL) {
        esp_pm_lock_release(s_pm_lock);
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
    return err;
}

void esp_openthread_sleep_process(void)
{
    otInstance *instance = esp_openthread_get_instance();
    otLinkModeConfig linkMode = otThreadGetLinkMode(instance);
    bool s_ot_sed_ssed = false;
    if (linkMode.mRxOnWhenIdle == false  && linkMode.mDeviceType == false && linkMode.mNetworkData == false) {
        s_ot_sed_ssed = true;
    }

    if (esp_ieee802154_get_state() == ESP_IEEE802154_RADIO_SLEEP  && s_ot_sed_ssed) {
        esp_ieee802154_sleep_cb();
        esp_pm_lock_release(s_pm_lock);
        s_ot_sleep = true;
    }
}

void esp_openthread_wakeup_process(void)
{
    if (s_ot_sleep) {
        esp_pm_lock_acquire(s_pm_lock);
        esp_ieee802154_wakeup_cb();
        s_ot_sleep = false;
    }
}
#endif // CONFIG_IEEE802154_SLEEP_ENABLE
