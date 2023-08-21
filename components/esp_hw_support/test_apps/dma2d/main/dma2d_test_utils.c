/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "unity.h"
#include "dma2d_test_utils.h"
#include "esp_private/dma2d.h"
#include "soc/dma2d_channel.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_log.h"

__attribute__((unused)) static const char *TAG = "dma2d_m2m";

#if CONFIG_DMA2D_OPERATION_FUNC_IN_IRAM || CONFIG_DMA2D_ISR_IRAM_SAFE
#define DMA2D_M2M_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DMA2D_M2M_MEM_ALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif

#if CONFIG_DMA2D_ISR_IRAM_SAFE
#define DMA2D_M2M_ATTR              IRAM_ATTR
#else
#define DMA2D_M2M_ATTR
#endif

typedef dma2d_m2m_trans_config_t dma2d_m2m_trans_desc_t;

typedef struct {
    dma2d_m2m_trans_desc_t m2m_trans_desc;
    dma2d_trans_config_t dma_chan_desc;
    uint32_t dma_trans_placeholder_head; /* Head of the memory for storing the 2D-DMA transaction elm */
} dma2d_m2m_transaction_t;

static dma2d_pool_handle_t dma2d_pool_handle;

esp_err_t dma2d_m2m_init(void)
{
    dma2d_pool_config_t dma2d_pool_config = {
        .pool_id = 0,
    };
    return dma2d_acquire_pool(&dma2d_pool_config, &dma2d_pool_handle);
}

esp_err_t dma2d_m2m_deinit(void)
{
    return dma2d_release_pool(dma2d_pool_handle);
}

static bool DMA2D_M2M_ATTR dma2d_m2m_transaction_done_cb(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    dma2d_m2m_transaction_t *trans_config = (dma2d_m2m_transaction_t *)user_data;
    dma2d_m2m_trans_desc_t *m2m_trans_desc = &trans_config->m2m_trans_desc;

    if (m2m_trans_desc->trans_eof_cb) {
        need_yield |= m2m_trans_desc->trans_eof_cb(m2m_trans_desc->user_data);
    }

    free(trans_config);

    return need_yield;
}

static bool DMA2D_M2M_ATTR dma2d_m2m_transaction_on_picked(uint32_t channel_num, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config)
{
    assert(channel_num == 2 && dma2d_chans && user_config);
    dma2d_m2m_transaction_t *trans_config = (dma2d_m2m_transaction_t *)user_config;
    dma2d_m2m_trans_desc_t *m2m_trans_desc = &trans_config->m2m_trans_desc;

    // Get the required 2D-DMA channel handles
    uint32_t dma_tx_chan_idx = 0;
    uint32_t dma_rx_chan_idx = 1;
    if (dma2d_chans[0].dir == DMA2D_CHANNEL_DIRECTION_RX) {
        dma_tx_chan_idx = 1;
        dma_rx_chan_idx = 0;
    }
    dma2d_channel_handle_t dma_tx_chan = dma2d_chans[dma_tx_chan_idx].chan;
    dma2d_channel_handle_t dma_rx_chan = dma2d_chans[dma_rx_chan_idx].chan;

    dma2d_trigger_t trig_periph = {
        .periph = DMA2D_TRIG_PERIPH_M2M,
        .periph_sel_id = SOC_DMA2D_TRIG_PERIPH_M2M_TX,
    };
    dma2d_connect(dma_tx_chan, &trig_periph);
    trig_periph.periph_sel_id = SOC_DMA2D_TRIG_PERIPH_M2M_RX;
    dma2d_connect(dma_rx_chan, &trig_periph);

    if (m2m_trans_desc->transfer_ability_config) {
        dma2d_set_transfer_ability(dma_tx_chan, m2m_trans_desc->transfer_ability_config);
        dma2d_set_transfer_ability(dma_rx_chan, m2m_trans_desc->transfer_ability_config);
    }

    if (m2m_trans_desc->tx_strategy_config) {
        dma2d_apply_strategy(dma_tx_chan, m2m_trans_desc->tx_strategy_config);
    }
    if (m2m_trans_desc->rx_strategy_config) {
        dma2d_apply_strategy(dma_rx_chan, m2m_trans_desc->rx_strategy_config);
    }

    if (m2m_trans_desc->tx_csc_config) {
        dma2d_configure_color_space_conversion(dma_tx_chan, m2m_trans_desc->tx_csc_config);
    }
    if (m2m_trans_desc->rx_csc_config) {
        dma2d_configure_color_space_conversion(dma_rx_chan, m2m_trans_desc->rx_csc_config);
    }

    dma2d_rx_event_callbacks_t dma_cbs = {
        .on_recv_eof = dma2d_m2m_transaction_done_cb,
    };
    dma2d_register_rx_event_callbacks(dma_rx_chan, &dma_cbs, (void *)trans_config);

    dma2d_set_desc_addr(dma_tx_chan, m2m_trans_desc->tx_desc_base_addr);
    dma2d_set_desc_addr(dma_rx_chan, m2m_trans_desc->rx_desc_base_addr);

    dma2d_start(dma_tx_chan);
    dma2d_start(dma_rx_chan);

    // No need to yield
    return false;
}

esp_err_t dma2d_m2m(const dma2d_m2m_trans_config_t *trans_config)
{
#if CONFIG_DMA2D_ISR_IRAM_SAFE
    if (trans_config->trans_eof_cb) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(trans_config->trans_eof_cb),
                            ESP_ERR_INVALID_ARG, TAG, "trans_eof_cb not in IRAM");
    }
    if (trans_config->user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(trans_config->user_data),
                            ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    dma2d_m2m_transaction_t *dma2d_m2m_trans = (dma2d_m2m_transaction_t *)heap_caps_calloc(1, sizeof(dma2d_m2m_transaction_t) + SIZEOF_DMA2D_TRANS_T, DMA2D_M2M_MEM_ALLOC_CAPS);
    TEST_ASSERT_NOT_NULL(dma2d_m2m_trans);

    dma2d_m2m_trans->dma_chan_desc.tx_channel_num = 1;
    dma2d_m2m_trans->dma_chan_desc.rx_channel_num = 1;
    dma2d_m2m_trans->dma_chan_desc.channel_flags = DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING;
    dma2d_m2m_trans->dma_chan_desc.channel_flags |= (trans_config->tx_csc_config == NULL) ? 0 : DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC;
    dma2d_m2m_trans->dma_chan_desc.channel_flags |= (trans_config->rx_csc_config == NULL) ? 0 : DMA2D_CHANNEL_FUNCTION_FLAG_RX_CSC;
    dma2d_m2m_trans->dma_chan_desc.specified_tx_channel_mask = 0;
    dma2d_m2m_trans->dma_chan_desc.specified_rx_channel_mask = 0;

    memcpy(&dma2d_m2m_trans->m2m_trans_desc, trans_config, sizeof(dma2d_m2m_trans_config_t));

    dma2d_m2m_trans->dma_chan_desc.user_config = (void *)dma2d_m2m_trans;
    dma2d_m2m_trans->dma_chan_desc.on_job_picked = dma2d_m2m_transaction_on_picked;

    esp_err_t ret = dma2d_enqueue(dma2d_pool_handle, &dma2d_m2m_trans->dma_chan_desc, (dma2d_trans_t *)&dma2d_m2m_trans->dma_trans_placeholder_head);
    if (ret != ESP_OK) {
        free(dma2d_m2m_trans);
    }
    return ret;
}
