/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_private/dw_gdma.h"
#include "hal/dw_gdma_ll.h"
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"

TEST_CASE("DW_GDMA channel allocation", "[DW_GDMA]")
{
    printf("install DMA channels exhaustively\r\n");
    dw_gdma_channel_static_config_t static_config = {
        .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
        .role = DW_GDMA_ROLE_MEM,
        .num_outstanding_requests = 1,
    };
    dw_gdma_channel_alloc_config_t alloc_config = {
        .src = static_config,
        .dst = static_config,
    };
    dw_gdma_channel_handle_t chans[DW_GDMA_LL_GROUPS][DW_GDMA_LL_CHANNELS_PER_GROUP];
    for (int i = 0; i < DW_GDMA_LL_GROUPS; i++) {
        for (int j = 0; j < DW_GDMA_LL_CHANNELS_PER_GROUP; j++) {
            TEST_ESP_OK(dw_gdma_new_channel(&alloc_config, &chans[i][j]));
        }
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, dw_gdma_new_channel(&alloc_config, &chans[0][0]));

    printf("delete DMA channels\r\n");
    for (int i = 0; i < DW_GDMA_LL_GROUPS; i++) {
        for (int j = 0; j < DW_GDMA_LL_CHANNELS_PER_GROUP; j++) {
            TEST_ESP_OK(dw_gdma_del_channel(chans[i][j]));
        }
    }
}

static bool test_dw_gdma_conti_mode_trans_done_cb(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    SemaphoreHandle_t done_sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(done_sem, &task_woken);
    return task_woken == pdTRUE;
}

TEST_CASE("DW_GDMA M2M Test: Contiguous Mode", "[DW_GDMA]")
{
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);

    printf("prepare the source and destination buffers\r\n");
    size_t sram_alignment = 0;
    TEST_ESP_OK(esp_cache_get_alignment(0, &sram_alignment));
    size_t alignment = MAX(sram_alignment, 8);
    uint8_t *src_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dst_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    TEST_ASSERT_NOT_NULL(dst_buf);
    for (int i = 0; i < 256; i++) {
        src_buf[i] = i;
    }
    if (sram_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync((void *)src_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }

    printf("allocate a channel for memory copy\r\n");
    dw_gdma_channel_static_config_t static_config = {
        .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
        .role = DW_GDMA_ROLE_MEM,
        .num_outstanding_requests = 1,
    };
    dw_gdma_channel_alloc_config_t alloc_config = {
        .src = static_config,
        .dst = static_config,
        .flow_controller = DW_GDMA_FLOW_CTRL_SELF, // DMA as the flow controller
        .chan_priority = 1,
    };
    dw_gdma_channel_handle_t m2m_chan = NULL;
    TEST_ESP_OK(dw_gdma_new_channel(&alloc_config, &m2m_chan));

    printf("register event handler\r\n");
    dw_gdma_event_callbacks_t cbs = {
        .on_full_trans_done = test_dw_gdma_conti_mode_trans_done_cb,
    };
    TEST_ESP_OK(dw_gdma_channel_register_event_callbacks(m2m_chan, &cbs, done_sem));

    printf("set up memory copy transaction\r\n");
    dw_gdma_block_transfer_config_t transfer_config = {
        .src = {
            .addr = (uint32_t)src_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .dst = {
            .addr = (uint32_t)dst_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .size = 256,
    };
    TEST_ESP_OK(dw_gdma_channel_config_transfer(m2m_chan, &transfer_config));

    printf("start the DMA engine\r\n");
    TEST_ESP_OK(dw_gdma_channel_enable_ctrl(m2m_chan, true));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(100)));
    // DMA should stop after the first block transfer is done
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(100)));

    printf("check the memory copy result\r\n");
    if (sram_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }
    for (int i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_UINT8(i, dst_buf[i]);
    }

    TEST_ESP_OK(dw_gdma_del_channel(m2m_chan));
    free(src_buf);
    free(dst_buf);
    vSemaphoreDelete(done_sem);
}

static bool test_dw_gdma_reload_mode_block_done_cb(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    SemaphoreHandle_t done_sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(done_sem, &task_woken);
    return task_woken == pdTRUE;
}

