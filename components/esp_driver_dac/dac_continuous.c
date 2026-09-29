/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

#include "dac_priv_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/dac_ll.h"
#include "driver/dac_continuous.h"
#include "esp_private/gdma_link.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "dac_priv_dma.h"

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#define DAC_DMA_MAX_BUF_SIZE        4092        // Max DMA buffer size is 4095 but better to align with 4 bytes, so set 4092 here

#define DAC_DMA_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)

struct dac_continuous_s {
    dac_continuous_config_t cfg;
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t    pm_lock;
#endif

    dac_event_callbacks_t   cbs;                        /* User event callbacks */
    void                    *user_data;

    uint32_t                cur_index;                  /* Index of the DMA descriptor that is currently being used by DMA. */
    uint32_t                used_desc_num;              /* Number of used DMA descriptors. Determines cur_index wrap-around. */

    QueueHandle_t           free_desc_queue;            /* Queue of free DMA descriptors indices. Only used in sync writing mode. */
    SemaphoreHandle_t       mutex;                      /* Serializes the public writing APIs (sync / cyclic) */
#if SOC_IS(ESP32)
    portMUX_TYPE            dma_lock;                   /* Serializes the link/restart decision between the sync writing task and the ISR */
    bool                    dma_running;                /* Whether the DMA is running (guarded by 'dma_lock'). Only used in sync writing mode. */
#endif

    gdma_link_list_handle_t link;                       /* DMA descriptor link list */
    uint8_t                 *bufs[];                    /* Array of DMA buffers pointers */
};

typedef enum {
    DAC_CONT_FSM_IDLE,
    DAC_CONT_FSM_REGISTERED,
    DAC_CONT_FSM_ENABLED,   // Ready and DMA is NOT running
    DAC_CONT_FSM_ASYNC,
    DAC_CONT_FSM_CYCLIC,
    DAC_CONT_FSM_SYNC,      // Sync writing mode. DMA may or may not be running.
    DAC_CONT_FSM_SYNC_WAIT, // Transition state for sync writing mode
    DAC_CONT_FSM_WAIT,      // Transition state
} dac_continuous_fsm_t;

static _Atomic dac_continuous_fsm_t s_dac_cont_fsm = DAC_CONT_FSM_IDLE;

static esp_err_t s_dac_continuous_stop_sync(dac_continuous_handle_t handle);

static void s_dac_free_dma_desc(dac_continuous_handle_t handle)
{
    if (handle->link != NULL) {
        gdma_del_link_list(handle->link);
        handle->link = NULL;
    }

    for (uint32_t i = 0; i < handle->cfg.desc_num; i++) {
        free(handle->bufs[i]);
        handle->bufs[i] = NULL;
    }
}

static esp_err_t s_dac_alloc_dma_desc(dac_continuous_handle_t handle)
{
    esp_err_t ret = ESP_OK;

    const uint32_t desc_num = handle->cfg.desc_num;
    const size_t buf_size = handle->cfg.buf_size;

    /* Allocate DMA buffers */
    for (uint32_t i = 0; i < desc_num; i++) {
        handle->bufs[i] = heap_caps_calloc(1, buf_size, DAC_DMA_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(handle->bufs[i], ESP_ERR_NO_MEM, err, TAG,  "failed to allocate dma buffer");
    }

    /**
     * Create the DMA descriptor link list.
     * The descriptor format of the link list item is binary-compatible with 'lldesc_t',
     * so the link list head address can be fed to the old DMA backend directly.
     */
    gdma_link_list_config_t link_cfg = {
        .num_items = desc_num,
        .item_alignment = 4,
        .flags = {
            .items_in_ext_mem = false,
            .check_owner = false,
        },
    };
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&link_cfg, &handle->link), err, TAG, "failed to create dma link list");

    /**
     * Mount each DMA buffer to its own link list item once.
     * The buffer<->item binding stays fixed afterwards.
     */
    for (uint32_t i = 0; i < desc_num; i++) {
        gdma_buffer_mount_config_t mount_cfg = {
            .buffer = handle->bufs[i],
            .length = buf_size,
            .buffer_alignment = 4,
            .flags = {
                .mark_final = GDMA_FINAL_LINK_TO_DEFAULT,
            },
        };
        ESP_GOTO_ON_ERROR(gdma_link_mount_buffers(handle->link, i, &mount_cfg, 1, NULL),
                          err, TAG, "failed to mount dma buffer");
    }

    return ESP_OK;
err:
    s_dac_free_dma_desc(handle);
    return ret;
}

bool dac_dma_done_callback(void *ctx)
{
    dac_continuous_handle_t handle = ctx;
    bool need_awoke = false;

    dac_continuous_fsm_t fsm = atomic_load(&s_dac_cont_fsm);
    if (fsm == DAC_CONT_FSM_SYNC || fsm == DAC_CONT_FSM_SYNC_WAIT) {
        /* Sync writing mode: Recycle the descriptor */
        BaseType_t tmp = pdFALSE;
        xQueueSendFromISR(handle->free_desc_queue, &handle->cur_index, &tmp);
        need_awoke |= (tmp == pdTRUE);
    }

    if (handle->cbs.on_convert_done) {
        dac_event_data_t evt_data = {
            .buf = handle->bufs[handle->cur_index],
            .buf_size = handle->cfg.buf_size,
            .write_bytes = gdma_link_get_length(handle->link, handle->cur_index),
        };
        need_awoke |= handle->cbs.on_convert_done(handle, &evt_data, handle->user_data);
    }

    handle->cur_index = (handle->cur_index + 1) % handle->used_desc_num;
    return need_awoke;
}

bool dac_dma_teof_callback(void *ctx)
{
    dac_continuous_handle_t handle = ctx;
    bool need_awoke = false;

    /**
     * Total EOF interrupt: DMA has reached the end of a descriptor chain (NULL next pointer).
     * This only occurs naturally in sync writing mode when all queued data has been transmitted.
     */
    bool dma_restart = false;
#if SOC_IS(ESP32)
    /**
     * Due to a hardware limitation affecting the ESP32 I2S DMA append() operation, dac_continuous_write()
     * uses start() to chain subsequent transfers. As a result, descriptor prefetching can cause issues.
     */
    dac_continuous_fsm_t fsm = atomic_load(&s_dac_cont_fsm);
    if (fsm == DAC_CONT_FSM_SYNC || fsm == DAC_CONT_FSM_SYNC_WAIT) {
        /* Check for any remaining descriptors (ignored due to prefetching), and restart the DMA */
        portENTER_CRITICAL_ISR(&handle->dma_lock);
        if (!handle->dma_running) {
            /* Stop already in progress, do not restart */
        } else if (gdma_link_check_end(handle->link, (int)handle->cur_index - 1) == false) {
            dac_priv_dma_trans_start(gdma_link_get_item_addr(handle->link, handle->cur_index));
            dma_restart = true;
        } else {
            handle->dma_running = false;
        }
        portEXIT_CRITICAL_ISR(&handle->dma_lock);
    }
#endif

    if (!dma_restart && handle->cbs.on_stop) {
        need_awoke |= handle->cbs.on_stop(handle, NULL, handle->user_data);
    }
    return need_awoke;
}

esp_err_t dac_continuous_new_channels(const dac_continuous_config_t *cont_cfg, dac_continuous_handle_t *ret_handle)
{
    /* Parameters validation */
    DAC_NULL_POINTER_CHECK(cont_cfg);
    DAC_NULL_POINTER_CHECK(ret_handle);
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL_MASK(cont_cfg->chan_mask) && cont_cfg->chan_mask, ESP_ERR_INVALID_ARG, TAG, "invalid dac channel mask");
    ESP_RETURN_ON_FALSE(cont_cfg->chan_mode != DAC_CHANNEL_MODE_ALTER || cont_cfg->chan_mask == DAC_CHANNEL_MASK_ALL,
                        ESP_ERR_INVALID_ARG, TAG, "alternate mode requires both DAC channels enabled");
    ESP_RETURN_ON_FALSE(cont_cfg->desc_num > 1, ESP_ERR_INVALID_ARG, TAG, "at least two DMA descriptor needed");
    ESP_RETURN_ON_FALSE(cont_cfg->buf_size > 0 && cont_cfg->buf_size % 2 == 0, ESP_ERR_INVALID_ARG, TAG, "buf_size must be a positive even number");
    ESP_RETURN_ON_FALSE(cont_cfg->buf_size <= DAC_DMA_MAX_BUF_SIZE, ESP_ERR_INVALID_ARG, TAG, "buf_size exceeds the maximum limit");

    esp_err_t ret = ESP_OK;

    /* FSM: IDLE -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_IDLE;
    if (!atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT)) {
        ESP_LOGE(TAG, "DAC continuous is already in use");
        return ESP_ERR_INVALID_STATE;
    }

    /* Register the channels */
    dac_channel_mask_t registered_chan_mask = 0;
    DAC_CHANNEL_MASK_FOREACH(chan, cont_cfg->chan_mask) {
        ESP_GOTO_ON_ERROR(dac_priv_channel_register(chan), err_dereg, TAG, "register dac channel %"PRIu32" failed", chan);
        registered_chan_mask |= BIT(chan);
    }

    /* Allocate continuous mode struct */
    dac_continuous_handle_t handle = heap_caps_calloc(1, sizeof(struct dac_continuous_s) + cont_cfg->desc_num * sizeof(uint8_t *), DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle, ESP_ERR_NO_MEM, err_dereg, TAG, "no memory for the dac continuous mode structure");

    handle->cfg = *cont_cfg;
    if (handle->cfg.clk_src == 0) {
        handle->cfg.clk_src = DAC_DIGI_CLK_SRC_DEFAULT;
    }

#if SOC_IS(ESP32)
    handle->dma_lock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
#endif

    handle->free_desc_queue = xQueueCreateWithCaps(cont_cfg->desc_num, sizeof(int), DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->free_desc_queue, ESP_ERR_NO_MEM, err_free, TAG, "Failed to create free descriptor queue");
    handle->mutex = xSemaphoreCreateMutexWithCaps(DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->mutex, ESP_ERR_NO_MEM, err_free, TAG, "Failed to create mutex");

    /* Create PM lock */
#if CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_lock_type = handle->cfg.clk_src == DAC_DIGI_CLK_SRC_APLL ? ESP_PM_NO_LIGHT_SLEEP : ESP_PM_APB_FREQ_MAX;
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(pm_lock_type, 0, "dac_driver", &handle->pm_lock), err_free, TAG, "Failed to create DAC pm lock");
#endif

    /* Create DMA descriptors and buffers */
    ESP_GOTO_ON_ERROR(s_dac_alloc_dma_desc(handle), err_free, TAG, "Failed to create DMA descriptors and buffers");

    /* Initialize DAC DMA peripheral */
    dac_dma_event_callbacks_t cbs = {
        .on_done = dac_dma_done_callback,
        .on_teof = dac_dma_teof_callback,
    };
    ESP_GOTO_ON_ERROR(dac_priv_dma_init(handle->cfg.clk_src, handle->cfg.freq_hz, handle->cfg.chan_mode == DAC_CHANNEL_MODE_ALTER, &cbs, handle),
                      err_desc, TAG, "Failed to initialize DAC DMA peripheral");

    /* FSM: WAIT -> REGISTERED */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_REGISTERED);

    *ret_handle = handle;
    return ret;

err_desc:
    s_dac_free_dma_desc(handle);
err_free:
    if (handle->free_desc_queue) {
        vQueueDeleteWithCaps(handle->free_desc_queue);
    }
    if (handle->mutex) {
        vSemaphoreDeleteWithCaps(handle->mutex);
    }
#if CONFIG_PM_ENABLE
    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
    }
#endif
    free(handle);
err_dereg:
    /* Deregister registered channels */
    DAC_CHANNEL_MASK_FOREACH(chan, registered_chan_mask) {
        dac_priv_channel_deregister(chan);
    }
    /* FSM: WAIT -> IDLE */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_IDLE);
    return ret;
}

esp_err_t dac_continuous_del_channels(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    /* FSM: REGISTERED -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_REGISTERED;
    if (!atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT)) {
        ESP_LOGE(TAG, "DAC continuous is not registered / disabled");
        return ESP_ERR_INVALID_STATE;
    }

    /* Deinitialize DMA peripheral */
    ESP_RETURN_ON_ERROR(dac_priv_dma_deinit(), TAG, "Failed to deinitialize DAC DMA peripheral");

    /* Free allocated resources */
    s_dac_free_dma_desc(handle);

    if (handle->free_desc_queue) {
        vQueueDeleteWithCaps(handle->free_desc_queue);
        handle->free_desc_queue = NULL;
    }
    if (handle->mutex) {
        vSemaphoreDeleteWithCaps(handle->mutex);
        handle->mutex = NULL;
    }

#if CONFIG_PM_ENABLE
    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
        handle->pm_lock = NULL;
    }
#endif

    /* Deregister the channels */
    DAC_CHANNEL_MASK_FOREACH(chan, handle->cfg.chan_mask) {
        dac_priv_channel_deregister(chan);
    }

    free(handle);

    /* FSM: WAIT -> IDLE */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_IDLE);
    return ESP_OK;
}

