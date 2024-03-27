/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include "ulp_lp_core_uart.h"

#if !CONFIG_ULP_ROM_PRINT_ENABLE

#define LP_UART_PORT_NUM LP_UART_NUM_0
#define BINARY_SUPPORT 1

#define is_digit(c) ((c >= '0') && (c <= '9'))

static void lp_uart_send_char(char c)
{
    int tx_len = 0;
    int loop_cnt = 0;

    /* Write one byte to LP UART. Break after few iterations if we are stuck for any reason. */
    while (tx_len != 1 && loop_cnt < 1000) {
        tx_len = lp_core_uart_tx_chars(LP_UART_PORT_NUM, (const void *)&c, 1);
        loop_cnt++;
    }
}

// Ported over ROM function _cvt()
static int lp_core_cvt(unsigned long long val, char *buf, long radix, char *digits)
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

// Ported over ROM function ets_vprintf()
static int lp_core_ets_vprintf(void (*putc)(char c), const char *fmt, va_list ap)
{
#ifdef BINARY_SUPPORT
    char buf[sizeof(long long) * 8];
#else
    char buf[32];
#endif
    char c, sign, *cp = buf;
    int left_prec, right_prec, zero_fill, pad, pad_on_right, islong, islonglong;
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
                    islong = 0;
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
                } else {
                    val = (long long)va_arg(ap, int);
                }

                if ((c == 'd') || (c == 'D')) {
                    if (val < 0) {
                        sign = '-';
                        val = -val;
                    }
                } else {
                    if (islonglong) {
                        ;
                    } else if (islong) {
                        val &= ((long long)1 << (sizeof(long) * 8)) - 1;
                    } else {
                        val &= ((long long)1 << (sizeof(int) * 8)) - 1;
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
                left_prec = sizeof(unsigned long) * 2;
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
                    length = lp_core_cvt(val, buf, 10, "0123456789");
                    break;
                case 'p':
                case 'x':
                    length = lp_core_cvt(val, buf, 16, "0123456789abcdef");
                    break;
                case 'X':
                    length = lp_core_cvt(val, buf, 16, "0123456789ABCDEF");
                    break;
                }
                cp = buf;
                break;
            case 's':
            case 'S':
                cp = va_arg(ap, char *);
                if (cp == NULL) {
                    cp = "<null>";
                }
                length = 0;
                while (cp[length] != '\0') {
                    length++;
                }
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
                    if (islonglong) {
                        length = sizeof(long long) * 8;
                    } else if (islong) {
                        length = sizeof(long) * 8;
                    } else {
                        length = sizeof(int) * 8;
                    }
                }
                for (int i = 0; i < length - 1; i++) {
                    buf[i] = ((val & ((long long)1 << i)) ? '1' : '.');
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

int lp_core_printf(const char* format, ...)
{
    /* Create a variable argument list */
    va_list ap;
    va_start(ap, format);

    /* Pass the input string and the argument list to ets_vprintf() */
    int ret = lp_core_ets_vprintf(lp_uart_send_char, format, ap);

    va_end(ap);

    return ret;
}

#endif /* !CONFIG_ULP_ROM_PRINT_ENABLE */
