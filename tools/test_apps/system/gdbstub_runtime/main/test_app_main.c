/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

int var_1;
int var_2;
int do_panic;


void foo(void)
{
    var_2+=2;
    var_2--;
}

void test_xesppie_loops(void);
void app_main(void)
{
    printf("tested app is running.\n");

    vTaskDelay(5000 / portTICK_PERIOD_MS);

#if SOC_CPU_HAS_HWLOOP
    test_xesppie_loops();
#endif

    while(1) {
        var_1++;
        if (var_1 % 10 == 0) {
label_1:
            foo();
        }
label_3:
        if (do_panic) {
            char *t = NULL;
label_5:
            *t = 'X';
        }
    }
}
