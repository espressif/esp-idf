/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Heap Task Tracking Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_task_info.h"
#include "esp_heap_caps.h"
#include "esp_random.h"
#include "esp_log.h"

static void print_single_task_info(TaskHandle_t task_hdl)
{
    heap_single_task_stat_t task_stat;
    /* call API to dynamically allocate the memory necessary to store the
     * information collected while calling heap_caps_get_single_task_stat */
    const esp_err_t ret_val = heap_caps_alloc_single_task_stat_arrays(&task_stat, task_hdl);
    assert(ret_val == ESP_OK);

    /* collect the information */
    heap_caps_get_single_task_stat(&task_stat, task_hdl);

    /* process the information retrieved */
    printf("\n--------------------------------------------------------------------------------\n");
    printf("PRINTING SINGLE TASK INFO\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("%s: %s: Peak Usage %"PRIu16", Current Usage %"PRIu16"\n", task_stat.stat.name,
                                                                      task_stat.stat.is_alive ? "ALIVE  " : "DELETED",
                                                                      task_stat.stat.overall_peak_usage,
                                                                      task_stat.stat.overall_current_usage);

    for (size_t heap_idx = 0; heap_idx < task_stat.heap_count; heap_idx++) {
        heap_stat_t heap_stat = task_stat.heap_stat_start[heap_idx];
        printf("    %s: Caps: %"PRIu32". Size %"PRIu16", Current Usage %"PRIu16", Peak Usage %"PRIu16", alloc count %"PRIu16"\n", heap_stat.name,
                                                                                                                                    heap_stat.caps,
                                                                                                                                    heap_stat.size,
                                                                                                                                    heap_stat.current_usage,
                                                                                                                                    heap_stat.peak_usage,
                                                                                                                                    heap_stat.alloc_count);

        for (size_t alloc_idx = 0; alloc_idx < heap_stat.alloc_count; alloc_idx++) {
            heap_task_block_t alloc_stat = heap_stat.alloc_stat[alloc_idx];
            printf("        %p: Size: %"PRIu32"\n", alloc_stat.address, alloc_stat.size);
        }
    }

    /* delete the memory dynamically allocated while calling heap_caps_alloc_all_task_stat_arrays */
    heap_caps_free_single_task_stat_arrays(&task_stat);
}

static void print_all_tasks_info(void)
{
    heap_all_tasks_stat_t tasks_stat;
    /* call API to dynamically allocate the memory necessary to store the
     * information collected while calling heap_caps_get_all_task_stat */
    const esp_err_t ret_val = heap_caps_alloc_all_task_stat_arrays(&tasks_stat);
    assert(ret_val == ESP_OK);

    /* collect the information */
    heap_caps_get_all_task_stat(&tasks_stat);

    /* process the information retrieved */
    printf("\n--------------------------------------------------------------------------------\n");
    printf("PRINTING ALL TASKS INFO\n");
    printf("--------------------------------------------------------------------------------\n");
    for (size_t task_idx = 0; task_idx < tasks_stat.task_count; task_idx++) {
        task_stat_t task_stat = tasks_stat.stat_arr[task_idx];
        printf("%s: %s: Peak Usage %"PRIu16", Current Usage %"PRIu16"\n", task_stat.name,
                                                                          task_stat.is_alive ? "ALIVE  " : "DELETED",
                                                                          task_stat.overall_peak_usage,
                                                                          task_stat.overall_current_usage);

        for (size_t heap_idx = 0; heap_idx < task_stat.heap_count; heap_idx++) {
            heap_stat_t heap_stat = task_stat.heap_stat[heap_idx];
            printf("    %s: Caps: %"PRIu32". Size %"PRIu16", Current Usage %"PRIu16", Peak Usage %"PRIu16", alloc count %"PRIu16"\n", heap_stat.name,
                                                                                                                                      heap_stat.caps,
                                                                                                                                      heap_stat.size,
                                                                                                                                      heap_stat.current_usage,
                                                                                                                                      heap_stat.peak_usage,
                                                                                                                                      heap_stat.alloc_count);

            for (size_t alloc_idx = 0; alloc_idx < heap_stat.alloc_count; alloc_idx++) {
                heap_task_block_t alloc_stat = heap_stat.alloc_stat[alloc_idx];
                printf("        %p: Size: %"PRIu32"\n", alloc_stat.address, alloc_stat.size);
            }
        }
    }

    /* delete the memory dynamically allocated while calling heap_caps_alloc_all_task_stat_arrays */
    heap_caps_free_all_task_stat_arrays(&tasks_stat);
}

static void no_leak_task(void *args)
{
    size_t size_a = 0;
    size_t size_b = 0;
    char *task_name = pcTaskGetName(*((TaskHandle_t*)args));

    while(1) {
        /* Allocate random amount of memory for demonstration */
        size_a = (esp_random() % 10000) + 1;
        size_b = (esp_random() % (10000 - size_a)) + 1;

        void *ptr_a = heap_caps_malloc(size_a, MALLOC_CAP_DEFAULT);
        void *ptr_b = heap_caps_malloc(size_b, MALLOC_CAP_DEFAULT);
        if (ptr_a == NULL || ptr_b == NULL) {
            ESP_LOGE(task_name, "Could not allocate heap memory");
            abort();
        }

        heap_caps_free(ptr_a);
        heap_caps_free(ptr_b);

        // print the task statistics (passing NULL will print info for
        // the currently running task)
        print_single_task_info(NULL);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void leaking_task(void *args)
{
    size_t size_a = 0;
    size_t size_b = 0;
    char *task_name = pcTaskGetName(*((TaskHandle_t*)args));

    while(1) {
        /* Allocate random amount of memory for demonstration */
        size_a = (esp_random() % 10000) + 1;
        size_b = (esp_random() % (10000 - size_a)) + 1;

        void *ptr_a = heap_caps_malloc(size_a, MALLOC_CAP_DEFAULT);
        void *ptr_b = heap_caps_malloc(size_b, MALLOC_CAP_DEFAULT);
        if (ptr_a == NULL || ptr_b == NULL) {
            ESP_LOGE(task_name, "Could not allocate heap memory");
            abort();
        }

        heap_caps_free(ptr_a);

        // don't free ptr_b on purpose to create unfreed memory for the task info to print
        // heap_caps_free(ptr_b);

        // print the task statistics (passing NULL will print info for
        // the currently running task)
        print_single_task_info(NULL);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    TaskHandle_t no_leak_task_hdl, leaking_task_hdl;

    /* Create example task to demonstrate heap_task_tracking */
    xTaskCreate(&no_leak_task, "no_leak_task", 3072, &no_leak_task_hdl, 0, &no_leak_task_hdl);
    xTaskCreate(&leaking_task, "leaking_task", 3072, &leaking_task_hdl, 0, &leaking_task_hdl);

    size_t counter = 4;
    while(counter != 0) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        counter--;
    }

    vTaskDelete(leaking_task_hdl);
    vTaskDelete(no_leak_task_hdl);

    print_all_tasks_info();
}
