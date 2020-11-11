/*
 Test of FreeRTOS "legacy" hook functions, and delete hook

 Only compiled in if the relevant options are enabled.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/semphr.h>
#include "driver/timer.h"
#include "unity.h"
#include "test_utils.h"

#if CONFIG_FREERTOS_LEGACY_HOOKS

static volatile unsigned idle_count;

void vApplicationIdleHook(void)
{
    idle_count++;
}

TEST_CASE("legacy idle hook is correctly called based on config", "[freertos]")
{
    idle_count = 0;
    vTaskDelay(10);
    TEST_ASSERT_NOT_EQUAL(0, idle_count); // The legacy idle hook should be called at least once
}

static volatile unsigned tick_count;

void IRAM_ATTR vApplicationTickHook(void)
{
    tick_count++;
}

TEST_CASE("legacy tick hook is called based on config", "[freertos]")
{
    unsigned before = xTaskGetTickCount();
    const unsigned SLEEP_FOR = 20;
    tick_count = before;
    vTaskDelay(SLEEP_FOR);
    TEST_ASSERT_UINT32_WITHIN_MESSAGE(3 * portNUM_PROCESSORS, before + SLEEP_FOR * portNUM_PROCESSORS, tick_count,
                                      "The legacy tick hook should have been called approx 1 time per tick per CPU");
}

#endif // CONFIG_FREERTOS_LEGACY_HOOKS

#if CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP

static volatile void *deleted_tcb;

static void taskDeletesItself(void *ignored)
{
    vTaskDelete(NULL);
}

void vPortCleanUpTCB(void *pxTCB)
{
    deleted_tcb = pxTCB;
}

TEST_CASE("static task cleanup hook is called based on config", "[freertos]")
{
    for(int i = 0; i < portNUM_PROCESSORS; i++) {
        printf("Creating task CPU %d\n", i);
        TaskHandle_t new_task = NULL;
        deleted_tcb = NULL;
        xTaskCreatePinnedToCore(taskDeletesItself, "delete", 2048, NULL, UNITY_FREERTOS_PRIORITY, &new_task, i);
        vTaskDelay(5);
        TEST_ASSERT_EQUAL_PTR(deleted_tcb, new_task); // TCB & TaskHandle are the same in FreeRTOS
    }
}

#endif // CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP
