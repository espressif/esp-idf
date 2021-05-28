/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is a STUB FILE used when compiling ESP-IDF to run tests on the host system.
 * The source file used normally for ESP-IDF has the same name but is located elsewhere.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stddef.h>

#include "esp_system.h"

uint32_t esp_random(void)
{
    return (uint32_t)rand();
}
