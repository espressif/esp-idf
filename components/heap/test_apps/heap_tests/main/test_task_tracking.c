/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "stdio.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_heap_task_info.h"

// This test only apply when task tracking is enabled
#if defined(CONFIG_HEAP_TASK_TRACKING) && defined(CONFIG_HEAP_TRACK_DELETED_TASKS)

#define ALLOC_BYTES 36

static void check_heap_task_info(const char *task_name, const bool task_active)
{
    heap_all_tasks_stat_t heap_tasks_stat;

    heap_tasks_stat.task_count = 10;
    heap_tasks_stat.heap_count = 20;
    heap_tasks_stat.alloc_count = 60;
    task_stat_t arr_task_stat[heap_tasks_stat.task_count];
    heap_stat_t arr_heap_stat[heap_tasks_stat.heap_count];
    heap_task_block_t arr_alloc_stat[heap_tasks_stat.alloc_count];
    heap_tasks_stat.stat_arr = arr_task_stat;
    heap_tasks_stat.heap_stat_start = arr_heap_stat;
    heap_tasks_stat.alloc_stat_start = arr_alloc_stat;

    heap_caps_get_all_task_stat(&heap_tasks_stat);

    bool task_found = false;
    for (size_t task_index = 0; task_index < heap_tasks_stat.task_count; task_index++) {
        // the prescheduler allocs and free are stored as a
        // task with a handle set to 0, avoid calling pcTaskGetName
        // in that case.
        task_stat_t task_stat = heap_tasks_stat.stat_arr[task_index];
        if (0 == strcmp(task_stat.name, task_name) && task_stat.is_alive == task_active) {
            task_found = true;
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

    // unlock main to check task tracking feature
    xTaskNotifyGive((TaskHandle_t)args);

    // wait for main to give back the hand to the task to delete the pointer
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    heap_caps_free(ptr);

    // unlock main to delete the task
    xTaskNotifyGive((TaskHandle_t)args);

   // wait for main to delete the task
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

static void test_task_a(void *args)
{
    test_task(args);
}

static void test_task_b(void *args)
{
    test_task(args);
}

/* This test will create a task, wait for the task to allocate / free memory
 * so it is added to the task tracking info in the heap component and then
 * call heap_caps_get_all_task_stat() and make sure a task with the name test_task
 * is in the list, and that the right ALLOC_BYTES are shown.
 *
 * Note: The memory allocated in the task is not freed for the sake of the test
 * so it is normal that memory leak will be reported by the test environment. It
 * shouldn't be more than the byte allocated by the task + associated metadata
 */
TEST_CASE("heap task tracking reports created / deleted task", "[heap]")
{
    TaskHandle_t test_task_handle;
    const char *task_name = "test_task_a";
    xTaskCreate(&test_task_a, task_name, 3072, (void *)xTaskGetCurrentTaskHandle(), 5, &test_task_handle);

    // wait for task to allocate memory and give the hand back to the test
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // check that the task is referenced in the list of task
    // by the task tracking feature. check that the task name is
    // matching and the task is running.
    check_heap_task_info(task_name, true);

    // unlock main to check task tracking feature
    xTaskNotifyGive(test_task_handle);

    // wait for the task to free the memory
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // delete the task.
    vTaskDelete(test_task_handle);

    // check that the task is referenced in the list of task
    // by the task tracking feature. check that the task name is
    // matching and the task is marked as deleted.
    check_heap_task_info(task_name, false);
}

/* The test case calls heap_caps_alloc_all_task_stat_arrays and heap_caps_get_all_task_stat
 * after creating new tasks and allocating in new heaps to check that the number of tasks, heaps and
 * allocation statistics provided by heap_caps_get_all_task_stat is updated accordingly.
*/
TEST_CASE("heap task tracking check alloc array and get all tasks info", "[heap]")
{
    // call heap_caps_alloc_all_task_stat_arrays and save the number of tasks, heaps and allocs
    // statistics available when the test starts
    heap_all_tasks_stat_t tasks_stat;
    esp_err_t ret_val = heap_caps_alloc_all_task_stat_arrays(&tasks_stat);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);
    ret_val = heap_caps_get_all_task_stat(&tasks_stat);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);

    const size_t nb_of_tasks_stat = tasks_stat.task_count;
    const size_t nb_of_heaps_stat = tasks_stat.heap_count;
    const size_t nb_of_allocs_stat = tasks_stat.alloc_count;
    heap_caps_free_all_task_stat_arrays(&tasks_stat);

    // Create a task that will allocate memory
    TaskHandle_t test_task_handle;
    const char *task_name = "test_task_b";
    xTaskCreate(&test_task_b, task_name, 3072, (void *)xTaskGetCurrentTaskHandle(), 5, &test_task_handle);

    // wait for the task to give the hand to the test and call heap_caps_alloc_all_task_stat_arrays.
    // Compare the number of tasks, heaps and allocs statistics available to make sure they contain the stats
    // related to the newly created task.
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    ret_val = heap_caps_alloc_all_task_stat_arrays(&tasks_stat);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);
    ret_val = heap_caps_get_all_task_stat(&tasks_stat);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);

    TEST_ASSERT(nb_of_tasks_stat < tasks_stat.task_count);
    TEST_ASSERT(nb_of_heaps_stat < tasks_stat.heap_count);
    TEST_ASSERT(nb_of_allocs_stat < tasks_stat.alloc_count);

    // free the arrays of stat in tasks_stat and reset the counters
    heap_caps_free_all_task_stat_arrays(&tasks_stat);

    // unlock task to delete allocated memory
    xTaskNotifyGive(test_task_handle);

    // wait for the task to free the memory
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // delete the task.
    vTaskDelete(test_task_handle);
}

