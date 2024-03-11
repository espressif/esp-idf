/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

int esp_log_util_cvt(unsigned long long val, char *buf, long radix, int pad, const char *digits)
{
#ifdef SUPPORT_LITTLE_RADIX
    char temp[64];
#else
    char temp[32];
#endif
    char *buf_or = buf;
    char *cp = temp;
    int length = 0;

    if (val == 0) {
        /* Special case */
        *cp++ = '0';
    } else {
        while (val) {
            *cp++ = digits[val % radix];
            val /= radix;
        }
    }
    while (cp != temp) {
        *buf++ = *--cp;
        length++;
    }
    *buf = '\0';
    if (length < pad) {
        pad = pad - length;
        int i = 0;
        while (pad-- > 0) {
            temp[i] = buf_or[i];
            buf_or[i] = '0';
            i++;
            length++;
        }
        for (int k = 0; k < i; k++) {
            buf_or[i + k] = temp[k];
        }
    }
    return (length);
}

int esp_log_util_cvt_hex(unsigned long long val, int pad, char *buf)
{
    return esp_log_util_cvt(val, buf, 16, pad, "0123456789abcdef");
}

int esp_log_util_cvt_dec(unsigned long long val, int pad, char *buf)
{
    return esp_log_util_cvt(val, buf, 10, pad, "0123456789");
}
