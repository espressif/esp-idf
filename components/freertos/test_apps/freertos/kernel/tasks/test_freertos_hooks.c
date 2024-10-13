/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

/*
Note: We disable this test when using the FreeRTOS SMP kernel as the port will already provide
a definition for vApplicationTickHook(). Thus this test cannot be run.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/semphr.h>
#include "unity.h"
#include "test_utils.h"

#if !CONFIG_FREERTOS_SMP
/*
Test FreeRTOS idle hook. Only compiled in if FreeRTOS idle hooks are enabled.
*/
#if CONFIG_FREERTOS_USE_IDLE_HOOK

static volatile unsigned idle_count;

void vApplicationIdleHook(void)
{
    idle_count++;
}

TEST_CASE("FreeRTOS idle hook", "[freertos]")
{
    idle_count = 0;
    vTaskDelay(10);
    TEST_ASSERT_NOT_EQUAL(0, idle_count); // The legacy idle hook should be called at least once
}

#endif // CONFIG_FREERTOS_USE_IDLE_HOOK

/*
Test the FreeRTOS tick hook. Only compiled in if FreeRTOS tick hooks are enabled.
*/
#if CONFIG_FREERTOS_USE_TICK_HOOK

static volatile unsigned tick_count;

void vApplicationTickHook(void)
{
    tick_count++;
}

TEST_CASE("FreeRTOS tick hook", "[freertos]")
{
    unsigned before = xTaskGetTickCount();
    const unsigned SLEEP_FOR = 20;
    tick_count = before;
    vTaskDelay(SLEEP_FOR);
    TEST_ASSERT_UINT32_WITHIN_MESSAGE(3 * CONFIG_FREERTOS_NUMBER_OF_CORES, before + SLEEP_FOR * CONFIG_FREERTOS_NUMBER_OF_CORES, tick_count,
                                      "The FreeRTOS tick hook should have been called approx 1 time per tick per CPU");
}

#endif // CONFIG_FREERTOS_USE_TICK_HOOK
#endif // !CONFIG_FREERTOS_SMP

#if CONFIG_FREERTOS_TASK_PRE_DELETION_HOOK

static volatile void *deleted_tcb;

static void taskDeletesItself(void *ignored)
{
    vTaskDelete(NULL);
}

void vTaskPreDeletionHook(void *pxTCB)
{
    deleted_tcb = pxTCB;
}

TEST_CASE("static task cleanup hook is called based on config", "[freertos]")
{
    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        printf("Creating task CPU %d\n", i);
        TaskHandle_t new_task = NULL;
        deleted_tcb = NULL;
        xTaskCreatePinnedToCore(taskDeletesItself, "delete", 2048, NULL, UNITY_FREERTOS_PRIORITY, &new_task, i);
        vTaskDelay(5);
        TEST_ASSERT_EQUAL_PTR(deleted_tcb, new_task); // TCB & TaskHandle are the same in FreeRTOS
    }
}

#endif // CONFIG_FREERTOS_TASK_PRE_DELETION_HOOK
