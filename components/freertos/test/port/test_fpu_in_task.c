/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if CONFIG_FREERTOS_SMP && SOC_CPU_HAS_FPU

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "xtensa/xtensa_api.h"
#include "esp_intr_alloc.h"
#include "unity.h"
#include "test_utils.h"

/*
Note: We need to declare the float here to prevent compiler optimizing float
operations into non-float instructions.
*/
static volatile float test_float;

/*
Test coprocessor (CP) usage in task

Purpose:
    - Test that CP can be used in a task (e.g., the FPU)
    - Test that unpinned tasks are pinned when the CP is used
    - Test that CP is properly saved in restored
    - Test that CP context is properly cleaned up when task is deleted
*/

#define COPROC_TASK_TEST_ITER   10

static void unpinned_task(void *arg)
{
    // Test that task gets pinned to the current core when it uses the coprocessor
    vTaskPreemptionDisable(NULL);   // Disable preemption to make current core ID doesn't change
    BaseType_t xCoreID = xPortGetCoreID();
    TEST_ASSERT_EQUAL(tskNO_AFFINITY, vTaskCoreAffinityGet(NULL));
    test_float = 1.1f;
    test_float *= 2.0f;
    TEST_ASSERT_EQUAL(2.2f, test_float);
    TEST_ASSERT_EQUAL(1 << xCoreID, vTaskCoreAffinityGet(NULL));
    vTaskPreemptionEnable(NULL);

    // Delay to trigger a solicited context switch
    vTaskDelay(1);
    // Test that CP context was saved and restored properly
    test_float *= 2.0f;
    TEST_ASSERT_EQUAL(4.4f, test_float);

    // Wait to be deleted
    xTaskNotifyGive((TaskHandle_t)arg);
    vTaskSuspend(NULL);
}

TEST_CASE("Test coproc usage in task", "[freertos]")
{
    TaskHandle_t unity_task_handle = xTaskGetCurrentTaskHandle();
    for (int i = 0; i < COPROC_TASK_TEST_ITER; i++) {
        TaskHandle_t unpinned_task_handle;
        xTaskCreate(unpinned_task, "unpin", 2048, (void *)unity_task_handle, UNITY_FREERTOS_PRIORITY + 1, &unpinned_task_handle);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelete(unpinned_task_handle);
        vTaskDelay(10); // Short delay to allow task memory to be freed
    }
}

#if CONFIG_FREERTOS_FPU_IN_ISR

/*
Test coprocessor (CP) usage in ISR

Purpose:
    - Test that CP can be used in an ISR
    - Test that an interrupted unpinned task will not be pinned by the ISR using the CP
*/

#define SW_ISR_LEVEL_1  7

static void software_isr(void *arg)
{
    (void) arg;
    xt_set_intclear(1 << SW_ISR_LEVEL_1);
    test_float *= 2;
}

static void unpinned_task_isr(void *arg)
{
    vTaskPreemptionDisable(NULL);   // Disable preemption to make current core ID doesn't change
    TEST_ASSERT_EQUAL(tskNO_AFFINITY, vTaskCoreAffinityGet(NULL));
    // Allocate ISR on the current core
    intr_handle_t handle;
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1, &software_isr, NULL, &handle);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);
    // Trigger the ISR
    xt_set_intset(1 << SW_ISR_LEVEL_1);
    // Test that task affinity hasn't changed
    TEST_ASSERT_EQUAL(tskNO_AFFINITY, vTaskCoreAffinityGet(NULL));
    // Free the ISR
    esp_intr_free(handle);
    // Wait to be deleted
    vTaskPreemptionEnable(NULL);
    xTaskNotifyGive((TaskHandle_t)arg);
    vTaskSuspend(NULL);
}

TEST_CASE("Test coproc usage in ISR", "[freertos]")
{
    TaskHandle_t unity_task_handle = xTaskGetCurrentTaskHandle();
    for (int i = 0; i < COPROC_TASK_TEST_ITER; i++) {
        // Initialize the test float
        test_float = 1.1f;
        // Create an unpinned task to trigger the ISR
        TaskHandle_t unpinned_task_handle;
        xTaskCreate(unpinned_task_isr, "unpin", 2048, (void *)unity_task_handle, UNITY_FREERTOS_PRIORITY + 1, &unpinned_task_handle);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // Check that the test float value
        TEST_ASSERT_EQUAL(2.2f, test_float);
        // Delete the unpinned task. Short delay to allow task memory to be freed
        vTaskDelete(unpinned_task_handle);
        vTaskDelay(10);
    }
}

#endif // CONFIG_FREERTOS_FPU_IN_ISR

#endif // CONFIG_FREERTOS_SMP && SOC_CPU_HAS_FPU
