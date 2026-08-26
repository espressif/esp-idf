/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "esp_rom_sys.h"
#include "unity.h"
#include "test_utils.h"
#include <stdio.h>
#include <inttypes.h>

#if !CONFIG_FREERTOS_SMP
/*
Scheduler suspension behavior has changed in SMP FreeRTOS, thus these tests are disabled for SMP FreeRTOS.
See IDF-5201
*/

/* ---------------------------------------------------------------------------------------------------------------------
FreeRTOS tick hooks during scheduler suspension

Purpose:
    - Test that esp_system FreeRTOS tick hooks are called even with scheduler suspension

Procedure:
    Each core gets tested in the role of core X
        - Create suspend_task pinned to core X which will register a tick hook on core X and suspend scheduler on core X
        - Register tick hook on core X
        - suspend_task suspends scheduling on core X for Y milliseconds and then resumes scheduling
        - Delay suspend_task for Y milliseconds more after scheduler resumption
        - De-register the tick hook
        - Verify the tick hook callback count

Expected:
        - The tick hook is called for Y * 2 times
--------------------------------------------------------------------------------------------------------------------- */

#define TEST_DELAY_MS 200
static volatile uint32_t tick_hook_count[portNUM_PROCESSORS];

static void IRAM_ATTR tick_hook(void)
{
    tick_hook_count[portGET_CORE_ID()] += portTICK_PERIOD_MS;
}

static void suspend_task(void *arg)
{
    TaskHandle_t main_task_hdl = (TaskHandle_t)arg;

    /* Fetch the current core ID */
    BaseType_t xCoreID = portGET_CORE_ID();

    /* Warm up the cache by running the scheduler suspension/resumption once.
     * This reduces the execution time variance caused by cache misses during
     * the actual test on targets like the esp32p4.
     */
    vTaskSuspendAll();
    xTaskResumeAll();
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Register tick hook */
    tick_hook_count[xCoreID] = 0;
    esp_register_freertos_tick_hook_for_cpu(tick_hook, xCoreID);

    /* Read the tick hook count before suspending */
    uint32_t initial_count = tick_hook_count[xCoreID];

    /* Suspend scheduler */
    vTaskSuspendAll();

    /* Suspend for TEST_DELAY_MS milliseconds */
    esp_rom_delay_us(TEST_DELAY_MS * 1000);

    /* Resume scheduler */
    xTaskResumeAll();

    /* Delay for a further TEST_DELAY_MS milliseconds after scheduler resumption */
    vTaskDelay(pdMS_TO_TICKS(TEST_DELAY_MS));

    /* Read the final tick hook count */
    uint32_t final_count = tick_hook_count[xCoreID];

    /* De-register tick hook */
    esp_deregister_freertos_tick_hook_for_cpu(tick_hook, xCoreID);

    /* Verify that the tick hook callback count equals the scheduler suspension time + the delay time.
     * We add a variation of 2 ticks to account for delays encountered during test setup and teardown.
     */
    printf("Core%d initial_count = %"PRIu32"\n", xCoreID, initial_count);
    printf("Core%d final_count = %"PRIu32"\n", xCoreID, final_count);
    TEST_ASSERT_INT_WITHIN(portTICK_PERIOD_MS * 2, TEST_DELAY_MS * 2, final_count - initial_count);

    /* Signal main task of test completion */
    xTaskNotifyGive(main_task_hdl);

    vTaskSuspend(NULL);
}

TEST_CASE("FreeRTOS tick hooks during scheduler suspension", "[esp_system]")
{
    /* Run test for each core */
    TaskHandle_t suspend_task_handle[portNUM_PROCESSORS];
    for (int x = 0; x < portNUM_PROCESSORS; x++) {
        xTaskCreatePinnedToCore(&suspend_task, "suspend_task", 8192, (void *)xTaskGetCurrentTaskHandle(), UNITY_FREERTOS_PRIORITY, &suspend_task_handle[x], x);

        /* Wait for test completion */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* Cleanup */
        vTaskSuspend(suspend_task_handle[x]);
        vTaskDelay(10);
        vTaskDelete(suspend_task_handle[x]);
    }
}

#endif /* !CONFIG_FREERTOS_SMP */
