/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "esp_private/startup_internal.h"
#include "esp_private/privilege_trap.h"
#include "esp_private/privilege_ecall.h"

ESP_SYSTEM_INIT_FN(init_privilege_user_traps, SECONDARY, BIT(0), 170)
{
    rv_user_trap_set_handlers(esp_privilege_ecall_handler);
    return ESP_OK;
}

void esp_privilege_init_include_func(void)
{
    // Hook to force the linker to include this file
}
