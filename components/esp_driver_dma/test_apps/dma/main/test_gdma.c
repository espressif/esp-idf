/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
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
#include "hal/efuse_hal.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"
#include "gdma_test_utils.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))

TEST_CASE("GDMA channel allocation", "[GDMA]")
{
    gdma_channel_alloc_config_t channel_config = {};
    gdma_channel_handle_t tx_channels[GDMA_LL_GET(PAIRS_PER_INST)] = {};
    gdma_channel_handle_t rx_channels[GDMA_LL_GET(PAIRS_PER_INST)] = {};

#if SOC_HAS(AHB_GDMA)
    // install TX channels
    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channels[i], NULL));
    };
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_ahb_channel(&channel_config, &tx_channels[0], NULL));

    // Free interrupts before installing RX interrupts to ensure enough free interrupts
    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
    }

    // install RX channels
    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, NULL, &rx_channels[i]));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_ahb_channel(&channel_config, NULL, &rx_channels[0]));

    for (int i = 0; i < GDMA_LL_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // SOC_HAS(AHB_GDMA)

    // install single and paired TX/RX channels
#if GDMA_LL_AHB_PAIRS_PER_GROUP >= 2
    // single tx channel
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channels[0], NULL));

    // create tx and rx channel pair
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, &tx_channels[1], &rx_channels[1]));
    // create single rx channel
    TEST_ESP_OK(gdma_new_ahb_channel(&channel_config, NULL, &rx_channels[0]));

    gdma_trigger_t fake_ahb_trigger1 = {
        .bus_id = SOC_GDMA_BUS_AHB,
        .instance_id = 0,
    };
    gdma_trigger_t fake_ahb_trigger2 = {
        .bus_id = SOC_GDMA_BUS_AHB,
        .instance_id = 1,
    };
    TEST_ESP_OK(gdma_connect(tx_channels[0], fake_ahb_trigger1));
    // can't connect multiple channels to the same peripheral
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gdma_connect(tx_channels[1], fake_ahb_trigger1));
    TEST_ESP_OK(gdma_connect(tx_channels[1], fake_ahb_trigger2));

    // but rx and tx can connect to the same peripheral
    TEST_ESP_OK(gdma_connect(rx_channels[0], fake_ahb_trigger1));
    TEST_ESP_OK(gdma_connect(rx_channels[1], fake_ahb_trigger2));
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(gdma_disconnect(tx_channels[i]));
        TEST_ESP_OK(gdma_disconnect(rx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // GDMA_LL_AHB_PAIRS_PER_GROUP >= 2

#if SOC_HAS(AXI_GDMA)
    // install TX channels
    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &tx_channels[i], NULL));
    };
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_axi_channel(&channel_config, &tx_channels[0], NULL));

    // Free interrupts before installing RX interrupts to ensure enough free interrupts
    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
    }

    // install RX channels
    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_axi_channel(&channel_config, NULL, &rx_channels[i]));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_axi_channel(&channel_config, NULL, &rx_channels[0]));

    for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // SOC_HAS(AXI_GDMA)

    // install single and paired TX/RX channels
