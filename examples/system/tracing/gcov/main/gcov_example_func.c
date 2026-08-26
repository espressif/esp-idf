/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>


void blink_dummy_func(void)
{
    static int i;
    printf("blink_dummy_func: Counter = %d\n", i++);
}
