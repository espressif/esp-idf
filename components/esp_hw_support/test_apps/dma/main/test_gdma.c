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
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"
#include "hal/dma_types.h"
#include "soc/soc_caps.h"
#include "hal/gdma_ll.h"
#include "hal/cache_ll.h"
#include "hal/cache_hal.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"
#include "gdma_test_utils.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))

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

static void test_gdma_config_link_list(gdma_channel_handle_t tx_chan, gdma_channel_handle_t rx_chan,
                                     gdma_link_list_handle_t *tx_link_list, gdma_link_list_handle_t *rx_link_list, size_t sram_alignment, bool dma_link_in_ext_mem)
{

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

    // create DMA link list for TX channel (a singly link with 3 nodes)
    gdma_link_list_config_t tx_link_list_config = {
        .buffer_alignment = 1,
        .item_alignment = 8, // 8-byte alignment required by the AXI-GDMA
        .num_items = 3,
        .flags = {
            .items_in_ext_mem = dma_link_in_ext_mem,
            .check_owner = true,
        }
    };
    TEST_ESP_OK(gdma_new_link_list(&tx_link_list_config, tx_link_list));
    // create DMA link list for RX channel
    gdma_link_list_config_t rx_link_list_config = {
        .buffer_alignment = sram_alignment, // RX buffer should be aligned to the cache line size, because we will do cache invalidate later
        .item_alignment = 8, // 8-byte alignment required by the AXI-GDMA
        .num_items = 5,
        .flags = {
            .items_in_ext_mem = dma_link_in_ext_mem,
            .check_owner = true,
        },
    };
    TEST_ESP_OK(gdma_new_link_list(&rx_link_list_config, rx_link_list));
}

static bool test_gdma_m2m_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    SemaphoreHandle_t done_sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(done_sem, &task_woken);
    return task_woken == pdTRUE;
}

