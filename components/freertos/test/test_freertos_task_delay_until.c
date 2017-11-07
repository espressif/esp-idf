/*
 Test for FreeRTOS vTaskDelayUntil() function by comparing the delay period of
 the function to comparing it to ref clock.
*/

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOSConfig.h"
#include "unity.h"
#include "test_utils.h"

#define TSK_PRIORITY    (UNITY_FREERTOS_PRIORITY + 1)

#define NO_OF_CYCLES    5
#define NO_OF_TASKS_PER_CORE 2
#define TICKS_TO_DELAY 10
#define TICK_RATE   configTICK_RATE_HZ
#define TICK_PERIOD_US (1000000/TICK_RATE)
#define IDEAL_DELAY_PERIOD_MS  ((1000*TICKS_TO_DELAY)/TICK_RATE)
#define IDEAL_DELAY_PERIOD_US   (IDEAL_DELAY_PERIOD_MS*1000)

#define TICKS_TO_MS(x)  (((x)*1000)/TICK_RATE)
#define REF_TO_ROUND_MS(x)    (((x)+500)/1000)

static SemaphoreHandle_t task_delete_semphr;

static void delaying_task(void* arg)
{
    uint64_t ref_prev, ref_current;
    TickType_t last_wake_time;
    TickType_t ticks_before_delay;

    vTaskDelay(1);  //Delay until next tick to synchronize operations to tick boundaries

    last_wake_time = xTaskGetTickCount();
    ticks_before_delay = last_wake_time;
    ref_prev = ref_clock_get();

    for(int i = 0; i < NO_OF_CYCLES; i++){
        //Delay of TICKS_TO_DELAY
        vTaskDelayUntil(&last_wake_time, TICKS_TO_DELAY);
        //Get current ref clock
        TEST_ASSERT_EQUAL(IDEAL_DELAY_PERIOD_MS, TICKS_TO_MS(xTaskGetTickCount() - ticks_before_delay));
        ref_current = ref_clock_get();
        TEST_ASSERT_UINT32_WITHIN(TICK_PERIOD_US, IDEAL_DELAY_PERIOD_US, (uint32_t)(ref_current - ref_prev));
        ref_prev = ref_current;
        ticks_before_delay = last_wake_time;
    }

    //Delete Task after prescribed number of cycles
    xSemaphoreGive(task_delete_semphr);
    vTaskDelete(NULL);
}

TEST_CASE("Test vTaskDelayUntil", "[freertos]")
{
    task_delete_semphr = xQueueCreateCountingSemaphore(NO_OF_TASKS_PER_CORE*portNUM_PROCESSORS, 0);
    ref_clock_init();

    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xTaskCreatePinnedToCore(delaying_task, "delay_pinned", 1024, NULL, TSK_PRIORITY, NULL, i);
        xTaskCreatePinnedToCore(delaying_task, "delay_no_aff", 1024, NULL, TSK_PRIORITY, NULL, tskNO_AFFINITY);
    }

    for(int i = 0; i < NO_OF_TASKS_PER_CORE*portNUM_PROCESSORS; i++){
        xSemaphoreTake(task_delete_semphr, portMAX_DELAY);
    }
    //Cleanup
    vSemaphoreDelete(task_delete_semphr);
    ref_clock_deinit();
}
