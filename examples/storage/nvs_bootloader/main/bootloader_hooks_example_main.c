/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>

void app_main(void)
{
    /**
     * Nothing special is done here, everything interesting in this example
     * is done in the custom bootloader code, located in:
     * `bootloader_components/nvs_bootloader_test/nvs_bootloader_test.c`
     */
    printf("User application is loaded and running.\n");
}
