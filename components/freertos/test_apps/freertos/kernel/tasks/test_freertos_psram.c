/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/idf_additions.h"
#include "esp_private/freertos_idf_additions_priv.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "unity_test_runner.h"
#include "memory_checks.h"

#define UNITY_FREERTOS_PRIORITY 5

#if CONFIG_SPIRAM

static void task_delete_itself(void *arg)
{
    printf("starting task\n");
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    xTaskNotifyGive((TaskHandle_t)arg);
    vTaskDelete(NULL);
}

static void task_should_never_run(void *arg)
{
    printf("ERROR: this task should not run, aborting\n");
    abort();
}

static BaseType_t create_task(TaskFunction_t function,
        size_t stack_size,
        void *task_arg,
        int core_num,
        UBaseType_t heap_caps,
        TaskHandle_t *task_handle)
{
#if CONFIG_FREERTOS_SMP
    UBaseType_t core_affinity_mask = (core_num == -1) ? tskNO_AFFINITY : 1 << core_num;
    return prvTaskCreateDynamicAffinitySetWithCaps(function,
                "self_delete",
                stack_size,
                task_arg,
                UNITY_FREERTOS_PRIORITY + 1,
                core_affinity_mask,
                heap_caps,
                task_handle);
#else
    const BaseType_t task_core_num = (core_num == -1) ? tskNO_AFFINITY : core_num;
    return prvTaskCreateDynamicPinnedToCoreWithCaps(function,
                "self_delete",
                stack_size,
                task_arg,
                UNITY_FREERTOS_PRIORITY + 1,
                task_core_num,
                heap_caps,
                task_handle);
#endif
}

TEST_CASE("Out of memory failure", "[freertos][psram]")
{
    TaskHandle_t task_handle = NULL;
    const size_t STACK_SIZE = 0x80000000; // far larger than the virtual address space on ESP32
    UBaseType_t HEAP_CAPS = (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    BaseType_t result = create_task(task_should_never_run,
                STACK_SIZE,
                (void *)xTaskGetCurrentTaskHandle(),
                -1,
                HEAP_CAPS,
                &task_handle);
    TEST_ASSERT_EQUAL(errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY, result);
    (void)task_handle;
}

TEST_CASE("Task with stack memory in PSRAM", "[freertos][psram]")
{
    TaskHandle_t task_handle = NULL;
    const size_t STACK_SIZE = 0x80000; // value is too large for any internal RAM on current chips
    UBaseType_t HEAP_CAPS = (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    BaseType_t result = create_task(task_delete_itself,
                STACK_SIZE,
                (void *)xTaskGetCurrentTaskHandle(),
                -1,
                HEAP_CAPS,
                &task_handle);
    TEST_ASSERT_EQUAL(pdPASS, result);

    // synchronize with the task to make sure we don't return too early, thus giving it enough time
    // to delete itself and giving the idle task time to clean up memory (see delay in tearDown()).
    xTaskNotifyGive(task_handle);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

#if !CONFIG_FREERTOS_UNICORE
typedef struct {
    size_t recorded_core_num;
    TaskHandle_t parent_handle;
} report_corenum_info_t;

static void task_report_corenum(void *arg)
{
    report_corenum_info_t *info = (report_corenum_info_t*) arg;
    info->recorded_core_num = xTaskGetAffinity(NULL);
    xTaskNotifyGive(info->parent_handle);
    vTaskSuspend(NULL);
}

/**
 * This test (roughly) verifies that xTaskCreateWithCapsAffinitySet() pins a task to the correct core.
 */
TEST_CASE("Task on specific core works", "[freertos][psram]")
{
    const size_t STACK_SIZE = 0x1000;
    report_corenum_info_t corenum_info;
    TaskHandle_t task_handle;

    for (int corenum = 0; corenum < CONFIG_SOC_CPU_CORES_NUM; corenum++) {
        corenum_info.recorded_core_num = 0xFFFFFFFF;
        corenum_info.parent_handle = xTaskGetCurrentTaskHandle();
        UBaseType_t HEAP_CAPS = (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        BaseType_t result = create_task(task_report_corenum,
                STACK_SIZE,
                (void *) &(corenum_info),
                corenum,
                HEAP_CAPS,
                &task_handle);

        TEST_ASSERT_EQUAL(pdPASS, result);

        // Wait for the created task to finish its job
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        TEST_ASSERT_EQUAL((size_t) corenum, corenum_info.recorded_core_num);

        vTaskDelete(task_handle);
    }
}
#endif // !CONFIG_FREERTOS_UNICORE

#endif // SPIRAM
