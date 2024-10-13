/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <string.h>

#include "esp_system.h"
#include "esp_cpu.h"

#include "soc/soc_caps.h"

void __attribute__((noreturn)) abort(void)
{
#define ERR_STR1  "abort() was called at PC 0x"
#define ERR_STR2  " on core "

    _Static_assert(UINTPTR_MAX == 0xffffffff, "abort() assumes 32-bit addresses");
    _Static_assert(SOC_CPU_CORES_NUM < 10, "abort() assumes number of cores is 1 to 9");

    char addr_buf[9] = { 0 };
    char core_buf[2] = { 0 };

    char buf[sizeof(ERR_STR1) + sizeof(addr_buf) + sizeof(core_buf) + sizeof(ERR_STR2) + 1 /* null char */] = { 0 };

    itoa((uint32_t)(__builtin_return_address(0) - 3), addr_buf, 16);
    itoa(esp_cpu_get_core_id(), core_buf, 10);

    const char *str[] = { ERR_STR1, addr_buf, ERR_STR2, core_buf };

    char *dest = buf;

    for (size_t i = 0; i < sizeof(str) / sizeof(str[0]); i++) {
        strcat(dest, str[i]);
    }

    esp_system_abort(buf);
}
