/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#include "esp_freertos_hooks.h"
#endif

#if !CONFIG_FREERTOS_UNICORE
static SemaphoreHandle_t test_sem;

static bool idle_hook_func(void)
{
    if (test_sem) {
        xSemaphoreGive(test_sem);
    }
    return true;
}

static void task_delete_func(void *arg)
{
    vTaskDelete(arg);
}
#endif // !CONFIG_FREERTOS_UNICORE

void unity_utils_task_delete(TaskHandle_t thandle)
{
    /* Self deletion can not free up associated task dynamic memory immediately,
     * hence not recommended for test scenarios */
    TEST_ASSERT_NOT_NULL_MESSAGE(thandle, "unity_utils_task_delete: handle is NULL");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(thandle, xTaskGetCurrentTaskHandle(), "unity_utils_task_delete: handle is of currently executing task");

#if CONFIG_FREERTOS_UNICORE
    vTaskDelete(thandle);
#else // CONFIG_FREERTOS_UNICORE
    const BaseType_t tsk_affinity = xTaskGetAffinity(thandle);
    const BaseType_t core_id = xPortGetCoreID();

    printf("Task_affinity: 0x%x, current_core: %d\n", tsk_affinity, core_id);

    if (tsk_affinity == tskNO_AFFINITY) {
        /* For no affinity case, we wait for idle hook to trigger on different core */
        esp_err_t ret = esp_register_freertos_idle_hook_for_cpu(idle_hook_func, !core_id);
        TEST_ASSERT_EQUAL_MESSAGE(ret, ESP_OK, "unity_utils_task_delete: failed to register idle hook");
        vTaskDelete(thandle);
        test_sem = xSemaphoreCreateBinary();
        TEST_ASSERT_NOT_NULL_MESSAGE(test_sem, "unity_utils_task_delete: failed to create semaphore");
        xSemaphoreTake(test_sem, portMAX_DELAY);
        esp_deregister_freertos_idle_hook_for_cpu(idle_hook_func, !core_id);
        vSemaphoreDelete(test_sem);
        test_sem = NULL;
    } else if (tsk_affinity != core_id) {
        /* Task affinity and current core are differnt, schedule IPC call (to delete task)
         * on core where task is pinned to */
        esp_ipc_call_blocking(tsk_affinity, task_delete_func, thandle);
    } else {
        /* Task affinity and current core are same, so we can safely proceed for deletion */
        vTaskDelete(thandle);
    }
#endif // !CONFIG_FREERTOS_UNICORE
}
