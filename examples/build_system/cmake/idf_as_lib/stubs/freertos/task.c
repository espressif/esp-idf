/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <unistd.h>
#include "freertos/task.h"

void vTaskDelay( const uint32_t xTicksToDelay )
{
    sleep(xTicksToDelay);
}
