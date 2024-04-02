/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/idf_additions.h"

#include "sdkconfig.h"

#if CONFIG_I2S_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"

#include "soc/i2s_periph.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"
#include "hal/i2s_hal.h"
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#endif

#if SOC_I2S_SUPPORTS_ADC_DAC
#include "hal/adc_ll.h"
#endif
#if SOC_I2S_SUPPORTS_APLL
#include "hal/clk_tree_ll.h"
#include "clk_ctrl_os.h"
#endif

#include "esp_private/i2s_platform.h"
#include "esp_private/esp_clk.h"

#include "driver/gpio.h"
#include "driver/i2s_common.h"
#include "i2s_private.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp_clock_output.h"
#endif

#include "clk_ctrl_os.h"
#include "esp_clk_tree.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_dma_utils.h"
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_cache.h"
#endif

#include "esp_rom_gpio.h"
#include "esp_memory_utils.h"

/* The actual max size of DMA buffer is 4095
 * Set 4092 here to align with 4-byte, so that the position of the slot data in the buffer will be relatively fixed */
#define I2S_DMA_BUFFER_MAX_SIZE     (4092)

static const char *TAG = "i2s_common";

__attribute__((always_inline))
inline void *i2s_dma_calloc(i2s_chan_handle_t handle, size_t num, size_t size, size_t *actual_size)
{
    void *ptr = NULL;
    esp_dma_mem_info_t dma_mem_info = {
        .extra_heap_caps = I2S_DMA_ALLOC_CAPS,
        .dma_alignment_bytes = 4,
    };
    //TODO: IDF-9636
    esp_dma_capable_calloc(num, size, &dma_mem_info, &ptr, actual_size);
    return ptr;
}

/*---------------------------------------------------------------------------
                             I2S Static APIs
 ----------------------------------------------------------------------------
    Scope: This file only
 ----------------------------------------------------------------------------*/

