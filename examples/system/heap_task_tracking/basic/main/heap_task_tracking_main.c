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

static void no_leak_task(void *args)
{
    size_t size_a = 0;
    size_t size_b = 0;
    char *task_name = pcTaskGetName(*((TaskHandle_t*)args));
    printf("Starting task: %s\n", task_name);

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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void leaking_task(void *args)
{
    size_t size_a = 0;
    size_t size_b = 0;
    char *task_name = pcTaskGetName(*((TaskHandle_t*)args));
    printf("Starting task: %s\n", task_name);

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
        // heap_caps_free(ptr_b);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    TaskHandle_t no_leak_task_hdl, leaking_task_hdl;

    /* Create example task to demonstrate heap_task_tracking */
    xTaskCreate(&no_leak_task, "no_leak_task", 3072, &no_leak_task_hdl, 5, &no_leak_task_hdl);
    xTaskCreate(&leaking_task, "leaking_task", 3072, &leaking_task_hdl, 5, &leaking_task_hdl);

    /* print task statistic periodically */
    for(size_t counter = 0; counter < 4; counter++) {
        /* print the overview stats of every task */
        printf("\n PRINTING OVERVIEW STATISTICS OF EACH TASK\n");
        heap_caps_print_all_task_stat_overview(stdout);

        /* print the overview statistics of the no leak task */
        printf("\n PRINTING OVERVIEW STATISTICS OF NO LEAK TASK\n");
        heap_caps_print_single_task_stat_overview(stdout, no_leak_task_hdl);

        /* print the overview statistics of the leaking task */
        printf("\n PRINTING OVERVIEW STATISTICS OF LEAKING TASK\n");
        heap_caps_print_single_task_stat_overview(stdout, leaking_task_hdl);

        if (counter == 2) {
            /* delete the leaking task and let the no leak task run
             * for some more time */
            printf("Deleting task: %s\n", pcTaskGetName(leaking_task_hdl));
            vTaskDelete(leaking_task_hdl);
        }

        /* wait for a second before running the loop again*/
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    /* Delete the no leak task */
    printf("Deleting task: %s\n", pcTaskGetName(no_leak_task_hdl));
    vTaskDelete(no_leak_task_hdl);

    /* print overview information of every task */
    printf("\n PRINTING OVERVIEW STATISTICS OF EACH TASK\n");
    heap_caps_print_all_task_stat_overview(stdout);

    /* print detailed statistics for every task */
    printf("\n PRINTING DETAILED STATISTICS OF EACH TASK\n");
    heap_caps_print_all_task_stat(stdout);
}
