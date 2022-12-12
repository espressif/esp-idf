/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include "unity.h"
#include "sdkconfig.h"

static struct timeval s_test_start, s_test_stop;

void unity_putc(int c)
{
    putc(c, stdout);
}

void unity_flush(void)
{
    fflush(stdout);
    fsync(fileno(stdout));
}

static void esp_unity_readline(char* dst, size_t len)
{
    /* Read line from console with support for echoing and backspaces */
    size_t write_index = 0;
    for (;;) {
        char c = 0;
        int result = fgetc(stdin);
        if (result == EOF) {
          continue;
        }
        c = (char) result;
        if (c == '\r' || c == '\n') {
            /* Add null terminator and return on newline */
            unity_putc('\n');
            dst[write_index] = '\0';
            return;
        } else if (c == '\b') {
            if (write_index > 0) {
                /* Delete previously entered character */
                write_index--;
                unity_putc('\b');
                unity_putc(' ');
                unity_putc('\b');
            }
        } else if (len > 0 && write_index < len - 1 && !iscntrl(c)) {
            /* Write a max of len - 1 characters to allow for null terminator */
            unity_putc(c);
            dst[write_index++] = c;
        }
    }
}

void unity_gets(char *dst, size_t len)
{
    esp_unity_readline(dst, len);
}

void unity_exec_time_start(void)
{
    gettimeofday(&s_test_start, NULL);
}

void unity_exec_time_stop(void)
{
    gettimeofday(&s_test_stop, NULL);
}

uint32_t unity_exec_time_get_ms(void)
{
    return (uint32_t) (((s_test_stop.tv_sec * 1000000ULL + s_test_stop.tv_usec) -
                        (s_test_start.tv_sec * 1000000ULL + s_test_start.tv_usec)) / 1000);
}
