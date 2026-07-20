/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "soc/ahb_dma_struct.h"
#include "hal/dma_types.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"
#include "esp_private/esp_cache_private.h"
#include "sys/lock.h"
#include "sdkconfig.h"
#include "asrc_adapter_priv.h"

#if ASRC_USE_RETENTION_LINK
#include "esp_private/sleep_retention.h"
#endif  /* ASRC_USE_RETENTION_LINK */

#define TAG "ASRC_ADAPTER"
#define ASRC_HW_GDMA_DESC_BUFFER_MAX_SIZE DMA_DESCRIPTOR_BUFFER_MAX_SIZE_16B_ALIGNED

#if ASRC_USE_RETENTION_LINK
/* Note: sleep retention APIs use OS locks, so use a lock rather than a light-weight critical section */
static _lock_t s_asrc_sleep_retention_lock;
static uint8_t s_asrc_retention_ref_count;
#endif  /* ASRC_USE_RETENTION_LINK */

static bool IRAM_ATTR asrc_hw_rx_eof(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    portBASE_TYPE higher_priority_task_awoken = pdFALSE;
    asrc_hw_gdma_evt_t asrc_evt = {.gdma_evt = ASRC_HW_GDMA_RECV_FRAME_DONE};
    xQueueSendFromISR(user_data, &asrc_evt, &higher_priority_task_awoken);
    return higher_priority_task_awoken;
}

#if ASRC_USE_RETENTION_LINK
static esp_err_t s_asrc_sleep_retention_init_cb(void *arg)
{
    esp_err_t ret = sleep_retention_entries_create(asrc_reg_retention_info[0].entry_array,
                                                   asrc_reg_retention_info[0].array_size,
                                                   REGDMA_LINK_PRI_ASRC, asrc_reg_retention_info[0].retention_module);
    ESP_RETURN_ON_ERROR(ret, TAG, "failed to allocate mem for sleep retention");
    return ret;
}

static void asrc_acquire_sleep_retention(void)
{
    sleep_retention_module_t module = asrc_reg_retention_info[0].retention_module;
    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create = {
                .handle = s_asrc_sleep_retention_init_cb,
                .arg = NULL,
            },
        },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_ATTACH,
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM),
    };

    _lock_acquire(&s_asrc_sleep_retention_lock);
    if (s_asrc_retention_ref_count == 0) {
        esp_err_t err = sleep_retention_module_init(module, &init_param);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "init sleep retention failed, ASRC configuration may be lost after sleep wakeup");
        } else {
#if defined(CONFIG_ASRC_ALLOW_PD) && CONFIG_ASRC_ALLOW_PD
            err = sleep_retention_module_allocate(module);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "create retention module failed, power domain can't turn off");
                /* don't call sleep_retention_module_deinit here, otherwise ASRC may be powered off during sleep */
            } else if (sleep_retention_module_attach(module) != ESP_OK) {
                ESP_LOGW(TAG, "attach retention module failed, power domain can't turn off");
            }
#endif  /* CONFIG_ASRC_ALLOW_PD */
        }
    }
    s_asrc_retention_ref_count++;
    _lock_release(&s_asrc_sleep_retention_lock);
}

static void asrc_release_sleep_retention(void)
{
    sleep_retention_module_t module = asrc_reg_retention_info[0].retention_module;

    _lock_acquire(&s_asrc_sleep_retention_lock);
    if (s_asrc_retention_ref_count == 0) {
        _lock_release(&s_asrc_sleep_retention_lock);
        return;
    }
    s_asrc_retention_ref_count--;
    if (s_asrc_retention_ref_count == 0) {
#if defined(CONFIG_ASRC_ALLOW_PD) && CONFIG_ASRC_ALLOW_PD
        if (sleep_retention_module_detach(module) == ESP_OK) {
            if (sleep_retention_module_free(module) != ESP_OK) {
                ESP_LOGW(TAG, "fail to free the retention link list");
            }
        } else {
            ESP_LOGW(TAG, "fail to detach the retention link list");
        }
#endif  /* CONFIG_ASRC_ALLOW_PD */
        if (sleep_retention_module_deinit(module) != ESP_OK) {
            ESP_LOGW(TAG, "fail to deinit the retention module");
        }
    }
    _lock_release(&s_asrc_sleep_retention_lock);
}
#endif  /* ASRC_USE_RETENTION_LINK */