#if GDMA_LL_AXI_PAIRS_PER_GROUP >= 2
    // single tx channel
    TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &tx_channels[0], NULL));

    // create tx and rx channel pair
    TEST_ESP_OK(gdma_new_axi_channel(&channel_config, &tx_channels[1], &rx_channels[1]));
    // create single rx channel
    TEST_ESP_OK(gdma_new_axi_channel(&channel_config, NULL, &rx_channels[0]));

    gdma_trigger_t fake_axi_trigger1 = {
        .bus_id = SOC_GDMA_BUS_AXI,
        .instance_id = 0,
    };
    gdma_trigger_t fake_axi_trigger2 = {
        .bus_id = SOC_GDMA_BUS_AXI,
        .instance_id = 1,
    };
    TEST_ESP_OK(gdma_connect(tx_channels[0], fake_axi_trigger1));
    // can't connect multiple channels to the same peripheral
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gdma_connect(tx_channels[1], fake_axi_trigger1));
    TEST_ESP_OK(gdma_connect(tx_channels[1], fake_axi_trigger2));

    // but rx and tx can connect to the same peripheral
    TEST_ESP_OK(gdma_connect(rx_channels[0], fake_axi_trigger1));
    TEST_ESP_OK(gdma_connect(rx_channels[1], fake_axi_trigger2));
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(gdma_disconnect(tx_channels[i]));
        TEST_ESP_OK(gdma_disconnect(rx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // GDMA_LL_AXI_PAIRS_PER_GROUP >= 2

#if SOC_HAS(LP_AHB_GDMA)
    // install TX channels
    for (int i = 0; i < GDMA_LL_LP_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_lp_ahb_channel(&channel_config, &tx_channels[i], NULL));
    };
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_lp_ahb_channel(&channel_config, &tx_channels[0], NULL));

    // Free interrupts before installing RX interrupts to ensure enough free interrupts
    for (int i = 0; i < GDMA_LL_LP_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
    }

    // install RX channels
    for (int i = 0; i < GDMA_LL_LP_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_new_lp_ahb_channel(&channel_config, NULL, &rx_channels[i]));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gdma_new_lp_ahb_channel(&channel_config, NULL, &rx_channels[0]));

    for (int i = 0; i < GDMA_LL_LP_AHB_PAIRS_PER_GROUP; i++) {
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // SOC_HAS(LP_AHB_GDMA)

    // install single and paired TX/RX channels
#if GDMA_LL_LP_AHB_PAIRS_PER_GROUP >= 2
    // single tx channel
    TEST_ESP_OK(gdma_new_lp_ahb_channel(&channel_config, &tx_channels[0], NULL));

    // create tx and rx channel pair
    TEST_ESP_OK(gdma_new_lp_ahb_channel(&channel_config, &tx_channels[1], &rx_channels[1]));
    // create single rx channel
    TEST_ESP_OK(gdma_new_lp_ahb_channel(&channel_config, NULL, &rx_channels[0]));

    gdma_trigger_t fake_lp_ahb_trigger1 = {
        .bus_id = SOC_GDMA_BUS_LP,
        .instance_id = 0,
    };
    gdma_trigger_t fake_lp_ahb_trigger2 = {
        .bus_id = SOC_GDMA_BUS_LP,
        .instance_id = 1,
    };
    TEST_ESP_OK(gdma_connect(tx_channels[0], fake_lp_ahb_trigger1));
    // can't connect multiple channels to the same peripheral
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gdma_connect(tx_channels[1], fake_lp_ahb_trigger1));
    TEST_ESP_OK(gdma_connect(tx_channels[1], fake_lp_ahb_trigger2));

    // but rx and tx can connect to the same peripheral
    TEST_ESP_OK(gdma_connect(rx_channels[0], fake_lp_ahb_trigger1));
    TEST_ESP_OK(gdma_connect(rx_channels[1], fake_lp_ahb_trigger2));
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(gdma_disconnect(tx_channels[i]));
        TEST_ESP_OK(gdma_disconnect(rx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(tx_channels[i]));
        TEST_ESP_OK(gdma_del_channel(rx_channels[i]));
    }
#endif // GDMA_LL_LP_AHB_PAIRS_PER_GROUP >= 2

}

