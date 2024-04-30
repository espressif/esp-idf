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

static uint32_t use_hwlp(uint32_t count)
{
    uint32_t ret;
    asm volatile(
        /* The toolchain doesn't support HWLP instructions yet, manually set it up */
        "la a2, start\n"
        "csrw 0x7c6, a2\n"
        "la a2, end\n"
        "csrw 0x7c7, a2\n"
        "csrw 0x7c8, a0\n"
        "li a1, 0\n"
        /* Hardware loops must have at least 8 32-bit instructions or 16 16-bit instructions */
        "start:\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "addi a1, a1, 1\n"
        "end:\n"
        "addi a1, a1, 1\n"
        "mv   %0, a1\n"
        "ret\n"
        : "=r"(ret) :);
    return ret;
}

static void other_task(void* arg)
{
    const TaskHandle_t main_task = (TaskHandle_t) arg;

    use_hwlp(10);

    xTaskNotifyGive(main_task);
    vTaskDelete(NULL);
}

TEST_CASE("HWLP: Context save does not affect stack watermark", "[freertos]")
{
    TaskHandle_t pvCreatedTask;
    /* Force the FreeRTOS port layer to store a HWLP context in the current task.
     * So let's use the it and make sure another task, on the SAME CORE, also uses it */
    const int core_id = xPortGetCoreID();
    const TaskHandle_t current_handle = xTaskGetCurrentTaskHandle();

    /* Get the current stack watermark */
    const UBaseType_t before_watermark = uxTaskGetStackHighWaterMark(current_handle);

    /* Use the HWLP unit, the context will NOT be flushed until another task starts using it */
    use_hwlp(20);

    xTaskCreatePinnedToCore(other_task,
                            "OtherTask",
                            2048,
                            (void*) current_handle,
                            CONFIG_UNITY_FREERTOS_PRIORITY - 1,
                            &pvCreatedTask,
                            core_id);

    vTaskDelay(10);

    /* Wait for other task to complete */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    const UBaseType_t after_watermark = uxTaskGetStackHighWaterMark(current_handle);

    TEST_ASSERT_TRUE(after_watermark > before_watermark / 2);
}

typedef struct {
    uint32_t count;
    TaskHandle_t main;
} ParamsHWLP;

void calculation(void* arg)
{
    ParamsHWLP* p = (ParamsHWLP*) arg;
    const uint32_t count = p->count;
    uint32_t result = 0;
    int i = 0;

    for (i = 0; i < 10; i++) {
        uint32_t current = use_hwlp(count);
        result += current;

        /* Give some time to the other to interrupt us before checking `f` value */
        esp_rom_delay_us(1000);

        /* Using TEST_ASSERT_TRUE triggers a stack overflow, make sure the count is still correct.
         * The function `use_hwlp` should return (count * 16)  */
        assert(count * 16 == current);

        /* Give the hand back to FreeRTOS to avoid any watchdog error */
        vTaskDelay(2);
    }

    /* Make sure the result is correct */
    assert(count * 16 * i == result);

    xTaskNotifyGive(p->main);
    vTaskDelete(NULL);
}

TEST_CASE("HWLP: Unsolicited context switch between tasks using the PIE", "[freertos]")
{
    /* Create two tasks that are on the same core and use the same FPU */
    TaskHandle_t unity_task_handle = xTaskGetCurrentTaskHandle();
    TaskHandle_t tasks[2];
    ParamsHWLP params[2] = {
        { .count =  10, .main = unity_task_handle },
        { .count = 200, .main = unity_task_handle },
    };

    xTaskCreatePinnedToCore(calculation, "Task1", 2048, params + 0, CONFIG_UNITY_FREERTOS_PRIORITY + 1, &tasks[0], 1);
    xTaskCreatePinnedToCore(calculation, "Task2", 2048, params + 1, CONFIG_UNITY_FREERTOS_PRIORITY + 1, &tasks[1], 1);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

#endif // SOC_CPU_HAS_HWLOOP
