/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stddef.h>
#include "inttypes_ext.h"

void app_main(void)
{
    size_t size = 47;
    printf("size is: %" PRIuSIZE "\n", size); // test IDF's PRIuSIZE
}
