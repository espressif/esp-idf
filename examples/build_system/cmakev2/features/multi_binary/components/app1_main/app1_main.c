/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "component1.h"
#include "component2.h"

void app_main(void)
{
    component1_print_hello();
    component2_print_hello();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
