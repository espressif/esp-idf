/*
 * Test backported deletion behavior by creating tasks of various affinities and
 * check if the task memory is freed immediately under the correct conditions.
 *
 * The behavior of vTaskDelete() has been backported form FreeRTOS v9.0.0. This
 * results in the immediate freeing of task memory and the immediate execution
 * of deletion callbacks under the following conditions...
 * - When deleting a task that is not currently running on either core
 * - When deleting a task that is pinned to the same core (with respect to
 *   the core that calls vTaskDelete()
 *
 * If the two conditions are not met, freeing of task memory and execution of
 * deletion callbacks will still be carried out by the Idle Task.
 */
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

#include "unity.h"

#define NO_OF_TSKS  3
#define DELAY_TICKS 2
#define HEAP_CAPS   (MALLOC_CAP_INTERNAL|MALLOC_CAP_DEFAULT)


static void tsk_self_del(void *param)
{
    vTaskDelete(NULL);  //Deleting self means deleting currently running task
}

static void tsk_extern_del(void *param)
{
    vTaskDelay(portMAX_DELAY);  //Await external deletion
}

TEST_CASE("FreeRTOS Delete Tasks", "[freertos]")
{
/* -------------- Test vTaskDelete() on currently running tasks ----------------*/
    uint32_t before_count = uxTaskGetNumberOfTasks();
    uint32_t before_heap = heap_caps_get_free_size(HEAP_CAPS);
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        for(int j = 0; j < NO_OF_TSKS; j++){
            TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(tsk_self_del, "tsk_self", 1024, NULL, configMAX_PRIORITIES - 1, NULL, i));
        }
    }
    vTaskDelay(DELAY_TICKS);    //Minimal delay to see if Idle task cleans up all tasks awaiting deletion in a single tick
    TEST_ASSERT_EQUAL(before_count, uxTaskGetNumberOfTasks());
    TEST_ASSERT_EQUAL(before_heap, heap_caps_get_free_size(HEAP_CAPS));

/* ------------- Test vTaskDelete() on not currently running tasks ------------ */
    TaskHandle_t handles[NO_OF_TSKS];
    before_heap = heap_caps_get_free_size(HEAP_CAPS);
    //Create task pinned to the same core that will not run during task deletion
    for(int j = 0 ; j < NO_OF_TSKS; j++){
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(tsk_extern_del, "tsk_extern", 4096, NULL, configMAX_PRIORITIES - 1, &handles[j], xPortGetCoreID()));
    }
    TEST_ASSERT_NOT_EQUAL(before_heap, heap_caps_get_free_size(HEAP_CAPS));    //Check tasks have been created
    //Delete the tasks, memory should be freed immediately
    for(int j = 0; j < NO_OF_TSKS; j++){
        vTaskDelete(handles[j]);
    }
    TEST_ASSERT_EQUAL(before_heap, heap_caps_get_free_size(HEAP_CAPS));

}
