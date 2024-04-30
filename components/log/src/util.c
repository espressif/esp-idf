/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

int esp_log_util_cvt(unsigned long long val, long radix, int pad, const char *digits, char *buf)
{
    char *orig_buf = buf;
    int length = 0;

    // val = 123
    do {
        *buf++ = digits[val % radix];
        val /= radix;
        length++;
    } while (val);
    //        3   2   1
    // buf = [0] [1] [2] [3]

    // length = 3, pad = 6
    while (pad > 0 && pad > length) {
        *buf++ = '0';
        length++;
    }
    *buf = '\0';
    // length = 6
    //        3   2   1   0   0   0  \0
    // buf = [0] [1] [2] [3] [4] [5] [6]

    --buf;
    // reverse the order of characters
    //             3   2   1   0   0   0  \0
    //            [0] [1] [2] [3] [4] [5] [6]
    // orig_buf -- ^                   ^ ----- buf
    while (orig_buf < buf) {
        char first_char = *orig_buf;
        char last_char = *buf;
        *buf-- = first_char;
        *orig_buf++ = last_char;
    }
    //        0   0   0   1   2   3  \0
    // buf = [0] [1] [2] [3] [4] [5] [6]

    return (length);
}

int esp_log_util_cvt_hex(unsigned long long val, int pad, char *buf)
{
    return esp_log_util_cvt(val, 16, pad, "0123456789abcdef", buf);
}

int esp_log_util_cvt_dec(unsigned long long val, int pad, char *buf)
{
    return esp_log_util_cvt(val, 10, pad, "0123456789", buf);
}
