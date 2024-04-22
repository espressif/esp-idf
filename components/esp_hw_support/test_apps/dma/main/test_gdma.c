/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include <sys/param.h>
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
#include "hal/cache_hal.h"
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

    // allocate the source and destination buffer from SRAM
    // |--------------------------------------------------|
    // | 128 bytes DMA descriptor | 128 bytes data buffer |
    // |--------------------------------------------------|
    size_t sram_alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    size_t alignment = MAX(sram_alignment, 8);
    uint8_t *src_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dst_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    TEST_ASSERT_NOT_NULL(dst_buf);
    dma_descriptor_align8_t *tx_descs = (dma_descriptor_align8_t *) src_buf;
    dma_descriptor_align8_t *rx_descs = (dma_descriptor_align8_t *) dst_buf;
    uint8_t *src_data = src_buf + 128;
    uint8_t *dst_data = dst_buf + 128;

    // prepare the source data
    for (int i = 0; i < 128; i++) {
        src_data[i] = i;
    }
    if (sram_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync((void *)src_data, 128, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }

#ifdef CACHE_LL_L2MEM_NON_CACHE_ADDR
    dma_descriptor_align8_t *tx_descs_nc = (dma_descriptor_align8_t *)(CACHE_LL_L2MEM_NON_CACHE_ADDR(tx_descs));
    dma_descriptor_align8_t *rx_descs_nc = (dma_descriptor_align8_t *)(CACHE_LL_L2MEM_NON_CACHE_ADDR(rx_descs));
#else
    dma_descriptor_align8_t *tx_descs_nc = tx_descs;
    dma_descriptor_align8_t *rx_descs_nc = rx_descs;
#endif

    tx_descs_nc[0].buffer = src_data;
    tx_descs_nc[0].dw0.size = 64;
    tx_descs_nc[0].dw0.length = 64;
    tx_descs_nc[0].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_descs_nc[0].dw0.suc_eof = 0;
    tx_descs_nc[0].next = &tx_descs[1]; // Note, the DMA doesn't recognize a non-cacheable address, here must be the cached address

    tx_descs_nc[1].buffer = src_data + 64;
    tx_descs_nc[1].dw0.size = 64;
    tx_descs_nc[1].dw0.length = 64;
    tx_descs_nc[1].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_descs_nc[1].dw0.suc_eof = 1;
    tx_descs_nc[1].next = NULL;

    rx_descs_nc->buffer = dst_data;
    rx_descs_nc->dw0.size = 128;
    rx_descs_nc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    rx_descs_nc->dw0.suc_eof = 1;
    rx_descs_nc->next = NULL;

    TEST_ESP_OK(gdma_start(rx_chan, (intptr_t)rx_descs));
    TEST_ESP_OK(gdma_start(tx_chan, (intptr_t)tx_descs));

    xSemaphoreTake(done_sem, portMAX_DELAY);

    if (sram_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_data, 128, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }

    // check the DMA descriptor write-back feature
    TEST_ASSERT_EQUAL(DMA_DESCRIPTOR_BUFFER_OWNER_CPU, tx_descs_nc[0].dw0.owner);
    TEST_ASSERT_EQUAL(DMA_DESCRIPTOR_BUFFER_OWNER_CPU, rx_descs_nc[0].dw0.owner);

    for (int i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL(i, dst_data[i]);
    }
    free((void *)src_buf);
    free((void *)dst_buf);
    vSemaphoreDelete(done_sem);
}

TEST_CASE("GDMA M2M Mode", "[GDMA][M2M]")
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