static void i2s_tx_channel_start(i2s_chan_handle_t handle)
{
    i2s_hal_tx_reset(&(handle->controller->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset((handle->dma.dma_chan));
#else
    i2s_hal_tx_reset_dma(&(handle->controller->hal));
#endif
    i2s_hal_tx_reset_fifo(&(handle->controller->hal));
#if SOC_GDMA_SUPPORTED
    gdma_start((handle->dma.dma_chan), (uint32_t) handle->dma.desc[0]);
#else
    esp_intr_enable(handle->dma.dma_chan);
    i2s_hal_tx_enable_intr(&(handle->controller->hal));
    i2s_hal_tx_enable_dma(&(handle->controller->hal));
    i2s_hal_tx_start_link(&(handle->controller->hal), (uint32_t) handle->dma.desc[0]);
#endif
    i2s_hal_tx_start(&(handle->controller->hal));
}

static void i2s_rx_channel_start(i2s_chan_handle_t handle)
{
    i2s_hal_rx_reset(&(handle->controller->hal));
#if SOC_GDMA_SUPPORTED
    gdma_reset(handle->dma.dma_chan);
#else
    i2s_hal_rx_reset_dma(&(handle->controller->hal));
#endif
    i2s_hal_rx_reset_fifo(&(handle->controller->hal));
#if SOC_GDMA_SUPPORTED
    gdma_start(handle->dma.dma_chan, (uint32_t) handle->dma.desc[0]);
#else
    esp_intr_enable(handle->dma.dma_chan);
    i2s_hal_rx_enable_intr(&(handle->controller->hal));
    i2s_hal_rx_enable_dma(&(handle->controller->hal));
    i2s_hal_rx_start_link(&(handle->controller->hal), (uint32_t) handle->dma.desc[0]);
#endif
    i2s_hal_rx_start(&(handle->controller->hal));
}

static void i2s_tx_channel_stop(i2s_chan_handle_t handle)
{
    i2s_hal_tx_stop(&(handle->controller->hal));
#if SOC_GDMA_SUPPORTED
    gdma_stop(handle->dma.dma_chan);
#else
    i2s_hal_tx_stop_link(&(handle->controller->hal));
    i2s_hal_tx_disable_intr(&(handle->controller->hal));
    i2s_hal_tx_disable_dma(&(handle->controller->hal));
    esp_intr_disable(handle->dma.dma_chan);
#endif
}

static void i2s_rx_channel_stop(i2s_chan_handle_t handle)
{
    i2s_hal_rx_stop(&(handle->controller->hal));
#if SOC_GDMA_SUPPORTED
    gdma_stop(handle->dma.dma_chan);
#else
    i2s_hal_rx_stop_link(&(handle->controller->hal));
    i2s_hal_rx_disable_intr(&(handle->controller->hal));
    i2s_hal_rx_disable_dma(&(handle->controller->hal));
    esp_intr_disable(handle->dma.dma_chan);
#endif
}

static esp_err_t i2s_destroy_controller_obj(i2s_controller_t **i2s_obj)
{
    I2S_NULL_POINTER_CHECK(TAG, i2s_obj);
    I2S_NULL_POINTER_CHECK(TAG, *i2s_obj);
    ESP_RETURN_ON_FALSE(!(*i2s_obj)->rx_chan && !(*i2s_obj)->tx_chan,
                        ESP_ERR_INVALID_STATE, TAG,
                        "there still have channels under this i2s controller");
    int id = (*i2s_obj)->id;
#if CONFIG_IDF_TARGET_ESP32
    if ((*i2s_obj)->mclk_out_hdl) {
        esp_clock_output_stop((*i2s_obj)->mclk_out_hdl);
    }
#endif
#if SOC_I2S_HW_VERSION_1
    i2s_ll_enable_dma((*i2s_obj)->hal.dev, false);
#endif
    free(*i2s_obj);
    *i2s_obj = NULL;
    return i2s_platform_release_occupation(id);
}

/**
 * @brief Acquire i2s controller object
 *
 * @param id        i2s port id
 * @param search_reverse   reverse the sequence of port acquirement
 *                  set false to acquire from I2S_NUM_0 first
 *                  set true to acquire from SOC_I2S_NUM - 1 first
 * @return
 *      - pointer of acquired i2s controller object
 */
static i2s_controller_t *i2s_acquire_controller_obj(int id)
{
    if (id < 0 || id >= SOC_I2S_NUM) {
        return NULL;
    }
    /* pre-alloc controller object */
    i2s_controller_t *pre_alloc = (i2s_controller_t *)heap_caps_calloc(1, sizeof(i2s_controller_t), I2S_MEM_ALLOC_CAPS);
    if (pre_alloc == NULL) {
        return NULL;
    }
    pre_alloc->id = id;
    i2s_hal_init(&pre_alloc->hal, id);
    pre_alloc->full_duplex = false;
    pre_alloc->tx_chan = NULL;
    pre_alloc->rx_chan = NULL;
    pre_alloc->mclk = I2S_GPIO_UNUSED;

    i2s_controller_t *i2s_obj = NULL;
    /* Try to occupy this i2s controller */
    if (i2s_platform_acquire_occupation(id, "i2s_driver") == ESP_OK) {
        portENTER_CRITICAL(&g_i2s.spinlock);
        i2s_obj = pre_alloc;
        g_i2s.controller[id] = i2s_obj;
        portEXIT_CRITICAL(&g_i2s.spinlock);
#if SOC_I2S_SUPPORTS_ADC_DAC
        if (id == I2S_NUM_0) {
            adc_ll_digi_set_data_source(0);
        }
#endif
    } else {
        free(pre_alloc);
        portENTER_CRITICAL(&g_i2s.spinlock);
        if (g_i2s.controller[id]) {
            i2s_obj = g_i2s.controller[id];
        }
        portEXIT_CRITICAL(&g_i2s.spinlock);
        if (i2s_obj == NULL) {
            ESP_LOGE(TAG, "i2s%d might be occupied by other component", id);
        }
    }

    return i2s_obj;
}

static inline bool i2s_take_available_channel(i2s_controller_t *i2s_obj, uint8_t chan_search_mask)
{
    bool is_available = false;

#if SOC_I2S_HW_VERSION_1
    /* In ESP32 and ESP32-S2, tx channel and rx channel are not totally separated
     * Take both two channels in case one channel can affect another
     */
    chan_search_mask = I2S_DIR_RX | I2S_DIR_TX;
#endif
    portENTER_CRITICAL(&g_i2s.spinlock);
    if (!(chan_search_mask & i2s_obj->chan_occupancy)) {
        i2s_obj->chan_occupancy |= chan_search_mask;
        is_available = true;
    }
    portEXIT_CRITICAL(&g_i2s.spinlock);
    return is_available;
}

static esp_err_t i2s_register_channel(i2s_controller_t *i2s_obj, i2s_dir_t dir, uint32_t desc_num)
{
    I2S_NULL_POINTER_CHECK(TAG, i2s_obj);

    esp_err_t ret = ESP_OK;

    i2s_chan_handle_t new_chan = (i2s_chan_handle_t)heap_caps_calloc(1, sizeof(struct i2s_channel_obj_t), I2S_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(new_chan, ESP_ERR_NO_MEM, TAG, "No memory for new channel");
    new_chan->mode = I2S_COMM_MODE_NONE;
    new_chan->role = I2S_ROLE_MASTER; // Set default role to master
    new_chan->dir = dir;
    new_chan->state = I2S_CHAN_STATE_REGISTER;
#if SOC_I2S_SUPPORTS_APLL
    new_chan->apll_en = false;
#endif
    new_chan->mode_info = NULL;
    new_chan->controller = i2s_obj;
#if CONFIG_PM_ENABLE
    new_chan->pm_lock = NULL; // Init in i2s_set_clock according to clock source
#endif
    new_chan->msg_queue = xQueueCreateWithCaps(desc_num - 1, sizeof(uint8_t *), I2S_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(new_chan->msg_queue, ESP_ERR_NO_MEM, err, TAG, "No memory for message queue");
    new_chan->mutex = xSemaphoreCreateMutexWithCaps(I2S_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(new_chan->mutex, ESP_ERR_NO_MEM, err, TAG, "No memory for mutex semaphore");
    new_chan->binary = xSemaphoreCreateBinaryWithCaps(I2S_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(new_chan->binary, ESP_ERR_NO_MEM, err, TAG, "No memory for binary semaphore");

    new_chan->callbacks.on_recv = NULL;
    new_chan->callbacks.on_recv_q_ovf = NULL;
    new_chan->callbacks.on_sent = NULL;
    new_chan->callbacks.on_send_q_ovf = NULL;
    new_chan->dma.rw_pos = 0;
    new_chan->dma.curr_ptr = NULL;
    new_chan->dma.curr_desc = NULL;
    new_chan->start = NULL;
    new_chan->stop = NULL;
    new_chan->reserve_gpio_mask = 0;

    if (dir == I2S_DIR_TX) {
        if (i2s_obj->tx_chan) {
            i2s_del_channel(i2s_obj->tx_chan);
        }
        i2s_obj->tx_chan = new_chan;

    } else {
        if (i2s_obj->rx_chan) {
            i2s_del_channel(i2s_obj->rx_chan);
        }
        i2s_obj->rx_chan = new_chan;
    }
    return ret;
err:
    if (new_chan->msg_queue) {
        vQueueDeleteWithCaps(new_chan->msg_queue);
    }
    if (new_chan->mutex) {
        vSemaphoreDeleteWithCaps(new_chan->mutex);
    }
    if (new_chan->binary) {
        vSemaphoreDeleteWithCaps(new_chan->binary);
    }
    free(new_chan);

    return ret;
}

#ifndef __cplusplus
/* To make sure the i2s_event_callbacks_t is same size as i2s_event_callbacks_internal_t */
_Static_assert(sizeof(i2s_event_callbacks_t) == sizeof(i2s_event_callbacks_internal_t), "Invalid size of i2s_event_callbacks_t structure");
#endif

esp_err_t i2s_channel_register_event_callback(i2s_chan_handle_t handle, const i2s_event_callbacks_t *callbacks, void *user_data)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, callbacks);
    esp_err_t ret = ESP_OK;
#if CONFIG_I2S_ISR_IRAM_SAFE
    if (callbacks->on_recv) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_recv), ESP_ERR_INVALID_ARG, TAG, "on_recv callback not in IRAM");
    }
    if (callbacks->on_recv_q_ovf) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_recv_q_ovf), ESP_ERR_INVALID_ARG, TAG, "on_recv_q_ovf callback not in IRAM");
    }
    if (callbacks->on_sent) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_sent), ESP_ERR_INVALID_ARG, TAG, "on_sent callback not in IRAM");
    }
    if (callbacks->on_send_q_ovf) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_send_q_ovf), ESP_ERR_INVALID_ARG, TAG, "on_send_q_ovf callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state < I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, err, TAG, "invalid state, I2S has enabled");
    memcpy(&(handle->callbacks), callbacks, sizeof(i2s_event_callbacks_t));
    handle->user_data = user_data;
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

uint32_t i2s_get_buf_size(i2s_chan_handle_t handle, uint32_t data_bit_width, uint32_t dma_frame_num)
{
    uint32_t active_chan = handle->active_slot;
    uint32_t bytes_per_sample = ((data_bit_width + 15) / 16) * 2;
    uint32_t bytes_per_frame = bytes_per_sample * active_chan;
    uint32_t bufsize = dma_frame_num * bytes_per_frame;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    /* bufsize need to align with cache line size */
    uint32_t alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    uint32_t aligned_frame_num = dma_frame_num;
    /* To make the buffer aligned with the cache line size, search for the ceil aligned size first,
       If the buffer size exceed the max DMA buffer size, toggle the sign to search for the floor aligned size */
    for (int sign = 1; bufsize % alignment != 0; aligned_frame_num += sign) {
        bufsize = aligned_frame_num * bytes_per_frame;
        /* If the buffer size exceed the max dma size */
        if (bufsize > I2S_DMA_BUFFER_MAX_SIZE) {
            sign = -1; // toggle the search sign
            aligned_frame_num = dma_frame_num;              // Reset the frame num
            bufsize = aligned_frame_num * bytes_per_frame;  // Reset the bufsize
        }
    }
    if (bufsize / bytes_per_frame != dma_frame_num) {
        ESP_LOGW(TAG, "dma frame num is adjusted to %"PRIu32" to align the dma buffer with %"PRIu32
                 ", bufsize = %"PRIu32, bufsize / bytes_per_frame, alignment, bufsize);
    }
#endif
    /* Limit DMA buffer size if it is out of range (DMA buffer limitation is 4092 bytes) */
    if (bufsize > I2S_DMA_BUFFER_MAX_SIZE) {
        uint32_t frame_num = I2S_DMA_BUFFER_MAX_SIZE / bytes_per_frame;
        bufsize = frame_num * bytes_per_frame;
        ESP_LOGW(TAG, "dma frame num is out of dma buffer size, limited to %"PRIu32, frame_num);
    }
    return bufsize;
}

