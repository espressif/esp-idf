/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include "unity.h"
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#include "test_utils.h"

#if SOC_CPU_CORES_NUM > 1
static volatile bool s_stall_task_run;
static volatile bool s_stall_task_started;
static volatile uint32_t s_stall_counter;

static void stall_counter_task(void *arg)
{
    s_stall_task_started = true;
    while (s_stall_task_run) {
        s_stall_counter++;
    }
    vTaskDelete(NULL);
}

TEST_CASE("CPU stall: other core stops and resumes", "[cpu][cpu_stall]")
{
    const int current_core = esp_cpu_get_core_id();
    const int other_core = !current_core;
    TaskHandle_t task_handle = NULL;

    s_stall_task_run = true;
    s_stall_task_started = false;
    s_stall_counter = 0;

    TEST_ASSERT_EQUAL(pdPASS,
                      xTaskCreatePinnedToCore(stall_counter_task, "cpu_stall_counter", 2048, NULL,
                                              UNITY_FREERTOS_PRIORITY + 1, &task_handle, other_core));

    for (int i = 0; i < 1000 && !s_stall_task_started; i++) {
        esp_rom_delay_us(100);
    }
    TEST_ASSERT_TRUE(s_stall_task_started);

    uint32_t start_count = s_stall_counter;
    for (int i = 0; i < 1000 && s_stall_counter == start_count; i++) {
        esp_rom_delay_us(100);
    }
    TEST_ASSERT_TRUE(s_stall_counter > start_count);

    esp_cpu_stall(other_core);
    esp_rom_delay_us(1000);
    uint32_t stalled_count = s_stall_counter;
    esp_rom_delay_us(20000);
    uint32_t stalled_count_late = s_stall_counter;
    esp_cpu_unstall(other_core);

    TEST_ASSERT_EQUAL_UINT32(stalled_count, stalled_count_late);

    for (int i = 0; i < 1000 && s_stall_counter == stalled_count_late; i++) {
        esp_rom_delay_us(100);
    }
    TEST_ASSERT_TRUE(s_stall_counter > stalled_count_late);

    s_stall_task_run = false;
    vTaskDelay(1);
}
#endif
