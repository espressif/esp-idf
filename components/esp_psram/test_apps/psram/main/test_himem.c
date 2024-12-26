/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inttypes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp32/rom/cache.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/himem.h"

#if CONFIG_SPIRAM_BANKSWITCH_ENABLE

//Fill memory with pseudo-random data generated from the given seed.
static void fill_mem_seed(int seed, void *mem, int len)
{
    uint32_t *p = (uint32_t *)mem;
    unsigned int rseed = seed ^ 0xa5a5a5a5;
    for (int i = 0; i < len / 4; i++) {
        *p++ = rand_r(&rseed);
    }
}

//Check the memory filled by fill_mem_seed. Returns true if the data is still intact.
static bool check_mem_seed(int seed, void *mem, int len)
{
    uint32_t *p = (uint32_t *)mem;
    unsigned int rseed = seed ^ 0xa5a5a5a5;
    for (int i = 0; i < len / 4; i++) {
        uint32_t ex = rand_r(&rseed);
        if (ex != *p) {
            printf("check_mem_seed: %p @ %p has 0x%08"PRIx32" expected 0x%08"PRIx32"\n", mem, p, *p, ex);
            return false;
        }
        p++;
    }
    return true;
}

//Allocate a himem region, fill it with data, check it and release it.
static bool test_region(int check_size, int seed)
{
    esp_himem_handle_t mh;
    esp_himem_rangehandle_t rh;
    bool ret = true;

    ESP_ERROR_CHECK(esp_himem_alloc(check_size, &mh));
    ESP_ERROR_CHECK(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ * 2, &rh));
    for (int i = 0; i < check_size; i += ESP_HIMEM_BLKSZ) {
        uint32_t *ptr = NULL;
        ESP_ERROR_CHECK(esp_himem_map(mh, rh, i, 0, ESP_HIMEM_BLKSZ, 0, (void**)&ptr));
        fill_mem_seed(i ^ seed, ptr, ESP_HIMEM_BLKSZ);
        ESP_ERROR_CHECK(esp_himem_unmap(rh, ptr, ESP_HIMEM_BLKSZ));
    }
    for (int i = 0; i < check_size; i += ESP_HIMEM_BLKSZ) {
        uint32_t *ptr;
        ESP_ERROR_CHECK(esp_himem_map(mh, rh, i, 0, ESP_HIMEM_BLKSZ, 0, (void**)&ptr));
        if (!check_mem_seed(i ^ seed, ptr, ESP_HIMEM_BLKSZ)) {
            printf("Error in block %d\n", i / ESP_HIMEM_BLKSZ);
            ret = false;
            break;
        }
        ESP_ERROR_CHECK(esp_himem_unmap(rh, ptr, ESP_HIMEM_BLKSZ));
    }
    ESP_ERROR_CHECK(esp_himem_free(mh));
    ESP_ERROR_CHECK(esp_himem_free_map_range(rh));
    return ret;
}

static volatile int testsDone;

static void memtest_thread(void *arg)
{
    int d = (int)arg;
    for (int i = 0; i < 4; i++) {
        printf("d=%d check=%d\n", d, i);
        test_region(2 * 1024 * 1024, d + (i << 16));
        vTaskDelay(d);
    }
    testsDone++; //note possible race here... not really an issue if the two tasks have different vTaskDelay args
    vTaskDelete(NULL);
}

TEST_CASE("high psram memory test", "[himem]")
{
    printf("Doing single-core test\n");
    assert(test_region(4 * 1024 * 1024, 0xaaaa));

    testsDone = 0;
    printf("Doing dual-core test...\n");
    xTaskCreatePinnedToCore(&memtest_thread, "th2", 1024 * 2, (void *)2, 5, NULL, 1);
    xTaskCreatePinnedToCore(&memtest_thread, "th1", 1024 * 2, (void *)5, 5, NULL, 0);
    while (testsDone != 2) {
        vTaskDelay(10);
    }

    printf("Done!\n");
    vTaskDelay(100);
}

#define CACHE_BLOCKSIZE     (32*1024)
#define ALLOCATIONS_SIZE    (CACHE_BLOCKSIZE / 2)
#define MAX_ALLOCATIONS     ((4*1024*1024) / ALLOCATIONS_SIZE)

TEST_CASE("psram heap doesn't affect himem", "[himem]")
{
    esp_himem_handle_t mh;
    esp_himem_rangehandle_t rh;
    uint32_t *ptr = NULL;
    /* Array containing all the areas from the heap */
    void* allocs[MAX_ALLOCATIONS] = { 0 };
    int allocs_idx = 0;

    /* Allocate a physical block from the PSRAM */
    ESP_ERROR_CHECK(esp_himem_alloc(CACHE_BLOCKSIZE, &mh));
    /* Allocate a range of virtual memory where we can map the allocated block */
    ESP_ERROR_CHECK(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &rh));
    /* Map that physical block in the allocated virtual memory */
    ESP_ERROR_CHECK(esp_himem_map(mh, rh, 0, 0, ESP_HIMEM_BLKSZ, 0, (void**)&ptr));
    /* Memset that part of the memory with an arbitrary pattern */
    memset(ptr, 0x42, CACHE_BLOCKSIZE);

    /* Allocate heap memory in PSRAM until there is no more memory */
    while (1) {
        uint8_t* data = heap_caps_malloc(ALLOCATIONS_SIZE, MALLOC_CAP_SPIRAM);
        /* If the allocation failed, there is no more memory, we can break */
        if (data == NULL) {
            break;
        }
        assert(allocs_idx < MAX_ALLOCATIONS);
        allocs[allocs_idx++] = data;
        /* Set the allocated memory to another pattern */
        memset(data, 0xAB, ALLOCATIONS_SIZE);
    }

    /* Make sure we allocated more than 127 blocks */
    printf("Allocated %d blocks\n", allocs_idx);
    TEST_ASSERT_GREATER_THAN_INT(127, allocs_idx);

    /* Check if the himem block has been altered by the heap */
    TEST_ASSERT_EACH_EQUAL_INT8(0x42, ptr, CACHE_BLOCKSIZE);

    /* Free all the allocated memories */
    for (int i = 0; i < allocs_idx; i++) {
        heap_caps_free(allocs[i]);
    }

    ESP_ERROR_CHECK(esp_himem_unmap(rh, ptr, ESP_HIMEM_BLKSZ));
    ESP_ERROR_CHECK(esp_himem_free(mh));
    ESP_ERROR_CHECK(esp_himem_free_map_range(rh));

    printf("Success!\n");
}

#endif

#endif // CONFIG_IDF_TARGET_ESP32
