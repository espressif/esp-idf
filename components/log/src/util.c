/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_rom_sys.h"

int esp_log_util_cvt(unsigned long long val, long radix, int pad, const char *digits, char *buf)
{
    return esp_rom_cvt(val, radix, pad, digits, buf);
}

int esp_log_util_cvt_hex(unsigned long long val, int pad, char *buf)
{
    return esp_rom_cvt(val, 16, pad, "0123456789abcdef", buf);
}

int esp_log_util_cvt_dec(unsigned long long val, int pad, char *buf)
{
    return esp_rom_cvt(val, 10, pad, "0123456789", buf);
}
