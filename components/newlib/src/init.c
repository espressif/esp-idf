/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp_newlib.h"
#include "esp_private/startup_internal.h"
#include "esp_bit_defs.h"

ESP_SYSTEM_INIT_FN(init_libc, CORE, BIT(0), 102)
{
    esp_libc_init();
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(init_libc_stdio, CORE, BIT(0), 115)
{
#if CONFIG_VFS_SUPPORT_IO
    esp_libc_init_global_stdio("/dev/console");
#else
    esp_libc_init_global_stdio();
#endif
    return ESP_OK;
}

// Hook to force the linker to include this file
void esp_libc_init_funcs(void)
{
}
