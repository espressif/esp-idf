#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"

#define TSK_PRIORITY    (UNITY_FREERTOS_PRIORITY + 1)

static TaskHandle_t blocked_task_handle;
static TaskHandle_t suspended_task_handle;
static SemaphoreHandle_t done_sem;

void test_task_get_state(void* arg)
{
    //Current task should return eRunning
    TEST_ASSERT(eTaskGetState(xTaskGetCurrentTaskHandle()) == eRunning);
    //Idle task of current core should return eReady
    TEST_ASSERT(eTaskGetState(xTaskGetIdleTaskHandle()) == eReady);
    //Blocked Task should return eBlocked
    TEST_ASSERT(eTaskGetState(blocked_task_handle) == eBlocked);
    //Suspended Task should return eSuspended
    TEST_ASSERT(eTaskGetState(suspended_task_handle) == eSuspended);

    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

void blocked_task(void *arg)
{
    while(1){
        vTaskDelay(portMAX_DELAY);
    }
}

void suspended_task(void *arg)
{
    while(1){
        vTaskSuspend(NULL);
    }
}

TEST_CASE("Test eTaskGetState", "[freertos]")
{
    done_sem = xQueueCreateCountingSemaphore(portNUM_PROCESSORS, 0);
    //Create blocked and suspended task
    xTaskCreatePinnedToCore(blocked_task, "Blocked task", 1024, NULL, TSK_PRIORITY, &blocked_task_handle, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(suspended_task, "Suspended task", 1024, NULL, TSK_PRIORITY, &suspended_task_handle, tskNO_AFFINITY);
    //Create testing task
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xTaskCreatePinnedToCore(test_task_get_state, "Test task", 1024, NULL, TSK_PRIORITY, NULL, i);
    }

    //Wait until done
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xSemaphoreTake(done_sem, portMAX_DELAY);
    }
    //Clean up blocked and suspended tasks
    vTaskDelete(blocked_task_handle);
    blocked_task_handle = NULL;
    vTaskDelete(suspended_task_handle);
    suspended_task_handle = NULL;
    vSemaphoreDelete(done_sem);

    vTaskDelay(10);     //Give time for idle to actually delete the tasks
}
