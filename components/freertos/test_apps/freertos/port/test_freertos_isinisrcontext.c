/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 See if xPortInIsrContext works
*/

#include <esp_types.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "esp_intr_alloc.h"
#include "esp_rom_sys.h"
#include "esp_freertos_hooks.h"

#if CONFIG_FREERTOS_CORETIMER_0

static volatile int in_int_context, int_handled;

static void testint(void)
{
    esp_rom_printf("INT!\n");
    if (xPortInIsrContext()) {
        in_int_context++;
    }
    int_handled++;
}

static void testthread(void *arg)
{
    in_int_context = 0;
    int_handled = 0;
    TEST_ASSERT(!xPortInIsrContext());
    esp_err_t err = esp_register_freertos_tick_hook_for_cpu(testint, xPortGetCoreID());
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(int_handled);
    TEST_ASSERT(in_int_context);
    esp_deregister_freertos_tick_hook_for_cpu(testint, xPortGetCoreID());
    vTaskDelete(NULL);
}

TEST_CASE("xPortInIsrContext test", "[freertos]")
{
    xTaskCreatePinnedToCore(testthread, "tst", 4096, NULL, 3, NULL, 0);
    vTaskDelay(150 / portTICK_PERIOD_MS);
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    xTaskCreatePinnedToCore(testthread, "tst", 4096, NULL, 3, NULL, 1);
    vTaskDelay(150 / portTICK_PERIOD_MS);
#endif
}

#endif