TEST_CASE("DW_GDMA M2M Test: Reload Mode", "[DW_GDMA]")
{
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);

    printf("prepare the source and destination buffers\r\n");
    size_t sram_alignment = 0;
    TEST_ESP_OK(esp_cache_get_alignment(0, &sram_alignment));
    size_t alignment = MAX(sram_alignment, 8);
    uint8_t *src_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dst_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    TEST_ASSERT_NOT_NULL(dst_buf);
    for (int i = 0; i < 256; i++) {
        src_buf[i] = i;
    }
    if (sram_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync((void *)src_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }

    printf("allocate a channel for memory copy\r\n");
    dw_gdma_channel_static_config_t static_config = {
        .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_RELOAD,
        .role = DW_GDMA_ROLE_MEM,
        .num_outstanding_requests = 1,
    };
    dw_gdma_channel_alloc_config_t alloc_config = {
        .src = static_config,
        .dst = static_config,
        .flow_controller = DW_GDMA_FLOW_CTRL_SELF, // DMA as the flow controller
        .chan_priority = 1,
    };
    dw_gdma_channel_handle_t m2m_chan = NULL;
    TEST_ESP_OK(dw_gdma_new_channel(&alloc_config, &m2m_chan));

    printf("register event handler\r\n");
    dw_gdma_event_callbacks_t cbs = {
        .on_block_trans_done = test_dw_gdma_reload_mode_block_done_cb,
    };
    TEST_ESP_OK(dw_gdma_channel_register_event_callbacks(m2m_chan, &cbs, done_sem));

    printf("set up memory copy transaction\r\n");
    dw_gdma_block_transfer_config_t transfer_config = {
        .src = {
            .addr = (uint32_t)src_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .dst = {
            .addr = (uint32_t)dst_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .size = 256,
    };
    TEST_ESP_OK(dw_gdma_channel_config_transfer(m2m_chan, &transfer_config));

    dw_gdma_block_markers_t markers = {
        .en_trans_done_intr = true, // enable block trans done interrupt
    };
    TEST_ESP_OK(dw_gdma_channel_set_block_markers(m2m_chan, markers));

    printf("start the DMA engine\r\n");
    TEST_ESP_OK(dw_gdma_channel_enable_ctrl(m2m_chan, true));

    // because of the auto-reload, we can keep receiving the block trans done event
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(100)));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(100)));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(100)));

    printf("check the memory copy result\r\n");
    if (sram_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }
    for (int i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_UINT8(i, dst_buf[i]);
    }

    // stop the DMA channel
    TEST_ESP_OK(dw_gdma_channel_enable_ctrl(m2m_chan, false));
    TEST_ESP_OK(dw_gdma_del_channel(m2m_chan));
    free(src_buf);
    free(dst_buf);
    vSemaphoreDelete(done_sem);
}

typedef struct {
    SemaphoreHandle_t done_sem;
    uint8_t count;
} test_gdma_shadow_mode_user_data_t;

static bool test_dw_gdma_shadow_mode_block_invalid_cb(dw_gdma_channel_handle_t chan, const dw_gdma_break_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    test_gdma_shadow_mode_user_data_t *udata = (test_gdma_shadow_mode_user_data_t *)user_data;
    udata->count++;

    dw_gdma_block_markers_t markers = {
        .is_last = true,  // mark the block as the last one
        .is_valid = true, // mark the block as valid so that the DMA can continue the transfer
    };
    dw_gdma_channel_set_block_markers(chan, markers);
    // after the block is marked as valid again, tell the DMA to continue the transfer
    dw_gdma_channel_continue(chan);
    return task_woken == pdTRUE;
}

static bool test_dw_gdma_shadow_mode_trans_done_cb(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    test_gdma_shadow_mode_user_data_t *udata = (test_gdma_shadow_mode_user_data_t *)user_data;
    SemaphoreHandle_t done_sem = udata->done_sem;
    xSemaphoreGiveFromISR(done_sem, &task_woken);
    return task_woken == pdTRUE;
}

