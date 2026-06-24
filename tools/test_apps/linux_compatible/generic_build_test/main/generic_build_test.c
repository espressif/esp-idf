/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stddef.h>
#include "inttypes_ext.h"

#include "esp_pthread.h"

void app_main(void)
{
    size_t size = 47;
    printf("size is: %" PRIuSIZE "\n", size); // test IDF's PRIuSIZE

    esp_pthread_cfg_t pthread_config = esp_pthread_get_default_config();
    (void)pthread_config;
}
