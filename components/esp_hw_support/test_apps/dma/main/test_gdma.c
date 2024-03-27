/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "soc/soc_caps.h"
#include "hal/gdma_ll.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"

TEST_CASE("GDMA channel allocation", "[GDMA]")
{
    gdma_channel_alloc_config_t channel_config = {};
    gdma_channel_handle_t tx_channels[SOC_GDMA_PAIRS_PER_GROUP_MAX] = {};
    gdma_channel_handle_t rx_channels[SOC_GDMA_PAIRS_PER_GROUP_MAX] = {};
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;

#if SOC_AHB_GDMA_SUPPORTED
    // install TX channels
    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channels[i]));
    };
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_ahb_channel(&channel_config, &tx_channels[0]));

    // Free interrupts before installing RX interrupts to ensure enough free interrupts
    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
    }

    // install RX channels
    channel_config.direction = GDMA_CHANNEL_DIRECTION_RX;
    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &rx_channels[i]));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_ahb_channel(&channel_config, &rx_channels[0]));

    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // SOC_AHB_GDMA_SUPPORTED

    // install single and paired TX/RX channels
#if GDMA_LL_AHB_PAIRS_PER_GROUP >= 2
    // single tx channel
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channels[0]));

    // create tx channel and reserve sibling
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    channel_config.flags.reserve_sibling = 1;
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channels[1]));
    // create rx channel and specify sibling channel
    channel_config.flags.reserve_sibling = 0;
    channel_config.sibling_chan = tx_channels[1]; // specify sibling channel
    channel_config.direction = GDMA_CHANNEL_DIRECTION_RX;
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &rx_channels[1]));
    channel_config.sibling_chan = NULL;
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &rx_channels[0]));

    TEST_ESP_OK(gdma_connect(tx_channels[0], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0)));
    // can't connect multiple channels to the same peripheral
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gdma_connect(tx_channels[1], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0)));
    TEST_ESP_OK(gdma_connect(tx_channels[1], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0)));

    TEST_ESP_OK(gdma_connect(rx_channels[0], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0)));
    // but rx and tx can connect to the same peripheral
    TEST_ESP_OK(gdma_connect(rx_channels[1], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0)));
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(gdma_disconnect(tx_channels[i]));
        TEST_ESP_OK(gdma_disconnect(rx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // GDMA_LL_AHB_PAIRS_PER_GROUP >= 2

#if SOC_AXI_GDMA_SUPPORTED
    // install TX channels
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &tx_channels[i]));
    };
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_axi_channel(&channel_config, &tx_channels[0]));

    // Free interrupts before installing RX interrupts to ensure enough free interrupts
    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
    }

    // install RX channels
    channel_config.direction = GDMA_CHANNEL_DIRECTION_RX;
    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &rx_channels[i]));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_axi_channel(&channel_config, &rx_channels[0]));

    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // SOC_AXI_GDMA_SUPPORTED

    // install single and paired TX/RX channels
#if GDMA_LL_AXI_PAIRS_PER_GROUP >= 2
    // single tx channel
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &tx_channels[0]));

    // create tx channel and reserve sibling
    channel_config.direction = GDMA_CHANNEL_DIRECTION_TX;
    channel_config.flags.reserve_sibling = 1;
    TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &tx_channels[1]));
    // create rx channel and specify sibling channel
    channel_config.flags.reserve_sibling = 0;
    channel_config.sibling_chan = tx_channels[1]; // specify sibling channel
    channel_config.direction = GDMA_CHANNEL_DIRECTION_RX;
    TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &rx_channels[1]));
    channel_config.sibling_chan = NULL;
    TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &rx_channels[0]));

    TEST_ESP_OK(gdma_connect(tx_channels[0], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SPI, 2)));
    // can't connect multiple channels to the same peripheral
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gdma_connect(tx_channels[1], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SPI, 2)));
    TEST_ESP_OK(gdma_connect(tx_channels[1], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0)));

    TEST_ESP_OK(gdma_connect(rx_channels[0], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SPI, 2)));
    // but rx and tx can connect to the same peripheral
    TEST_ESP_OK(gdma_connect(rx_channels[1], GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0)));
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(gdma_disconnect(tx_channels[i]));
        TEST_ESP_OK(gdma_disconnect(rx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // GDMA_LL_AXI_PAIRS_PER_GROUP >= 2
}

static bool test_gdma_m2m_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    SemaphoreHandle_t done_sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(done_sem, &task_woken);
    return task_woken == pdTRUE;
}