static void test_gdma_m2m_transaction(gdma_channel_handle_t tx_chan, gdma_channel_handle_t rx_chan, bool dma_link_in_ext_mem, bool trig_retention_backup)
{
    size_t sram_alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = test_gdma_m2m_rx_eof_callback,
    };
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);
    TEST_ESP_OK(gdma_register_rx_event_callbacks(rx_chan, &rx_cbs, done_sem));

    gdma_link_list_handle_t tx_link_list = NULL;
    gdma_link_list_handle_t rx_link_list = NULL;
    test_gdma_config_link_list(tx_chan, rx_chan, &tx_link_list, &rx_link_list, sram_alignment, dma_link_in_ext_mem);

    // allocate the source buffer from SRAM
    uint8_t *src_data = heap_caps_calloc(1, 128, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_data);

    // allocate the destination buffer from SRAM
    uint8_t *dst_data = heap_caps_calloc(1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(dst_data);

    // prepare the source data
    for (int i = 0; i < 128; i++) {
        src_data[i] = i;
    }
    if (sram_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync(src_data, 128, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }
    // test DMA can read data from main flash
#if SOC_DMA_CAN_ACCESS_FLASH
    const char *src_string = "GDMA can read data from MSPI Flash";
    size_t src_string_len = strlen(src_string);
    TEST_ASSERT_TRUE(esp_ptr_in_drom(src_string));

    // Only gonna copy length = src_string_len, set the character after to be 0xFF as a canary
    dst_data[128 + src_string_len] = 0xFF;
    if (sram_alignment) {
        // do write-back for the dst data because it's in the cache
        TEST_ESP_OK(esp_cache_msync(dst_data, 256, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }
#endif

    gdma_buffer_mount_config_t tx_buf_mount_config[] = {
        [0] = {
            .buffer = src_data,
            .length = 64,
        },
        [1] = {
            .buffer = src_data + 64,
            .length = 64,
#if !SOC_DMA_CAN_ACCESS_FLASH
            .flags = {
                .mark_eof = true,
                .mark_final = true, // using singly list, so terminate the link here
            }
#endif
        },
#if SOC_DMA_CAN_ACCESS_FLASH
        [2] = {
            .buffer = (void *)src_string,
            .length = src_string_len,
            .flags = {
                .mark_eof = true,
                .mark_final = true, // using singly list, so terminate the link here
            }
        },
#endif
    };
    TEST_ESP_OK(gdma_link_mount_buffers(tx_link_list, 0, tx_buf_mount_config, sizeof(tx_buf_mount_config) / sizeof(gdma_buffer_mount_config_t), NULL));

    gdma_buffer_mount_config_t rx_buf_mount_config = {
        .buffer = dst_data,
        .length = 256,
    };
    TEST_ESP_OK(gdma_link_mount_buffers(rx_link_list, 0, &rx_buf_mount_config, 1, NULL));

    if (trig_retention_backup) {
        test_gdma_trigger_retention_backup(tx_chan, rx_chan);
    }

    TEST_ESP_OK(gdma_start(rx_chan, gdma_link_get_head_addr(rx_link_list)));
    TEST_ESP_OK(gdma_start(tx_chan, gdma_link_get_head_addr(tx_link_list)));

    xSemaphoreTake(done_sem, 1000 / portTICK_PERIOD_MS);

    if (sram_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_data, 256, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }

    // The owner bit should been written back by the DMA
    gdma_lli_owner_t owner = GDMA_LLI_OWNER_DMA;
    TEST_ESP_OK(gdma_link_get_owner(tx_link_list, 0, &owner));
    TEST_ASSERT_EQUAL(GDMA_LLI_OWNER_CPU, owner);
    TEST_ESP_OK(gdma_link_get_owner(rx_link_list, 0, &owner));
    TEST_ASSERT_EQUAL(GDMA_LLI_OWNER_CPU, owner);

    // validate the destination data
    for (int i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL(i, dst_data[i]);
    }
#if SOC_DMA_CAN_ACCESS_FLASH
    TEST_ASSERT_TRUE(dst_data[128 + src_string_len] == 0xFF);
    dst_data[128 + src_string_len] = '\0';
    TEST_ASSERT_TRUE(strcmp(src_string, (const char *)((uint32_t)dst_data + 128)) == 0);
#endif
    free(src_data);
    free(dst_data);
    TEST_ESP_OK(gdma_del_link_list(tx_link_list));
    TEST_ESP_OK(gdma_del_link_list(rx_link_list));
    vSemaphoreDelete(done_sem);
}

static void test_gdma_m2m_mode(bool trig_retention_backup)
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

    test_gdma_m2m_transaction(tx_chan, rx_chan, false, trig_retention_backup);

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

    // the AXI GDMA allows to put the DMA link list in the external memory
    test_gdma_m2m_transaction(tx_chan, rx_chan, true, trig_retention_backup);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif // SOC_AXI_GDMA_SUPPORTED
}

TEST_CASE("GDMA M2M Mode", "[GDMA][M2M]")
{
    test_gdma_m2m_mode(false);
#if SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // test again with retention
    test_gdma_m2m_mode(true);
#endif
}

typedef struct {
    SemaphoreHandle_t done_sem;
    dma_buffer_split_array_t *align_array;
    size_t split_alignment;
    bool need_invalidate;
} test_gdma_context_t;

static bool test_gdma_m2m_unalgined_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    test_gdma_context_t *user_ctx = (test_gdma_context_t*)user_data;
    for (int i = 0; i < 3; i++) {
        if(user_ctx->align_array->aligned_buffer[i].aligned_buffer && user_ctx->need_invalidate) {
            TEST_ESP_OK(esp_cache_msync(user_ctx->align_array->aligned_buffer[i].aligned_buffer, ALIGN_UP(user_ctx->align_array->aligned_buffer[i].length, user_ctx->split_alignment), ESP_CACHE_MSYNC_FLAG_DIR_M2C));
        }
    }
    TEST_ESP_OK(esp_dma_merge_aligned_buffers(user_ctx->align_array));
    xSemaphoreGiveFromISR(user_ctx->done_sem, &task_woken);
    return task_woken == pdTRUE;
}

static void test_gdma_m2m_unalgined_buffer_test(uint8_t *dst_data, uint8_t *src_data, size_t data_length, size_t offset_len, size_t split_alignment)
{
    TEST_ASSERT_NOT_NULL(src_data);
    TEST_ASSERT_NOT_NULL(dst_data);
    gdma_channel_handle_t tx_chan = NULL;
    gdma_channel_handle_t rx_chan = NULL;
    gdma_channel_alloc_config_t tx_chan_alloc_config = {};
    gdma_channel_alloc_config_t rx_chan_alloc_config = {};
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
    size_t sram_alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);

    gdma_link_list_handle_t tx_link_list = NULL;
    gdma_link_list_handle_t rx_link_list = NULL;
    test_gdma_config_link_list(tx_chan, rx_chan, &tx_link_list, &rx_link_list, sram_alignment, false);

    // prepare the source data
    for (int i = 0; i < data_length; i++) {
        src_data[i] = i;
    }
    if (sram_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync(src_data, ALIGN_UP(data_length, sram_alignment), ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }

    gdma_buffer_mount_config_t tx_buf_mount_config[] = {
        [0] = {
            .buffer = src_data,
            .length = data_length,
            .flags = {
                .mark_eof = true,
                .mark_final = true, // using singly list, so terminate the link here
            }
        }
    };
    TEST_ESP_OK(gdma_link_mount_buffers(tx_link_list, 0, tx_buf_mount_config, sizeof(tx_buf_mount_config) / sizeof(gdma_buffer_mount_config_t), NULL));

    // allocate stash_buffer, should be freed by the user
    void *stash_buffer = heap_caps_aligned_calloc(split_alignment, 2, split_alignment, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t stash_buffer_len = 2 * split_alignment;
    dma_buffer_split_array_t align_array = {0};
    gdma_buffer_mount_config_t rx_aligned_buf_mount_config[3] = {0};

    TEST_ESP_OK(esp_dma_split_buffer_to_aligned(dst_data + offset_len, data_length, stash_buffer, stash_buffer_len, split_alignment, &align_array));
    for (int i = 0; i < 3; i++) {
        rx_aligned_buf_mount_config[i].buffer = align_array.aligned_buffer[i].aligned_buffer;
        rx_aligned_buf_mount_config[i].length = align_array.aligned_buffer[i].length;
    }
    TEST_ESP_OK(gdma_link_mount_buffers(rx_link_list, 0, rx_aligned_buf_mount_config, 3, NULL));

    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = test_gdma_m2m_unalgined_rx_eof_callback,
    };
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);
    test_gdma_context_t user_ctx = {
        .done_sem = done_sem,
        .align_array = &align_array,
        .split_alignment = split_alignment,
        .need_invalidate = sram_alignment ? true : false,
    };
    TEST_ESP_OK(gdma_register_rx_event_callbacks(rx_chan, &rx_cbs, &user_ctx));

    TEST_ESP_OK(gdma_start(rx_chan, gdma_link_get_head_addr(rx_link_list)));
    TEST_ESP_OK(gdma_start(tx_chan, gdma_link_get_head_addr(tx_link_list)));

    xSemaphoreTake(done_sem, 1000 / portTICK_PERIOD_MS);

    // validate the destination data
    for (int i = 0; i < data_length; i++) {
        TEST_ASSERT_EQUAL(i % 256 , dst_data[i + offset_len]);
    }

    free(stash_buffer);
    TEST_ESP_OK(gdma_del_link_list(tx_link_list));
    TEST_ESP_OK(gdma_del_link_list(rx_link_list));
    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
    vSemaphoreDelete(done_sem);
}

TEST_CASE("GDMA M2M Unaligned RX Buffer Test", "[GDMA][M2M]")
{
    uint8_t *sbuf = heap_caps_aligned_calloc(64, 1, 10240, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dbuf = heap_caps_aligned_calloc(64, 1, 10240, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    size_t split_alignment = 64;
    // case buffer len less than buffer alignment
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 60, 0, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 60, 4, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 60, 2, split_alignment);

    // case buffer head aligned
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 246, 0, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 8182, 0, split_alignment);

    // case buffer tail aligned
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 246, 10, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 8182, 10, split_alignment);

    // case buffer unaligned
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 100, 10, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 10, 60, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 256, 10, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 8192, 10, split_alignment);

    // case buffer full aligned
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 256, 0, split_alignment);
    test_gdma_m2m_unalgined_buffer_test(dbuf, sbuf, 8192, 0, split_alignment);

    free(sbuf);
    free(dbuf);
}
