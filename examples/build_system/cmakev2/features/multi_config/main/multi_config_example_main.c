/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "func.h"

void app_main(void)
{
    printf("This app is built for running on: " CONFIG_EXAMPLE_PRODUCT_NAME "\n");

    /* This will call func() either from func_dev.c or func_prod.c, depending on
     * the build configuration.
     */
    func();

    printf("See README.md for building and running other app configurations.\n");
}
