/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "soc/soc_caps.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"

typedef struct {
    uint32_t init_value;
    uint32_t crc_bit_width;
    uint32_t poly_hex;
    bool reverse_data_mask;
    uint32_t expected_result;
} test_crc_case_t;
static test_crc_case_t crc_test_cases[] = {
    // CRC8, x^8+x^2+x+1
    [0] = {
        .crc_bit_width = 8,
        .init_value = 0x00,
        .poly_hex = 0x07,
        .expected_result = 0xC6,
    },
    [1] = {
        .crc_bit_width = 8,
        .init_value = 0x00,
        .poly_hex = 0x07,
        .reverse_data_mask = true, // refin = true
        .expected_result = 0xDE,
    },
    // CRC16, x^16+x^12+x^5+1
    [2] = {
        .crc_bit_width = 16,
        .init_value = 0xFFFF,
        .poly_hex = 0x1021,
        .expected_result = 0x5289,
    },
    // CRC32, x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
    [3] = {
        .crc_bit_width = 32,
        .init_value = 0xFFFFFFFF,
        .poly_hex = 0x04C11DB7,
        .expected_result = 0x63B3E283,
    }
};

// CRC online: https://www.lddgo.net/en/encrypt/crc
static void test_gdma_crc_calculation(gdma_channel_handle_t tx_chan, int test_num_crc_algorithm)
{
    uint32_t crc_result = 0;
    const char *test_input_string = "Share::Connect::Innovate";
    size_t input_data_size = strlen(test_input_string);
    // this test case also test the GDMA can fetch data from MSPI Flash
    TEST_ASSERT_TRUE(esp_ptr_in_drom(test_input_string));
    printf("Calculate CRC value for string: \"%s\"\r\n", test_input_string);

    gdma_trigger_t m2m_trigger = GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0);
    // get a free DMA trigger ID
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(tx_chan, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    TEST_ESP_OK(gdma_connect(tx_chan, m2m_trigger));

    size_t sram_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    size_t alignment = MAX(sram_cache_line_size, 8);
    dma_descriptor_align8_t *tx_descs = heap_caps_aligned_calloc(alignment, 1, sizeof(dma_descriptor_align8_t),
                                                                 MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(tx_descs);

    tx_descs->buffer = (void *)test_input_string;
    tx_descs->dw0.size = input_data_size + 1; // +1 for '\0'
    tx_descs->dw0.length = input_data_size;
    tx_descs->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_descs->dw0.suc_eof = 1;
    tx_descs->next = NULL;

    if (sram_cache_line_size) {
        // do write-back for the buffer because it's in the cache
        TEST_ESP_OK(esp_cache_msync((void *)tx_descs, sizeof(dma_descriptor_align8_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
    }

    for (int i = 0; i < test_num_crc_algorithm; i++) {
        gdma_crc_calculator_config_t crc_config = {
            .crc_bit_width = crc_test_cases[i].crc_bit_width,
            .init_value = crc_test_cases[i].init_value,
            .poly_hex = crc_test_cases[i].poly_hex,
            .reverse_data_mask = crc_test_cases[i].reverse_data_mask,
        };
        TEST_ESP_OK(gdma_config_crc_calculator(tx_chan, &crc_config));
        TEST_ESP_OK(gdma_reset(tx_chan));
        TEST_ESP_OK(gdma_start(tx_chan, (intptr_t)tx_descs));
        // simply wait for the transfer done
        vTaskDelay(pdMS_TO_TICKS(100));
        TEST_ESP_OK(gdma_crc_get_result(tx_chan, &crc_result));
        printf("CRC Result: 0x%"PRIx32"\r\n", crc_result);
        TEST_ASSERT_EQUAL(crc_test_cases[i].expected_result, crc_result);
    }

    free(tx_descs);
}

TEST_CASE("GDMA CRC Calculation", "[GDMA][CRC]")
{
    gdma_channel_handle_t tx_chan = NULL;
    gdma_channel_alloc_config_t tx_chan_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
#if SOC_AHB_GDMA_SUPPORTED
    printf("Test CRC calculation for AHB GDMA\r\n");
    TEST_ESP_OK(gdma_new_ahb_channel(&tx_chan_alloc_config, &tx_chan));
    test_gdma_crc_calculation(tx_chan, 4);
    TEST_ESP_OK(gdma_del_channel(tx_chan));
#endif // SOC_AHB_GDMA_SUPPORTED

#if SOC_AXI_GDMA_SUPPORTED
    printf("Test CRC calculation for AXI GDMA\r\n");
    TEST_ESP_OK(gdma_new_axi_channel(&tx_chan_alloc_config, &tx_chan));
    test_gdma_crc_calculation(tx_chan, 3);
    TEST_ESP_OK(gdma_del_channel(tx_chan));
#endif // SOC_AXI_GDMA_SUPPORTED
}
