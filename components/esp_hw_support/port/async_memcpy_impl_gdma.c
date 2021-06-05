// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "freertos/FreeRTOS.h"
#include "soc/periph_defs.h"
#include "soc/soc_memory_layout.h"
#include "soc/soc_caps.h"
#include "hal/gdma_ll.h"
#include "hal/gdma_hal.h"
#include "driver/periph_ctrl.h"
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
        .owner_check = true
    };

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
    return true;
}