static void test_gdma_config_link_list(gdma_channel_handle_t tx_chan, gdma_channel_handle_t rx_chan,
                                       gdma_link_list_handle_t *tx_link_list, gdma_link_list_handle_t *rx_link_list,
                                       size_t burst_size, bool dma_link_in_ext_mem)
{

    gdma_strategy_config_t strategy = {
        .auto_update_desc = true,
        .owner_check = true,
    };
    TEST_ESP_OK(gdma_apply_strategy(tx_chan, &strategy));
    TEST_ESP_OK(gdma_apply_strategy(rx_chan, &strategy));

    gdma_transfer_config_t transfer_cfg = {
        .max_data_burst_size = burst_size,
#if SOC_DMA_CAN_ACCESS_FLASH
        .access_ext_mem = true,
#endif
    };
    TEST_ESP_OK(gdma_config_transfer(tx_chan, &transfer_cfg));
    TEST_ESP_OK(gdma_config_transfer(rx_chan, &transfer_cfg));

    gdma_trigger_t m2m_trigger = GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0);
    // get a free DMA trigger ID for memory copy
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(tx_chan, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    TEST_ESP_OK(gdma_connect(tx_chan, m2m_trigger));
    TEST_ESP_OK(gdma_connect(rx_chan, m2m_trigger));

    // create DMA link list for TX channel (a singly link with 3 nodes)
    gdma_link_list_config_t tx_link_list_config = {
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

    if (efuse_hal_flash_encryption_enabled()) {
        dma_link_in_ext_mem = false;
    }

    gdma_link_list_handle_t tx_link_list = NULL;
    gdma_link_list_handle_t rx_link_list = NULL;
    test_gdma_config_link_list(tx_chan, rx_chan, &tx_link_list, &rx_link_list, 16, dma_link_in_ext_mem);

    size_t int_mem_alignment = 0;
    size_t ext_mem_alignment = 0;
    TEST_ESP_OK(gdma_get_alignment_constraints(tx_chan, &int_mem_alignment, &ext_mem_alignment));

    // allocate the source buffer from SRAM
    uint8_t *src_data = heap_caps_aligned_calloc(int_mem_alignment, 1, 128, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_data);

    // allocate the destination buffer from SRAM
    uint8_t *dst_data = heap_caps_aligned_calloc(int_mem_alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
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
    static const char src_string[] __attribute__((aligned(GDMA_LL_GET(ACCESS_ENCRYPTION_MEM_ALIGNMENT)))) = "GDMA can read MSPI Flash data!!!";
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
            .buffer_alignment = int_mem_alignment,
            .length = 64,
        },
        [1] = {
            .buffer = src_data + 64,
            .buffer_alignment = int_mem_alignment,
            .length = 64,
#if !SOC_DMA_CAN_ACCESS_FLASH
            .flags = {
                .mark_eof = true,
                .mark_final = GDMA_FINAL_LINK_TO_NULL, // using singly list, so terminate the link here
            }
#endif
        },
#if SOC_DMA_CAN_ACCESS_FLASH
        [2] = {
            .buffer = (void *)src_string,
            .buffer_alignment = ext_mem_alignment,
            .length = src_string_len,
            .flags = {
                .mark_eof = true,
                .mark_final = GDMA_FINAL_LINK_TO_NULL, // using singly list, so terminate the link here
            }
        },
#endif
    };
    TEST_ESP_OK(gdma_link_mount_buffers(tx_link_list, 0, tx_buf_mount_config, sizeof(tx_buf_mount_config) / sizeof(gdma_buffer_mount_config_t), NULL));

    gdma_buffer_mount_config_t rx_buf_mount_config = {
        .buffer = dst_data,
        .buffer_alignment = sram_alignment, // RX buffer should be aligned to the cache line size, because we will do cache invalidate later
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
    gdma_channel_alloc_config_t chan_alloc_config = {};

#if SOC_HAS(AHB_GDMA)
    TEST_ESP_OK(gdma_new_ahb_channel(&chan_alloc_config, &tx_chan, &rx_chan));

    test_gdma_m2m_transaction(tx_chan, rx_chan, false, trig_retention_backup);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
    TEST_ESP_OK(gdma_new_axi_channel(&chan_alloc_config, &tx_chan, &rx_chan));

    bool lli_in_ext_mem = false;
#if SOC_SPIRAM_SUPPORTED
    // the AXI GDMA allows to put the DMA link list in the external memory
    lli_in_ext_mem = true;
#endif
    test_gdma_m2m_transaction(tx_chan, rx_chan, lli_in_ext_mem, trig_retention_backup);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif // SOC_HAS(AXI_GDMA)

#if SOC_HAS(LP_AHB_GDMA)
    TEST_ESP_OK(gdma_new_lp_ahb_channel(&chan_alloc_config, &tx_chan, &rx_chan));

    test_gdma_m2m_transaction(tx_chan, rx_chan, false, trig_retention_backup);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif // SOC_HAS(LP_AHB_GDMA)
}

TEST_CASE("GDMA M2M Mode", "[GDMA][M2M]")
{
    test_gdma_m2m_mode(false);
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // test again with retention
    test_gdma_m2m_mode(true);
#endif
}

typedef struct {
    SemaphoreHandle_t done_sem;
    dma_buffer_split_array_t *align_array;
} test_gdma_context_t;

static bool test_gdma_m2m_unaligned_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    test_gdma_context_t *user_ctx = (test_gdma_context_t*)user_data;
    TEST_ESP_OK(esp_dma_merge_aligned_rx_buffers(user_ctx->align_array));
    xSemaphoreGiveFromISR(user_ctx->done_sem, &task_woken);
    return task_woken == pdTRUE;
}

