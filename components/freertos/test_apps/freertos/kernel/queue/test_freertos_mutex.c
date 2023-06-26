/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_system.h"

/* If assertions aren't set to fail this code still crashes, but not with an abort... */
#if CONFIG_FREERTOS_CHECK_MUTEX_GIVEN_BY_OWNER && !CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE

static void mutex_release_task(void* arg)
{
    SemaphoreHandle_t mutex = (SemaphoreHandle_t) arg;
    xSemaphoreGive(mutex);
    TEST_FAIL_MESSAGE("should not be reached");
}

static void check_reset_reason_panic(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());
}

static void do_mutex_release_assert(void)
{
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    xSemaphoreTake(mutex, portMAX_DELAY);
    xTaskCreate(&mutex_release_task, "mutex_release", 2048, mutex, UNITY_FREERTOS_PRIORITY + 1, NULL);
    vTaskDelay(1);
}

TEST_CASE_MULTIPLE_STAGES("mutex released not by owner causes an assert", "[freertos][reset=assert,SW_CPU_RESET]",
                          do_mutex_release_assert,
                          check_reset_reason_panic)

#endif