esp_err_t dac_continuous_register_event_callback(dac_continuous_handle_t handle, const dac_event_callbacks_t *callbacks, void *user_data)
{
    DAC_NULL_POINTER_CHECK(handle);

    if (callbacks == NULL) {
        memset(&handle->cbs, 0, sizeof(dac_event_callbacks_t));
        handle->user_data = NULL;
        return ESP_OK;
    }
#if CONFIG_DAC_ISR_IRAM_SAFE
    if (callbacks->on_convert_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_convert_done), ESP_ERR_INVALID_ARG, TAG, "on_convert_done callback not in IRAM");
    }
    if (callbacks->on_stop) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_stop), ESP_ERR_INVALID_ARG, TAG, "on_stop callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    handle->cbs = *callbacks;
    handle->user_data = user_data;

    return ESP_OK;
}

esp_err_t dac_continuous_enable(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    /* FSM: REGISTERED -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_REGISTERED;
    if (!atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT)) {
        ESP_LOGE(TAG, "DAC continuous is not registered / disabled");
        return ESP_ERR_INVALID_STATE;
    }

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif

    DAC_CHANNEL_MASK_FOREACH(chan, handle->cfg.chan_mask) {
        dac_priv_channel_enable(chan, DAC_DATA_SOURCE_DMA);
    }
    dac_priv_dma_enable();

    /* FSM: WAIT -> ENABLED */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_ENABLED);
    return ESP_OK;
}

esp_err_t dac_continuous_disable(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    /* For backward compatibility, check if there is any ongoing cyclic conversion and stop it */
    if (atomic_load(&s_dac_cont_fsm) == DAC_CONT_FSM_CYCLIC) {
        ESP_LOGW(TAG, "It is recommended to explicitly stop the cyclic conversion by calling dac_continuous_stop_cyclically() before performing other operations.");
        ESP_RETURN_ON_ERROR(dac_continuous_stop_cyclically(handle), TAG, "Failed to stop cyclic conversion");
    }

    /* Check if there is any ongoing SYNC writing and stop it */
    if (atomic_load(&s_dac_cont_fsm) == DAC_CONT_FSM_SYNC) {
        ESP_RETURN_ON_ERROR(s_dac_continuous_stop_sync(handle), TAG, "Failed to stop sync writing");
    }

    /* FSM: ENABLED -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_ENABLED;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "DAC continuous is running/not enabled");

    dac_priv_dma_disable();

    DAC_CHANNEL_MASK_FOREACH(chan, handle->cfg.chan_mask) {
        dac_priv_channel_disable(chan);
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(handle->pm_lock);
#endif

    /* FSM: WAIT -> REGISTERED */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_REGISTERED);
    return ESP_OK;
}

//////////////////////////////////// Async writing ////////////////////////////////////

esp_err_t dac_continuous_start_async_writing(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->cbs.on_convert_done, ESP_ERR_INVALID_STATE, TAG,
                        "please register 'on_convert_done' callback before starting asynchronous writing");

    /* For backward compatibility, check if there is any ongoing cyclic conversion and stop it */
    if (atomic_load(&s_dac_cont_fsm) == DAC_CONT_FSM_CYCLIC) {
        ESP_LOGW(TAG, "It is recommended to explicitly stop the cyclic conversion by calling dac_continuous_stop_cyclically() before performing other operations.");
        ESP_RETURN_ON_ERROR(dac_continuous_stop_cyclically(handle), TAG, "Failed to stop cyclic conversion");
    }

    /* Check if there is any ongoing SYNC writing and stop it */
    if (atomic_load(&s_dac_cont_fsm) == DAC_CONT_FSM_SYNC) {
        ESP_RETURN_ON_ERROR(s_dac_continuous_stop_sync(handle), TAG, "Failed to stop sync writing");
    }

    /* FSM: ENABLED -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_ENABLED;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "DAC continuous is running/not enabled");

    /* Link all descriptors as a ring */
    for (int i = 0; i < handle->cfg.desc_num; i++) {
        memset(handle->bufs[i], 0, handle->cfg.buf_size);
        gdma_link_set_length(handle->link, i, handle->cfg.buf_size);
        gdma_link_set_owner(handle->link, i, GDMA_LLI_OWNER_DMA);
        gdma_link_concat(handle->link, i, handle->link, (i < handle->cfg.desc_num - 1) ? i + 1 : 0);
    }

    handle->cur_index = 0;
    handle->used_desc_num = handle->cfg.desc_num;
    /* Start with an all-zero buffer. User will be notified by the 'on_convert_done' callback, then load the data into the buffer. */
    dac_priv_dma_trans_start(gdma_link_get_head_addr(handle->link));

    /* FSM: WAIT -> ASYNC */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_ASYNC);

    return ESP_OK;
}

