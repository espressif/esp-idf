/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_private/privilege_trap.h"

/* Read by the exception entry code in vectors.S */
rv_user_ecall_handler_t rv_user_trap_ecall_handler;

void rv_user_trap_set_handlers(rv_user_ecall_handler_t ecall)
{
    rv_user_trap_ecall_handler = ecall;
}
