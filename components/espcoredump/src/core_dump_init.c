/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#if CONFIG_ESP_COREDUMP_ENABLE

#include "esp_private/startup_internal.h"
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

#endif /* CONFIG_ESP_COREDUMP_ENABLE */
