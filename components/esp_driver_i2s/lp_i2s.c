/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_I2S_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_clk_tree.h"
#include "hal/hal_utils.h"
#include "hal/lp_i2s_hal.h"
#include "hal/lp_i2s_ll.h"
#include "driver/i2s_types.h"
#include "driver/lp_i2s.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/i2s_platform.h"
#include "i2s_private.h"
#include "soc/i2s_periph.h"

#define LP_I2S_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)

static const char *TAG = "LP_I2S";

static esp_err_t s_i2s_register_channel(lp_i2s_controller_t *ctlr, i2s_dir_t dir);
static inline bool s_lp_i2s_take_available_channel(lp_i2s_controller_t *ctlr, uint8_t chan_search_mask);
static void s_i2s_default_isr(void *arg);

esp_err_t lp_i2s_new_channel(const lp_i2s_chan_config_t *chan_cfg, lp_i2s_chan_handle_t *ret_tx_handle, lp_i2s_chan_handle_t *ret_rx_handle)
{
#if CONFIG_I2S_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    ESP_RETURN_ON_FALSE(chan_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(chan_cfg->id < SOC_LP_I2S_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid LP I2S port id");
    ESP_RETURN_ON_FALSE(chan_cfg->role == I2S_ROLE_SLAVE, ESP_ERR_INVALID_ARG, TAG, "invalid argument: LP I2S not support master");
#if !LP_I2S_LL_TX_SUPPORTED
    ESP_RETURN_ON_FALSE(!ret_tx_handle, ESP_ERR_INVALID_ARG, TAG, "tx not supported");
#endif
#if !LP_I2S_LL_RX_SUPPORTED
    ESP_RETURN_ON_FALSE(!ret_rx_handle, ESP_ERR_INVALID_ARG, TAG, "rx not supported");
#endif
    ESP_RETURN_ON_FALSE(chan_cfg->threshold % 4 == 0, ESP_ERR_INVALID_ARG, TAG, "threshold must be in multiple of 4");
    ESP_RETURN_ON_FALSE(chan_cfg->threshold <= LP_I2S_LL_RX_MEM_THRESH_BYTES_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid threshold");

    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_ERROR(i2s_platform_acquire_occupation(I2S_CTLR_LP, chan_cfg->id, "lp_i2s_driver"), TAG, "failed to acquire lp i2s controller");

    lp_i2s_controller_t *ctlr = heap_caps_calloc(1, sizeof(lp_i2s_controller_t), LP_I2S_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(ctlr, ESP_ERR_NO_MEM, TAG, "no mem");
    ctlr->id = chan_cfg->id;

    ESP_GOTO_ON_ERROR(esp_intr_alloc(lp_i2s_periph_signal[ctlr->id].irq, ESP_INTR_FLAG_IRAM, s_i2s_default_isr, ctlr, &ctlr->intr), err1, TAG, "allocate interrupt failed");

    uint8_t chan_search_mask = 0;
#if LP_I2S_LL_TX_SUPPORTED
    chan_search_mask |= ret_tx_handle ? I2S_DIR_TX : 0;
#endif
#if LP_I2S_LL_RX_SUPPORTED
    chan_search_mask |= ret_rx_handle ? I2S_DIR_RX : 0;
#endif

    portENTER_CRITICAL(&g_i2s.spinlock);
    g_i2s.lp_controller[chan_cfg->id] = ctlr;
    portEXIT_CRITICAL(&g_i2s.spinlock);

    bool channel_found = s_lp_i2s_take_available_channel(ctlr, chan_search_mask);
    ESP_GOTO_ON_FALSE(channel_found, ESP_ERR_NOT_FOUND, err1, TAG, "no available channel found");

    if (ret_rx_handle) {
        ESP_GOTO_ON_ERROR(s_i2s_register_channel(ctlr, I2S_DIR_RX), err1, TAG, "register lp i2s rx channel failed");
        ctlr->rx_chan->role = chan_cfg->role;
        ctlr->rx_chan->threshold = chan_cfg->threshold;
        *ret_rx_handle = ctlr->rx_chan;
        ESP_LOGD(TAG, "rx channel is registered on LP_I2S%d successfully", ctlr->id);

        ctlr->rx_chan->semphr = xSemaphoreCreateBinaryWithCaps(LP_I2S_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(ctlr->rx_chan->semphr, ESP_ERR_NO_MEM, err0, TAG, "No memory for binary semaphore");
    }

    PERIPH_RCC_ATOMIC() {
        lp_i2s_ll_enable_module_clock(chan_cfg->id, true);
        lp_i2s_ll_reset_module_clock(chan_cfg->id);
        if (chan_search_mask & I2S_DIR_RX) {
            lp_i2s_ll_enable_rx_module_clock(chan_cfg->id, true);
        }
    }
    lp_i2s_hal_init(&ctlr->hal, ctlr->id);
    lp_i2s_ll_enable_mem(chan_cfg->id, true);
    lp_i2s_ll_clk_gate_en(ctlr->hal.dev, true);
    lp_i2s_ll_rx_mem_clk_gate_en(ctlr->hal.dev, true);
    lp_i2s_ll_rx_reg_clk_gate_en(ctlr->hal.dev, true);
    lp_i2s_ll_rx_reset(ctlr->hal.dev);
    lp_i2s_ll_rx_reset_fifo(ctlr->hal.dev);
    lp_i2s_ll_rx_reset_fifo_mem(ctlr->hal.dev);

    if (ctlr->rx_chan->role == I2S_ROLE_SLAVE) {
        lp_i2s_ll_set_rx_master_slave_mode(ctlr->hal.dev, true);
    }
    lp_i2s_ll_set_rx_stop_mode(ctlr->hal.dev, LP_I2S_LL_RX_STOP_MODE_START_CLEAR);
    lp_i2s_ll_set_rx_mem_threshold(ctlr->hal.dev, chan_cfg->threshold / 4);
    lp_i2s_ll_rx_clear_interrupt_status(ctlr->hal.dev, LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT);

    return ESP_OK;

err0:
    if (ctlr->rx_chan) {
        vSemaphoreDeleteWithCaps(ctlr->rx_chan->semphr);
        free(ctlr->rx_chan);
        ctlr->rx_chan = NULL;
    }

err1:
    /* if the controller object has no channel, find the corresponding global object and destroy it */
    if (ctlr != NULL && ctlr->rx_chan == NULL) {
        esp_intr_free(ctlr->intr);
        i2s_platform_release_occupation(I2S_CTLR_LP, chan_cfg->id);
        free(ctlr);
    }
    return ret;
}

esp_err_t lp_i2s_channel_enable(lp_i2s_chan_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(chan->state < I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, TAG, "the channel is in enabled state already");

    lp_i2s_evt_data_t evt_data = {};
    if (chan->cbs.on_request_new_trans) {
        chan->cbs.on_request_new_trans(chan, &evt_data, chan->user_data);
        ESP_RETURN_ON_FALSE(evt_data.trans.buffer, ESP_ERR_INVALID_STATE, TAG, "no transaction buffer");
        ESP_RETURN_ON_FALSE(((evt_data.trans.buflen % LP_I2S_LL_RX_MEM_POP_BYTES) == 0), ESP_ERR_INVALID_STATE, TAG, "invalid transaction buflen, not aligned to %d", LP_I2S_LL_RX_MEM_POP_BYTES);
        ESP_LOGD(TAG, "evt_data.trans.buffer: %p, evt_data.trans.buflen: %d", evt_data.trans.buffer, evt_data.trans.buflen);
        chan->trans = evt_data.trans;
    }

    chan->state = I2S_CHAN_STATE_RUNNING;
    portENTER_CRITICAL(&g_i2s.spinlock);
    lp_i2s_ll_rx_enable_interrupt(chan->ctlr->hal.dev, LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT, true);
    lp_i2s_ll_rx_start(chan->ctlr->hal.dev);
    portEXIT_CRITICAL(&g_i2s.spinlock);

    return ESP_OK;
}

esp_err_t lp_i2s_channel_read(lp_i2s_chan_handle_t chan, lp_i2s_trans_t *trans, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(chan->state == I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, TAG, "the channel isn't enabled");
    ESP_RETURN_ON_FALSE(!chan->cbs.on_request_new_trans, ESP_ERR_INVALID_STATE, TAG, "on_request_new_trans registered, no use of this read API");

    TickType_t ticks_to_wait = timeout_ms / portTICK_PERIOD_MS;
    if (timeout_ms == LP_I2S_MAX_DELAY) {
        ticks_to_wait = portMAX_DELAY;
    }

    BaseType_t r = xSemaphoreTake(chan->semphr, ticks_to_wait);
    if (r != pdTRUE) {
        ESP_LOGW(TAG, "lp_i2s read API, new data receiving timeout");
        return ESP_ERR_TIMEOUT;
    }

    size_t len = MIN(lp_i2s_ll_get_rx_mem_fifo_cnt(chan->ctlr->hal.dev), trans->buflen);
    lp_i2s_ll_read_buffer(chan->ctlr->hal.dev, trans->buffer, len);
    trans->received_size = len;
    portENTER_CRITICAL(&g_i2s.spinlock);
    lp_i2s_ll_rx_clear_interrupt_status(chan->ctlr->hal.dev, LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT);
    lp_i2s_ll_rx_enable_interrupt(chan->ctlr->hal.dev, LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT, true);
    portEXIT_CRITICAL(&g_i2s.spinlock);

    return ESP_OK;
}

esp_err_t lp_i2s_channel_read_until_bytes(lp_i2s_chan_handle_t chan, lp_i2s_trans_t *trans)
{
    size_t received_size = 0;
    size_t recv_len = trans->buflen;
    lp_i2s_trans_t t = {};
    while (1) {
        t.buffer = trans->buffer + received_size;
        t.buflen = MIN(chan->threshold, recv_len);
        ESP_RETURN_ON_ERROR(lp_i2s_channel_read(chan, &t, LP_I2S_MAX_DELAY), TAG, "failed to do lp i2s read");
        received_size += t.received_size;
        recv_len -= t.received_size;
        ESP_LOGD(TAG, "received_size: %d, recv_len: %d, t.received_size: %d", received_size, recv_len, t.received_size);
        if (recv_len == 0) {
            break;
        }
    }
    trans->received_size = received_size;

    return ESP_OK;
}

esp_err_t lp_i2s_channel_disable(lp_i2s_chan_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(chan->state  > I2S_CHAN_STATE_READY, ESP_ERR_INVALID_STATE, TAG, "the channel is disabled already");
    chan->state = I2S_CHAN_STATE_READY;

    portENTER_CRITICAL(&g_i2s.spinlock);
    lp_i2s_ll_rx_enable_interrupt(chan->ctlr->hal.dev, LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT, false);
    lp_i2s_ll_rx_stop(chan->ctlr->hal.dev);
    portEXIT_CRITICAL(&g_i2s.spinlock);

    return ESP_OK;
}

esp_err_t lp_i2s_del_channel(lp_i2s_chan_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(chan->state < I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, TAG, "the channel can't be deleted unless it is disabled");

    int id = chan->ctlr->id;
    portENTER_CRITICAL(&g_i2s.spinlock);
    if (chan->dir == I2S_DIR_RX) {
        g_i2s.lp_controller[chan->ctlr->id]->rx_chan = NULL;
    }

    g_i2s.lp_controller[id] = NULL;
    portEXIT_CRITICAL(&g_i2s.spinlock);

    ESP_RETURN_ON_ERROR(esp_intr_free(chan->ctlr->intr), TAG, "failed to free intr");
    vSemaphoreDeleteWithCaps(chan->semphr);
    ESP_RETURN_ON_ERROR(i2s_platform_release_occupation(I2S_CTLR_LP, id), TAG, "failed to release lp i2s controller");
    free(chan->ctlr);
    free(chan);

    return ESP_OK;
}

#ifndef __cplusplus
/* To make sure the i2s_event_callbacks_t is same size as i2s_event_callbacks_internal_t */
_Static_assert(sizeof(lp_i2s_evt_cbs_t) == sizeof(lp_i2s_evt_cbs_internal_t), "Invalid size of lp_i2s_evt_cbs_t structure");
#endif

esp_err_t lp_i2s_register_event_callbacks(lp_i2s_chan_handle_t handle, const lp_i2s_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(handle && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(handle->state < I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, TAG, "the channel is in enabled state already");

    handle->cbs.on_thresh_met = cbs->on_thresh_met;
    handle->cbs.on_request_new_trans = cbs->on_request_new_trans;
    handle->user_data = user_data;

    return ESP_OK;
}

static inline bool s_lp_i2s_take_available_channel(lp_i2s_controller_t *ctlr, uint8_t chan_search_mask)
{
    bool is_available = false;

    portENTER_CRITICAL(&g_i2s.spinlock);
    if (!(chan_search_mask & ctlr->chan_occupancy)) {
        ctlr->chan_occupancy |= chan_search_mask;
        is_available = true;
    }
    portEXIT_CRITICAL(&g_i2s.spinlock);
    return is_available;
}

static esp_err_t s_i2s_register_channel(lp_i2s_controller_t *ctlr, i2s_dir_t dir)
{
    esp_err_t ret = ESP_OK;

    lp_i2s_chan_handle_t new_chan = (lp_i2s_chan_handle_t)heap_caps_calloc(1, sizeof(struct lp_i2s_channel_obj_t), LP_I2S_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(new_chan, ESP_ERR_NO_MEM, TAG, "No memory for new channel");
    new_chan->mode = I2S_COMM_MODE_NONE;
    new_chan->role = I2S_ROLE_MASTER;
    new_chan->dir = dir;
    new_chan->state = I2S_CHAN_STATE_REGISTER;
    new_chan->ctlr = ctlr;

    if (dir == I2S_DIR_RX) {
        ctlr->rx_chan = new_chan;
    }

    return ret;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
static void IRAM_ATTR s_i2s_default_isr(void *arg)
{
    lp_i2s_controller_t *ctlr = (lp_i2s_controller_t *)arg;
    bool need_yield = false;
    BaseType_t high_task_woken = pdFALSE;
    ESP_DRAM_LOGD(TAG, "in isr, rx_mem_fifo_cnt: %d bytes", lp_i2s_ll_get_rx_mem_fifo_cnt(ctlr->hal.dev));

    if (ctlr->rx_chan->cbs.on_request_new_trans) {
        size_t len = MIN(lp_i2s_ll_get_rx_mem_fifo_cnt(ctlr->hal.dev), ctlr->rx_chan->trans.buflen);
        ESP_DRAM_LOGD(TAG, "len: %d", len);
        lp_i2s_ll_read_buffer(ctlr->hal.dev, ctlr->rx_chan->trans.buffer, len);
        lp_i2s_ll_rx_clear_interrupt_status(ctlr->hal.dev, LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT);
        ctlr->rx_chan->trans.received_size = len;

        lp_i2s_evt_data_t edata = {
            .trans = ctlr->rx_chan->trans,
        };
        if (ctlr->rx_chan->cbs.on_thresh_met) {
            need_yield |= ctlr->rx_chan->cbs.on_thresh_met(ctlr->rx_chan, &edata, ctlr->rx_chan->user_data);
        }

        assert(ctlr->rx_chan->cbs.on_request_new_trans);
        lp_i2s_evt_data_t new_edata = {};
        ctlr->rx_chan->cbs.on_request_new_trans(ctlr->rx_chan, &new_edata, ctlr->rx_chan->user_data);
        memcpy(&ctlr->rx_chan->trans, &new_edata.trans, sizeof(lp_i2s_trans_t));

    } else {
        portENTER_CRITICAL_ISR(&g_i2s.spinlock);
        lp_i2s_ll_rx_enable_interrupt(ctlr->hal.dev, LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT, false);
        portEXIT_CRITICAL_ISR(&g_i2s.spinlock);
        xSemaphoreGiveFromISR(ctlr->rx_chan->semphr, &high_task_woken);
    }

    need_yield |= high_task_woken == pdTRUE;
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
