/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_pm.h"
#include "esp_private/esp_clk.h"

#include "sdkconfig.h"

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_PM_ENABLE

#define MHZ (1000000)

static SemaphoreHandle_t test_sem;
static volatile bool test_timeout_occurred = false;

static void producer_task(void *arg)
{
    while (1) {
        xSemaphoreGive(test_sem);
        vTaskDelay(pdMS_TO_TICKS(5));  // Give semaphore every 5ms
    }
}

static void consumer_task(void *arg)
{
    while (1) {
        // Try to take semaphore with 1 second timeout
        // Should never timeout as producer is giving the semaphore every 5ms
        if (xSemaphoreTake(test_sem, pdMS_TO_TICKS(1000)) == pdFALSE) {
            test_timeout_occurred = true;  // Mark that timeout occurred
            break;
        }
    }

    vTaskSuspend(NULL);
}

/**
 * Test case to verify timing accuracy during tickless idle mode
 *
 * This test verifies that FreeRTOS maintains accurate timing when the system
 * enters and exits tickless idle mode, particularly when combined with scheduler
 * suspension operations.
 *
 * Test Setup:
 * - Configures the system to use tickless idle mode
 * - Creates a producer-consumer pair using a semaphore:
 *   * Producer gives semaphore every 5ms
 *   * Consumer takes semaphore with 1-second timeout
 * - Periodically suspend and resume the scheduler
 *
 * Expected Behavior:
 * - The consumer should never timeout as the producer gives the semaphore
 *   every 5ms, well within the 1-second timeout window
 * - Any timing inaccuracies during tickless idle would manifest as
 *   unexpected semaphore timeouts
 * - The test passes if no timeouts occur.
 */
TEST_CASE("Test semaphore timeout during tickless idle", "[freertos]")
{
    // Configure tickless idle
    esp_pm_config_t pm_config = {
        .max_freq_mhz = esp_clk_cpu_freq() / MHZ,
        .min_freq_mhz = esp_clk_cpu_freq() / MHZ,
        .light_sleep_enable = true,
    };
    TEST_ESP_OK(esp_pm_configure(&pm_config));

    // Create test semaphore
    test_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(test_sem);

    test_timeout_occurred = false;

    // Create producer and consumer tasks
    TaskHandle_t producer_handle, consumer_handle;
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(producer_task, "producer", 2048, NULL, 1, &producer_handle));
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(consumer_task, "consumer", 2048, NULL, 2, &consumer_handle));

    // Run the test for a while to ensure tickless idle mode has
    // enough time to enter and exit multiple times
    vTaskDelay(pdMS_TO_TICKS(10000));

    // Suspend scheduler briefly
    vTaskSuspendAll();

    // Resume scheduler. This unrolls pended tick events.
    // This should not cause any unexpected timeouts if
    // tickless idle mode is working correctly.
    xTaskResumeAll();

    // Verify no timeout occurred
    TEST_ASSERT_FALSE(test_timeout_occurred);

    // Disable power management before exit to prevent memory leaks
    esp_pm_config_t default_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .light_sleep_enable = false,
    };
    esp_pm_configure(&default_config);

    // Cleanup
    vTaskDelete(producer_handle);
    vTaskDelete(consumer_handle);
    vSemaphoreDelete(test_sem);
}

#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_PM_ENABLE */
