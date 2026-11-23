/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "esp_crypto_lock.h"
#include "esp_partition.h"

// There should be no memory allocated during the test.
#define TEST_MEMORY_INITIAL_LEAK_THRESHOLD (-700)


static size_t before_free_8bit;
static size_t before_free_32bit;
static ssize_t actual_leak_threshold = TEST_MEMORY_INITIAL_LEAK_THRESHOLD;


static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d)\n", type, before_free, after_free, delta);
    TEST_ASSERT_MESSAGE(delta >= actual_leak_threshold, "memory leak");
}

void setUp(void)
{
    /* Lazy-init crypto locks (FreeRTOS mutexes) before measuring free heap.
     * First HMAC use otherwise allocates ~248 bytes and trips the leak check.
     */
#if SOC_HMAC_SUPPORTED
    esp_crypto_hmac_lock_acquire();
    esp_crypto_hmac_lock_release();
#endif

    /* Partition-list and MMU mutexes are created on first _lock_acquire().
     * Startup loads the partition table before the scheduler starts, when
     * that call does not allocate, so the first test otherwise keeps the
     * partition lock (~124 bytes) and the first encrypted read keeps the
     * MMU lock (another ~124 bytes). Fail if this warmup cannot run, so a
     * missing partition does not hide the leak again.
     */
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                           ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    TEST_ASSERT_NOT_NULL(part);
    const void *mapped;
    esp_partition_mmap_handle_t handle;
    TEST_ESP_OK(esp_partition_mmap(part, 0, 1, ESP_PARTITION_MMAP_DATA, &mapped, &handle));
    esp_partition_munmap(handle);

    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");

    // Reset the threshold to the zero after initial check
    actual_leak_threshold = 0;
}

void app_main(void)
{
    printf("Running nvs bootloader support component tests\n");
    unity_run_menu();
}
