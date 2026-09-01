/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_ON_STARTUP

#include "esp_ota_ops.h"
#include "esp_log.h"
#include "esp_private/esp_sys_event_app_init.h"

ESP_LOG_ATTR_TAG(TAG, "ota_auto_confirm");

ESP_PRE_APP_MAIN_HANDLER_REGISTER(esp_ota_auto_confirm, 200)
{
    (void)user_arg;
    (void)ctx;

    esp_ota_img_states_t ota_state;
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (esp_ota_get_state_partition(running, &ota_state) != ESP_OK) {
        return ESP_OK;
    }

    if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
        ESP_LOGI(TAG, "Auto-confirming OTA application as valid");

    } else if (ota_state == ESP_OTA_IMG_NEW) {
        /* The bootloader does not support rollback: it did not transition
         * the OTA state from ESP_OTA_IMG_NEW to ESP_OTA_IMG_PENDING_VERIFY.
         * Mark as VALID to keep the state consistent. */
        ESP_LOGW(TAG, "Bootloader is not capable of rollback");

    } else {
        return ESP_OK;
    }

    esp_err_t err = esp_ota_mark_app_valid_cancel_rollback();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mark app valid: %s", esp_err_to_name(err));
    }
    return ESP_OK;
}

void esp_ota_auto_confirm_include(void) {}

#endif /* CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_ON_STARTUP */