static void test_gdma_m2m_mode(gdma_channel_handle_t tx_chan, gdma_channel_handle_t rx_chan)
{
    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = test_gdma_m2m_rx_eof_callback,
    };
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ESP_OK(gdma_register_rx_event_callbacks(rx_chan, &rx_cbs, done_sem));

    gdma_strategy_config_t strategy = {
        .auto_update_desc = true,
        .owner_check = true,
    };
    TEST_ESP_OK(gdma_apply_strategy(tx_chan, &strategy));
    TEST_ESP_OK(gdma_apply_strategy(rx_chan, &strategy));

    gdma_trigger_t m2m_trigger = GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0);
    // get a free DMA trigger ID for memory copy
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(tx_chan, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    TEST_ESP_OK(gdma_connect(tx_chan, m2m_trigger));
    TEST_ESP_OK(gdma_connect(rx_chan, m2m_trigger));

    uint8_t *src_buf = heap_caps_aligned_alloc(64, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dst_buf = heap_caps_aligned_alloc(64, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    TEST_ASSERT_NOT_NULL(dst_buf);
    memset(src_buf, 0, 256);
    memset(dst_buf, 0, 256);

    dma_descriptor_align8_t *tx_descs = (dma_descriptor_align8_t *) src_buf;
    dma_descriptor_align8_t *rx_descs = (dma_descriptor_align8_t *) dst_buf;
    uint8_t *src_data = src_buf + 64;
    uint8_t *dst_data = dst_buf + 64;

    // prepare the source data
    for (int i = 0; i < 128; i++) {
        src_data[i] = i;
    }

#if CONFIG_IDF_TARGET_ESP32P4
    // CPU and DMA both can write to the DMA descriptor, so if there is a cache, multiple descriptors may reside in the same cache line
    // causing data inconsistency. To avoid this, we want to access the descriptor memory without the cache.
    dma_descriptor_align8_t *tx_descs_noncache = (dma_descriptor_align8_t *)(CACHE_LL_L2MEM_NON_CACHE_ADDR(tx_descs));
    dma_descriptor_align8_t *rx_descs_noncache = (dma_descriptor_align8_t *)(CACHE_LL_L2MEM_NON_CACHE_ADDR(rx_descs));

    tx_descs_noncache[0].buffer = src_data;
    tx_descs_noncache[0].dw0.size = 64;
    tx_descs_noncache[0].dw0.length = 64;
    tx_descs_noncache[0].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_descs_noncache[0].dw0.suc_eof = 0;
    tx_descs_noncache[0].next = &tx_descs[1]; // Note, the DMA doesn't recognize a non-cacheable address, here must be the cached address

    tx_descs_noncache[1].buffer = src_data + 64;
    tx_descs_noncache[1].dw0.size = 64;
    tx_descs_noncache[1].dw0.length = 64;
    tx_descs_noncache[1].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_descs_noncache[1].dw0.suc_eof = 1;
    tx_descs_noncache[1].next = NULL;

    rx_descs_noncache->buffer = dst_data;
    rx_descs_noncache->dw0.size = 128;
    rx_descs_noncache->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    rx_descs_noncache->dw0.suc_eof = 1;
    rx_descs_noncache->next = NULL;
#else
    tx_descs->buffer = src_data;
    tx_descs->dw0.size = 128;
    tx_descs->dw0.length = 128;
    tx_descs->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_descs->dw0.suc_eof = 1;
    tx_descs->next = NULL;

    rx_descs->buffer = dst_data;
    rx_descs->dw0.size = 128;
    rx_descs->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    rx_descs->next = NULL;
#endif

#if CONFIG_IDF_TARGET_ESP32P4
    // do write-back for the source data because it's in the cache
    TEST_ESP_OK(esp_cache_msync((void *)src_data, 128, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
#endif

    TEST_ESP_OK(gdma_start(rx_chan, (intptr_t)rx_descs));
    TEST_ESP_OK(gdma_start(tx_chan, (intptr_t)tx_descs));

    xSemaphoreTake(done_sem, portMAX_DELAY);

#if CONFIG_IDF_TARGET_ESP32P4
    // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
    TEST_ESP_OK(esp_cache_msync((void *)dst_data, 128, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
#endif

    // check the DMA descriptor write-back feature
    TEST_ASSERT_EQUAL(DMA_DESCRIPTOR_BUFFER_OWNER_CPU, tx_descs[0].dw0.owner);
    TEST_ASSERT_EQUAL(DMA_DESCRIPTOR_BUFFER_OWNER_CPU, rx_descs[0].dw0.owner);

    for (int i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL(i, dst_data[i]);
    }
    free((void *)src_buf);
    free((void *)dst_buf);
    vSemaphoreDelete(done_sem);
}

TEST_CASE("GDMA M2M Mode", "[GDMA]")
{
    gdma_channel_handle_t tx_chan = NULL;
    gdma_channel_handle_t rx_chan = NULL;
    gdma_channel_alloc_config_t tx_chan_alloc_config = {};
    gdma_channel_alloc_config_t rx_chan_alloc_config = {};

#if SOC_AHB_GDMA_SUPPORTED
    tx_chan_alloc_config = (gdma_channel_alloc_config_t) {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
        .flags.reserve_sibling = true,
    };
    TEST_ESP_OK(gdma_new_ahb_channel(&tx_chan_alloc_config, &tx_chan));
    rx_chan_alloc_config = (gdma_channel_alloc_config_t) {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = tx_chan,
    };
    TEST_ESP_OK(gdma_new_ahb_channel(&rx_chan_alloc_config, &rx_chan));

    test_gdma_m2m_mode(tx_chan, rx_chan);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif // SOC_AHB_GDMA_SUPPORTED

#if SOC_AXI_GDMA_SUPPORTED
    tx_chan_alloc_config = (gdma_channel_alloc_config_t) {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
        .flags.reserve_sibling = true,
    };
    TEST_ESP_OK(gdma_new_axi_channel(&tx_chan_alloc_config, &tx_chan));
    rx_chan_alloc_config = (gdma_channel_alloc_config_t) {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = tx_chan,
    };
    TEST_ESP_OK(gdma_new_axi_channel(&rx_chan_alloc_config, &rx_chan));

    test_gdma_m2m_mode(tx_chan, rx_chan);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif // SOC_AXI_GDMA_SUPPORTED
}

#if SOC_GDMA_SUPPORT_CRC
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
    printf("Calculate CRC value for string: \"%s\"\r\n", test_input_string);

    gdma_trigger_t m2m_trigger = GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0);
    // get a free DMA trigger ID
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(tx_chan, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    TEST_ESP_OK(gdma_connect(tx_chan, m2m_trigger));

    uint8_t *src_buf = heap_caps_aligned_calloc(64, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    dma_descriptor_align8_t *tx_descs = (dma_descriptor_align8_t *) src_buf;
    uint8_t *src_data = src_buf + 64;
    memcpy(src_data, test_input_string, input_data_size);

    tx_descs->buffer = src_data;
    tx_descs->dw0.size = 256 - 64;
    tx_descs->dw0.length = input_data_size;
    tx_descs->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_descs->dw0.suc_eof = 1;
    tx_descs->next = NULL;

#if CONFIG_IDF_TARGET_ESP32P4
    // do write-back for the buffer because it's in the cache
    Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)src_buf, 256);
#endif

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

    free(src_buf);
}

TEST_CASE("GDMA CRC Calculation", "[GDMA]")
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
#endif // SOC_GDMA_SUPPORT_CRC
