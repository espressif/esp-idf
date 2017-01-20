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
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Delete Tasks", "[freertos][ignore]")
{
    xTaskCreatePinnedToCore(task_delete_self, "tsk_self_a", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    xTaskCreatePinnedToCore(task_delete_self, "tsk_self_a", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    printf("Done?\n");
}
