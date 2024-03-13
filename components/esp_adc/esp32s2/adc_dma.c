/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 *  This file is a target specific for DAC DMA peripheral
 *  Target: ESP32-S2
 *  DAC DMA peripheral (data source): SPI3 (i.e. use SPI DMA to transmit data)
 *  DAC DMA interrupt source: SPI3
 *  DAC digital controller clock source: DIG_SARADC_CLK (root clock: APB or APLL)
 */

#include "hal/spi_types.h"
#include "hal/spi_ll.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/adc_dma.h"

#define ADC_DMA_SPI_HOST                SPI3_HOST
#define ADC_DMA_INTR_MASK               SPI_LL_INTR_IN_SUC_EOF

static IRAM_ATTR void adc_dma_intr_handler(void *arg)
{
    adc_continuous_ctx_t *ctx = (adc_continuous_ctx_t *)arg;
    bool need_yield = false;

    bool conversion_finish = spi_ll_get_intr(ctx->adc_dma.adc_spi_dev, ADC_DMA_INTR_MASK);
    if (conversion_finish) {
        spi_ll_clear_intr(ctx->adc_dma.adc_spi_dev, ADC_DMA_INTR_MASK);
        intptr_t desc_addr = spi_dma_ll_get_in_suc_eof_desc_addr(ctx->adc_dma.adc_spi_dev, ctx->adc_dma.spi_dma_ctx->rx_dma_chan.chan_id);
        ctx->rx_eof_desc_addr = desc_addr;
        need_yield = ctx->adc_intr_func(ctx);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t adc_dma_intr_event_init(adc_continuous_ctx_t *adc_ctx)
{
    return (esp_intr_alloc(spicommon_irqdma_source_for_host(ADC_DMA_SPI_HOST), ESP_INTR_FLAG_IRAM, adc_dma_intr_handler,
                           (void *)adc_ctx, &adc_ctx->adc_dma.dma_intr_hdl));
}

esp_err_t adc_dma_init(adc_dma_t *adc_dma)
{
    esp_err_t ret = ESP_OK;
    //ADC utilises SPI3 DMA on ESP32S2
    bool spi_success = false;
    spi_success = spicommon_periph_claim(ADC_DMA_SPI_HOST, "adc");
    if (spi_success != true) {
        return ESP_FAIL;
    }
    ret = spicommon_dma_chan_alloc(SPI3_HOST, SPI_DMA_CH_AUTO, &(adc_dma->spi_dma_ctx));
    if (ret != ESP_OK) {
        return ret;
    }
    adc_dma->adc_spi_dev = SPI_LL_GET_HW(ADC_DMA_SPI_HOST);

    return ESP_OK;
}

esp_err_t adc_dma_deinit(adc_dma_t adc_dma)
{
    esp_intr_free(adc_dma.dma_intr_hdl);
    spicommon_dma_chan_free(adc_dma.spi_dma_ctx);
    spicommon_periph_free(ADC_DMA_SPI_HOST);
    return ESP_OK;
}

esp_err_t adc_dma_start(adc_dma_t adc_dma, dma_descriptor_t *addr)
{
    spi_ll_clear_intr(adc_dma.adc_spi_dev, ADC_DMA_INTR_MASK);
    spi_ll_enable_intr(adc_dma.adc_spi_dev, ADC_DMA_INTR_MASK);
    spi_dma_ll_rx_start(adc_dma.adc_spi_dev, adc_dma.spi_dma_ctx->rx_dma_chan.chan_id, (lldesc_t *)addr);
    return ESP_OK;
}

esp_err_t adc_dma_stop(adc_dma_t adc_dma)
{
    spi_ll_disable_intr(adc_dma.adc_spi_dev, ADC_DMA_INTR_MASK);
    spi_ll_clear_intr(adc_dma.adc_spi_dev, ADC_DMA_INTR_MASK);
    spi_dma_ll_rx_stop(adc_dma.adc_spi_dev, adc_dma.spi_dma_ctx->rx_dma_chan.chan_id);
    return ESP_OK;
}

esp_err_t adc_dma_reset(adc_dma_t adc_dma)
{
    spi_dma_ll_rx_reset(adc_dma.adc_spi_dev, adc_dma.spi_dma_ctx->rx_dma_chan.chan_id);
    return ESP_OK;
}
