/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_heap_task_info.h"

// This test only apply when task tracking is enabled
#if defined(CONFIG_HEAP_TASK_TRACKING)

#define MAX_TASK_NUM 10  // Max number of per tasks info that it can store
#define MAX_BLOCK_NUM 10  // Max number of per block info that it can store
#define ALLOC_BYTES 36

static void check_heap_task_info(TaskHandle_t taskHdl)
{
    size_t num_totals = 0;
    heap_task_totals_t s_totals_arr[MAX_TASK_NUM];
    heap_task_block_t s_block_arr[MAX_BLOCK_NUM];

    heap_task_info_params_t heap_info = {0};
    heap_info.caps[0] = MALLOC_CAP_32BIT;       // Gets heap info with CAP_32BIT capabilities
    heap_info.mask[0] = MALLOC_CAP_32BIT;
    heap_info.tasks = NULL;                     // Passing NULL captures heap info for all tasks
    heap_info.num_tasks = 0;
    heap_info.totals = s_totals_arr;            // Gets task wise allocation details
    heap_info.num_totals = &num_totals;
    heap_info.max_totals = MAX_TASK_NUM;        // Maximum length of "s_totals_arr"
    heap_info.blocks = s_block_arr;             // Gets block wise allocation details. For each block, gets owner task, address and size
    heap_info.max_blocks = MAX_BLOCK_NUM;       // Maximum length of "s_block_arr"

    heap_caps_get_per_task_info(&heap_info);

    bool task_found = false;
    for (int i = 0 ; i < *heap_info.num_totals; i++) {
        // the prescheduler allocs and free are stored as a
        // task with a handle set to 0, avoid calling pcTaskGetName
        // in that case.
        if (heap_info.totals[i].task != 0 && (uint32_t*)(heap_info.totals[i].task) == (uint32_t*)taskHdl) {
            task_found = true;
            // check the number of byte allocated according to the task tracking feature
            // and make sure it matches the expected value. The size returned by the
            // heap_caps_get_per_task_info includes the size of the block owner (4 bytes)
            TEST_ASSERT(heap_info.totals[i].size[0] == ALLOC_BYTES + 4);
        }
    }
    TEST_ASSERT_TRUE(task_found);
}

static void test_task(void *args)
{
    void *ptr = heap_caps_malloc(ALLOC_BYTES, MALLOC_CAP_32BIT);
    if (ptr == NULL) {
        abort();
    }

    // unlock main too check task tracking feature
    xTaskNotifyGive((TaskHandle_t)args);

    // wait for main to delete this task
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

/* This test will create a task, wait for the task to allocate / free memory
 * so it is added to the task tracking info in the heap component and then
 * call heap_caps_get_per_task_info() and make sure a task with the name test_task
 * is in the list, and that the right ALLOC_BYTES are shown.
 *
 * Note: The memory allocated in the task is not freed for the sake of the test
 * so it is normal that memory leak will be reported by the test environment. It
 * shouldn't be more than the byte allocated by the task + associated metadata
 */
TEST_CASE("heap task tracking reports created task", "[heap]")
{
    TaskHandle_t test_task_handle;

    xTaskCreate(&test_task, "test_task", 3072, (void *)xTaskGetCurrentTaskHandle(), 5, &test_task_handle);

    // wait for task to allocate memory and give the hand back to the test
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // check that the task is referenced in the list of task
    // by the task tracking feature. Check the number of bytes
    // the task has allocated and make sure it is matching the
    // expected value.
    check_heap_task_info(test_task_handle);

    // delete the task.
    vTaskDelete(test_task_handle);
}

#endif // CONFIG_HEAP_TASK_TRACKING
