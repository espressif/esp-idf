/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Entry point of a U-mode program that umode_elf_add_program() builds. The domain starts it with the two
 * arguments of esp_privilege_umode_domain_run() in a0 and a1, which pass through to user_main(). Its return value
 * in a0 ends the run through the exit ECALL, so the program never returns to the caller's return address. */

#include "esp_privilege_abi.h"

    .section .text._start, "ax"
    .global _start
    .type _start, @function
_start:
    call user_main
    li a7, ESP_PRIVILEGE_ECALL_EXIT
    ecall
    /* The exit ECALL does not return */
1:  j 1b
    .size _start, . - _start
