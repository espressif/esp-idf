/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include "esp_rom_sys.h"

static void call_linux_putc(char c);

static void (*s_esp_rom_putc)(char c) = call_linux_putc;

static void call_linux_putc(char c) {
    putc(c, stdout);
}

#define is_digit(c) ((c >= '0') && (c <= '9'))

static int _cvt(unsigned long long val, char *buf, long radix, const char *digits)
{
#ifdef SUPPORT_LITTLE_RADIX
    char temp[64];
#else
    char temp[32];
#endif
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
    return (length);
}

static int esp_rom_vprintf(void (*putc)(char c), const char *fmt, va_list ap)
{
#ifdef BINARY_SUPPORT
    char buf[sizeof(long long)*8];
    int i;
#else
    char buf[32];
#endif
    char c, sign;
    const char *cp = buf;
    int left_prec, right_prec, zero_fill, pad, pad_on_right,
        islong, islonglong;
    long long val = 0;
    int res = 0, length = 0;

    while ((c = *fmt++) != '\0') {
        if (c == '%') {
            c = *fmt++;
            left_prec = right_prec = pad_on_right = islong = islonglong = 0;
            if (c == '-') {
                c = *fmt++;
                pad_on_right++;
            }
            if (c == '0') {
                zero_fill = true;
                c = *fmt++;
            } else {
                zero_fill = false;
            }
            while (is_digit(c)) {
                left_prec = (left_prec * 10) + (c - '0');
                c = *fmt++;
            }
            if (c == '.') {
                c = *fmt++;
                zero_fill++;
                while (is_digit(c)) {
                    right_prec = (right_prec * 10) + (c - '0');
                    c = *fmt++;
                }
            } else {
                right_prec = left_prec;
            }
            sign = '\0';
            if (c == 'l') {
                c = *fmt++;
                islong = 1;
                if (c == 'l') {
                    c = *fmt++;
                    islonglong = 1;
                }
            }
            switch (c) {
            case 'p':
                islong = 1;
            case 'd':
            case 'D':
            case 'x':
            case 'X':
            case 'u':
            case 'U':
#ifdef BINARY_SUPPORT
            case 'b':
            case 'B':
#endif
                if (islonglong) {
                    val = va_arg(ap, long long);
                } else if (islong) {
                    val = (long long)va_arg(ap, long);
                } else{
                    val = (long long)va_arg(ap, int);
                }
                if ((c == 'd') || (c == 'D')) {
                    if (val < 0) {
                        sign = '-';
                        val = -val;
                    }
                } else {
                    if (islong) {
                        val &= (((long long)1) << (sizeof(long) * 8)) - 1;
                    } else{
                        val &= (((long long)1) << (sizeof(int) * 8)) - 1;
                    }
                }
                break;
            default:
                break;
            }

            switch (c) {
            case 'p':
                (*putc)('0');
                (*putc)('x');
                zero_fill = true;
                left_prec = sizeof(unsigned long)*2;
            case 'd':
            case 'D':
            case 'u':
            case 'U':
            case 'x':
            case 'X':
                switch (c) {
                case 'd':
                case 'D':
                case 'u':
                case 'U':
                    length = _cvt(val, buf, 10, "0123456789");
                    break;
                case 'p':
                case 'x':
                    length = _cvt(val, buf, 16, "0123456789abcdef");
                    break;
                case 'X':
                    length = _cvt(val, buf, 16, "0123456789ABCDEF");
                    break;
                }
                cp = buf;
                break;
            case 's':
            case 'S':
                cp = va_arg(ap, char *);
                if (cp == NULL)  {
                    cp = "<null>";
                }
                length = 0;
                while (cp[length] != '\0') length++;
                break;
            case 'c':
            case 'C':
                c = va_arg(ap, int /*char*/);
                (*putc)(c);
                res++;
                continue;
#ifdef BINARY_SUPPORT
            case 'b':
            case 'B':
                length = left_prec;
                if (left_prec == 0) {
                    if (islonglong)
                        length = sizeof(long long)*8;
                    else if (islong)
                        length = sizeof(long)*8;
                    else
                        length = sizeof(int)*8;
                }
                for (i = 0;  i < length-1;  i++) {
                    buf[i] = ((val & ((long long)1<<i)) ? '1' : '.');
                }
                cp = buf;
                break;
#endif
            case '%':
                (*putc)('%');
                break;
            default:
                (*putc)('%');
                (*putc)(c);
                res += 2;
            }
            pad = left_prec - length;
            if (sign != '\0') {
                pad--;
            }
            if (zero_fill) {
                c = '0';
                if (sign != '\0') {
                    (*putc)(sign);
                    res++;
                    sign = '\0';
                }
            } else {
                c = ' ';
            }
            if (!pad_on_right) {
                while (pad-- > 0) {
                    (*putc)(c);
                    res++;
                }
            }
            if (sign != '\0') {
                (*putc)(sign);
                res++;
            }
            while (length-- > 0) {
                c = *cp++;
                (*putc)(c);
                res++;
            }
            if (pad_on_right) {
                while (pad-- > 0) {
                    (*putc)(' ');
                    res++;
                }
            }
        } else {
            (*putc)(c);
            res++;
        }
    }
    return (res);
}

int esp_rom_printf(const char *fmt, ...)
{

    va_list list;
    va_start(list, fmt);
    int result = esp_rom_vprintf(s_esp_rom_putc, fmt, list);
    va_end(list);
    return result;
}

void esp_rom_delay_us(uint32_t us)
{
    int sleep_result = usleep(us);
    assert(sleep_result == 0);
    (void)sleep_result; // Prevents compiler from optimizing out usleep() due to unused result. Also prevents warning.
}

void esp_rom_install_channel_putc(int channel, void (*putc)(char c))
{
    if (putc != NULL) {
        s_esp_rom_putc = putc;
    }
}

void esp_rom_install_uart_printf(void)
{
    // Since this is the linux implementation, we don't set any "UART" putc function, but the one which delegates to
    // the Linux libc version of putc.
    s_esp_rom_putc = call_linux_putc;
}

soc_reset_reason_t esp_rom_get_reset_reason(int cpu_no)
{
    return RESET_REASON_CHIP_POWER_ON;
}
