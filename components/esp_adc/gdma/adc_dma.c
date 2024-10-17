/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/gdma.h"
#include "esp_private/adc_dma.h"

static IRAM_ATTR bool adc_dma_in_suc_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    assert(event_data);
    adc_continuous_ctx_t *ctx = (adc_continuous_ctx_t *)user_data;
    ctx->rx_eof_desc_addr = event_data->rx_eof_desc_addr;
    return ctx->adc_intr_func(user_data);
}

esp_err_t adc_dma_intr_event_init(adc_continuous_ctx_t *adc_ctx)
{
    gdma_rx_event_callbacks_t cbs = {
        .on_recv_eof = adc_dma_in_suc_eof_callback
    };
    gdma_register_rx_event_callbacks(adc_ctx->adc_dma.gdma_chan, &cbs, adc_ctx);
    return ESP_OK;
}

esp_err_t adc_dma_init(adc_dma_t *adc_dma)
{
    esp_err_t ret = ESP_OK;
    //alloc rx gdma channel
    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
    };
    ret = gdma_new_ahb_channel(&rx_alloc_config, &(adc_dma->gdma_chan));
    if (ret != ESP_OK) {
        return ret;
    }
    gdma_connect(adc_dma->gdma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_ADC, 0));

    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = true,
        .owner_check = true
    };
    gdma_apply_strategy(adc_dma->gdma_chan, &strategy_config);

    return ESP_OK;
}

esp_err_t adc_dma_deinit(adc_dma_t adc_dma)
{
    gdma_disconnect(adc_dma.gdma_chan);
    gdma_del_channel(adc_dma.gdma_chan);
    return ESP_OK;
}

esp_err_t adc_dma_start(adc_dma_t adc_dma, dma_descriptor_t *addr)
{
    return gdma_start(adc_dma.gdma_chan, (intptr_t)addr);
}

esp_err_t adc_dma_stop(adc_dma_t adc_dma)
{
    return gdma_stop(adc_dma.gdma_chan);
}

esp_err_t adc_dma_reset(adc_dma_t adc_dma)
{
    return gdma_reset(adc_dma.gdma_chan);
}
