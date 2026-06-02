/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern void unrelated_function(void);

void app_main(void)
{
#if CONFIG_TEST_CONFIGDEP_OPTION
    printf("CONFIGDEP_OPTION: ENABLED\n");
#else
    printf("CONFIGDEP_OPTION: DISABLED\n");
#endif
    unrelated_function();
    vTaskDelay(pdMS_TO_TICKS(1000));
}
