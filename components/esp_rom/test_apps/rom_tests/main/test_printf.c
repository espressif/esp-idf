/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include "sdkconfig.h"
#include "esp_rom_sys.h"
#include "unity.h"

static int my_printf(const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    int len = esp_rom_vprintf(fmt, list);
    va_end(list);
    return len;
}

TEST_CASE("Test esp_rom_vprintf", "[rom][vprintf]")
{
    int len = esp_rom_printf("esp_rom_printf function works %d %d %d\n", 1, 2, 3);
    TEST_ASSERT_EQUAL_INT(36, len);
    len = my_printf("esp_rom_vprintf function works %d %d %d\n", 1, 2, 3);
    TEST_ASSERT_EQUAL_INT(37, len);
}
