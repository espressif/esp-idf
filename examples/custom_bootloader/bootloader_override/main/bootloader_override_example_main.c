/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>

void app_main(void)
{
    /**
     * Nothing special is done here, everything interesting in this example
     * is done in the custom bootloader code, located in:
     * `bootloader_components/main/bootloader_start.c`
     */
    printf("Application started!\n");
}
