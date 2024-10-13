/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Test for multicore FreeRTOS. This test spins up threads, fiddles with queues etc.
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "test_utils.h"

volatile static int done;
volatile static int error;

static void tskTestRand(void *pvParameters)
{
    int l;
    srand(0x1234);
    vTaskDelay((int)pvParameters / portTICK_PERIOD_MS);
    l = rand();
    printf("Rand1: %d\n", l);
    if (l != 869320854) {
        error++;
    }
    vTaskDelay((int)pvParameters / portTICK_PERIOD_MS);
    l = rand();
    printf("Rand2: %d\n", l);
    if (l != 1148737841) {
        error++;
    }
    done++;
    vTaskDelete(NULL);
}

// TODO: split this thing into separate orthogonal tests
TEST_CASE("Test for per-task non-reentrant tasks", "[freertos]")
{
    done = 0;
    error = 0;
    const uint32_t stack_size = 3072;
    xTaskCreatePinnedToCore(tskTestRand, "tsk1", stack_size, (void *)100, 3, NULL, 0);
    xTaskCreatePinnedToCore(tskTestRand, "tsk2", stack_size, (void *)200, 3, NULL, 0);
    xTaskCreatePinnedToCore(tskTestRand, "tsk3", stack_size, (void *)300, 3, NULL, CONFIG_FREERTOS_NUMBER_OF_CORES - 1);
    xTaskCreatePinnedToCore(tskTestRand, "tsk4", stack_size, (void *)400, 3, NULL, 0);
    while (done != 4) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    TEST_ASSERT(error == 0);
}
