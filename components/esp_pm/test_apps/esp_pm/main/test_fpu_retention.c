/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_private/esp_clk.h"
#include "esp_pm.h"
#include "esp_task.h"
#include "soc/soc_caps.h"

#define MHZ (1000 * 1000)
#define FP_EPS            1e-5f
#define FP_CTX_ITERATIONS 20
#define TEST_TASKS        4

/* Context Switch Test */

static struct {
    int fail_count;
    SemaphoreHandle_t done;
    float seed;
} s_fp_ctx[TEST_TASKS];

static void fp_context_task(void *arg)
{
    vTaskDelay(2);
    int idx = (int)(intptr_t)arg;
    volatile float seed = s_fp_ctx[idx].seed;

    for (int i = 0; i < FP_CTX_ITERATIONS; i++) {
        volatile float v0 = seed * 6.789f;
        vTaskDelay(10);
        volatile float v1 =  v0 + 8.987f;
        vTaskDelay(10);
        volatile float v2 = v1 * v1;
        vTaskDelay(10);
        volatile float v3 = sqrt(v2);
        vTaskDelay(10);
        volatile float v4 = v3 - 8.987f;
        vTaskDelay(10);
        volatile float result = v4 / 6.789f;
        if (fabsf(result - seed) > FP_EPS) {
            s_fp_ctx[idx].fail_count++;
        }
    }
    xSemaphoreGive(s_fp_ctx[idx].done);
    vTaskDelete(NULL);
}

TEST_CASE("Test PD_CPU lightsleep preserves FP registers", "[rv_fp]")
{
#if CONFIG_PM_ENABLE
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;
    int xtal_freq = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = xtal_freq,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
#endif

    const float seeds = 3.1415926f;
    SemaphoreHandle_t sem = xSemaphoreCreateCounting(TEST_TASKS, 0);
    TEST_ASSERT_NOT_NULL(sem);

    for (int i = 0; i < TEST_TASKS; i++) {
        s_fp_ctx[i].fail_count = 0;
        s_fp_ctx[i].done = sem;
        s_fp_ctx[i].seed = seeds * i;
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(
            fp_context_task, "fp", 4096, (void*)(intptr_t)i,
            ESP_TASK_MAIN_PRIO, NULL, i % CONFIG_FREERTOS_NUMBER_OF_CORES));
    }

    for (int i = 0; i < TEST_TASKS; i++) {
        TEST_ASSERT_TRUE(xSemaphoreTake(sem, pdMS_TO_TICKS(20000)));
    }
    vSemaphoreDelete(sem);

    int total = 0;
    for (int i = 0; i < TEST_TASKS; i++) {
        total += s_fp_ctx[i].fail_count;
    }
    TEST_ASSERT_EQUAL_MESSAGE(0, total, "FPU context corruption detected");
    printf("FPU context retention test passed\n");

#if CONFIG_PM_ENABLE
    // Disable lightsleep and DFS
    pm_config.min_freq_mhz = cur_freq_mhz;
    pm_config.light_sleep_enable = false;
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
#endif
}