esp_err_t i2s_free_dma_desc(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    if (!handle->dma.desc) {
        return ESP_OK;
    }
    for (int i = 0; i < handle->dma.desc_num; i++) {
        if (handle->dma.bufs[i]) {
            free(handle->dma.bufs[i]);
            handle->dma.bufs[i] = NULL;
        }
        if (handle->dma.desc[i]) {
            free(handle->dma.desc[i]);
            handle->dma.desc[i] = NULL;
        }
    }
    if (handle->dma.bufs) {
        free(handle->dma.bufs);
        handle->dma.bufs = NULL;
    }
    if (handle->dma.desc) {
        free(handle->dma.desc);
        handle->dma.desc = NULL;
    }

    return ESP_OK;
}

esp_err_t i2s_alloc_dma_desc(i2s_chan_handle_t handle, uint32_t num, uint32_t bufsize)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(bufsize <= I2S_DMA_BUFFER_MAX_SIZE, ESP_ERR_INVALID_ARG, TAG, "dma buffer can't be bigger than %d", I2S_DMA_BUFFER_MAX_SIZE);
    handle->dma.desc_num = num;
    handle->dma.buf_size = bufsize;

    /* Descriptors must be in the internal RAM */
    handle->dma.desc = (lldesc_t **)heap_caps_calloc(num, sizeof(lldesc_t *), I2S_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->dma.desc, ESP_ERR_NO_MEM, err, TAG, "create I2S DMA descriptor array failed");
    handle->dma.bufs = (uint8_t **)heap_caps_calloc(num, sizeof(uint8_t *), I2S_MEM_ALLOC_CAPS);
    size_t desc_size = 0;
    for (int i = 0; i < num; i++) {
        /* Allocate DMA descriptor */
        handle->dma.desc[i] = (lldesc_t *) i2s_dma_calloc(handle, 1, sizeof(lldesc_t), &desc_size);
        ESP_GOTO_ON_FALSE(handle->dma.desc[i], ESP_ERR_NO_MEM, err, TAG,  "allocate DMA description failed");
        handle->dma.desc[i]->owner = 1;
        handle->dma.desc[i]->eof = 1;
        handle->dma.desc[i]->sosf = 0;
        handle->dma.desc[i]->length = bufsize;
        handle->dma.desc[i]->size = bufsize;
        handle->dma.desc[i]->offset = 0;
        handle->dma.bufs[i] = (uint8_t *) i2s_dma_calloc(handle, 1, bufsize * sizeof(uint8_t), NULL);
        ESP_GOTO_ON_FALSE(handle->dma.bufs[i], ESP_ERR_NO_MEM, err, TAG,  "allocate DMA buffer failed");
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync(handle->dma.bufs[i], bufsize * sizeof(uint8_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
        handle->dma.desc[i]->buf = handle->dma.bufs[i];
        ESP_LOGV(TAG, "desc addr: %8p\tbuffer addr:%8p", handle->dma.desc[i], handle->dma.bufs[i]);
    }
    /* Connect DMA descriptor as a circle */
    for (int i = 0; i < num; i++) {
        /* Link to the next descriptor */
        STAILQ_NEXT(handle->dma.desc[i], qe) = (i < (num - 1)) ? (handle->dma.desc[i + 1]) : handle->dma.desc[0];
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync(handle->dma.desc[i], desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
    }
    if (handle->dir == I2S_DIR_RX) {
        i2s_ll_rx_set_eof_num(handle->controller->hal.dev, bufsize);
    }
    ESP_LOGD(TAG, "DMA malloc info: dma_desc_num = %"PRIu32", dma_desc_buf_size = dma_frame_num * slot_num * data_bit_width = %"PRIu32, num, bufsize);
    return ESP_OK;
err:
    i2s_free_dma_desc(handle);
    return ret;
}

#if SOC_I2S_SUPPORTS_APLL
static uint32_t i2s_set_get_apll_freq(uint32_t mclk_freq_hz)
{
    /* Calculate the expected APLL  */
    int mclk_div = (int)((CLK_LL_APLL_MIN_HZ / mclk_freq_hz) + 1);
    /* apll_freq = mclk * div
        * when div = 1, hardware will still divide 2
        * when div = 0, the final mclk will be unpredictable
        * So the div here should be at least 2 */
    mclk_div = mclk_div < 2 ? 2 : mclk_div;
    uint32_t expt_freq = mclk_freq_hz * mclk_div;
    if (expt_freq > CLK_LL_APLL_MAX_HZ) {
        ESP_LOGE(TAG, "The required APLL frequency exceed its maximum value");
        return 0;
    }
    uint32_t real_freq = 0;
    esp_err_t ret = periph_rtc_apll_freq_set(expt_freq, &real_freq);
    if (ret == ESP_ERR_INVALID_ARG) {
        ESP_LOGE(TAG, "set APLL freq failed due to invalid argument");
        return 0;
    }
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "APLL is occupied already, it is working at %"PRIu32" Hz while the expected frequency is %"PRIu32" Hz", real_freq, expt_freq);
        ESP_LOGW(TAG, "Trying to work at %"PRIu32" Hz...", real_freq);
    }
    ESP_LOGD(TAG, "APLL expected frequency is %"PRIu32" Hz, real frequency is %"PRIu32" Hz", expt_freq, real_freq);
    return real_freq;
}
#endif

uint32_t i2s_get_source_clk_freq(i2s_clock_src_t clk_src, uint32_t mclk_freq_hz)
{
    uint32_t clk_freq = 0;
#if SOC_I2S_SUPPORTS_APLL
    if (clk_src == I2S_CLK_SRC_APLL) {
        return i2s_set_get_apll_freq(mclk_freq_hz);
    }
#endif
    esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_freq);
    return clk_freq;
}

/* Temporary ignore the deprecated warning of i2s_event_data_t::data */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#if SOC_GDMA_SUPPORTED
static bool IRAM_ATTR i2s_dma_rx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_chan_handle_t handle = (i2s_chan_handle_t)user_data;
    BaseType_t need_yield1 = 0;
    BaseType_t need_yield2 = 0;
    BaseType_t user_need_yield = 0;
    lldesc_t *finish_desc;
    uint32_t dummy;

    finish_desc = (lldesc_t *)event_data->rx_eof_desc_addr;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    esp_cache_msync((void *)finish_desc->buf, handle->dma.buf_size, ESP_CACHE_MSYNC_FLAG_INVALIDATE);
