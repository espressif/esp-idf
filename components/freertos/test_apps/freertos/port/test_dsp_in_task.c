/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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

#if SOC_CPU_HAS_DSP

#define TEST_NUM_TASKS       4

typedef struct {
    int32_t  id;
    uint32_t result;
    TaskHandle_t main;
    SemaphoreHandle_t sem;
} dsp_params_t;

/**
 * @brief Multiplies the given ID by a constant.
 *
 * @param id Value to multiply
 */
uint32_t dsp_id_mul(uint32_t id);

/**
 * @brief DSP Assembly routine need to access this constant, make it public.
 * It will be used as a multiplier.
 */
const uint32_t g_dsp_constant = 100000;

static void pinned_task(void *arg)
{
    dsp_params_t *param = (dsp_params_t*) arg;
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    param->result = dsp_id_mul(param->id);

    /* Indicate done and wait to be deleted */
    xSemaphoreGive((SemaphoreHandle_t)param->sem);
    vTaskSuspend(NULL);
}

TEST_CASE("DSP: Usage in task", "[freertos]")
{
    TaskHandle_t task_handles[CONFIG_FREERTOS_NUMBER_OF_CORES][TEST_NUM_TASKS];
    dsp_params_t params[CONFIG_FREERTOS_NUMBER_OF_CORES][TEST_NUM_TASKS];

    SemaphoreHandle_t done_sem = xSemaphoreCreateCounting(TEST_NUM_TASKS, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, done_sem);

    // Create test tasks for each core
    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        for (int j = 0; j < TEST_NUM_TASKS; j++) {
            params[i][j] = (dsp_params_t) {
                .id = i * TEST_NUM_TASKS + j + 1,
                .sem = done_sem,
            };
            TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(pinned_task, "task", 4096, (void *) &params[i][j], UNITY_FREERTOS_PRIORITY + 1, &task_handles[i][j], i));
        }
    }

    // Start the created tasks
    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        for (int j = 0; j < TEST_NUM_TASKS; j++) {
            xTaskNotifyGive(task_handles[i][j]);
        }
    }

    // Wait for the tasks to complete
    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES * TEST_NUM_TASKS; i++) {
        xSemaphoreTake(done_sem, portMAX_DELAY);
    }

    // Delete the tasks
    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        for (int j = 0; j < TEST_NUM_TASKS; j++) {
            vTaskDelete(task_handles[i][j]);
        }
    }

    // Check the values
    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        for (int j = 0; j < TEST_NUM_TASKS; j++) {
            dsp_params_t* param = &params[i][j];
            TEST_ASSERT_EQUAL(param->id * g_dsp_constant, param->result);
        }
    }

    vSemaphoreDelete(done_sem);
}

#endif /* SOC_CPU_HAS_DSP */