static void test_gdma_m2m_unaligned_buffer_test(uint8_t *dst_data, uint8_t *src_data, size_t data_length, size_t offset_len)
{
    TEST_ASSERT_NOT_NULL(src_data);
    TEST_ASSERT_NOT_NULL(dst_data);
    memset(src_data, 0, data_length + offset_len);
    memset(dst_data, 0, data_length + offset_len);
    gdma_channel_handle_t tx_chan = NULL;
    gdma_channel_handle_t rx_chan = NULL;
    gdma_channel_alloc_config_t chan_alloc_config = {};
    TEST_ESP_OK(gdma_new_ahb_channel(&chan_alloc_config, &tx_chan, &rx_chan));
    size_t sram_alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);

    gdma_link_list_handle_t tx_link_list = NULL;
    gdma_link_list_handle_t rx_link_list = NULL;
    test_gdma_config_link_list(tx_chan, rx_chan, &tx_link_list, &rx_link_list, 0, false);

    size_t rx_mem_alignment = 0;
    TEST_ESP_OK(gdma_get_alignment_constraints(rx_chan, &rx_mem_alignment, NULL));

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
            .buffer_alignment = 1,
            .length = data_length,
            .flags = {
                .mark_eof = true,
                .mark_final = GDMA_FINAL_LINK_TO_NULL, // using singly list, so terminate the link here
            }
        }
    };
    TEST_ESP_OK(gdma_link_mount_buffers(tx_link_list, 0, tx_buf_mount_config, sizeof(tx_buf_mount_config) / sizeof(gdma_buffer_mount_config_t), NULL));

    dma_buffer_split_array_t align_array = {0};
    gdma_buffer_mount_config_t rx_aligned_buf_mount_config[3] = {0};
    uint8_t* stash_buffer = NULL;
    TEST_ESP_OK(esp_dma_split_rx_buffer_to_cache_aligned(dst_data + offset_len, data_length, &align_array, &stash_buffer));
    for (int i = 0; i < 3; i++) {
        rx_aligned_buf_mount_config[i].buffer = align_array.aligned_buffer[i].aligned_buffer;
        rx_aligned_buf_mount_config[i].buffer_alignment = MAX(sram_alignment, rx_mem_alignment);
        rx_aligned_buf_mount_config[i].length = align_array.aligned_buffer[i].length;
    }
    TEST_ESP_OK(gdma_link_mount_buffers(rx_link_list, 0, rx_aligned_buf_mount_config, 3, NULL));

    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = test_gdma_m2m_unaligned_rx_eof_callback,
    };
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);
    test_gdma_context_t user_ctx = {
        .done_sem = done_sem,
        .align_array = &align_array,
    };
    TEST_ESP_OK(gdma_register_rx_event_callbacks(rx_chan, &rx_cbs, &user_ctx));

    TEST_ESP_OK(gdma_start(rx_chan, gdma_link_get_head_addr(rx_link_list)));
    TEST_ESP_OK(gdma_start(tx_chan, gdma_link_get_head_addr(tx_link_list)));

    xSemaphoreTake(done_sem, 1000 / portTICK_PERIOD_MS);

    // validate the destination data
    for (int i = 0; i < data_length; i++) {
        TEST_ASSERT_EQUAL(i % 256, dst_data[i + offset_len]);
    }

    TEST_ESP_OK(gdma_del_link_list(tx_link_list));
    TEST_ESP_OK(gdma_del_link_list(rx_link_list));
    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
    vSemaphoreDelete(done_sem);
    free(stash_buffer);
}

TEST_CASE("GDMA M2M Unaligned RX Buffer Test", "[GDMA][M2M]")
{
    if (efuse_hal_flash_encryption_enabled()) {
        TEST_PASS_MESSAGE("Flash encryption is enabled, skip this test");
    }

    uint8_t *sbuf = heap_caps_aligned_calloc(64, 1, 10240, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dbuf = heap_caps_aligned_calloc(64, 1, 10240, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    // case buffer len less than buffer alignment
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 60, 0);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 60, 4);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 60, 2);

    // case buffer head aligned
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 246, 0);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 8182, 0);

    // case buffer tail aligned
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 246, 10);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 8182, 10);

    // case buffer unaligned
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 100, 10);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 10, 60);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 256, 10);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 8192, 10);

    // case buffer full aligned
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 256, 0);
    test_gdma_m2m_unaligned_buffer_test(dbuf, sbuf, 8192, 0);

    free(sbuf);
    free(dbuf);
}

