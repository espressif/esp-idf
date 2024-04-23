/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "inttypes.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "unity.h"

#include "esp_private/esp_cache_private.h"

#include "esp_memory_utils.h"

const static char *TAG = "CACHE_MALLOC_TEST";

TEST_CASE("test esp_cache_aligned_malloc_prefer", "[cache]")
{
    void *ptr = NULL;
    size_t actual_size = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    TEST_ESP_OK(esp_cache_aligned_malloc_prefer(40, &ptr, &actual_size, 1, MALLOC_CAP_DMA, 0));
#pragma GCC diagnostic pop
    TEST_ASSERT(esp_ptr_dma_capable(ptr));
    ESP_LOGI(TAG, "actual size: 0x%x", actual_size);

    free(ptr);
}

TEST_CASE("test esp_cache_aligned_calloc_prefer", "[cache]")
{
    void *ptr = NULL;
    size_t actual_size = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    TEST_ESP_OK(esp_cache_aligned_calloc_prefer(1, 40, &ptr, &actual_size, 1, MALLOC_CAP_DMA, 0));
#pragma GCC diagnostic pop
    TEST_ASSERT(esp_ptr_dma_capable(ptr));
    ESP_LOGI(TAG, "actual size: 0d%d", actual_size);

    free(ptr);
}
