/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/spi_ll.h"
#include "hal/dac_ll.h"
#include "hal/adc_ll.h"
#include "soc/lldesc.h"
#include "esp_private/dac_dma.h"
#include "esp_private/periph_ctrl.h"
#include "driver/spi_common_internal.h"
#include "esp_check.h"

#define DAC_DMA_PERIPH_SPI_HOST          SPI3_HOST

typedef struct {
    void                *periph_dev;    /* DMA peripheral device address */
    uint32_t            dma_chan;
    intr_handle_t       intr_handle;    /* Interrupt handle */
} dac_dma_periph_spi_t;

static dac_dma_periph_spi_t *s_ddp = NULL; // Static DAC DMA peripheral structure pointer

static const char *TAG = "DAC_DMA";

extern portMUX_TYPE dac_spinlock; /* Global DAC spinlock */

/**
 * @brief Calculate and set DAC data frequency
 * @note  DAC clcok shares clock devider with ADC, the clock source is APB or APLL on ESP32-S2
 *        freq_hz = (source_clk / (clk_div + (b / a) + 1)) / interval
 *        interval range: 1~4095, to avoid decimal as possible, all calculations will base on interval = 4000
 * @param freq_hz    DAC byte frequency
 * @return
 *      - ESP_OK    config success
 *      - ESP_ERR_INVALID_ARG   invalid frequency
 */
// TODO: check clock again, the dma data seems abnormal
static esp_err_t dac_dma_periph_set_clock(uint32_t freq_hz){
    ESP_RETURN_ON_FALSE(freq_hz >= 80, ESP_ERR_INVALID_ARG, TAG, "the DAC frequency should be greater than 80 Hz");
    // TODO: replace 80000000 with APB or APLL clock frequency
    // when interval = 4000, max_freq = 20k min_freq = 80
    uint32_t freq_khz = freq_hz / 1000;
    /* If freq_khz < 20k, interval = 4000 is enough, so mutiple = 1,
     * otherwise interval need to zoom out to increase the max_freq,
     * And in order to avoid decimal as possible, multiple better to be 2^n */
    uint32_t multiple = freq_khz < 20 ? 1 : 1 << (32 - __builtin_clz(freq_khz / 20)); // Multiple need to be 2^n to avoid decimal
    uint32_t interval = 4000 / multiple; // Zoom in the max/min supported freq by zooming out interval
    ESP_RETURN_ON_FALSE(interval > 0, ESP_ERR_INVALID_ARG, TAG, "the DAC frequency is too big");

    uint32_t clk_div = (80000000 / interval) / freq_hz;
    uint32_t mod = (80000000 / interval) % freq_hz;
    uint32_t a = 0;
    uint32_t b = 1;
    if (mod == 0) {
        goto finish;
    }

    uint32_t min_diff = mod + 1;
    for (uint32_t tmp_b = 1; tmp_b < 64; tmp_b++) {
        uint32_t tmp_a = (uint32_t)(((mod * b) / (float)freq_hz) + 0.5);
        uint32_t diff = (uint32_t)abs((int)(mod * tmp_b) - (int)(freq_hz * tmp_a));
        if (diff == 0) {
            a = tmp_a;
            b = tmp_b;
            goto finish;
        }
        if (diff < min_diff) {
            min_diff = diff;
            a = tmp_a;
            b = tmp_b;
        }
    }

finish:
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_clk_inv(true);
    dac_ll_digi_set_trigger_interval(interval); // secondary clock division
    adc_ll_digi_controller_clk_div(clk_div, b, a);
    adc_ll_digi_clk_sel(false);
    portEXIT_CRITICAL(&dac_spinlock);
    return ESP_OK;
}

esp_err_t dac_dma_periph_init(int chan_num, uint32_t freq_hz, bool is_alternate)
{
    esp_err_t ret = ESP_OK;
    /* Acquire DMA peripheral */
    ESP_RETURN_ON_FALSE(spicommon_periph_claim(DAC_DMA_PERIPH_SPI_HOST, "dac_dma"), ESP_ERR_NOT_FOUND, TAG, "Failed to acquire DAC DMA peripheral");
    // TODO: reference count, maybe only  required on s2
    periph_module_enable(PERIPH_SARADC_MODULE);
    /* Allocate DAC DMA peripheral object */
    s_ddp = (dac_dma_periph_spi_t *)calloc(1, sizeof(dac_dma_periph_spi_t));
    ESP_GOTO_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, err, TAG, "No memory for DAC DMA object");
    s_ddp->periph_dev = (void *)SPI_LL_GET_HW(DAC_DMA_PERIPH_SPI_HOST);
    // TODO: clock may related to convert mode (mono/stereo)
    ESP_GOTO_ON_ERROR(dac_dma_periph_set_clock(freq_hz), err, TAG, "Failed to set clock of DMA peripheral");

    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_set_convert_mode(is_alternate);
    portEXIT_CRITICAL(&dac_spinlock);
    return ret;