esp_err_t dac_continuous_stop_async_writing(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    /* FSM: ASYNC -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_ASYNC;
    if (!atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT)) {
        ESP_LOGE(TAG, "DAC continuous is not in asynchronous writing mode");
        return ESP_ERR_INVALID_STATE;
    }

    dac_priv_dma_trans_stop();

    /* FSM: WAIT -> ENABLED */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_ENABLED);

    return ESP_OK;
}

//////////////////////////////////// Data loading ////////////////////////////////////

#define DAC_SAMPLE_TYPE_U8   1
#define DAC_SAMPLE_TYPE_U16  2

/**
 * The caller has already aligned each sample to 16 bits.
 * Each sample then occupies 2 bytes in the input buffer, and the length
 * arguments of the write APIs are in bytes rather than samples.
 */
#if SOC_DAC_DMA_16BIT_ALIGN && !CONFIG_DAC_DMA_AUTO_16BIT_ALIGN
#define DAC_MANUAL_16BIT_ALIGN  1
#else
#define DAC_MANUAL_16BIT_ALIGN  0
#endif

/**
 * @brief The type logically suited for storing DAC samples.
 */
FORCE_INLINE_ATTR uint8_t s_dac_sample_type(dac_continuous_handle_t handle)
{
    // TODO: get the bitwidth from the channel configuration (dac_priv_chan_cfg_get_bitwidth()), then infer the type
    (void)handle;
#if SOC_IS(ESP32) || SOC_IS(ESP32S2)
    return DAC_SAMPLE_TYPE_U8;
#elif SOC_IS(ESP32S31)
    return DAC_SAMPLE_TYPE_U16;
#else
#error "Unsupported SOC"
#endif
}

/**
 * @brief The actual number of bytes occupied by each sample in the DMA buffer.
 */
FORCE_INLINE_ATTR size_t s_dac_dma_bytes_per_sample(dac_continuous_handle_t handle)
{
#if SOC_DAC_DMA_16BIT_ALIGN
    (void)handle;
    return 2;
#else
    return s_dac_sample_type(handle);
#endif
}

/**
 * @brief The actual number of bytes occupied by each sample in the input data.
 */
FORCE_INLINE_ATTR size_t s_dac_input_bytes_per_sample(dac_continuous_handle_t handle)
{
#if DAC_MANUAL_16BIT_ALIGN
    (void)handle;
    return 2;
#else
    return s_dac_sample_type(handle);
#endif
}

/**
 * @brief Load samples into the DMA descriptor
 *
 * @param desc_index Index of the DMA descriptor to load data into.
 * @param cnt Number of samples.
 * @param auto_balance Whether to balance the data between the last two descriptors. If disabled, we will load as much data as possible.
 * @return Loaded sample count. The remaining sample count is (cnt - return_value)
 */
size_t dac_load_data_into_desc(dac_continuous_handle_t handle, int desc_index, const void *data, size_t cnt, bool auto_balance)
{
    const size_t dma_bps = s_dac_dma_bytes_per_sample(handle);
    const size_t sample_per_buf = handle->cfg.buf_size / dma_bps;
    size_t load_cnt;
    if (cnt <= sample_per_buf) {
        load_cnt = cnt;  // load all
    } else if (auto_balance && cnt < sample_per_buf * 2) {
        /**
         * The remaining samples can fit into two descriptors.
         * Load about half now so the next round takes the rest.
         */
        load_cnt = (cnt + 1) / 2;
    } else {
        load_cnt = sample_per_buf;  // load as much as possible
    }

    if (s_dac_sample_type(handle) == DAC_SAMPLE_TYPE_U8) {
        const uint8_t *src = data;
        uint8_t *dst = handle->bufs[desc_index];
        for (size_t i = 0; i < load_cnt; i++) {
#if DAC_MANUAL_16BIT_ALIGN
            uint8_t code = src[i * 2 + 1];
#else
            uint8_t code = src[i];
#endif
            code += handle->cfg.offset;
#if SOC_DAC_DMA_16BIT_ALIGN
            dst[i * 2 + 1] = code;
#else
            dst[i] = code;
#endif
        }
    } else {  // DAC_SAMPLE_TYPE_U16
        const uint16_t *src = data;
        uint16_t *dst = (uint16_t *)handle->bufs[desc_index];
        for (size_t i = 0; i < load_cnt; i++) {
            dst[i] = src[i] + handle->cfg.offset;
        }
    }

    gdma_link_set_length(handle->link, desc_index, load_cnt * dma_bps);
    gdma_link_set_owner(handle->link, desc_index, GDMA_LLI_OWNER_DMA);

    return load_cnt;
}

