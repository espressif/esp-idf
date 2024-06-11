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

#define TASKS_STATUS_ARRAY_LEN  16

/**
 * On RISC-V targets that have coprocessors, the contexts are saved at the lowest address of the stack,
 * which can lead to wrong stack watermark calculation in FreeRTOS in theory.
 * As such, the port layer of FreeRTOS will adjust the lowest address of the stack when a coprocessor
 * context is saved.
 */
#if SOC_CPU_HAS_PIE

/**
 * @brief Performs the signed sum of two 4-word vectors using the PIE.
 *
 * @param a First vector
 * @param b Second vector
 * @param dst Destination to store the sum
 */
void pie_vector_unsigned_add(const uint32_t a[4], const uint32_t b[4], uint32_t dst[4]);

static void other_task(void* arg)
{
    uint32_t a[4] = { 1, 2, 3, 4};
    uint32_t b[4] = { 42, 43, 44, 45};
    uint32_t dst[4] = { 0 };
    const TaskHandle_t main_task = (TaskHandle_t) arg;

    /* This task must also use the PIE coprocessor to force a PIE context flush on the main task */
    pie_vector_unsigned_add(a, b, dst);

    xTaskNotifyGive(main_task);
    vTaskDelete(NULL);
}

TEST_CASE("PIE: Context save does not affect stack watermark", "[freertos]")
{
    /* Setup some random values */
    uint32_t a[4] = { 0x3f00ffff, 0xffe10045, 0xffe10096, 0x42434546};
    uint32_t b[4] = { 0x42, 0xbb43, 0x6644, 0x845};
    uint32_t dst[4] = { 0 };

    TaskHandle_t pvCreatedTask;
    /* Force the FreeRTOS port layer to store a PIE context in the current task.
     * So let's use the PIE and make sure another task, on the SAME CORE, also uses it */
    const int core_id = xPortGetCoreID();
    const TaskHandle_t current_handle = xTaskGetCurrentTaskHandle();

    /* Get the current stack watermark */
    const UBaseType_t before_watermark = uxTaskGetStackHighWaterMark(current_handle);

    /* Use the PIE unit, the context will NOT be flushed until another task starts using it */
    pie_vector_unsigned_add(a, b, dst);

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

    /* The current task has seen a PIE registers context save, so we have at least 8 16-byte registers saved on the
     * stack, which represents 128 bytes. In practice, it may be very different, for example a call to printf would
     * result is more than 1KB of additional stack space used. So let's just make sure that the watermark is bigger
     * than 50% of the former watermark. */
    TEST_ASSERT_TRUE(after_watermark > before_watermark / 2);
}

#endif // SOC_CPU_HAS_PIE