err:
    dac_dma_periph_deinit();
    return ret;
}

esp_err_t dac_dma_periph_deinit(void)
{
    ESP_RETURN_ON_FALSE(spicommon_periph_free(DAC_DMA_PERIPH_SPI_HOST), ESP_FAIL, TAG, "Failed to release DAC DMA peripheral");
    // TODO: reference count, maybe only  required on s2
    periph_module_disable(PERIPH_SARADC_MODULE);
    if (s_ddp) {
        if (s_ddp->intr_handle) {
            dac_dma_periph_deregister_intr();
        }
        free(s_ddp);
        s_ddp = NULL;
    }
    return ESP_OK;
}

esp_err_t dac_dma_periph_register_intr(intr_handler_t intr_handler_func, void *user_ctx)
{
    ESP_RETURN_ON_FALSE(s_ddp, ESP_ERR_INVALID_STATE, TAG, "DAC DMA peripheral has not initialized yet");
    ESP_RETURN_ON_ERROR(spicommon_dma_chan_alloc(DAC_DMA_PERIPH_SPI_HOST, SPI_DMA_CH_AUTO, &s_ddp->dma_chan, &s_ddp->dma_chan),
                        TAG, "Failed to allocate dma peripheral channel");
    esp_err_t ret = ESP_OK;
    /* Regigster interrupt */
    ESP_GOTO_ON_ERROR(esp_intr_alloc(spicommon_irqdma_source_for_host(DAC_DMA_PERIPH_SPI_HOST),
                        0, intr_handler_func, user_ctx, &(s_ddp->intr_handle)), err, TAG, "Failed to register DAC DMA interrupt");
    portENTER_CRITICAL(&dac_spinlock);
    spi_ll_enable_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF);
    portEXIT_CRITICAL(&dac_spinlock);
    return ret;
err:
    spicommon_dma_chan_free(DAC_DMA_PERIPH_SPI_HOST);
    return ret;
}

esp_err_t dac_dma_periph_deregister_intr(void)
{
    ESP_RETURN_ON_FALSE(s_ddp, ESP_ERR_INVALID_STATE, TAG, "DAC DMA peripheral has not initialized yet");
    ESP_RETURN_ON_ERROR(spicommon_dma_chan_free(DAC_DMA_PERIPH_SPI_HOST), TAG, "Failed to free dma peripheral channel");
    if (s_ddp->intr_handle) {
        portENTER_CRITICAL(&dac_spinlock);
        spi_ll_disable_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF);
        portEXIT_CRITICAL(&dac_spinlock);
        esp_intr_free(s_ddp->intr_handle);
        s_ddp->intr_handle = NULL;
    }
    return ESP_OK;
}

void dac_dma_periph_enable(void)
{
    portENTER_CRITICAL(&dac_spinlock);
    spi_dma_ll_tx_reset(s_ddp->periph_dev, s_ddp->dma_chan);
    spi_ll_dma_tx_fifo_reset(s_ddp->periph_dev);
    dac_ll_digi_trigger_output(true);
    portEXIT_CRITICAL(&dac_spinlock);
    /* Enable interrupt */
    esp_intr_enable(s_ddp->intr_handle);
}

void dac_dma_periph_disable(void)
{
    portENTER_CRITICAL(&dac_spinlock);
    spi_dma_ll_tx_reset(s_ddp->periph_dev, s_ddp->dma_chan);
    spi_ll_dma_tx_fifo_reset(s_ddp->periph_dev);
    spi_dma_ll_tx_stop(s_ddp->periph_dev, s_ddp->dma_chan);
    dac_ll_digi_trigger_output(false);
    portEXIT_CRITICAL(&dac_spinlock);
    /* Disable interrupt */
    esp_intr_disable(s_ddp->intr_handle);
}

bool IRAM_ATTR dac_dma_periph_intr_is_triggered(void)
{
    uint32_t is_triggered = spi_ll_get_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF);
    spi_ll_clear_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF);
    return is_triggered;
}

uint32_t IRAM_ATTR dac_dma_periph_intr_get_eof_desc(void)
{
    return spi_dma_ll_get_out_eof_desc_addr(s_ddp->periph_dev, s_ddp->dma_chan);
}

void dac_dma_periph_dma_trans_start(uint32_t desc_addr)
{
    portENTER_CRITICAL(&dac_spinlock);
    spi_dma_ll_tx_reset(s_ddp->periph_dev, s_ddp->dma_chan);
    spi_ll_dma_tx_fifo_reset(s_ddp->periph_dev);
    spi_dma_ll_tx_start(s_ddp->periph_dev, s_ddp->dma_chan, (lldesc_t *)desc_addr);
    portEXIT_CRITICAL(&dac_spinlock);
}
