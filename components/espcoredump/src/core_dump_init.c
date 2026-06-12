/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include "esp_private/startup_internal.h"
#include "esp_private/esp_sys_event_system_init.h"
#include "esp_private/esp_sys_event_panic.h"
#include "esp_private/panic_internal.h"
#include "esp_core_dump.h"

// Hook to force the linker to include this file
void esp_system_include_coredump_init(void)
{
}

ESP_SYSTEM_INIT_FN(init_coredump, SECONDARY, BIT(0), 130)
{
    esp_core_dump_init();
    return ESP_OK;
}

// Panic event handler - writes core dump on panic
ESP_PANIC_HANDLER_REGISTER(esp_coredump_panic, 200)
{
    (void)user_arg;
    esp_panic_ctx_t *panic_ctx = (esp_panic_ctx_t *)ctx;

    static bool s_dumping_core = false;
    if (s_dumping_core) {
        panic_print_str("Re-entered core dump! Exception happened during core dump!\r\n");
        return ESP_ERR_INVALID_STATE;
    }
    s_dumping_core = true;
    esp_core_dump_write((panic_info_t *)panic_ctx->info);
    s_dumping_core = false;
    return ESP_OK;
}
