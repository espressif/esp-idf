/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "esp_privilege_abi.h"
#include "privilege_code.h"

/* Return address of the U-mode code that the fixture runs. Ends the run with the exit request. */
#define EXIT_STR(x) #x
#define EXIT_XSTR(x) EXIT_STR(x)

IRAM_ATTR __attribute__((naked)) void privilege_exit_trampoline(void)
{
    __asm__ volatile("li a7, " EXIT_XSTR(ESP_PRIVILEGE_ECALL_EXIT) "\n"
                     "ecall\n"
                     "1: j 1b");
}

IRAM_ATTR int privilege_arith(uint32_t arg0, uint32_t arg1)
{
    privilege_shared_t *shared = (privilege_shared_t *)arg1;
    shared->out = arg0 * 3;
    return arg0 + 1;
}

IRAM_ATTR int privilege_get_sp(uint32_t arg0, uint32_t arg1)
{
    int sp;
    __asm__ volatile("mv %0, sp" : "=r"(sp));
    return sp;
}

IRAM_ATTR int privilege_get_gp(uint32_t arg0, uint32_t arg1)
{
    int gp;
    __asm__ volatile("mv %0, gp" : "=r"(gp));
    return gp;
}

IRAM_ATTR int privilege_count_until_stop(uint32_t arg0, uint32_t arg1)
{
    privilege_shared_t *shared = (privilege_shared_t *)arg1;
    while (!shared->stop) {
        shared->counter++;
    }
    return shared->counter;
}