TEST_CASE("DW_GDMA M2M Test: Shadow Mode", "[DW_GDMA]")
{
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);

    printf("prepare the source and destination buffers\r\n");
    size_t sram_alignment = 0;
    TEST_ESP_OK(esp_cache_get_alignment(0, &sram_alignment));
    size_t alignment = MAX(sram_alignment, 8);
    uint8_t *src_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dst_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    TEST_ASSERT_NOT_NULL(dst_buf);
    for (int i = 0; i < 256; i++) {
        src_buf[i] = i;
    }
    if (sram_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync((void *)src_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }

    printf("allocate a channel for memory copy\r\n");
    dw_gdma_channel_static_config_t static_config = {
        .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_SHADOW,
        .role = DW_GDMA_ROLE_MEM,
        .num_outstanding_requests = 1,
    };
    dw_gdma_channel_alloc_config_t alloc_config = {
        .src = static_config,
        .dst = static_config,
        .flow_controller = DW_GDMA_FLOW_CTRL_SELF, // DMA as the flow controller
        .chan_priority = 1,
    };
    dw_gdma_channel_handle_t m2m_chan = NULL;
    TEST_ESP_OK(dw_gdma_new_channel(&alloc_config, &m2m_chan));

    printf("set up memory copy transaction\r\n");
    dw_gdma_block_transfer_config_t transfer_config = {
        .src = {
            .addr = (uint32_t)src_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .dst = {
            .addr = (uint32_t)dst_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .size = 256,
    };
    TEST_ESP_OK(dw_gdma_channel_config_transfer(m2m_chan, &transfer_config));
    dw_gdma_block_markers_t markers = {
        .is_valid = true, // mark the block as valid so that the DMA can start the transfer
    };
    TEST_ESP_OK(dw_gdma_channel_set_block_markers(m2m_chan, markers));

    printf("register event handler\r\n");
    dw_gdma_event_callbacks_t cbs = {
        .on_invalid_block = test_dw_gdma_shadow_mode_block_invalid_cb,
        .on_full_trans_done = test_dw_gdma_shadow_mode_trans_done_cb,
    };
    test_gdma_shadow_mode_user_data_t user_data = {
        .done_sem = done_sem,
        .count = 0,
    };
    TEST_ESP_OK(dw_gdma_channel_register_event_callbacks(m2m_chan, &cbs, &user_data));

    printf("start the DMA engine\r\n");
    TEST_ESP_OK(dw_gdma_channel_enable_ctrl(m2m_chan, true));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(1000)));
    // should only go into the block invalid callback for once
    TEST_ASSERT_EQUAL_UINT8(1, user_data.count);

    printf("check the memory copy result\r\n");
    if (sram_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }
    for (int i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_UINT8(i, dst_buf[i]);
    }

    TEST_ESP_OK(dw_gdma_del_channel(m2m_chan));
    free(src_buf);
    free(dst_buf);
    vSemaphoreDelete(done_sem);
}

typedef struct {
    SemaphoreHandle_t done_sem;
    void *dst_buffer_addr;
    size_t dst_buffer_size;
    uint8_t count;
} test_gdma_list_mode_user_data_t;

static bool test_dw_gdma_list_mode_trans_done_cb(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    BaseType_t task_woken = pdFALSE;
    test_gdma_list_mode_user_data_t *udata = (test_gdma_list_mode_user_data_t *)user_data;
    SemaphoreHandle_t done_sem = udata->done_sem;
    xSemaphoreGiveFromISR(done_sem, &task_woken);
    return task_woken == pdTRUE;
}