esp_err_t dac_continuous_write_asynchronously(dac_continuous_handle_t handle, uint8_t *dma_buf, size_t dma_buf_len,
                                              const void *data, size_t cnt, size_t *loaded_cnt)
{
    DAC_NULL_POINTER_CHECK_ISR(handle);
    DAC_NULL_POINTER_CHECK_ISR(dma_buf);
    DAC_NULL_POINTER_CHECK_ISR(data);
    ESP_RETURN_ON_FALSE_ISR(cnt > 0, ESP_ERR_INVALID_ARG, TAG, "cnt must be > 0");
#if DAC_MANUAL_16BIT_ALIGN
    ESP_RETURN_ON_FALSE_ISR(cnt % 2 == 0, ESP_ERR_INVALID_ARG, TAG, "cnt must be even when AUTO_16BIT_ALIGN is disabled");
    cnt /= 2;   // Make cnt represent the actual number of samples.
#endif

    /* FSM: ASYNC -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_ASYNC;
    if (!atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT)) {
        ESP_EARLY_LOGE(TAG, "DAC continuous is not in asynchronous writing mode");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_OK;

    /**
     * Normally, dma_buf_len should always be equal to the buffer size of descriptors
     */
    if (dma_buf_len != handle->cfg.buf_size) {
        ESP_EARLY_LOGW(TAG, "dma_buf_len != DMA buffer size. This parameter is ignored.");
    }

    /* Find the corresponding DMA descriptor index */
    int index = 0;
    for (; index < handle->cfg.desc_num; index++) {
        if (dma_buf == handle->bufs[index]) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE_ISR(index < handle->cfg.desc_num, ESP_ERR_NOT_FOUND, clean_up, TAG, "Corresponding DMA descriptor not found");

    /* Load data into DMA buffer. We disable the auto balance here because the total length is actually uncertain. */
    size_t loaded = dac_load_data_into_desc(handle, index, data, cnt, false);
    if (loaded_cnt) {
#if DAC_MANUAL_16BIT_ALIGN
        *loaded_cnt = loaded * 2;    // Convert to a byte count for compatibility with the legacy semantics.
#else
        *loaded_cnt = loaded;
#endif
    }

clean_up:
    /* FSM: WAIT -> ASYNC */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_ASYNC);
    return ret;
}

//////////////////////////////////// Cyclic writing ////////////////////////////////////

