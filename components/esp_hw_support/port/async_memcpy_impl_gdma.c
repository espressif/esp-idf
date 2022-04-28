/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "soc/periph_defs.h"
#include "soc/soc_memory_layout.h"
#include "soc/soc_caps.h"
#include "hal/gdma_ll.h"
#include "hal/gdma_hal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_async_memcpy_impl.h"

IRAM_ATTR static bool async_memcpy_impl_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    async_memcpy_impl_t *mcp_impl = (async_memcpy_impl_t *)user_data;
    mcp_impl->rx_eof_addr = event_data->rx_eof_desc_addr;

    async_memcpy_isr_on_rx_done_event(mcp_impl);
    return mcp_impl->isr_need_yield;
}

esp_err_t async_memcpy_impl_init(async_memcpy_impl_t *impl)
{
    esp_err_t ret = ESP_OK;
    // create TX channel and reserve sibling channel for future use
    gdma_channel_alloc_config_t tx_alloc_config = {
        .flags.reserve_sibling = 1,
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ret = gdma_new_channel(&tx_alloc_config, &impl->tx_channel);
    if (ret != ESP_OK) {
        goto err;
    }

    // create RX channel and specify it should be reside in the same pair as TX
    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = impl->tx_channel,
    };
    ret = gdma_new_channel(&rx_alloc_config, &impl->rx_channel);
    if (ret != ESP_OK) {
        goto err;
    }

    gdma_connect(impl->rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0));
    gdma_connect(impl->tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_M2M, 0));

    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = true,
        .owner_check = true,
    };

    gdma_transfer_ability_t transfer_ability = {
        .sram_trans_align = impl->sram_trans_align,
        .psram_trans_align = impl->psram_trans_align,
    };
    ret = gdma_set_transfer_ability(impl->tx_channel, &transfer_ability);
    if (ret != ESP_OK) {
        goto err;
    }
    ret = gdma_set_transfer_ability(impl->rx_channel, &transfer_ability);
    if (ret != ESP_OK) {
        goto err;
    }
    gdma_apply_strategy(impl->tx_channel, &strategy_config);
    gdma_apply_strategy(impl->rx_channel, &strategy_config);

    gdma_rx_event_callbacks_t cbs = {
        .on_recv_eof = async_memcpy_impl_rx_eof_callback
    };
    ret = gdma_register_rx_event_callbacks(impl->rx_channel, &cbs, impl);

err:
    return ret;
}

esp_err_t async_memcpy_impl_deinit(async_memcpy_impl_t *impl)
{
    gdma_disconnect(impl->rx_channel);
    gdma_disconnect(impl->tx_channel);
    gdma_del_channel(impl->rx_channel);
    gdma_del_channel(impl->tx_channel);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_start(async_memcpy_impl_t *impl, intptr_t outlink_base, intptr_t inlink_base)
{
    gdma_start(impl->rx_channel, inlink_base);
    gdma_start(impl->tx_channel, outlink_base);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_stop(async_memcpy_impl_t *impl)
{
    gdma_stop(impl->rx_channel);
    gdma_stop(impl->tx_channel);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_restart(async_memcpy_impl_t *impl)
{
    gdma_append(impl->rx_channel);
    gdma_append(impl->tx_channel);
    return ESP_OK;
}

bool async_memcpy_impl_is_buffer_address_valid(async_memcpy_impl_t *impl, void *src, void *dst)
{
    bool valid = true;
    if (esp_ptr_external_ram(dst)) {
        if (impl->psram_trans_align) {
            valid = valid && (((intptr_t)dst & (impl->psram_trans_align - 1)) == 0);
        }
    } else {
        if (impl->sram_trans_align) {
            valid = valid && (((intptr_t)dst & (impl->sram_trans_align - 1)) == 0);
        }
    }
    return valid;
}
