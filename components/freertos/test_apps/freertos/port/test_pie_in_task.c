/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <math.h>
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"

/* PIE instructions set is currently only supported in GCC compiler */
#if SOC_CPU_HAS_PIE

/**
 * @brief Performs the signed sum of two 4-word vectors using the PIE.
 *
 * @param a First vector
 * @param b Second vector
 * @param dst Destination to store the sum
 */
void pie_vector_signed_add(const int32_t a[4], const int32_t b[4], int32_t dst[4]);

/* ------------------------------------------------------------------------------------------------------------------ */

typedef struct {
    int32_t cst;
    TaskHandle_t main;
    SemaphoreHandle_t sem;
} pie_params_t;

/*
Test PIE usage from a task context

Purpose:
    - Test that the PIE can be used from a task context
    - Test that PIE context is properly saved and restored
    - Test that PIE context is cleaned up on task deletion by running multiple iterations
Procedure:
    - Create TEST_PINNED_NUM_TASKS tasks pinned to each core
    - Start each task
    - Each task updates a float variable and then blocks (to allow other tasks to run thus forcing the a PIE context
      save and restore).
    - Delete each task
    - Repeat test for TEST_PINNED_NUM_ITERS iterations
Expected:
    - Correct float value calculated by each task
    - Each task cleans up its PIE context on deletion
*/

#define TEST_PINNED_NUM_TASKS       3
#define TEST_PINNED_NUM_ITERS       5

static void pinned_task(void *arg)
{
    pie_params_t *param = (pie_params_t*) arg;
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    int32_t constant = 42 * param->cst;
    int32_t a[4] = { constant, constant, constant, constant };
    int32_t b[4] = { 10, 20, 30, 40 };
    int32_t dst[4] = { 0 };

    pie_vector_signed_add(a, b, dst);

    for (int i = 0; i < sizeof(a) / sizeof(uint32_t); i++) {
        TEST_ASSERT_EQUAL(dst[i], a[i] + b[i]);
    }

    // Indicate done and wait to be deleted
    xSemaphoreGive((SemaphoreHandle_t)param->sem);
    vTaskSuspend(NULL);
}

TEST_CASE("PIE: Usage in task", "[freertos]")
{
    SemaphoreHandle_t done_sem = xSemaphoreCreateCounting(CONFIG_FREERTOS_NUMBER_OF_CORES * TEST_PINNED_NUM_TASKS, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, done_sem);

    for (int iter = 0; iter < TEST_PINNED_NUM_ITERS; iter++) {
        TaskHandle_t task_handles[CONFIG_FREERTOS_NUMBER_OF_CORES][TEST_PINNED_NUM_TASKS];
        pie_params_t params[CONFIG_FREERTOS_NUMBER_OF_CORES][TEST_PINNED_NUM_TASKS];

        // Create test tasks for each core
        for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
            for (int j = 0; j < TEST_PINNED_NUM_TASKS; j++) {
                params[i][j] = (pie_params_t) {
                    .cst = i + j + 1,
                    .sem = done_sem,
                };
                TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(pinned_task, "task", 4096, (void *) &params[i][j], UNITY_FREERTOS_PRIORITY + 1, &task_handles[i][j], i));
            }
        }

        // Start the created tasks
        for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
            for (int j = 0; j < TEST_PINNED_NUM_TASKS; j++) {
                xTaskNotifyGive(task_handles[i][j]);
            }
        }

        // Wait for the tasks to complete
        for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES * TEST_PINNED_NUM_TASKS; i++) {
            xSemaphoreTake(done_sem, portMAX_DELAY);
        }

        // Delete the tasks
        for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
            for (int j = 0; j < TEST_PINNED_NUM_TASKS; j++) {
                vTaskDelete(task_handles[i][j]);
            }
        }

        vTaskDelay(10); // Short delay to allow idle task to be free task memory and FPU contexts
    }

    vSemaphoreDelete(done_sem);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*
Test PIE usage will pin an unpinned task

Purpose:
    - Test that unpinned tasks are automatically pinned to the current core on the task's first use of the PIE
    - Test that PIE context is cleaned up on task deletion by running multiple iterations
Procedure:
    - Create an unpinned task
    - Task disables scheduling/preemption to ensure that it does not switch cores
    - Task uses the PIE
    - Task checks its core affinity after PIE usage
    - Task deletes itself
    - Repeat test for TEST_UNPINNED_NUM_ITERS iterations
Expected:
    - Task remains unpinned until its first usage of the PIE
    - The task becomes pinned to the current core after first use of the PIE
    - Each task cleans up its PIE context on deletion
