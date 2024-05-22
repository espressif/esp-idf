/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <string.h>
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

/**
 * On RISC-V targets that have coprocessors, the contexts are saved at the lowest address of the stack,
 * which can lead to wrong stack watermark calculation in FreeRTOS in theory.
 * As such, the port layer of FreeRTOS will adjust the lowest address of the stack when a coprocessor
 * context is saved.
 */
#if SOC_CPU_HAS_HWLOOP

uint32_t use_hwlp(uint32_t count);

TEST_CASE("HWLP: Context save does not affect stack watermark", "[freertos]")
{
    /* Force the FreeRTOS port layer to store a HWLP context in the current task.
     * So let's use the it and make sure another task, on the SAME CORE, also uses it */
    const TaskHandle_t current_handle = xTaskGetCurrentTaskHandle();

    /* Get the current stack watermark */
    const UBaseType_t before_watermark = uxTaskGetStackHighWaterMark(current_handle);

    /* Use the HWLP unit, the context will NOT be flushed until a context switch is done */
    use_hwlp(20);

    /* Make sure FreeRTOS switches to another task, even Idle task, so that the current Task saves
     * the HWLP current context */
    vTaskDelay(10);

    const UBaseType_t after_watermark = uxTaskGetStackHighWaterMark(current_handle);

    TEST_ASSERT_TRUE(after_watermark > before_watermark / 2);
}

#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1

typedef struct {
    uint32_t count;
    TaskHandle_t main;
} hwlp_params_t;

static void calculation(void* arg)
{
    hwlp_params_t* p = (hwlp_params_t*) arg;
    const uint32_t count = p->count;
    uint32_t result = 0;
    int i = 0;

    for (i = 0; i < 50000; i++) {
        uint32_t current = use_hwlp(count);
        result += current;

        /* Using TEST_ASSERT_TRUE triggers a stack overflow, make sure the count is still correct.
         * The function `use_hwlp` should return (count * 16)  */
        assert(count * 16 == current);
    }

    /* Make sure the result is correct */
    assert(count * 16 * i == result);

    xTaskNotifyGive(p->main);
    vTaskDelete(NULL);
}

TEST_CASE("HWLP: Unsolicited context switch between tasks using HWLP", "[freertos]")
{
    /* Create two tasks that are on the same core and use the same FPU */
    TaskHandle_t unity_task_handle = xTaskGetCurrentTaskHandle();
    TaskHandle_t tasks[2];
    hwlp_params_t params[2] = {
        { .count = 1024, .main = unity_task_handle },
        { .count = 2048, .main = unity_task_handle },
    };

    xTaskCreatePinnedToCore(calculation, "Task1", 2048, params + 0, CONFIG_UNITY_FREERTOS_PRIORITY + 1, &tasks[0], 1);
    xTaskCreatePinnedToCore(calculation, "Task2", 2048, params + 1, CONFIG_UNITY_FREERTOS_PRIORITY + 1, &tasks[1], 1);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

#endif /* CONFIG_FREERTOS_NUMBER_OF_CORES > 1 */

#endif // SOC_CPU_HAS_HWLOOP
