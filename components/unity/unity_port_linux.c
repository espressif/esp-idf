/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
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
