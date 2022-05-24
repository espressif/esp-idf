/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/spi_common_internal.h"
#include "esp_private/periph_ctrl.h"
#include "hal/spi_ll.h"
#include "hal/dac_ll.h"
#include "hal/adc_ll.h"
#include "soc/lldesc.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "../dac_dma.h"
#include "clk_ctrl_os.h"
#if CONFIG_DAC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_check.h"
#include "esp_attr.h"

#define DAC_DMA_PERIPH_SPI_HOST          SPI3_HOST

typedef struct {
    void                *periph_dev;    /* DMA peripheral device address */
    uint32_t            dma_chan;
    intr_handle_t       intr_handle;    /* Interrupt handle */
    bool                use_apll;       /* Whether use APLL as digital controller clock source */
} dac_dma_periph_spi_t;

static dac_dma_periph_spi_t *s_ddp = NULL; // Static DAC DMA peripheral structure pointer

static const char *TAG = "DAC_DMA";

extern portMUX_TYPE dac_spinlock; /* Global DAC spinlock */

static uint32_t dac_set_apll_freq(uint32_t expt_freq)
{
    /* Set APLL coefficients to the given frequency */
    uint32_t real_freq = 0;
    esp_err_t ret = periph_rtc_apll_freq_set(expt_freq, &real_freq);
    if (ret == ESP_ERR_INVALID_ARG) {
        return 0;
    }
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "APLL is occupied already, it is working at %d Hz", real_freq);
    }
    ESP_LOGD(TAG, "APLL expected frequency is %d Hz, real frequency is %d Hz", expt_freq, real_freq);
    return real_freq;
}

/**
 * @brief Calculate and set DAC data frequency
 * @note  DAC clcok shares clock devider with ADC, the clock source is APB or APLL on ESP32-S2
 *        freq_hz = (source_clk / (clk_div + (b / a) + 1)) / interval
 *        interval range: 1~4095, to avoid decimal as possible, all calculations will base on interval = 4000
 * @param freq_hz    DAC byte transmit frequency
 * @return
 *      - ESP_OK    config success
 *      - ESP_ERR_INVALID_ARG   invalid frequency
 */
static esp_err_t dac_dma_periph_set_clock(uint32_t freq_hz, bool is_apll){
    /* Step 1: Determine the digital clock source frequency */
    uint32_t digi_ctrl_freq; // Digital controller clock
    if (is_apll) {
        /* Theoretical frequency range (due to the limitation of DAC, the maximum frequency may not reach):
         * SOC_APLL_MAX_HZ: 119.24 Hz ~ 67.5 MHz
         * SOC_APLL_MIN_HZ: 5.06 Hz ~ 2.65 MHz */
        digi_ctrl_freq = dac_set_apll_freq(freq_hz < 120 ? SOC_APLL_MIN_HZ :SOC_APLL_MAX_HZ);
        ESP_RETURN_ON_FALSE(digi_ctrl_freq, ESP_ERR_INVALID_ARG, TAG, "set APLL coefficients failed");
    } else {
        digi_ctrl_freq = APB_CLK_FREQ;
    }

    /* Step 2: Determine the interval */
    uint32_t total_div = digi_ctrl_freq / freq_hz;
    uint32_t interval;
    /* For the case that smaller than the minimum ADC controller division, the required frequency is too big */
    ESP_RETURN_ON_FALSE(total_div >= 2, ESP_ERR_INVALID_ARG, TAG, "the DAC frequency is too big");
    if (total_div < 256) { // For the case that smaller than the maximum ADC controller division
        /* Fix the interval to 1, the division is fully realized by the ADC controller clock divider */
        interval = 1;
    } else if (total_div < 8192) { // for the case that smaller than the maximum inverval
        /* Set the interval to 'total_div / 2', fix the integer part of ADC controller clock division to 2 */
        interval = total_div / 2;
    } else {
        /* Fix the interval to 4095, */
        interval = 4095;
    }
    ESP_RETURN_ON_FALSE(interval * 256 > total_div, ESP_ERR_INVALID_ARG, TAG, "the DAC frequency is too small");

    /* Step 3: Calculate the coefficients of ADC digital controller divider*/
    uint32_t fsclk = interval * freq_hz; /* The clock frequency that produced by ADC controller divider */
    uint32_t clk_div = digi_ctrl_freq / fsclk;
    uint32_t mod = digi_ctrl_freq % fsclk;
    uint32_t a = 0;
    uint32_t b = 1;
    if (mod == 0) {
        goto finish;
    }
    uint32_t min_diff = mod + 1;
    for (uint32_t tmp_b = 1; tmp_b < 64; tmp_b++) {
        uint32_t tmp_a = (uint32_t)(((mod * b) / (float)fsclk) + 0.5);
        uint32_t diff = (uint32_t)abs((int)(mod * tmp_b) - (int)(fsclk * tmp_a));
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
    /* Step 4: Set the clock coefficients */
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_clk_inv(true);
    dac_ll_digi_set_trigger_interval(interval); // secondary clock division
    adc_ll_digi_controller_clk_div(clk_div - 1, b, a);
    adc_ll_digi_clk_sel(is_apll);
    portEXIT_CRITICAL(&dac_spinlock);
    return ESP_OK;
}

esp_err_t dac_dma_periph_init(uint8_t chan_num, uint32_t freq_hz, bool is_alternate, bool is_apll)
{
#if CONFIG_DAC_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    /* Acquire DMA peripheral */
    ESP_RETURN_ON_FALSE(spicommon_periph_claim(DAC_DMA_PERIPH_SPI_HOST, "dac_dma"), ESP_ERR_NOT_FOUND, TAG, "Failed to acquire DAC DMA peripheral");
    periph_module_enable(PERIPH_SARADC_MODULE);
    /* Allocate DAC DMA peripheral object */
    s_ddp = (dac_dma_periph_spi_t *)calloc(1, sizeof(dac_dma_periph_spi_t));
    ESP_GOTO_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, err, TAG, "No memory for DAC DMA object");
    s_ddp->periph_dev = (void *)SPI_LL_GET_HW(DAC_DMA_PERIPH_SPI_HOST);

    if (is_apll) {
        periph_rtc_apll_acquire();
        s_ddp->use_apll = true;
    }
    /* When transmit alternately, twice frequency is needed to guarantee the convert frequency in one channel */
    uint32_t trans_freq_hz = freq_hz * (is_alternate ? 2 : 1);
    ESP_GOTO_ON_ERROR(dac_dma_periph_set_clock(trans_freq_hz, is_apll), err, TAG, "Failed to set clock of DMA peripheral");

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
    periph_module_disable(PERIPH_SARADC_MODULE);
    if (s_ddp) {
        if (s_ddp->intr_handle) {
            dac_dma_periph_deregister_intr();
        }
        if (s_ddp->use_apll) {
            periph_rtc_apll_release();
            s_ddp->use_apll = false;
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
    spi_ll_enable_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF | SPI_LL_INTR_OUT_TOTAL_EOF);
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
        spi_ll_disable_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF | SPI_LL_INTR_OUT_TOTAL_EOF);
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

uint32_t IRAM_ATTR dac_dma_periph_intr_is_triggered(void)
{
    uint32_t ret = 0;
    ret |= spi_ll_get_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF) ? DAC_DMA_EOF_INTR : 0;
    ret |= spi_ll_get_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_TOTAL_EOF) ? DAC_DMA_TEOF_INTR : 0;
    spi_ll_clear_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_EOF);
    spi_ll_clear_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_TOTAL_EOF);
    return ret;
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
