/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/memprot.h"
#else
#include "esp_memprot.h"
#endif
#endif

#define SHUTDOWN_HANDLERS_NO 5

static shutdown_handler_t shutdown_handlers[SHUTDOWN_HANDLERS_NO];

esp_err_t esp_register_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            return ESP_ERR_INVALID_STATE;
        } else if (shutdown_handlers[i] == NULL) {
            shutdown_handlers[i] = handler;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            shutdown_handlers[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}


void esp_restart(void)
{
    for (int i = SHUTDOWN_HANDLERS_NO - 1; i >= 0; i--) {
        if (shutdown_handlers[i]) {
            shutdown_handlers[i]();
        }
    }

#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionDisable(NULL);
#else
    // Disable scheduler on this core.
    vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP

    bool digital_reset_needed = false;
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#if CONFIG_IDF_TARGET_ESP32S2
    if (esp_memprot_is_intr_ena_any() || esp_memprot_is_locked_any()) {
        digital_reset_needed = true;
    }
#else
    bool is_on = false;
    if (esp_mprot_is_intr_ena_any(&is_on) != ESP_OK || is_on) {
        digital_reset_needed = true;
    } else if (esp_mprot_is_conf_locked_any(&is_on) != ESP_OK || is_on) {
        digital_reset_needed = true;
    }
#endif
#endif
    if (digital_reset_needed) {
        esp_restart_noos_dig();
    }
    esp_restart_noos();
}
