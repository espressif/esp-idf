#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "esp_ipc.h"
#include "test_utils.h"

static void mutex_release_task(void* arg)
{
    SemaphoreHandle_t mutex = (SemaphoreHandle_t) arg;
    xSemaphoreGive(mutex);
    TEST_FAIL_MESSAGE("should not be reached");
}

TEST_CASE("mutex released not by owner causes an assert", "[freertos][reset=abort,SW_CPU_RESET]")
{
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    xSemaphoreTake(mutex, portMAX_DELAY);
    xTaskCreate(&mutex_release_task, "mutex_release", 2048, mutex, UNITY_FREERTOS_PRIORITY + 1, NULL);
    vTaskDelay(1);
}
