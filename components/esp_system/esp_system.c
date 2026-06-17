/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_private/esp_sys_event_internal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Adapter for esp_register_shutdown_handler(): the public API validates the
// handler before storing it in user_arg, and SHUTDOWN does not pass a ctx.
static esp_err_t legacy_shutdown_trampoline(void *user_arg, void *ctx)
{
    (void)ctx;
    shutdown_handler_t fn = (shutdown_handler_t)user_arg;
    fn();
    return ESP_OK;
}

esp_err_t esp_register_shutdown_handler(shutdown_handler_t handler)
{
    if (handler == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return esp_sys_event_register(ESP_SYS_EVENT_SHUTDOWN,
                                  legacy_shutdown_trampoline,
                                  (void *)handler);
}

esp_err_t esp_unregister_shutdown_handler(shutdown_handler_t handler)
{
    if (handler == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = esp_sys_event_unregister(ESP_SYS_EVENT_SHUTDOWN,
                                             legacy_shutdown_trampoline,
                                             (void *)handler);
    if (err == ESP_ERR_NOT_FOUND) {
        return ESP_ERR_INVALID_STATE;
    }
    return err;
}

void esp_restart(void)
{
    esp_sys_event_trigger(ESP_SYS_EVENT_SHUTDOWN, NULL);

#if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionDisable(NULL);
#else
    // Disable scheduler on this core.
    vTaskSuspendAll();
#endif // #if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )

    esp_restart_noos();
}