#endif
    i2s_event_data_t evt = {
        .data = &(finish_desc->buf),
        .dma_buf = (void *)finish_desc->buf,
        .size = handle->dma.buf_size,
    };
    if (handle->callbacks.on_recv) {
        user_need_yield |= handle->callbacks.on_recv(handle, &evt, handle->user_data);
    }
    if (xQueueIsQueueFullFromISR(handle->msg_queue)) {
        xQueueReceiveFromISR(handle->msg_queue, &dummy, &need_yield1);
        if (handle->callbacks.on_recv_q_ovf) {
            evt.data = NULL;
            user_need_yield |= handle->callbacks.on_recv_q_ovf(handle, &evt, handle->user_data);
        }
    }
    xQueueSendFromISR(handle->msg_queue, &(finish_desc->buf), &need_yield2);

    return need_yield1 | need_yield2 | user_need_yield;
}

static bool IRAM_ATTR i2s_dma_tx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    i2s_chan_handle_t handle = (i2s_chan_handle_t)user_data;
    BaseType_t need_yield1 = 0;
    BaseType_t need_yield2 = 0;
    BaseType_t user_need_yield = 0;
    lldesc_t *finish_desc;
    uint32_t dummy;

    finish_desc = (lldesc_t *)event_data->tx_eof_desc_addr;
    void *curr_buf = (void *)finish_desc->buf;
    i2s_event_data_t evt = {
        .data = &(finish_desc->buf),
        .dma_buf = curr_buf,
        .size = handle->dma.buf_size,
    };
    if (handle->dma.auto_clear_before_cb) {
        memset(curr_buf, 0, handle->dma.buf_size);
    }
    if (handle->callbacks.on_sent) {
        user_need_yield |= handle->callbacks.on_sent(handle, &evt, handle->user_data);
    }
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    /* Sync buffer after the callback in case users update the buffer in the callback */
    if (handle->dma.auto_clear_before_cb || handle->callbacks.on_sent) {
        esp_cache_msync(curr_buf, handle->dma.buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }
#endif
    if (xQueueIsQueueFullFromISR(handle->msg_queue)) {
        xQueueReceiveFromISR(handle->msg_queue, &dummy, &need_yield1);
        if (handle->callbacks.on_send_q_ovf) {
            evt.data = NULL;
            evt.dma_buf = NULL;
            user_need_yield |= handle->callbacks.on_send_q_ovf(handle, &evt, handle->user_data);
        }
    }
    if (handle->dma.auto_clear_after_cb) {
        memset(curr_buf, 0, handle->dma.buf_size);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync(curr_buf, handle->dma.buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
    }
    xQueueSendFromISR(handle->msg_queue, &(finish_desc->buf), &need_yield2);

    return need_yield1 | need_yield2 | user_need_yield;
}

#else

static void IRAM_ATTR i2s_dma_rx_callback(void *arg)
{
    BaseType_t need_yield1 = 0;
    BaseType_t need_yield2 = 0;
    BaseType_t user_need_yield = 0;
    lldesc_t *finish_desc = NULL;
    i2s_event_data_t evt;
    i2s_chan_handle_t handle = (i2s_chan_handle_t)arg;
    uint32_t dummy;

    uint32_t status = i2s_hal_get_intr_status(&(handle->controller->hal));
    i2s_hal_clear_intr_status(&(handle->controller->hal), status);
    if (!status) {
        return;
    }

    if (handle && (status & I2S_LL_EVENT_RX_EOF)) {
        i2s_hal_get_in_eof_des_addr(&(handle->controller->hal), (uint32_t *)&finish_desc);
        evt.data = &(finish_desc->buf);
        evt.dma_buf = (void *)finish_desc->buf;
        evt.size = handle->dma.buf_size;
        if (handle->callbacks.on_recv) {
            user_need_yield |= handle->callbacks.on_recv(handle, &evt, handle->user_data);
        }
        if (xQueueIsQueueFullFromISR(handle->msg_queue)) {
            xQueueReceiveFromISR(handle->msg_queue, &dummy, &need_yield1);
            if (handle->callbacks.on_recv_q_ovf) {
                evt.data = NULL;
                evt.dma_buf = NULL;
                user_need_yield |= handle->callbacks.on_recv_q_ovf(handle, &evt, handle->user_data);
            }
        }
        xQueueSendFromISR(handle->msg_queue, &(finish_desc->buf), &need_yield2);
    }

    if (need_yield1 || need_yield2 || user_need_yield) {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR i2s_dma_tx_callback(void *arg)
{
    BaseType_t need_yield1 = 0;
    BaseType_t need_yield2 = 0;
    BaseType_t user_need_yield = 0;
    lldesc_t *finish_desc = NULL;
    i2s_event_data_t evt;
    i2s_chan_handle_t handle = (i2s_chan_handle_t)arg;
    uint32_t dummy;

    uint32_t status = i2s_hal_get_intr_status(&(handle->controller->hal));
    i2s_hal_clear_intr_status(&(handle->controller->hal), status);
    if (!status) {
        return;
    }

    if (handle && (status & I2S_LL_EVENT_TX_EOF)) {
        i2s_hal_get_out_eof_des_addr(&(handle->controller->hal), (uint32_t *)&finish_desc);
        void *curr_buf = (void *)finish_desc->buf;
        evt.data = &(finish_desc->buf);
        evt.dma_buf = curr_buf;
        evt.size = handle->dma.buf_size;
        // Auto clear the dma buffer before data sent
        if (handle->dma.auto_clear_before_cb) {
            memset(curr_buf, 0, handle->dma.buf_size);
        }
        if (handle->callbacks.on_sent) {
            user_need_yield |= handle->callbacks.on_sent(handle, &evt, handle->user_data);
        }
        if (xQueueIsQueueFullFromISR(handle->msg_queue)) {
            xQueueReceiveFromISR(handle->msg_queue, &dummy, &need_yield1);
            if (handle->callbacks.on_send_q_ovf) {
                evt.data = NULL;
                user_need_yield |= handle->callbacks.on_send_q_ovf(handle, &evt, handle->user_data);
            }
        }
        // Auto clear the dma buffer after data sent
        if (handle->dma.auto_clear_after_cb) {
            memset(curr_buf, 0, handle->dma.buf_size);
        }
        xQueueSendFromISR(handle->msg_queue, &(finish_desc->buf), &need_yield2);
    }

    if (need_yield1 || need_yield2 || user_need_yield) {
        portYIELD_FROM_ISR();
    }
}
#endif

#pragma GCC diagnostic pop

/**
 * @brief   I2S DMA interrupt initialization
 * @note    I2S will use GDMA if chip supports, and the interrupt is triggered by GDMA.
 *
 * @param   handle      I2S channel handle
 * @param   intr_flag   Interrupt allocation flag
 * @return
 *      - ESP_OK                    I2S DMA interrupt initialize success
 *      - ESP_ERR_NOT_FOUND         GDMA channel not found
 *      - ESP_ERR_INVALID_ARG       Invalid arguments
 *      - ESP_ERR_INVALID_STATE     GDMA state error
 */
esp_err_t i2s_init_dma_intr(i2s_chan_handle_t handle, int intr_flag)
{
    i2s_port_t port_id = handle->controller->id;
    ESP_RETURN_ON_FALSE((port_id >= 0) && (port_id < SOC_I2S_NUM), ESP_ERR_INVALID_ARG, TAG, "invalid handle");
#if SOC_GDMA_SUPPORTED
    /* Set GDMA trigger module */
    gdma_trigger_t trig = {.periph = GDMA_TRIG_PERIPH_I2S};

    switch (port_id) {
#if SOC_I2S_NUM > 1
    case I2S_NUM_1:
        trig.instance_id = SOC_GDMA_TRIG_PERIPH_I2S1;
        break;
#endif
    default:
        trig.instance_id = SOC_GDMA_TRIG_PERIPH_I2S0;
        break;
    }

    /* Set GDMA config */
    gdma_channel_alloc_config_t dma_cfg = {};
    if (handle->dir == I2S_DIR_TX) {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_TX;
        /* Register a new GDMA tx channel */
        ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_cfg, &handle->dma.dma_chan), TAG, "Register tx dma channel error");
        ESP_RETURN_ON_ERROR(gdma_connect(handle->dma.dma_chan, trig), TAG, "Connect tx dma channel error");
        gdma_tx_event_callbacks_t cb = {.on_trans_eof = i2s_dma_tx_callback};
        /* Set callback function for GDMA, the interrupt is triggered by GDMA, then the GDMA ISR will call the  callback function */
        gdma_register_tx_event_callbacks(handle->dma.dma_chan, &cb, handle);
    } else {
        dma_cfg.direction = GDMA_CHANNEL_DIRECTION_RX;
        /* Register a new GDMA rx channel */
        ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_cfg, &handle->dma.dma_chan), TAG, "Register rx dma channel error");
        ESP_RETURN_ON_ERROR(gdma_connect(handle->dma.dma_chan, trig), TAG, "Connect rx dma channel error");
        gdma_rx_event_callbacks_t cb = {.on_recv_eof = i2s_dma_rx_callback};
        /* Set callback function for GDMA, the interrupt is triggered by GDMA, then the GDMA ISR will call the  callback function */
        gdma_register_rx_event_callbacks(handle->dma.dma_chan, &cb, handle);
    }