esp_err_t dac_continuous_write_cyclically(dac_continuous_handle_t handle, const void *data, size_t cnt, size_t *loaded_cnt)
{
    DAC_NULL_POINTER_CHECK(handle);
    DAC_NULL_POINTER_CHECK(data);
    ESP_RETURN_ON_FALSE(cnt > 0, ESP_ERR_INVALID_ARG, TAG, "cnt must be > 0");
#if DAC_MANUAL_16BIT_ALIGN
    ESP_RETURN_ON_FALSE(cnt % 2 == 0, ESP_ERR_INVALID_ARG, TAG, "cnt must be even when AUTO_16BIT_ALIGN is disabled");
    cnt /= 2;   // Make cnt represent the actual number of samples.
#endif
    ESP_RETURN_ON_FALSE(cnt <= (handle->cfg.buf_size / s_dac_dma_bytes_per_sample(handle)) * handle->cfg.desc_num,
                        ESP_ERR_INVALID_ARG, TAG, "Data size exceeds the total DMA buffer size");

    esp_err_t ret = ESP_OK;

    /* Serialize with the other writing APIs */
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->mutex, portMAX_DELAY) == pdTRUE,
                        ESP_ERR_TIMEOUT, TAG, "Take mutex timeout");

    /* For backward compatibility, check if there is any ongoing cyclic conversion and stop it */
    if (atomic_load(&s_dac_cont_fsm) == DAC_CONT_FSM_CYCLIC) {
        ESP_GOTO_ON_ERROR(dac_continuous_stop_cyclically(handle), err, TAG, "Failed to stop cyclic conversion");
    }

    /* Check if there is any ongoing SYNC writing and stop it */
    if (atomic_load(&s_dac_cont_fsm) == DAC_CONT_FSM_SYNC) {
        ESP_GOTO_ON_ERROR(s_dac_continuous_stop_sync(handle), err, TAG, "Failed to stop sync writing");
    }

    /* FSM: ENABLED -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_ENABLED;
    ESP_GOTO_ON_FALSE(atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT),
                      ESP_ERR_INVALID_STATE, err, TAG, "DAC continuous is running/not enabled");

    size_t remain = cnt;
    const uint8_t *src = data;
    uint32_t index = 0;
    for (; index < handle->cfg.desc_num && remain > 0; index++) {
        size_t loaded = dac_load_data_into_desc(handle, index, src, remain, true);
        remain -= loaded;
        src += loaded * s_dac_input_bytes_per_sample(handle);
    }
    /* All data should be loaded */
    assert(remain == 0);

    /* Link the used descriptors as a ring: 0 -> 1 -> ... -> (index-1) -> 0 */
    for (int k = 0; k < index - 1; k++) {
        gdma_link_concat(handle->link, k, handle->link, k + 1);
    }
    gdma_link_concat(handle->link, index - 1, handle->link, 0);

    handle->cur_index = 0;
    handle->used_desc_num = index;
    dac_priv_dma_trans_start(gdma_link_get_head_addr(handle->link));

    /* FSM: WAIT -> CYCLIC */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_CYCLIC);

    if (loaded_cnt) {
#if DAC_MANUAL_16BIT_ALIGN
        *loaded_cnt = cnt * 2;    // Convert to a byte count for compatibility with the legacy semantics.
#else
        *loaded_cnt = cnt;
#endif
    }

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t dac_continuous_stop_cyclically(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);

    /* FSM: CYCLIC -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_CYCLIC;
    if (!atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT)) {
        ESP_LOGE(TAG, "DAC continuous is not in cyclic writing mode");
        return ESP_ERR_INVALID_STATE;
    }

    dac_priv_dma_trans_stop();

    /* FSM: WAIT -> ENABLED */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_ENABLED);

    return ESP_OK;
}

//////////////////////////////////// Synchronous writing ////////////////////////////////////

esp_err_t dac_continuous_write(dac_continuous_handle_t handle, const void *data, size_t cnt, size_t *loaded_cnt, int timeout_ms)
{
    DAC_NULL_POINTER_CHECK(handle);
    DAC_NULL_POINTER_CHECK(data);
    ESP_RETURN_ON_FALSE(cnt > 0, ESP_ERR_INVALID_ARG, TAG, "cnt must be > 0");
#if DAC_MANUAL_16BIT_ALIGN
    ESP_RETURN_ON_FALSE(cnt % 2 == 0, ESP_ERR_INVALID_ARG, TAG, "cnt must be even when AUTO_16BIT_ALIGN is disabled");
    cnt /= 2;   // Make cnt represent the actual number of samples.
#endif

    esp_err_t ret = ESP_OK;

    TickType_t timeout_tick = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    const uint8_t *src = data;
    size_t remain = cnt;

    /* Serialize with the other writing APIs */
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->mutex, timeout_tick) == pdTRUE,
                        ESP_ERR_TIMEOUT, TAG, "Take mutex timeout");

    dac_continuous_fsm_t fsm = atomic_load(&s_dac_cont_fsm);
    switch (fsm) {
    case DAC_CONT_FSM_CYCLIC:
        /* For backward compatibility, check if there is any ongoing cyclic conversion and stop it */
        ESP_LOGW(TAG, "It is recommended to explicitly stop the cyclic conversion by calling dac_continuous_stop_cyclically() before performing other operations.");
        ESP_GOTO_ON_ERROR(dac_continuous_stop_cyclically(handle), err, TAG, "Failed to stop cyclic conversion");
        [[fallthrough]];

    case DAC_CONT_FSM_ENABLED:
        /* FSM: ENABLED -> SYNC_WAIT */
        dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_ENABLED;
        ESP_GOTO_ON_FALSE(atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_SYNC_WAIT),
                          ESP_ERR_INVALID_STATE, err, TAG, "DAC continuous is running/not enabled");

        /* Reset the free_desc_queue and the cur_index */
        xQueueReset(handle->free_desc_queue);
        for (int i = 1; i < handle->cfg.desc_num; i++) {    // skip 0 because we will use it right now
            xQueueSend(handle->free_desc_queue, &i, 0);
        }
        handle->cur_index = 0;
        handle->used_desc_num = handle->cfg.desc_num;

        /* Load one descriptor and start the DMA */
        size_t loaded = dac_load_data_into_desc(handle, 0, src, remain, true);
        remain -= loaded;
        src += loaded * s_dac_input_bytes_per_sample(handle);
        gdma_link_concat(handle->link, 0, NULL, 0);