static void task_self_check(void *args)
{
    const size_t alloc_size = 100;
    const uint32_t caps = MALLOC_CAP_32BIT | MALLOC_CAP_DMA;

    // call heap_caps_alloc_single_task_stat_arrays on the current task. Since no alloc was made, the
    // function should return ESP_OK but the heap_count and alloc_count should be 0, the pointer to the
    // allocated arrays should be NULL.
    heap_single_task_stat_t task_stat;
    esp_err_t ret_val = heap_caps_alloc_single_task_stat_arrays(&task_stat, NULL);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);
    TEST_ASSERT_EQUAL(task_stat.heap_count, 0);
    TEST_ASSERT_EQUAL(task_stat.alloc_count, 0);
    TEST_ASSERT_NULL(task_stat.heap_stat_start);
    TEST_ASSERT_NULL(task_stat.alloc_stat_start);

    // allocate memory
    void *ptr = heap_caps_malloc(alloc_size, caps);

    // allocate arrays for the statistics of the task. This time, it should succeed as we just
    // allocated memory. This information should be stored in the task info list.
    ret_val = heap_caps_alloc_single_task_stat_arrays(&task_stat, NULL);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);

    // The number of heap info should be one and the number of alloc should be one too
    TEST_ASSERT_EQUAL(1, task_stat.heap_count);
    TEST_ASSERT_EQUAL(1, task_stat.alloc_count);

    ret_val = heap_caps_get_single_task_stat(&task_stat, NULL);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);

    // the caps of the heap info should contain the caps used to allocate the memory
    TEST_ASSERT((task_stat.stat.heap_stat[0].caps & caps) == caps);

    // The size of the alloc found in the stat should be not null and the address
    // of the alloc should match too
    TEST_ASSERT(task_stat.stat.heap_stat[0].alloc_stat[0].size > 0);
    TEST_ASSERT(task_stat.stat.heap_stat[0].alloc_stat[0].address == ptr);

    // free the memory and get the updated statistics on the task
    heap_caps_free(ptr);
    heap_caps_free_single_task_stat_arrays(&task_stat);

    ret_val = heap_caps_alloc_single_task_stat_arrays(&task_stat, NULL);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);

    // The number of heap info should be one and the number of alloc should be zero
    // since the allocated memory was just freed
    TEST_ASSERT_EQUAL(1, task_stat.heap_count);
    TEST_ASSERT_EQUAL(0, task_stat.alloc_count);

    ret_val = heap_caps_get_single_task_stat(&task_stat, NULL);
    TEST_ASSERT_EQUAL(ret_val, ESP_OK);

    TEST_ASSERT((task_stat.stat.heap_stat[0].caps & caps) == caps);
    TEST_ASSERT(task_stat.stat.heap_stat[0].alloc_stat == NULL);

    // unlock main to check task tracking feature
    xTaskNotifyGive((TaskHandle_t)args);

    // wait for main to give back the hand to the task to delete the pointer
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

/* The test case calls heap_caps_alloc_single_task_stat_arrays and heap_caps_get_single_task_stat
 * after creating new task and allocating in new heaps to check that the number of heaps and
 * allocation statistics provided by heap_caps_get_single_task_stat is updated accordingly.
*/
TEST_CASE("heap task tracking check alloc arrays and get info on specific task", "[heap]")
{
    TaskHandle_t test_task_handle;
    const char *task_name = "task_self_check";
    xTaskCreate(&task_self_check, task_name, 3072, (void *)xTaskGetCurrentTaskHandle(), 5, &test_task_handle);

    // wait for the task to free the memory
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // delete the task.
    vTaskDelete(test_task_handle);
}

#endif // CONFIG_HEAP_TASK_TRACKING
