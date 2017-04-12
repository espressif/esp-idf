#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "unity.h"

static void task_delete_self(void *param)
{
    printf("Task %p running on core %d. Deleting shortly...\n", xTaskGetCurrentTaskHandle(), xPortGetCoreID());
    vTaskDelay(5);
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Delete Tasks", "[freertos]")
{
    uint32_t before_count = uxTaskGetNumberOfTasks();

    xTaskCreatePinnedToCore(task_delete_self, "tsk_self_a", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    xTaskCreatePinnedToCore(task_delete_self, "tsk_self_a", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    TEST_ASSERT_EQUAL(before_count + 2, uxTaskGetNumberOfTasks());
    vTaskDelay(200 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(before_count, uxTaskGetNumberOfTasks());
}