#else
    intr_flag |= handle->intr_prio_flags;
    /* Initialize I2S module interrupt */
    if (handle->dir == I2S_DIR_TX) {
        esp_intr_alloc_intrstatus(i2s_periph_signal[port_id].irq, intr_flag,
                                  (uint32_t)i2s_ll_get_interrupt_status_reg(handle->controller->hal.dev), I2S_LL_TX_EVENT_MASK,
                                  i2s_dma_tx_callback, handle, &handle->dma.dma_chan);
    } else {
        esp_intr_alloc_intrstatus(i2s_periph_signal[port_id].irq, intr_flag,
                                  (uint32_t)i2s_ll_get_interrupt_status_reg(handle->controller->hal.dev), I2S_LL_RX_EVENT_MASK,
                                  i2s_dma_rx_callback, handle, &handle->dma.dma_chan);
    }
    /* Start DMA */
    i2s_ll_enable_dma(handle->controller->hal.dev, true);
#endif // SOC_GDMA_SUPPORTED
    return ESP_OK;
}

static uint64_t s_i2s_get_pair_chan_gpio_mask(i2s_chan_handle_t handle)
{
    if (handle->dir == I2S_DIR_TX) {
        return handle->controller->rx_chan ? handle->controller->rx_chan->reserve_gpio_mask : 0;
    }
    return handle->controller->tx_chan ? handle->controller->tx_chan->reserve_gpio_mask : 0;
}

void i2s_output_gpio_reserve(i2s_chan_handle_t handle, int gpio_num)
{
    bool used_by_pair_chan = false;
    /* If the gpio is used by the pair channel do not show warning for this case */
    if (handle->controller->full_duplex) {
        used_by_pair_chan = !!(s_i2s_get_pair_chan_gpio_mask(handle) & BIT64(gpio_num));
    }
    /* reserve the GPIO output path, because we don't expect another peripheral to signal to the same GPIO */
    if (!used_by_pair_chan && (esp_gpio_reserve(BIT64(gpio_num)) & BIT64(gpio_num))) {
        ESP_LOGW(TAG, "GPIO %d is not usable, maybe conflict with others", gpio_num);
    }
    handle->reserve_gpio_mask |= BIT64(gpio_num);
}

void i2s_output_gpio_revoke(i2s_chan_handle_t handle, uint64_t gpio_mask)
{
    uint64_t revoke_mask = gpio_mask;
    /* If the gpio is used by the pair channel do not show warning for this case */
    if (handle->controller->full_duplex) {
        uint64_t pair_chan_gpio_mask = s_i2s_get_pair_chan_gpio_mask(handle);
        /* Only revoke the gpio which is not used by the pair channel */
        revoke_mask = (pair_chan_gpio_mask ^ gpio_mask) & gpio_mask;
    }
    esp_gpio_revoke(revoke_mask);
    handle->reserve_gpio_mask &= ~gpio_mask;
}

void i2s_gpio_check_and_set(i2s_chan_handle_t handle, int gpio, uint32_t signal_idx, bool is_input, bool is_invert)
{
    /* Ignore the pin if pin = I2S_GPIO_UNUSED */
    if (gpio != (int)I2S_GPIO_UNUSED) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        if (is_input) {
            /* Set direction, for some GPIOs, the input function are not enabled as default */
            gpio_set_direction(gpio, GPIO_MODE_INPUT);
            esp_rom_gpio_connect_in_signal(gpio, signal_idx, is_invert);
        } else {
            i2s_output_gpio_reserve(handle, gpio);
            gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
            esp_rom_gpio_connect_out_signal(gpio, signal_idx, is_invert, 0);
        }
    }
}

void i2s_gpio_loopback_set(i2s_chan_handle_t handle, int gpio, uint32_t out_sig_idx, uint32_t in_sig_idx)
{
    if (gpio != (int)I2S_GPIO_UNUSED) {
        i2s_output_gpio_reserve(handle,  gpio);
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
        gpio_set_direction(gpio, GPIO_MODE_INPUT_OUTPUT);
        esp_rom_gpio_connect_out_signal(gpio, out_sig_idx, 0, 0);
        esp_rom_gpio_connect_in_signal(gpio, in_sig_idx, 0);
    }
}