static bool test_dw_gdma_list_mode_invalid_block_cb(dw_gdma_channel_handle_t chan, const dw_gdma_break_event_data_t *event_data, void *user_data)
{
    test_gdma_list_mode_user_data_t *udata = (test_gdma_list_mode_user_data_t *)user_data;
    dw_gdma_lli_handle_t lli = event_data->invalid_lli;
    udata->count++;
    // clear the destination buffer
    memset(udata->dst_buffer_addr, 0, udata->dst_buffer_size);
    esp_cache_msync(udata->dst_buffer_addr, udata->dst_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    dw_gdma_block_markers_t markers = {
        .is_last = true,  // mark the next block as the last one
        .is_valid = true, // mark the block as valid so that the DMA can continue the transfer
    };
    dw_gdma_lli_set_block_markers(lli, markers);
    // after the item is marked as valid again, tell the DMA to continue the transfer
    dw_gdma_channel_continue(chan);
    return false;
}

TEST_CASE("DW_GDMA M2M Test: Link-List Mode", "[DW_GDMA]")
{
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(done_sem);

    printf("prepare the source and destination buffers\r\n");
    size_t sram_alignment = 0;
    TEST_ESP_OK(esp_cache_get_alignment(0, &sram_alignment));
    size_t alignment = MAX(sram_alignment, 8);
    uint8_t *src_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *dst_buf = heap_caps_aligned_calloc(alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    TEST_ASSERT_NOT_NULL(dst_buf);
    for (int i = 0; i < 256; i++) {
        src_buf[i] = i;
    }
    if (sram_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync((void *)src_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }

    printf("allocate a channel for memory copy\r\n");
    dw_gdma_channel_static_config_t static_config = {
        .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_LIST,
        .role = DW_GDMA_ROLE_MEM,
        .num_outstanding_requests = 1,
    };
    dw_gdma_channel_alloc_config_t alloc_config = {
        .src = static_config,
        .dst = static_config,
        .flow_controller = DW_GDMA_FLOW_CTRL_SELF, // DMA as the flow controller
        .chan_priority = 1,
    };
    dw_gdma_channel_handle_t m2m_chan = NULL;
    TEST_ESP_OK(dw_gdma_new_channel(&alloc_config, &m2m_chan));

    printf("create singly DMA link list\r\n");
    dw_gdma_link_list_config_t link_list_config = {
        .num_items = 2,
        .link_type = DW_GDMA_LINKED_LIST_TYPE_SINGLY,
    };
    dw_gdma_link_list_handle_t link_list = NULL;
    TEST_ESP_OK(dw_gdma_new_link_list(&link_list_config, &link_list));

    printf("set up memory copy transaction\r\n");
    dw_gdma_block_transfer_config_t transfer_config = {
        .src = {
            .addr = (uint32_t)src_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .dst = {
            .addr = (uint32_t)dst_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 0,
        },
        .size = 128,
    };
    dw_gdma_block_markers_t markers = {
        .is_valid = true, // mark the block as valid so that the DMA can start the transfer
    };
    TEST_ESP_OK(dw_gdma_lli_config_transfer(dw_gdma_link_list_get_item(link_list, 0), &transfer_config));
    TEST_ESP_OK(dw_gdma_lli_set_block_markers(dw_gdma_link_list_get_item(link_list, 0), markers));
    transfer_config.src.addr = (uint32_t)(src_buf + 128);
    transfer_config.dst.addr = (uint32_t)(dst_buf + 128);
    markers.is_last = true;
    TEST_ESP_OK(dw_gdma_lli_config_transfer(dw_gdma_link_list_get_item(link_list, 1), &transfer_config));
    TEST_ESP_OK(dw_gdma_lli_set_block_markers(dw_gdma_link_list_get_item(link_list, 1), markers));

    printf("register event handler\r\n");
    dw_gdma_event_callbacks_t cbs = {
        .on_full_trans_done = test_dw_gdma_list_mode_trans_done_cb,
        .on_invalid_block = test_dw_gdma_list_mode_invalid_block_cb,
    };
    test_gdma_list_mode_user_data_t user_data = {
        .done_sem = done_sem,
        .count = 0,
        .dst_buffer_addr = dst_buf,
        .dst_buffer_size = 256,
    };
    TEST_ESP_OK(dw_gdma_channel_register_event_callbacks(m2m_chan, &cbs, &user_data));

    printf("use the link list\r\n");
    TEST_ESP_OK(dw_gdma_channel_use_link_list(m2m_chan, link_list));
    printf("start the DMA engine\r\n");
    TEST_ESP_OK(dw_gdma_channel_enable_ctrl(m2m_chan, true));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(1000)));

    printf("check the memory copy result\r\n");
    if (sram_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }
    for (int i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_UINT8(i, dst_buf[i]);
    }

    // delete the singly link list, and create a circular link list instead
    TEST_ESP_OK(dw_gdma_del_link_list(link_list));
    printf("create circular DMA link list\r\n");
    link_list_config.link_type = DW_GDMA_LINKED_LIST_TYPE_CIRCULAR;
    link_list_config.num_items = 1;
    TEST_ESP_OK(dw_gdma_new_link_list(&link_list_config, &link_list));
    // set the transfer parameters for the link list item
    transfer_config.size = 256;
    transfer_config.src.addr = (uint32_t)src_buf;
    transfer_config.dst.addr = (uint32_t)dst_buf;
    TEST_ESP_OK(dw_gdma_lli_config_transfer(dw_gdma_link_list_get_item(link_list, 0), &transfer_config));
    markers.is_valid = true;
    markers.is_last = false;
    TEST_ESP_OK(dw_gdma_lli_set_block_markers(dw_gdma_link_list_get_item(link_list, 0), markers));

    printf("use the link list\r\n");
    TEST_ESP_OK(dw_gdma_channel_use_link_list(m2m_chan, link_list));
    TEST_ESP_OK(dw_gdma_channel_enable_ctrl(m2m_chan, true));

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done_sem, pdMS_TO_TICKS(1000)));
    // should only go into the block invalid callback for once
    TEST_ASSERT_EQUAL_UINT8(1, user_data.count);

    printf("check the memory copy result\r\n");
    if (sram_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }
    for (int i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_UINT8(i, dst_buf[i]);
    }

    TEST_ESP_OK(dw_gdma_del_link_list(link_list));
    TEST_ESP_OK(dw_gdma_del_channel(m2m_chan));
    free(src_buf);
    free(dst_buf);
    vSemaphoreDelete(done_sem);
}

