/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * Declare the symbol pointing to the former implementation of esp_restart function
 */
extern void __real_esp_restart(void);


/**
 * Redefine esp_restart function to print a message before actually restarting
 */
void __wrap_esp_restart(void)
{
    printf("Restarting in progress...\n");
    /* Call the former implementation to actually restart the board */
    __real_esp_restart();
}

void app_main(void)
{
    printf("Restarting in 5 seconds...\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    esp_restart();
}