esp_err_t i2s_check_set_mclk(i2s_chan_handle_t handle, i2s_port_t id, int gpio_num, i2s_clock_src_t clk_src, bool is_invert)
{
    if (gpio_num == (int)I2S_GPIO_UNUSED) {
        return ESP_OK;
    }
#if CONFIG_IDF_TARGET_ESP32
    bool is_i2s0 = id == I2S_NUM_0;
    bool is_apll = clk_src == I2S_CLK_SRC_APLL;
    if (g_i2s.controller[id]->mclk_out_hdl == NULL) {
        i2s_output_gpio_reserve(handle, gpio_num);
        soc_clkout_sig_id_t clkout_sig = is_apll ? CLKOUT_SIG_APLL : (is_i2s0 ? CLKOUT_SIG_I2S0 : CLKOUT_SIG_I2S1);
        ESP_RETURN_ON_ERROR(esp_clock_output_start(clkout_sig, gpio_num, &(g_i2s.controller[id]->mclk_out_hdl)), TAG, "mclk configure failed");
    }
#else
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "mck_io_num invalid");
#if SOC_I2S_HW_VERSION_2
    if (clk_src == I2S_CLK_SRC_EXTERNAL) {
        i2s_gpio_check_and_set(handle, gpio_num, i2s_periph_signal[id].mck_in_sig, true, is_invert);
    } else
#endif  // SOC_I2S_HW_VERSION_2
    {
        i2s_gpio_check_and_set(handle, gpio_num, i2s_periph_signal[id].mck_out_sig, false, is_invert);
    }
#endif  // CONFIG_IDF_TARGET_ESP32
    ESP_LOGD(TAG, "MCLK is pinned to GPIO%d on I2S%d", gpio_num, id);
    return ESP_OK;
}

/*---------------------------------------------------------------------------
                            I2S bus Public APIs
 ----------------------------------------------------------------------------
    Scope: Public
 ----------------------------------------------------------------------------*/
esp_err_t i2s_new_channel(const i2s_chan_config_t *chan_cfg, i2s_chan_handle_t *tx_handle, i2s_chan_handle_t *rx_handle)
{
#if CONFIG_I2S_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    /* Parameter validity check */
    I2S_NULL_POINTER_CHECK(TAG, chan_cfg);
    I2S_NULL_POINTER_CHECK(TAG, tx_handle || rx_handle);
    ESP_RETURN_ON_FALSE(chan_cfg->id < SOC_I2S_NUM || chan_cfg->id == I2S_NUM_AUTO, ESP_ERR_INVALID_ARG, TAG, "invalid I2S port id");
    ESP_RETURN_ON_FALSE(chan_cfg->dma_desc_num >= 2, ESP_ERR_INVALID_ARG, TAG, "there should be at least 2 DMA buffers");
    ESP_RETURN_ON_FALSE(chan_cfg->intr_priority >= 0 && chan_cfg->intr_priority <= 7, ESP_ERR_INVALID_ARG, TAG, "intr_priority should be within 0~7");

    esp_err_t ret = ESP_OK;
    i2s_controller_t *i2s_obj = NULL;
    i2s_port_t id = chan_cfg->id;
    bool channel_found = false;
    uint8_t chan_search_mask = 0;
    chan_search_mask |= tx_handle ? I2S_DIR_TX : 0;
    chan_search_mask |= rx_handle ? I2S_DIR_RX : 0;

    /* Channel will be registered to one i2s port automatically if id is I2S_NUM_AUTO
     * Otherwise, the channel will be registered to the specific port. */
    if (id == I2S_NUM_AUTO) {
        for (int i = 0; i < SOC_I2S_NUM && !channel_found; i++) {
            i2s_obj = i2s_acquire_controller_obj(i);
            if (!i2s_obj) {
                continue;
            }
            channel_found = i2s_take_available_channel(i2s_obj, chan_search_mask);
        }
        ESP_RETURN_ON_FALSE(i2s_obj, ESP_ERR_NOT_FOUND, TAG, "get i2s object failed");
    } else {
        i2s_obj = i2s_acquire_controller_obj(id);
        ESP_RETURN_ON_FALSE(i2s_obj, ESP_ERR_NOT_FOUND, TAG, "get i2s object failed");
        channel_found = i2s_take_available_channel(i2s_obj, chan_search_mask);
    }
    ESP_GOTO_ON_FALSE(channel_found, ESP_ERR_NOT_FOUND, err, TAG, "no available channel found");
    /* Register and specify the tx handle */
    if (tx_handle) {
        ESP_GOTO_ON_ERROR(i2s_register_channel(i2s_obj, I2S_DIR_TX, chan_cfg->dma_desc_num),
                          err, TAG, "register I2S tx channel failed");
        i2s_obj->tx_chan->role = chan_cfg->role;
        i2s_obj->tx_chan->intr_prio_flags = chan_cfg->intr_priority ? BIT(chan_cfg->intr_priority) : ESP_INTR_FLAG_LOWMED;
        i2s_obj->tx_chan->dma.auto_clear_after_cb = chan_cfg->auto_clear_after_cb;
        i2s_obj->tx_chan->dma.auto_clear_before_cb = chan_cfg->auto_clear_before_cb;
        i2s_obj->tx_chan->dma.desc_num = chan_cfg->dma_desc_num;
        i2s_obj->tx_chan->dma.frame_num = chan_cfg->dma_frame_num;
        i2s_obj->tx_chan->start = i2s_tx_channel_start;
        i2s_obj->tx_chan->stop = i2s_tx_channel_stop;
        *tx_handle = i2s_obj->tx_chan;
        ESP_LOGD(TAG, "tx channel is registered on I2S%d successfully", i2s_obj->id);
    }
    /* Register and specify the rx handle */
    if (rx_handle) {
        ESP_GOTO_ON_ERROR(i2s_register_channel(i2s_obj, I2S_DIR_RX, chan_cfg->dma_desc_num),
                          err, TAG, "register I2S rx channel failed");
        i2s_obj->rx_chan->role = chan_cfg->role;
        i2s_obj->rx_chan->intr_prio_flags = chan_cfg->intr_priority ? BIT(chan_cfg->intr_priority) : ESP_INTR_FLAG_LOWMED;
        i2s_obj->rx_chan->dma.desc_num = chan_cfg->dma_desc_num;
        i2s_obj->rx_chan->dma.frame_num = chan_cfg->dma_frame_num;
        i2s_obj->rx_chan->start = i2s_rx_channel_start;
        i2s_obj->rx_chan->stop = i2s_rx_channel_stop;
        *rx_handle = i2s_obj->rx_chan;
        ESP_LOGD(TAG, "rx channel is registered on I2S%d successfully", i2s_obj->id);
    }

    if ((tx_handle != NULL) && (rx_handle != NULL)) {
        i2s_obj->full_duplex = true;
    }

    return ESP_OK;
    /* i2s_obj allocated but register channel failed */
err:
    /* if the controller object has no channel, find the corresponding global object and destroy it */
    if (i2s_obj != NULL && i2s_obj->rx_chan == NULL && i2s_obj->tx_chan == NULL) {
        for (int i = 0; i < SOC_I2S_NUM; i++) {
            if (i2s_obj == g_i2s.controller[i]) {
                i2s_destroy_controller_obj(&g_i2s.controller[i]);
                break;
            }
        }
    }
    return ret;
}