TEST_CASE("DW_GDMA M2M Test: memory set with fixed address", "[DW_GDMA]")
{
    printf("prepare the source and destination buffers\r\n");
    // memset: source in psram and destination in sram
    size_t ext_mem_alignment = 0;
    size_t int_mem_alignment = 0;
    TEST_ESP_OK(esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &ext_mem_alignment));
    TEST_ESP_OK(esp_cache_get_alignment(0, &int_mem_alignment));
    uint8_t *src_buf = heap_caps_aligned_calloc(ext_mem_alignment, 1, 256, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    uint8_t *dst_buf = heap_caps_aligned_calloc(int_mem_alignment, 1, 256, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src_buf);
    TEST_ASSERT_NOT_NULL(dst_buf);
    // prepare the source buffer, only the first byte has a non-zero value
    for (int i = 0; i < 256; i++) {
        src_buf[i] = 0;
    }
    src_buf[0] = 66;
    if (ext_mem_alignment) {
        // do write-back for the source data because it's in the cache
        TEST_ESP_OK(esp_cache_msync((void *)src_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
    }

    printf("allocate a channel for memory set\r\n");
    dw_gdma_channel_static_config_t static_config = {
        .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
        .role = DW_GDMA_ROLE_MEM,
        .num_outstanding_requests = 1,
    };
    dw_gdma_channel_alloc_config_t alloc_config = {
        .src = static_config,
        .dst = static_config,
        .flow_controller = DW_GDMA_FLOW_CTRL_SELF, // DMA as the flow controller
        .chan_priority = 1,
    };
    dw_gdma_channel_handle_t m2m_chan = NULL;
    TEST_ESP_OK(dw_gdma_new_channel(&alloc_config, &m2m_chan));

    printf("set up memory set transaction\r\n");
    dw_gdma_block_transfer_config_t transfer_config = {
        .src = {
            .addr = (uint32_t)src_buf,
            .burst_mode = DW_GDMA_BURST_MODE_FIXED,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 1, // Note for ESP32P4, if the buffer is in PSRAM and the burst mode is fixed, we can't set the burst length larger than 1
        },
        .dst = {
            .addr = (uint32_t)dst_buf,
            .burst_mode = DW_GDMA_BURST_MODE_INCREMENT,
            .width = DW_GDMA_TRANS_WIDTH_8,
            .burst_items = 4,
            .burst_len = 1,
        },
        .size = 256,
    };
    TEST_ESP_OK(dw_gdma_channel_config_transfer(m2m_chan, &transfer_config));

    printf("start the DMA engine\r\n");
    TEST_ESP_OK(dw_gdma_channel_enable_ctrl(m2m_chan, true));
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("check the memory set result\r\n");
    if (int_mem_alignment) {
        // the destination data are not reflected to the cache, so do an invalidate to ask the cache load new data
        TEST_ESP_OK(esp_cache_msync((void *)dst_buf, 256, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    }
    for (int i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_UINT8(66, dst_buf[i]);
    }

    TEST_ESP_OK(dw_gdma_del_channel(m2m_chan));
    free(src_buf);
    free(dst_buf);
}
