/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* For U-mode programs that umode_elf_add_program() builds */

#pragma once

#include <stdint.h>
#include "esp_privilege_abi.h"

/* Entry function of the program, called by _start in umode_start.S. Its return value ends the run. */
int user_main(uint32_t arg0, uint32_t arg1);

/* End the run with a value, as if user_main() had returned it */
__attribute__((noreturn)) static inline void umode_exit(int value)
{
    register int a0 __asm__("a0") = value;
    register int a7 __asm__("a7") = ESP_PRIVILEGE_ECALL_EXIT;
    __asm__ volatile("ecall" :: "r"(a0), "r"(a7) : "memory");
    __builtin_unreachable();
}
