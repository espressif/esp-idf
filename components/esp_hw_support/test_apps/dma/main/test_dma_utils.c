/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_log.h"
#include "esp_dma_utils.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/gdma.h"
#include "soc/soc_caps.h"

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

static const char *TAG = "test_dma_utils";


#if CONFIG_SPIRAM
/**
 * To test the API logic is correct, here we simply use max value under default sdkconfig
 */
#if CONFIG_IDF_TARGET_ESP32P4
#define TEST_BUFFER_PSRAM_ALIGNMENT        64
#else
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define TEST_BUFFER_PSRAM_ALIGNMENT        4
#else
#define TEST_BUFFER_PSRAM_ALIGNMENT        32
#endif
#endif

TEST_CASE("test esp_dma_capable_malloc for PSRAM", "[dma_utils]")
{
    size_t test_size = 0;
    void *test_ptr = NULL;
    size_t actual_size = 0;

    esp_dma_mem_info_t dma_mem_info = {
        .heap_caps = MALLOC_CAP_SPIRAM,
        .dma_alignment = 4,
        .custom_alignment = 4,
    };

    //------ psram ------//
    //aligned
    test_size = TEST_BUFFER_PSRAM_ALIGNMENT;
    ESP_LOGI(TAG, "to alloc 0x%zx", test_size);
    TEST_ESP_OK(esp_dma_capable_malloc(test_size, &dma_mem_info, &test_ptr, &actual_size));
    ESP_LOGI(TAG, "get test_ptr: %p, actual_size: 0x%zx", test_ptr, actual_size);
    TEST_ASSERT((uint32_t)test_ptr % TEST_BUFFER_PSRAM_ALIGNMENT == 0);
    TEST_ASSERT(test_size == actual_size);
    free(test_ptr);

    //unaligned
    test_size = TEST_BUFFER_PSRAM_ALIGNMENT + TEST_BUFFER_PSRAM_ALIGNMENT / 2;
    ESP_LOGI(TAG, "to alloc 0x%zx", test_size);
    TEST_ESP_OK(esp_dma_capable_malloc(test_size, &dma_mem_info, &test_ptr, &actual_size));
    ESP_LOGI(TAG, "get test_ptr: %p, actual_size: 0x%zx", test_ptr, actual_size);
    TEST_ASSERT((uint32_t)test_ptr % TEST_BUFFER_PSRAM_ALIGNMENT == 0);
    TEST_ASSERT(ALIGN_UP_BY(test_size, TEST_BUFFER_PSRAM_ALIGNMENT) == actual_size);
    free(test_ptr);
}
#endif

TEST_CASE("test custom alignment", "[dma_utils]")
{
    size_t test_size = 0;
    void *test_ptr = NULL;
    size_t actual_size = 0;
    size_t custom_alignment = 512;

    esp_dma_mem_info_t dma_mem_info = {
        .heap_caps = MALLOC_CAP_SPIRAM,
        .dma_alignment = 4,
        .custom_alignment = custom_alignment,
    };
    test_size = custom_alignment + 3;
    ESP_LOGI(TAG, "to alloc 0x%zx", test_size);
    TEST_ESP_OK(esp_dma_capable_malloc(test_size, &dma_mem_info, &test_ptr, &actual_size));
    ESP_LOGI(TAG, "get test_ptr: %p, actual_size: 0x%zx", test_ptr, actual_size);
    TEST_ASSERT((uint32_t)test_ptr % custom_alignment == 0);
    TEST_ASSERT(ALIGN_UP_BY(test_size, custom_alignment) == actual_size);
    free(test_ptr);
}

TEST_CASE("test esp_dma_is_buffer_alignment_satisfied", "[dma_utils]")
{
    size_t test_size = 64;
    void *test_ptr = NULL;

    esp_dma_mem_info_t dma_mem_info = {
        .heap_caps = MALLOC_CAP_DMA,
        .dma_alignment = 4,
    };
    TEST_ESP_OK(esp_dma_capable_malloc(test_size, &dma_mem_info, &test_ptr, NULL));
    ESP_LOGI(TAG, "test_ptr %p", test_ptr);
    bool is_aligned = esp_dma_is_buffer_alignment_satisfied(test_ptr, test_size, &dma_mem_info);
    TEST_ASSERT(is_aligned);
    is_aligned = esp_dma_is_buffer_alignment_satisfied(test_ptr + 3, test_size, &dma_mem_info);
    TEST_ASSERT(!is_aligned);
}


#if CONFIG_IDF_TARGET_ESP32P4
#define TEST_DMA_ALIGNMENT_INT    8
#else
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define TEST_DMA_ALIGNMENT_INT    4
#else
#define TEST_DMA_ALIGNMENT_INT    8
#endif
#endif

TEST_CASE("test esp_dma_get_alignment", "[dma_utils]")
{
    size_t dma_alignment = 0;
    gdma_channel_handle_t tx_channel = NULL;
#if SOC_GDMA_SUPPORTED
    gdma_channel_alloc_config_t channel_config = {};
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;

    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channel));

    gdma_transfer_ability_t ability = {
        .psram_trans_align = 0,
        .sram_trans_align = 8,
    };
    TEST_ESP_OK(gdma_set_transfer_ability(tx_channel, &ability));
#endif

    dma_alignment_info_t internal_info = {};

    TEST_ESP_OK(esp_dma_get_alignment(tx_channel, &internal_info, &dma_alignment));
    ESP_LOGI(TAG, "dma_alignment: 0x%x", dma_alignment);
    TEST_ASSERT(dma_alignment == TEST_DMA_ALIGNMENT_INT);
}

#if SOC_GDMA_SUPPORTED
TEST_CASE("test esp_dma_get_alignment with no transfer ability set", "[dma_utils]")
{
    size_t dma_alignment = 0;
    gdma_channel_handle_t tx_channel = NULL;
    gdma_channel_alloc_config_t channel_config = {};
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channel));

    dma_alignment_info_t internal_info = {};
    TEST_ESP_OK(esp_dma_get_alignment(tx_channel, &internal_info, &dma_alignment));
    ESP_LOGI(TAG, "dma_alignment: 0x%x", dma_alignment);
    TEST_ASSERT(dma_alignment == 4);
}
#endif
