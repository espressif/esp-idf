/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "privilege_code.h"

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
