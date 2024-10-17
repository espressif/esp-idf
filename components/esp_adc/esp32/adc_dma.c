/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 *  This file is a target specific for DAC DMA peripheral
 *  Target: ESP32
 *  DAC DMA peripheral (data source): I2S0 (i.e. use I2S DMA to transmit data)
 *  DAC DMA interrupt source: I2S0
 *  DAC digital controller clock source: I2S ws signal (root clock: D2PLL or APLL)
 */

#include "hal/i2s_types.h"
#include "driver/i2s_types.h"
#include "soc/i2s_periph.h"
#include "esp_private/i2s_platform.h"
#include "esp_private/adc_dma.h"
#include "hal/i2s_ll.h"

#define ADC_DMA_I2S_HOST                ADC_HAL_DMA_I2S_HOST
#define ADC_DMA_INTR_MASK               BIT(9)

static IRAM_ATTR void adc_dma_intr_handler(void *arg)
{
    adc_continuous_ctx_t *ctx = (adc_continuous_ctx_t *)arg;
    bool need_yield = false;

    bool conversion_finish = i2s_ll_get_intr_status(ctx->adc_dma.adc_i2s_dev) & ADC_DMA_INTR_MASK;
    if (conversion_finish) {
        i2s_ll_clear_intr_status(ctx->adc_dma.adc_i2s_dev, ADC_DMA_INTR_MASK);
        uint32_t desc_addr;
        i2s_ll_rx_get_eof_des_addr(ctx->adc_dma.adc_i2s_dev, &desc_addr);
        ctx->rx_eof_desc_addr = (intptr_t)desc_addr;
        need_yield = ctx->adc_intr_func(ctx);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t adc_dma_intr_event_init(adc_continuous_ctx_t *adc_ctx)
{
    return (esp_intr_alloc(i2s_periph_signal[ADC_DMA_I2S_HOST].irq, ESP_INTR_FLAG_IRAM, adc_dma_intr_handler,
                           (void *)adc_ctx, &adc_ctx->adc_dma.dma_intr_hdl));
}

esp_err_t adc_dma_init(adc_dma_t *adc_dma)
{
    esp_err_t ret = ESP_OK;
    //ADC utilises I2S0 DMA on ESP32
    ret = i2s_platform_acquire_occupation(I2S_CTLR_HP, ADC_DMA_I2S_HOST, "adc");
    if (ret != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }
    adc_dma->adc_i2s_dev = I2S_LL_GET_HW(ADC_DMA_I2S_HOST);
    return ESP_OK;
}

esp_err_t adc_dma_deinit(adc_dma_t adc_dma)
{
    esp_intr_free(adc_dma.dma_intr_hdl);
    i2s_platform_release_occupation(I2S_CTLR_HP, ADC_DMA_I2S_HOST);
    return ESP_OK;
}

esp_err_t adc_dma_start(adc_dma_t adc_dma, dma_descriptor_t *addr)
{
    i2s_ll_clear_intr_status(adc_dma.adc_i2s_dev, ADC_DMA_INTR_MASK);
    i2s_ll_enable_intr(adc_dma.adc_i2s_dev, ADC_DMA_INTR_MASK, true);

    i2s_ll_enable_dma(adc_dma.adc_i2s_dev, true);
    i2s_ll_rx_start_link(adc_dma.adc_i2s_dev, (uint32_t)addr);
    return ESP_OK;
}

esp_err_t adc_dma_stop(adc_dma_t adc_dma)
{
    i2s_ll_enable_intr(adc_dma.adc_i2s_dev, ADC_DMA_INTR_MASK, false);
    i2s_ll_clear_intr_status(adc_dma.adc_i2s_dev, ADC_DMA_INTR_MASK);
    i2s_ll_rx_stop_link(adc_dma.adc_i2s_dev);
    return ESP_OK;
}

esp_err_t adc_dma_reset(adc_dma_t adc_dma)
{
    i2s_ll_rx_reset_dma(adc_dma.adc_i2s_dev);
    return ESP_OK;
}