esp_err_t i2s_del_channel(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->state < I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, TAG, "the channel can't be deleted unless it is disabled");
    i2s_controller_t *i2s_obj = handle->controller;
    int __attribute__((unused)) id = i2s_obj->id;
    i2s_dir_t __attribute__((unused)) dir = handle->dir;
    bool is_bound = true;

#if SOC_I2S_HW_VERSION_2
    I2S_CLOCK_SRC_ATOMIC() {
        if (dir == I2S_DIR_TX) {
            i2s_ll_tx_disable_clock(handle->controller->hal.dev);
        } else {
            i2s_ll_rx_disable_clock(handle->controller->hal.dev);
        }
    }
#endif
#if SOC_I2S_SUPPORTS_APLL
    if (handle->apll_en) {
        /* Must switch back to D2CLK on ESP32-S2,
         * because the clock of some registers are bound to APLL,
         * otherwise, once APLL is disabled, the registers can't be updated anymore */
        I2S_CLOCK_SRC_ATOMIC() {
            if (handle->dir == I2S_DIR_TX) {
                i2s_ll_tx_clk_set_src(handle->controller->hal.dev, I2S_CLK_SRC_DEFAULT);
            } else {
                i2s_ll_rx_clk_set_src(handle->controller->hal.dev, I2S_CLK_SRC_DEFAULT);
            }
        }
        periph_rtc_apll_release();
    }
#endif
#if CONFIG_PM_ENABLE
    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
    }
#endif
    if (handle->reserve_gpio_mask) {
        i2s_output_gpio_revoke(handle, handle->reserve_gpio_mask);
    }
    if (handle->mode_info) {
        free(handle->mode_info);
    }
    if (handle->dma.desc) {
        i2s_free_dma_desc(handle);
    }
    if (handle->msg_queue) {
        vQueueDeleteWithCaps(handle->msg_queue);
    }
    if (handle->mutex) {
        vSemaphoreDeleteWithCaps(handle->mutex);
    }
    if (handle->binary) {
        vSemaphoreDeleteWithCaps(handle->binary);
    }
#if SOC_I2S_HW_VERSION_1
    i2s_obj->chan_occupancy = 0;
#else
    i2s_obj->chan_occupancy &= ~(uint32_t)dir;
#endif
    if (handle->dma.dma_chan) {
#if SOC_GDMA_SUPPORTED
        gdma_disconnect(handle->dma.dma_chan);
        gdma_del_channel(handle->dma.dma_chan);
#else
        esp_intr_free(handle->dma.dma_chan);
#endif
    }
    if (handle == i2s_obj->tx_chan) {
        free(i2s_obj->tx_chan);
        i2s_obj->tx_chan = NULL;
        i2s_obj->full_duplex = false;
    } else if (handle == i2s_obj->rx_chan) {
        free(i2s_obj->rx_chan);
        i2s_obj->rx_chan = NULL;
        i2s_obj->full_duplex = false;
    } else {
        /* Indicate the delete channel is an unbound free channel */
        is_bound = false;
        free(handle);
    }

    /* If the delete channel was bound to a controller before,
       we need to destroy this controller object if there is no channel any more */
    if (is_bound) {
        if (!(i2s_obj->tx_chan) && !(i2s_obj->rx_chan)) {
            i2s_destroy_controller_obj(&g_i2s.controller[i2s_obj->id]);
        }
        ESP_LOGD(TAG, "%s channel on I2S%d deleted", dir == I2S_DIR_TX ? "tx" : "rx", id);
    }

    return ESP_OK;
}

esp_err_t i2s_channel_get_info(i2s_chan_handle_t handle, i2s_chan_info_t *chan_info)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    I2S_NULL_POINTER_CHECK(TAG, chan_info);

    /* Find whether the handle is a registered i2s handle or still available */
    for (int i = 0; i < SOC_I2S_NUM; i++) {
        if (g_i2s.controller[i] != NULL) {
            if (g_i2s.controller[i]->tx_chan == handle ||
                    g_i2s.controller[i]->rx_chan == handle) {
                goto found;
            }
        }
    }
    return ESP_ERR_NOT_FOUND;
found:
    /* Assign the handle information */
    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    chan_info->id = handle->controller->id;
    chan_info->dir = handle->dir;
    chan_info->role = handle->role;
    chan_info->mode = handle->mode;
    chan_info->total_dma_buf_size = handle->state >= I2S_CHAN_STATE_READY ? handle->dma.desc_num * handle->dma.buf_size : 0;
    if (handle->controller->full_duplex) {
        if (handle->dir == I2S_DIR_TX) {
            chan_info->pair_chan = handle->controller->rx_chan;
        } else {
            chan_info->pair_chan = handle->controller->tx_chan;
        }
    } else {
        chan_info->pair_chan = NULL;
    }
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t i2s_channel_enable(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "the channel has already enabled or not initialized");
#if CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif
    handle->start(handle);
    handle->state = I2S_CHAN_STATE_RUNNING;
    /* Reset queue */
    xQueueReset(handle->msg_queue);
    xSemaphoreGive(handle->mutex);
    /* Give the binary semaphore to enable reading / writing task */
    xSemaphoreGive(handle->binary);

    ESP_LOGD(TAG, "i2s %s channel enabled", handle->dir == I2S_DIR_TX ? "tx" : "rx");
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_disable(i2s_chan_handle_t handle)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(handle->state > I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, err, TAG, "the channel has not been enabled yet");
    /* Update the state to force quit the current reading/writing operation */
    handle->state = I2S_CHAN_STATE_READY;
    /* Waiting for reading/wrinting operation quit
     * It should be acquired before assigning the pointer to NULL,
     * otherwise may cause NULL pointer panic while reading/writing threads haven't release the lock */
    xSemaphoreTake(handle->binary, portMAX_DELAY);
    /* Reset the descriptor pointer */
    handle->dma.curr_ptr = NULL;
    handle->dma.curr_desc = NULL;
    handle->dma.rw_pos = 0;
    handle->stop(handle);
#if CONFIG_PM_ENABLE
    esp_pm_lock_release(handle->pm_lock);
#endif
    xSemaphoreGive(handle->mutex);
    ESP_LOGD(TAG, "i2s %s channel disabled", handle->dir == I2S_DIR_TX ? "tx" : "rx");
    return ret;

