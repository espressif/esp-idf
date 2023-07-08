/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include <esp_types.h>
#include <stdio.h>
#include "unity.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include <stdlib.h>
#include <sys/param.h>
#include <string.h>

//This test only makes sense with poisoning disabled (light or comprehensive)
#if !defined(CONFIG_HEAP_POISONING_COMPREHENSIVE) && !defined(CONFIG_HEAP_POISONING_LIGHT)

#define NUM_POINTERS 128
#define ITERATIONS 5000

static const char *TAG = "test_heap";

TEST_CASE("Heap many random allocations timings", "[heap]")
{
    void *p[NUM_POINTERS] = { 0 };
    size_t s[NUM_POINTERS] = { 0 };

    uint32_t cycles_before;
    uint64_t alloc_time_average = 0;
    uint64_t free_time_average = 0;
    uint64_t realloc_time_average = 0;

    for (int i = 0; i < ITERATIONS; i++) {
        uint8_t n = (uint32_t)rand() % NUM_POINTERS;

        if (i % 4 == 0) {
            /* 1 in 4 iterations, try to realloc the buffer instead
               of using malloc/free
            */
            size_t new_size = (uint32_t)rand() % 1024;

            cycles_before = esp_cpu_get_cycle_count();
            void *new_p = heap_caps_realloc(p[n], new_size, MALLOC_CAP_DEFAULT);
            realloc_time_average = esp_cpu_get_cycle_count() - cycles_before;

            ESP_LOGD(TAG, "realloc %p -> %p (%zu -> %zu) time spent cycles: %lld", p[n], new_p, s[n], new_size, realloc_time_average);
            heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true);
            if (new_size == 0 || new_p != NULL) {
                p[n] = new_p;
                s[n] = new_size;
                if (new_size > 0) {
                    memset(p[n], n, new_size);
                }
            }
            continue;
        }

        if (p[n] != NULL) {
            if (s[n] > 0) {
                /* Verify pre-existing contents of p[n] */
                uint8_t compare[s[n]];
                memset(compare, n, s[n]);
                TEST_ASSERT(( memcmp(compare, p[n], s[n]) == 0 ));
            }
            TEST_ASSERT(heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true));

            cycles_before = esp_cpu_get_cycle_count();
            heap_caps_free(p[n]);
            free_time_average = esp_cpu_get_cycle_count() - cycles_before;

           ESP_LOGD(TAG, "freed %p (%zu) time spent cycles: %lld", p[n], s[n], free_time_average);

            if (!heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true)) {
                printf("FAILED iteration %d after freeing %p\n", i, p[n]);
                heap_caps_dump(MALLOC_CAP_DEFAULT);
                TEST_ASSERT(0);
            }
        }

        s[n] = rand() % 1024;
        heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true);
        cycles_before = esp_cpu_get_cycle_count();
        p[n] = heap_caps_malloc(s[n], MALLOC_CAP_DEFAULT);
        alloc_time_average = esp_cpu_get_cycle_count() - cycles_before;

        ESP_LOGD(TAG, "malloc %p (%zu) time spent cycles: %lld", p[n], s[n], alloc_time_average);

        if (!heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true)) {
            printf("FAILED iteration %d after mallocing %p (%zu bytes)\n", i, p[n], s[n]);
            heap_caps_dump(MALLOC_CAP_DEFAULT);
            TEST_ASSERT(0);
        }

        if (p[n] != NULL) {
            memset(p[n], n, s[n]);
        }
    }

    for (int i = 0; i < NUM_POINTERS; i++) {
        cycles_before = esp_cpu_get_cycle_count();
        heap_caps_free( p[i]);
        free_time_average = esp_cpu_get_cycle_count() - cycles_before;

        if (!heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true)) {
            printf("FAILED during cleanup after freeing %p\n", p[i]);
            heap_caps_dump(MALLOC_CAP_DEFAULT);
            TEST_ASSERT(0);
        }
    }

    TEST_ASSERT(heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true));
}
#endif