esp_err_t asrc_hw_gdma_create_channel(int asrc_idx, void *user_data, uint16_t max_data_burst_size,
                                      asrc_hw_gdma_channel_handle_t *dma_tx_chan, asrc_hw_gdma_channel_handle_t *dma_rx_chan)
{
    ESP_RETURN_ON_FALSE(dma_tx_chan, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    ESP_RETURN_ON_FALSE(dma_rx_chan, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    esp_err_t ret = ESP_OK;
    gdma_trigger_t trig = {0};
    trig.instance_id = asrc_idx == 0 ? SOC_GDMA_TRIG_PERIPH_ASRC0 : SOC_GDMA_TRIG_PERIPH_ASRC1;
    trig.bus_id = asrc_idx == 0 ? SOC_GDMA_TRIG_PERIPH_ASRC0_BUS : SOC_GDMA_TRIG_PERIPH_ASRC1_BUS;
    gdma_channel_alloc_config_t dma_cfg = {0};
    gdma_channel_handle_t dma_tx = NULL;
    gdma_channel_handle_t dma_rx = NULL;
    bool tx_connected = false;
    bool rx_connected = false;
    ret = gdma_new_ahb_channel(&dma_cfg, &dma_tx, &dma_rx);
    ESP_RETURN_ON_ERROR(ret, TAG, "Fail to new gdma channel");

    ESP_GOTO_ON_ERROR(gdma_connect(dma_tx, trig), cleanup, TAG, "Fail to connect tx channel");
    tx_connected = true;
    ESP_GOTO_ON_ERROR(gdma_connect(dma_rx, trig), cleanup, TAG, "Fail to connect rx channel");
    rx_connected = true;
    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = false,
        .owner_check = true,
        .eof_till_data_popped = true,
    };
    ESP_GOTO_ON_ERROR(gdma_apply_strategy(dma_tx, &strategy_config), cleanup, TAG, "Fail to apply tx strategy");
    ESP_GOTO_ON_ERROR(gdma_apply_strategy(dma_rx, &strategy_config), cleanup, TAG, "Fail to apply rx strategy");
    gdma_transfer_config_t transfer_config = {
        .max_data_burst_size = max_data_burst_size,
        .access_ext_mem = true,
    };
    ESP_GOTO_ON_ERROR(gdma_config_transfer(dma_rx, &transfer_config), cleanup, TAG, "Fail to config rx transfer");
    ESP_GOTO_ON_ERROR(gdma_config_transfer(dma_tx, &transfer_config), cleanup, TAG, "Fail to config tx transfer");
    gdma_rx_event_callbacks_t cb = {.on_recv_eof = asrc_hw_rx_eof};
    ESP_GOTO_ON_ERROR(gdma_register_rx_event_callbacks(dma_rx, &cb, user_data), cleanup, TAG, "Fail to register rx event callbacks");
#if ASRC_USE_RETENTION_LINK
    /* Register retention while creating channel, same style as GDMA */
    asrc_acquire_sleep_retention();
#endif  /* ASRC_USE_RETENTION_LINK */
    *dma_tx_chan = dma_tx;
    *dma_rx_chan = dma_rx;
    return ESP_OK;
cleanup:
    if (tx_connected) {
        gdma_disconnect(dma_tx);
    }
    if (rx_connected) {
        gdma_disconnect(dma_rx);
    }
    if (dma_tx != NULL) {
        gdma_del_channel(dma_tx);
    }
    if (dma_rx != NULL) {
        gdma_del_channel(dma_rx);
    }
    *dma_tx_chan = NULL;
    *dma_rx_chan = NULL;
    return ret;
}

esp_err_t asrc_hw_gdma_create_link_list(uint32_t byte_cnt, asrc_hw_gdma_link_list_handle_t *list_hd, uint32_t *max_desc_num)
{
    ESP_RETURN_ON_FALSE(list_hd, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    ESP_RETURN_ON_FALSE(max_desc_num, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    esp_err_t ret = ESP_OK;
    int32_t desc_num = byte_cnt / ASRC_HW_GDMA_DESC_BUFFER_MAX_SIZE;
    if (byte_cnt % ASRC_HW_GDMA_DESC_BUFFER_MAX_SIZE != 0) {
        desc_num++;
    }
    gdma_link_list_handle_t list = (gdma_link_list_handle_t)(*list_hd);
    *list_hd = NULL;
    if (desc_num > *max_desc_num) {
        if (list != NULL) {
            gdma_del_link_list(list);
            list = NULL;
        }
        gdma_link_list_config_t config = {
            .num_items = desc_num,
        };
        ret = gdma_new_link_list(&config, &list);
        ESP_RETURN_ON_ERROR(ret, TAG, "Fail to new link list");
        *max_desc_num = desc_num;
    }
    *list_hd = list;
    return ESP_OK;
}

esp_err_t asrc_hw_gdma_mount_link_list(asrc_hw_gdma_link_list_handle_t list_hd, uint32_t desc_num, uint8_t *buf, uint32_t byte_cnt)
{
    ESP_RETURN_ON_FALSE(buf, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    ESP_RETURN_ON_FALSE(list_hd, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    esp_err_t ret = ESP_OK;
    uint32_t remaining_byte_cnt = byte_cnt;
    gdma_buffer_mount_config_t mount_config[desc_num] = {};
    for (int i = 0; i < desc_num; i++) {
        mount_config[i].buffer = buf;
        mount_config[i].flags.bypass_buffer_align_check = true;
        if ((i + 1) != desc_num) {
            mount_config[i].length = ASRC_HW_GDMA_DESC_BUFFER_MAX_SIZE;
            mount_config[i].flags.mark_eof = 0;
            mount_config[i].flags.mark_final = GDMA_FINAL_LINK_TO_DEFAULT;
            remaining_byte_cnt -= ASRC_HW_GDMA_DESC_BUFFER_MAX_SIZE;
        } else {
            mount_config[i].length = remaining_byte_cnt;
            mount_config[i].flags.mark_eof = 1;
            mount_config[i].flags.mark_final = GDMA_FINAL_LINK_TO_NULL;
        }
        buf += ASRC_HW_GDMA_DESC_BUFFER_MAX_SIZE;
    }
    ret = gdma_link_mount_buffers((gdma_link_list_handle_t)list_hd, 0, mount_config, desc_num, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to mount link list, ret: %d", ret);
        return ret;
    }
    return ESP_OK;
}

esp_err_t asrc_hw_gdma_start_channel(asrc_hw_gdma_channel_handle_t dma_chan, asrc_hw_gdma_link_list_handle_t desc)
{
    ESP_RETURN_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    ESP_RETURN_ON_FALSE(desc, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    gdma_channel_handle_t dma_chan_handle = (gdma_channel_handle_t)dma_chan;
    gdma_reset(dma_chan_handle);
    gdma_start(dma_chan_handle, gdma_link_get_head_addr((gdma_link_list_handle_t)desc));
    return ESP_OK;
}

void asrc_hw_gdma_free_link_list(asrc_hw_gdma_link_list_handle_t list_hd)
{
    if (list_hd != NULL) {
        gdma_del_link_list((gdma_link_list_handle_t)list_hd);
    }
}

void asrc_hw_gdma_destroy_channel(asrc_hw_gdma_channel_handle_t dma_tx_chan, asrc_hw_gdma_channel_handle_t dma_rx_chan)
{
    if (dma_tx_chan != NULL) {
        gdma_channel_handle_t dma_tx_chan_handle = (gdma_channel_handle_t)dma_tx_chan;
        gdma_disconnect(dma_tx_chan_handle);
        gdma_del_channel(dma_tx_chan_handle);
    }
    if (dma_rx_chan != NULL) {
        gdma_channel_handle_t dma_rx_chan_handle = (gdma_channel_handle_t)dma_rx_chan;
        gdma_disconnect(dma_rx_chan_handle);
        gdma_del_channel(dma_rx_chan_handle);
    }
#if ASRC_USE_RETENTION_LINK
    if (dma_tx_chan != NULL || dma_rx_chan != NULL) {
        asrc_release_sleep_retention();
    }
#endif  /* ASRC_USE_RETENTION_LINK */
}

esp_err_t asrc_hw_get_buffer_alignment(uint32_t heap_caps, size_t *out_alignment)
{
    return esp_cache_get_alignment(heap_caps, out_alignment);
}

esp_err_t asrc_hw_check_buffer_alignment(uint8_t *buffer, uint32_t buffer_size)
{
    if (esp_ptr_external_ram(buffer) == true) {
        size_t cache_line_size = 0;
        esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &cache_line_size);
        bool aligned_addr = (((uint32_t)buffer % cache_line_size) == 0) && ((buffer_size % cache_line_size) == 0);
        if (!aligned_addr) {
            ESP_LOGE(TAG, "The buffer address(%p) or the buffer size(%ld) is(are) not aligned with cache line size (%ld)",
                     buffer, buffer_size, cache_line_size);
            ESP_LOGE(TAG, "Please use esp_asrc_alloc_mem to malloc your buffer");
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

esp_err_t asrc_hw_cache_msync_c2m(uint8_t *buffer, uint32_t buffer_size)
{
    if (esp_ptr_external_ram(buffer) == true) {
        esp_err_t ret = esp_cache_msync(buffer, buffer_size,
                                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
        ESP_RETURN_ON_ERROR(ret, TAG, "Inbuf cache sync failed");
    }
    return ESP_OK;
}

esp_err_t asrc_hw_cache_msync_m2c(uint8_t *buffer, uint32_t buffer_size)
{
    if (esp_ptr_external_ram(buffer) == true) {
        esp_err_t ret = esp_cache_msync(buffer, buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        ESP_RETURN_ON_ERROR(ret, TAG, "Outbuf cache sync failed");
    }
    return ESP_OK;
}
