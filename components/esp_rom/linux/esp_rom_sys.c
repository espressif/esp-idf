/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "esp_rom_sys.h"

void esp_rom_output_putc(char c)
{
    putc(c, stdout);
}

void esp_rom_delay_us(uint32_t us)
{
    struct timespec wait_time = {
        .tv_sec = us / 1000000,
        .tv_nsec = us % 1000000 * 1000,
    };
    /*
        If nanosleep() is interrupted by a signal handler, nanosleep()
        returns -1, sets errno to EINTR, and writes the remaining time.
        The remaining time is used to call nanosleep() again and complete the specified pause.
    */
    while (nanosleep(&wait_time, &wait_time) == -1 && errno == EINTR) { };
}

soc_reset_reason_t esp_rom_get_reset_reason(int cpu_no)
{
    return RESET_REASON_CHIP_POWER_ON;
}

void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    esp_rom_printf("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
                   failedexpr, file, line,
                   func ? ", function: " : "", func ? func : "");
    while (1) {}
}
