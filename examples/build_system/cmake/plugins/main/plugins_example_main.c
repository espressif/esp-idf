/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "plugins_api.h"

void app_main(void)
{
    example_plugins_list();
    example_plugins_greet("World");
}