#if SOC_IS(ESP32)
        /* It is safe to operate without the lock here because the DMA is not running yet. */
        handle->dma_running = true;
#endif
        dac_priv_dma_trans_start(gdma_link_get_head_addr(handle->link));

        goto skip_cas;

    case DAC_CONT_FSM_SYNC:
        /* FSM: SYNC -> SYNC_WAIT */
        expected_fsm = DAC_CONT_FSM_SYNC;
        ESP_GOTO_ON_FALSE(atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_SYNC_WAIT),
                          ESP_ERR_INVALID_STATE, err, TAG, "CAS failed: SYNC -> SYNC_WAIT");

skip_cas:
        while (remain > 0) {
            int index;
            if (xQueueReceive(handle->free_desc_queue, &index, timeout_tick) != pdTRUE) {
                ret = ESP_ERR_TIMEOUT;
                break;
            }
            size_t loaded = dac_load_data_into_desc(handle, index, src, remain, true);
            remain -= loaded;
            src += loaded * s_dac_input_bytes_per_sample(handle);
            /**
             * link: (index-1) -> index -> NULL
             * NOTE: gdma_link_concat() can normalize the index to be between 0 and desc_num - 1.
             */
            gdma_link_concat(handle->link, index, NULL, 0);

#if SOC_IS(ESP32)
            /**
             * The ESP32 I2S DMA append() (restart) has a hardware limitation, so we re-issue start() when the DMA has stopped. See IDF-15791.
             * Synchronize with the TEOF handler via dma_lock to prevent duplicate or missed starts.
             */
            portENTER_CRITICAL(&handle->dma_lock);
            gdma_link_concat(handle->link, index - 1, handle->link, index);
            if (!handle->dma_running) {
                handle->dma_running = true;
                dac_priv_dma_trans_start(gdma_link_get_item_addr(handle->link, index));
            }
            portEXIT_CRITICAL(&handle->dma_lock);
#else
            gdma_link_concat(handle->link, index - 1, handle->link, index);
            dac_priv_dma_trans_append();
#endif
        }
        break;

    default:
        ESP_LOGE(TAG, "Unexpected FSM state: %u", fsm);
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }

    /* FSM: SYNC_WAIT -> SYNC */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_SYNC);
err:
    xSemaphoreGive(handle->mutex);
    if (loaded_cnt) {
#if DAC_MANUAL_16BIT_ALIGN
        *loaded_cnt = (cnt - remain) * 2;    // Convert to a byte count for compatibility with the legacy semantics.
#else
        *loaded_cnt = cnt - remain;
#endif
    }
    return ret;
}

static esp_err_t s_dac_continuous_stop_sync(dac_continuous_handle_t handle)
{
    /* FSM: SYNC -> WAIT */
    dac_continuous_fsm_t expected_fsm = DAC_CONT_FSM_SYNC;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&s_dac_cont_fsm, &expected_fsm, DAC_CONT_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "DAC continuous is not in sync writing mode");

#if SOC_IS(ESP32)
    /**
     * Serialize with the TEOF ISR which may also call trans_start() on another core.
     * Both must be guarded by dma_lock to prevent concurrent hardware register access.
     */
    portENTER_CRITICAL(&handle->dma_lock);
    dac_priv_dma_trans_stop();
    handle->dma_running = false;
    portEXIT_CRITICAL(&handle->dma_lock);
#else
    dac_priv_dma_trans_stop();
#endif

    /* FSM: WAIT -> ENABLED */
    atomic_store(&s_dac_cont_fsm, DAC_CONT_FSM_ENABLED);

    return ESP_OK;
}

uint8_t dac_continuous_get_bitwidth(dac_continuous_handle_t handle)
{
    if (!handle) {
        return 0;
    }

    return SOC_DAC_RESOLUTION;
}