[[maybe_unused]] static void test_gdma_memcpy_from_to_psram(gdma_channel_handle_t tx_chan, gdma_channel_handle_t rx_chan)
{
#define COPY_SIZE (40*1024)
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);
    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = test_gdma_m2m_rx_eof_callback,
    };
    TEST_ESP_OK(gdma_register_rx_event_callbacks(rx_chan, &rx_cbs, done_sem));

    gdma_strategy_config_t strategy = {
        .auto_update_desc = true,
        .owner_check = true,
        .eof_till_data_popped = true,
    };
    TEST_ESP_OK(gdma_apply_strategy(tx_chan, &strategy));
    TEST_ESP_OK(gdma_apply_strategy(rx_chan, &strategy));

    gdma_transfer_config_t transfer_cfg = {
        .max_data_burst_size = 32,
        .access_ext_mem = true, // allow to do memory copy from/to external memory
    };
    TEST_ESP_OK(gdma_config_transfer(tx_chan, &transfer_cfg));
    TEST_ESP_OK(gdma_config_transfer(rx_chan, &transfer_cfg));

    gdma_trigger_t m2m_trigger = GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0);
    // get a free DMA trigger ID for memory copy
    uint32_t free_m2m_id_mask = 0;
    gdma_get_free_m2m_trig_id_mask(tx_chan, &free_m2m_id_mask);
    m2m_trigger.instance_id = __builtin_ctz(free_m2m_id_mask);
    TEST_ESP_OK(gdma_connect(tx_chan, m2m_trigger));
    TEST_ESP_OK(gdma_connect(rx_chan, m2m_trigger));

    gdma_link_list_handle_t tx_link_list = NULL;
    gdma_link_list_handle_t rx_link_list = NULL;
    // create DMA link list for TX channel
    gdma_link_list_config_t tx_link_list_config = {
        .item_alignment = 8, // 8-byte alignment required by the AXI-GDMA
        .num_items = 20,
        .flags = {
            .items_in_ext_mem = false,
        }
    };
    TEST_ESP_OK(gdma_new_link_list(&tx_link_list_config, &tx_link_list));
    // create DMA link list for RX channel
    gdma_link_list_config_t rx_link_list_config = {
        .item_alignment = 8, // 8-byte alignment required by the AXI-GDMA
        .num_items = 20,
        .flags = {
            .items_in_ext_mem = false,
        },
    };
    TEST_ESP_OK(gdma_new_link_list(&rx_link_list_config, &rx_link_list));

    // allocate the source buffer from SRAM
    uint8_t *src_data = heap_caps_aligned_calloc(32, 1, COPY_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_data);
    TEST_ASSERT_TRUE(esp_ptr_internal(src_data));
    // prepare the source data
    for (int i = 0; i < COPY_SIZE; i++) {
        src_data[i] = i;
    }
    size_t sram_cache_line_sz = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    size_t psram_cache_line_sz = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    // do cache sync if necessary
    if (sram_cache_line_sz) {
        TEST_ESP_OK(esp_cache_msync(src_data, COPY_SIZE, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    }

    // allocate the destination buffer from PSRAM
    uint8_t *dst_data = heap_caps_aligned_calloc(32, 1, COPY_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(dst_data);
    TEST_ASSERT_TRUE(esp_ptr_external_ram(dst_data));
    if (psram_cache_line_sz) {
        TEST_ESP_OK(esp_cache_msync(dst_data, COPY_SIZE, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    }

    gdma_buffer_mount_config_t tx_buf_mount_config = {
        .buffer = src_data,
        .buffer_alignment = 32,
        .length = COPY_SIZE,
        .flags = {
            .mark_eof = true,
            .mark_final = GDMA_FINAL_LINK_TO_NULL, // using singly list, so terminate the link here
        }
    };
    TEST_ESP_OK(gdma_link_mount_buffers(tx_link_list, 0, &tx_buf_mount_config, 1, NULL));

    gdma_buffer_mount_config_t rx_buf_mount_config = {
        .buffer = dst_data,
        .buffer_alignment = 32,
        .length = COPY_SIZE,
        .flags = {
            .mark_final = GDMA_FINAL_LINK_TO_NULL, // using singly list, so terminate the link here
        }
    };
    TEST_ESP_OK(gdma_link_mount_buffers(rx_link_list, 0, &rx_buf_mount_config, 1, NULL));

    TEST_ESP_OK(gdma_start(rx_chan, gdma_link_get_head_addr(rx_link_list)));
    TEST_ESP_OK(gdma_start(tx_chan, gdma_link_get_head_addr(tx_link_list)));

    xSemaphoreTake(done_sem, pdMS_TO_TICKS(1000));

    /// let the DMA to copy the data back to the source buffer again
    /// clear the "src_data" because now we want to use it as the destination buffer
    memset(src_data, 0, COPY_SIZE);
    // do cache sync if necessary
    if (sram_cache_line_sz) {
        TEST_ESP_OK(esp_cache_msync(src_data, COPY_SIZE, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    }

    tx_buf_mount_config.buffer = dst_data;
    TEST_ESP_OK(gdma_link_mount_buffers(tx_link_list, 0, &tx_buf_mount_config, 1, NULL));
    rx_buf_mount_config.buffer = src_data;
    TEST_ESP_OK(gdma_link_mount_buffers(rx_link_list, 0, &rx_buf_mount_config, 1, NULL));

    TEST_ESP_OK(gdma_start(rx_chan, gdma_link_get_head_addr(rx_link_list)));
    TEST_ESP_OK(gdma_start(tx_chan, gdma_link_get_head_addr(tx_link_list)));

    xSemaphoreTake(done_sem, pdMS_TO_TICKS(1000));

    bool compare_result = true;
    for (int i = 0; i < COPY_SIZE; i++) {
        if (src_data[i] != i % 256) {
            printf("miss match! src_data[%d]=%d, should be %d\n", i, src_data[i], i % 256);
            compare_result = false;
        }
        if (dst_data[i] != i % 256) {
            printf("miss match! dst_data[%d]=%d, should be %d\n", i, dst_data[i], i % 256);
            compare_result = false;
        }
    }
    TEST_ASSERT_TRUE(compare_result);

    free(src_data);
    free(dst_data);
    TEST_ESP_OK(gdma_del_link_list(tx_link_list));
    TEST_ESP_OK(gdma_del_link_list(rx_link_list));
    vSemaphoreDelete(done_sem);
#undef COPY_SIZE
}

#if SOC_SPIRAM_SUPPORTED
TEST_CASE("GDMA memory copy SRAM->PSRAM->SRAM", "[GDMA][M2M]")
{
    [[maybe_unused]] gdma_channel_handle_t tx_chan = NULL;
    [[maybe_unused]] gdma_channel_handle_t rx_chan = NULL;
    [[maybe_unused]] gdma_channel_alloc_config_t chan_alloc_config = {};

#if SOC_HAS(AHB_GDMA)
#if GDMA_LL_GET(AHB_PSRAM_CAPABLE)
    printf("Testing AHB-GDMA memory copy SRAM->PSRAM->SRAM\n");
    TEST_ESP_OK(gdma_new_ahb_channel(&chan_alloc_config, &tx_chan, &rx_chan));

    test_gdma_memcpy_from_to_psram(tx_chan, rx_chan);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
#if GDMA_LL_GET(AXI_PSRAM_CAPABLE)
    printf("Testing AXI-GDMA memory copy SRAM->PSRAM->SRAM\n");
    TEST_ESP_OK(gdma_new_axi_channel(&chan_alloc_config, &tx_chan, &rx_chan));

    test_gdma_memcpy_from_to_psram(tx_chan, rx_chan);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif
#endif // SOC_HAS(AXI_GDMA)

#if SOC_HAS(LP_AHB_GDMA)
    printf("Testing LP-AHB-GDMA memory copy SRAM->PSRAM->SRAM\n");
    TEST_ESP_OK(gdma_new_lp_ahb_channel(&chan_alloc_config, &tx_chan, &rx_chan));

    test_gdma_memcpy_from_to_psram(tx_chan, rx_chan);

    TEST_ESP_OK(gdma_del_channel(tx_chan));
    TEST_ESP_OK(gdma_del_channel(rx_chan));
#endif // SOC_HAS(LP_AHB_GDMA)
}
#endif // SOC_SPIRAM_SUPPORTED
