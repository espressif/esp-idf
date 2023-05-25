/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

int var_1;
int var_2;

void foo(void)
{
    var_2++;
}

void app_main(void)
{
    printf("tested app is runnig.\n");
    while(1) {
        var_1++;
        if (var_1 % 10 == 0) {
            foo();
        }
    }
}
