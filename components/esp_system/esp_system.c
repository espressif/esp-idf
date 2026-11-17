/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_private/esp_sys_event_internal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if !CONFIG_IDF_TARGET_LINUX
#include "soc/soc_caps.h"
#endif
#if SOC_RTC_WDT_SUPPORTED && (CONFIG_ESP_SYSTEM_SHUTDOWN_HANDLER_TIMEOUT_MS > 0)
#include "hal/wdt_hal.h"
#include "soc/rtc.h"
#endif

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

#if SOC_RTC_WDT_SUPPORTED && (CONFIG_ESP_SYSTEM_SHUTDOWN_HANDLER_TIMEOUT_MS > 0)
static void arm_shutdown_handler_wdt(void)
{
    wdt_hal_context_t rtc_wdt_ctx;
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
    uint64_t stage_timeout_ticks = ((uint64_t)CONFIG_ESP_SYSTEM_SHUTDOWN_HANDLER_TIMEOUT_MS *
                                    rtc_clk_slow_freq_get_hz()) / 1000ULL;
    if (stage_timeout_ticks > UINT32_MAX) {
        stage_timeout_ticks = UINT32_MAX;
    }
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    // Stage 0 is a system reset, stage 1 an RTC reset.
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, (uint32_t)stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE1, (uint32_t)stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_set_flashboot_en(&rtc_wdt_ctx, true);
    wdt_hal_enable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
}
#endif // SOC_RTC_WDT_SUPPORTED && CONFIG_ESP_SYSTEM_SHUTDOWN_HANDLER_TIMEOUT_MS

void esp_restart(void)
{
#if SOC_RTC_WDT_SUPPORTED && (CONFIG_ESP_SYSTEM_SHUTDOWN_HANDLER_TIMEOUT_MS > 0)
    // Before calling shutdown handlers, arm the RTC watchdog. esp_restart_noos()
    // arms it only later, for 1 s, which is too late if a handler never returns.
    arm_shutdown_handler_wdt();
#endif

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