err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t i2s_channel_preload_data(i2s_chan_handle_t tx_handle, const void *src, size_t size, size_t *bytes_loaded)
{
    I2S_NULL_POINTER_CHECK(TAG, tx_handle);
    ESP_RETURN_ON_FALSE(tx_handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "this channel is not tx channel");
    ESP_RETURN_ON_FALSE(tx_handle->state == I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, TAG, "data can only be preloaded when the channel is READY");

    uint8_t *data_ptr = (uint8_t *)src;
    size_t remain_bytes = size;
    size_t total_loaded_bytes = 0;

    xSemaphoreTake(tx_handle->mutex, portMAX_DELAY);

    /* The pre-load data will be loaded from the first descriptor */
    if (tx_handle->dma.curr_desc == NULL) {
        tx_handle->dma.curr_desc = tx_handle->dma.desc[0];
        tx_handle->dma.curr_ptr = (void *)tx_handle->dma.desc[0]->buf;
        tx_handle->dma.rw_pos = 0;
    }

    /* Loop until no bytes in source buff remain or the descriptors are full */
    while (remain_bytes) {
        size_t bytes_can_load = remain_bytes > (tx_handle->dma.buf_size - tx_handle->dma.rw_pos) ?
                                (tx_handle->dma.buf_size - tx_handle->dma.rw_pos) : remain_bytes;
        /* When all the descriptors has loaded data, no more bytes can be loaded, break directly */
        if (bytes_can_load == 0) {
            break;
        }
        /* Load the data from the last loaded position */
        memcpy((uint8_t *)(tx_handle->dma.curr_ptr + tx_handle->dma.rw_pos), data_ptr, bytes_can_load);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync(tx_handle->dma.curr_ptr, tx_handle->dma.buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
        data_ptr += bytes_can_load;             // Move forward the data pointer
        total_loaded_bytes += bytes_can_load;   // Add to the total loaded bytes
        remain_bytes -= bytes_can_load;         // Update the remaining bytes to be loaded
        tx_handle->dma.rw_pos += bytes_can_load;   // Move forward the dma buffer position
        /* When the current position reach the end of the dma buffer */
        if (tx_handle->dma.rw_pos == tx_handle->dma.buf_size) {
            /* If the next descriptor is not the first descriptor, keep load to the first descriptor
             * otherwise all descriptor has been loaded, break directly, the dma buffer position
             * will remain at the end of the last dma buffer */
            if (STAILQ_NEXT((lldesc_t *)tx_handle->dma.curr_desc, qe) != tx_handle->dma.desc[0]) {
                tx_handle->dma.curr_desc = STAILQ_NEXT((lldesc_t *)tx_handle->dma.curr_desc, qe);
                tx_handle->dma.curr_ptr = (void *)(((lldesc_t *)tx_handle->dma.curr_desc)->buf);
                tx_handle->dma.rw_pos = 0;
            } else {
                break;
            }
        }
    }
    *bytes_loaded = total_loaded_bytes;

    xSemaphoreGive(tx_handle->mutex);

    return ESP_OK;
}

esp_err_t i2s_channel_write(i2s_chan_handle_t handle, const void *src, size_t size, size_t *bytes_written, uint32_t timeout_ms)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_TX, ESP_ERR_INVALID_ARG, TAG, "this channel is not tx channel");

    esp_err_t ret = ESP_OK;
    char *data_ptr;
    char *src_byte;
    size_t bytes_can_write;
    if (bytes_written) {
        *bytes_written = 0;
    }

    /* The binary semaphore can only be taken when the channel has been enabled and no other writing operation in progress */
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->binary, pdMS_TO_TICKS(timeout_ms)) == pdTRUE, ESP_ERR_INVALID_STATE, TAG, "The channel is not enabled");
    src_byte = (char *)src;
    while (size > 0 && handle->state == I2S_CHAN_STATE_RUNNING) {
        if (handle->dma.rw_pos == handle->dma.buf_size || handle->dma.curr_ptr == NULL) {
            if (xQueueReceive(handle->msg_queue, &(handle->dma.curr_ptr), pdMS_TO_TICKS(timeout_ms)) == pdFALSE) {
                ret = ESP_ERR_TIMEOUT;
                break;
            }
            handle->dma.rw_pos = 0;
        }
        data_ptr = (char *)handle->dma.curr_ptr;
        data_ptr += handle->dma.rw_pos;
        bytes_can_write = handle->dma.buf_size - handle->dma.rw_pos;
        if (bytes_can_write > size) {
            bytes_can_write = size;
        }
        memcpy(data_ptr, src_byte, bytes_can_write);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_cache_msync(handle->dma.curr_ptr, handle->dma.buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif
        size -= bytes_can_write;
        src_byte += bytes_can_write;
        handle->dma.rw_pos += bytes_can_write;
        if (bytes_written) {
            (*bytes_written) += bytes_can_write;
        }
    }
    xSemaphoreGive(handle->binary);

    return ret;
}

esp_err_t i2s_channel_read(i2s_chan_handle_t handle, void *dest, size_t size, size_t *bytes_read, uint32_t timeout_ms)
{
    I2S_NULL_POINTER_CHECK(TAG, handle);
    ESP_RETURN_ON_FALSE(handle->dir == I2S_DIR_RX, ESP_ERR_INVALID_ARG, TAG, "this channel is not rx channel");

    esp_err_t ret = ESP_OK;
    uint8_t *data_ptr;
    uint8_t *dest_byte;
    int bytes_can_read;
    if (bytes_read) {
        *bytes_read = 0;
    }
    dest_byte = (uint8_t *)dest;
    /* The binary semaphore can only be taken when the channel has been enabled and no other reading operation in progress */
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->binary, pdMS_TO_TICKS(timeout_ms)) == pdTRUE, ESP_ERR_INVALID_STATE, TAG, "The channel is not enabled");
    while (size > 0 && handle->state == I2S_CHAN_STATE_RUNNING) {
        if (handle->dma.rw_pos == handle->dma.buf_size || handle->dma.curr_ptr == NULL) {
            if (xQueueReceive(handle->msg_queue, &(handle->dma.curr_ptr), pdMS_TO_TICKS(timeout_ms)) == pdFALSE) {
                ret = ESP_ERR_TIMEOUT;
                break;
            }
            handle->dma.rw_pos = 0;
        }
        data_ptr = (uint8_t *)handle->dma.curr_ptr;
        data_ptr += handle->dma.rw_pos;
        bytes_can_read = handle->dma.buf_size - handle->dma.rw_pos;
        if (bytes_can_read > (int)size) {
            bytes_can_read = size;
        }
        memcpy(dest_byte, data_ptr, bytes_can_read);
        size -= bytes_can_read;
        dest_byte += bytes_can_read;
        handle->dma.rw_pos += bytes_can_read;
        if (bytes_read) {
            (*bytes_read) += bytes_can_read;
        }
    }
    xSemaphoreGive(handle->binary);

    return ret;
}

#if SOC_I2S_SUPPORTS_TX_SYNC_CNT
uint32_t i2s_sync_get_bclk_count(i2s_chan_handle_t tx_handle)
{
    return i2s_ll_tx_get_bclk_sync_count(tx_handle->controller->hal.dev);
}

uint32_t i2s_sync_get_fifo_count(i2s_chan_handle_t tx_handle)
{
    return i2s_ll_tx_get_fifo_sync_count(tx_handle->controller->hal.dev);
}

void i2s_sync_reset_bclk_count(i2s_chan_handle_t tx_handle)
{
    i2s_ll_tx_reset_bclk_sync_counter(tx_handle->controller->hal.dev);
}

void i2s_sync_reset_fifo_count(i2s_chan_handle_t tx_handle)
{
    i2s_ll_tx_reset_fifo_sync_counter(tx_handle->controller->hal.dev);
}
#endif  // SOC_I2S_SUPPORTS_TX_SYNC_CNT