*/

#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1

#define TEST_UNPINNED_NUM_ITERS     5

static void unpinned_task(void *arg)
{
    // Disable scheduling/preemption to make sure current core ID doesn't change
#if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )
    vTaskPreemptionDisable(NULL);
#else
    vTaskSuspendAll();
#endif
    BaseType_t cur_core_num = xPortGetCoreID();
    // Check that the task is unpinned
#if !CONFIG_FREERTOS_UNICORE
#if CONFIG_FREERTOS_SMP
    TEST_ASSERT_EQUAL(tskNO_AFFINITY, vTaskCoreAffinityGet(NULL));
#else
    TEST_ASSERT_EQUAL(tskNO_AFFINITY, xTaskGetCoreID(NULL));
#endif
#endif // !CONFIG_FREERTOS_UNICORE

    int32_t a[4] = { 0, 1, 2, 3};
    int32_t b[4] = { 111, 222, 333, 444 };
    int32_t dst[4] = { 0 };

    pie_vector_signed_add(a, b, dst);

    for (int i = 0; i < sizeof(a) / sizeof(uint32_t); i++) {
        TEST_ASSERT_EQUAL(dst[i], a[i] + b[i]);
    }

#if !CONFIG_FREERTOS_UNICORE
#if CONFIG_FREERTOS_SMP
    TEST_ASSERT_EQUAL(1 << cur_core_num, vTaskCoreAffinityGet(NULL));
#else
    TEST_ASSERT_EQUAL(cur_core_num, xTaskGetCoreID(NULL));
#endif
#endif // !CONFIG_FREERTOS_UNICORE
    // Re-enable scheduling/preemption
#if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )
    vTaskPreemptionEnable(NULL);
#else
    xTaskResumeAll();
#endif

    // Indicate done and self delete
    xTaskNotifyGive((TaskHandle_t)arg);
    vTaskDelete(NULL);
}

TEST_CASE("PIE: Usage in unpinned task", "[freertos]")
{
    TaskHandle_t unity_task_handle = xTaskGetCurrentTaskHandle();
    for (int iter = 0; iter < TEST_UNPINNED_NUM_ITERS; iter++) {
        // Create unpinned task
        xTaskCreate(unpinned_task, "unpin", 4096, (void *)unity_task_handle, UNITY_FREERTOS_PRIORITY + 1, NULL);
        // Wait for task to complete
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(10); // Short delay to allow task memory to be freed
    }
}

/**
 * @brief Function performing some simple calculation using the PIE coprocessor.
 *        The goal is to be preempted by a task that also uses the PIE on the same core.
 */
static void pie_calculation(void* arg)
{
    pie_params_t* p = (pie_params_t*) arg;
    const int32_t cst = p->cst;
    int32_t a[4] = { cst, cst, cst, cst };
    int32_t dst[4] = { 0 };

    for (int i = 0; i < 10; i++) {
        pie_vector_signed_add(a, dst, dst);

        /* Give some time to the other to interrupt us before checking `f` value */
        esp_rom_delay_us(1000);

        /* Using TEST_ASSERT_TRUE triggers a stack overflow, make sure the sign is still correct */
        assert((dst[0] < 0 && cst < 0) || (dst[0] > 0 && cst > 0));

        /* Give the hand back to FreeRTOS to avoid any watchdog error */
        vTaskDelay(2);
    }

    /* Make sure the result is correct */
    assert((dst[0] * cst == 10));

    xTaskNotifyGive(p->main);
    vTaskDelete(NULL);
}

TEST_CASE("PIE: Unsolicited context switch between tasks using the PIE", "[freertos]")
{
    /* Create two tasks that are on the same core and use the same FPU */
    TaskHandle_t unity_task_handle = xTaskGetCurrentTaskHandle();
    TaskHandle_t tasks[2];
    pie_params_t params[2] = {
        { .cst =  1, .main = unity_task_handle },
        { .cst = -1, .main = unity_task_handle },
    };

    xTaskCreatePinnedToCore(pie_calculation, "Task1", 2048, params + 0, UNITY_FREERTOS_PRIORITY + 1, &tasks[0], 1);
    xTaskCreatePinnedToCore(pie_calculation, "Task2", 2048, params + 1, UNITY_FREERTOS_PRIORITY + 1, &tasks[1], 1);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

#endif /* CONFIG_FREERTOS_NUMBER_OF_CORES > 1 */

#endif /* SOC_CPU_HAS_PIE */
