/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
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
#include "rom/cache.h"

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

    dma_descriptor_t *tx_desc = (dma_descriptor_t *) src_buf;
    dma_descriptor_t *rx_desc = (dma_descriptor_t *) dst_buf;
    uint8_t *src_data = src_buf + 64;
    uint8_t *dst_data = dst_buf + 64;

    for (int i = 0; i < 100; i++) {
        src_data[i] = i;
    }

    tx_desc->buffer = src_data;
    tx_desc->dw0.size = 100;
    tx_desc->dw0.length = 100;
    tx_desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    tx_desc->dw0.suc_eof = 1;
    tx_desc->next = NULL;

    rx_desc->buffer = dst_data;
    rx_desc->dw0.size = 100;
    rx_desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    rx_desc->next = NULL;

#if CONFIG_IDF_TARGET_ESP32P4
    // descriptors are in the cache, DMA engine may not see the changes, so do a write-back
    Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)tx_desc, sizeof(tx_desc));
    Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)rx_desc, sizeof(rx_desc));
    // do write-back for the source data
    Cache_WriteBack_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)src_data, 100);
#endif

    TEST_ESP_OK(gdma_start(rx_chan, (intptr_t)rx_desc));
    TEST_ESP_OK(gdma_start(tx_chan, (intptr_t)tx_desc));

    xSemaphoreTake(done_sem, portMAX_DELAY);

#if CONFIG_IDF_TARGET_ESP32P4
    // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
    Cache_Invalidate_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)dst_data, 100);
    // the DMA descriptors are updated by the DMA as well, so do an invalidate
    Cache_Invalidate_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)tx_desc, sizeof(tx_desc));
    Cache_Invalidate_Addr(CACHE_MAP_L1_DCACHE, (uint32_t)rx_desc, sizeof(rx_desc));
#endif

    // check the DMA descriptor write-back feature
    TEST_ASSERT_EQUAL(DMA_DESCRIPTOR_BUFFER_OWNER_CPU, tx_desc->dw0.owner);
    TEST_ASSERT_EQUAL(DMA_DESCRIPTOR_BUFFER_OWNER_CPU, rx_desc->dw0.owner);

    for (int i = 0; i < 100; i++) {
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
