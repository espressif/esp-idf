/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 *  Target-specific DAC DMA backend implementation
 *  Target: ESP32-S2
 *  DAC DMA peripheral (data source): SPI3 (i.e. use SPI DMA to transmit data)
 *  DAC DMA interrupt source: SPI3
 *  DAC digital controller clock source: DIG_SARADC_CLK (root clock: APB or APLL)
 */

#include "dac_priv_common.h"
#include "sdkconfig.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "hal/spi_ll.h"
#include "hal/dac_ll.h"
#include "hal/adc_ll.h"
#include "hal/hal_utils.h"
#include "hal/clk_tree_ll.h"
#include "soc/lldesc.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "dac_priv_dma.h"
#include "esp_clk_tree.h"
#include "esp_log.h"
#include "esp_check.h"

#define DAC_DMA_PERIPH_SPI_HOST          SPI3_HOST

#if CONFIG_DAC_ISR_IRAM_SAFE
#define DAC_DMA_INTR_ALLOC_FLAGS         (ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)
#else
#define DAC_DMA_INTR_ALLOC_FLAGS         (ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_INTRDISABLED)
#endif

typedef struct {
    void                *periph_dev;    /* DMA peripheral device address */
    uint32_t            dma_chan;
    intr_handle_t       intr_handle;    /* Interrupt handle */
    soc_periph_dac_digi_clk_src_t clk_src;  /* Acquired clock source; 0 means not enabled yet */
    dac_dma_event_callbacks_t cbs;      /* Event callbacks */
    void                *ctx;           /* Driver context for callbacks */
} dac_dma_periph_spi_t;

static dac_dma_periph_spi_t *s_ddp = NULL; // Static DAC DMA peripheral structure pointer

void dac_priv_dma_intr_handler(void *arg)
{
    dac_dma_periph_spi_t *ddp = arg;
    bool need_yield = false;

    bool done = spi_ll_get_intr(ddp->periph_dev, SPI_LL_INTR_OUT_DONE);
    bool teof = spi_ll_get_intr(ddp->periph_dev, SPI_LL_INTR_OUT_TOTAL_EOF);
    spi_ll_clear_intr(ddp->periph_dev, SPI_LL_INTR_OUT_DONE);
    spi_ll_clear_intr(ddp->periph_dev, SPI_LL_INTR_OUT_TOTAL_EOF);

    if (done && ddp->cbs.on_done) {
        need_yield |= ddp->cbs.on_done(ddp->ctx);
    }
    if (teof && ddp->cbs.on_teof) {
        need_yield |= ddp->cbs.on_teof(ddp->ctx);
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static uint32_t s_dac_set_apll_freq(uint32_t expt_freq)
{
    /* Set APLL coefficients to the given frequency */
    uint32_t real_freq = 0;
    esp_err_t ret = esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, expt_freq, &real_freq);
    if (ret == ESP_ERR_INVALID_ARG) {
        return 0;
    }
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "APLL is occupied already, it is working at %"PRIu32" Hz", real_freq);
    }
    ESP_LOGD(TAG, "APLL expected frequency is %"PRIu32" Hz, real frequency is %"PRIu32" Hz", expt_freq, real_freq);
    return real_freq;
}

/**
 * @brief Calculate and set DAC data frequency
 * @note  DAC clock shares clock divider with ADC, the clock source is APB or APLL on ESP32-S2
 *        freq_hz = (source_clk / (clk_div + (b / a) + 1)) / interval
 *        interval range: 1~4095
 * @param clk_src    DAC digital controller clock source
 * @param freq_hz    DAC byte transmit frequency
 * @return
 *      - ESP_OK    config success
 *      - ESP_ERR_INVALID_ARG   invalid frequency
 */
static esp_err_t s_dac_priv_dma_set_clock(soc_periph_dac_digi_clk_src_t clk_src, uint32_t freq_hz)
{
    /* Step 1: Determine the digital clock source frequency */
    uint32_t digi_ctrl_freq; // Digital controller clock
    if (clk_src == DAC_DIGI_CLK_SRC_APLL) {
        /* Theoretical frequency range (due to the limitation of DAC, the maximum frequency may not reach):
         * CLK_LL_APLL_MAX_HZ: 119.24 Hz ~ 67.5 MHz
         * CLK_LL_APLL_MIN_HZ: 5.06 Hz ~ 2.65 MHz */
        digi_ctrl_freq = s_dac_set_apll_freq(freq_hz < 120 ? CLK_LL_APLL_MIN_HZ : CLK_LL_APLL_MAX_HZ);
        ESP_RETURN_ON_FALSE(digi_ctrl_freq, ESP_ERR_INVALID_ARG, TAG, "set APLL coefficients failed");
    } else {
        ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &digi_ctrl_freq), TAG, "get clock source frequency failed");
    }

    /* Step 2: Determine the interval */
    uint32_t total_div = digi_ctrl_freq / freq_hz;
    uint32_t interval;
    /* For the case that smaller than the minimum ADC controller division, the required frequency is too big */
    ESP_RETURN_ON_FALSE(total_div >= 2, ESP_ERR_INVALID_ARG, TAG, "the DAC frequency is too big");
    if (total_div < 256) { // For the case that smaller than the maximum ADC controller division
        /* Fix the interval to 1, the division is fully realized by the ADC controller clock divider */
        interval = 1;
    } else if (total_div < 8192) { // for the case that smaller than the maximum interval
        /* Set the interval to 'total_div / 2', fix the integer part of ADC controller clock division to 2 */
        interval = total_div / 2;
    } else {
        /* Fix the interval to 4095, */
        interval = 4095;
    }
    ESP_RETURN_ON_FALSE(interval * 256 > total_div, ESP_ERR_INVALID_ARG, TAG, "the DAC frequency is too small");

    /* Step 3: Calculate the coefficients of ADC digital controller divider */
    hal_utils_clk_info_t adc_clk_info = {
        .src_freq_hz = digi_ctrl_freq / interval,
        .exp_freq_hz = freq_hz,
        .max_integ = 257,
        .min_integ = 1,
        .max_fract = 64,
    };
    hal_utils_clk_div_t adc_clk_div = {};
    hal_utils_calc_clk_div_frac_accurate(&adc_clk_info, &adc_clk_div);

    /* Step 4: Set the clock coefficients */
    dac_ll_digi_clk_inv(true);
    dac_ll_digi_set_trigger_interval(interval); // secondary clock division
    adc_ll_digi_controller_clk_div(adc_clk_div.integer - 1, adc_clk_div.denominator, adc_clk_div.numerator);
    adc_ll_digi_clk_sel((adc_continuous_clk_src_t)clk_src);
    return ESP_OK;
}

esp_err_t dac_priv_dma_init(soc_periph_dac_digi_clk_src_t clk_src, uint32_t freq_hz, bool is_alternate,
                            const dac_dma_event_callbacks_t *cbs, void *ctx)
{
    ESP_RETURN_ON_FALSE(clk_src == DAC_DIGI_CLK_SRC_APB || clk_src == DAC_DIGI_CLK_SRC_APLL, ESP_ERR_INVALID_ARG, TAG, "invalid DAC digital clock source");
    DAC_NULL_POINTER_CHECK(cbs);

    esp_err_t ret = ESP_OK;

    /* Allocate DAC DMA peripheral object */
    s_ddp = (dac_dma_periph_spi_t *)heap_caps_calloc(1, sizeof(dac_dma_periph_spi_t), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, TAG, "No memory for DAC DMA object");

    /* Acquire DMA peripheral */
    ESP_GOTO_ON_FALSE(spicommon_periph_claim(DAC_DMA_PERIPH_SPI_HOST, "dac_dma"), ESP_ERR_NOT_FOUND, err, TAG, "Failed to acquire DAC DMA peripheral");
    adc_apb_periph_claim();
    s_ddp->periph_dev = (void *)SPI_LL_GET_HW(DAC_DMA_PERIPH_SPI_HOST);

    /* Configure clock source and frequency */
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)clk_src, true), err, TAG, "enable DAC digital clock source failed");
    s_ddp->clk_src = clk_src;
    /* When transmit alternately, twice frequency is needed to guarantee the convert frequency in one channel */
    uint32_t trans_freq_hz = freq_hz * (is_alternate ? 2 : 1);
    ESP_GOTO_ON_ERROR(s_dac_priv_dma_set_clock(clk_src, trans_freq_hz), err, TAG, "Failed to set clock of DMA peripheral");

    ESP_GOTO_ON_ERROR(spicommon_dma_chan_alloc(DAC_DMA_PERIPH_SPI_HOST, SPI_DMA_CH_AUTO, 0),
                      err, TAG, "Failed to allocate dma peripheral channel");
    s_ddp->dma_chan = spi_bus_get_dma_ctx(DAC_DMA_PERIPH_SPI_HOST)->rx_dma_chan.chan_id;
    spi_ll_enable_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_DONE | SPI_LL_INTR_OUT_TOTAL_EOF);
    dac_ll_digi_set_convert_mode(is_alternate);

    s_ddp->cbs = *cbs;
    s_ddp->ctx = ctx;
    ESP_GOTO_ON_ERROR(esp_intr_alloc(spicommon_irqdma_source_for_host(DAC_DMA_PERIPH_SPI_HOST), DAC_DMA_INTR_ALLOC_FLAGS, dac_priv_dma_intr_handler, s_ddp, &s_ddp->intr_handle),
                      err, TAG, "Failed to register DAC DMA interrupt");
    return ret;
err:
    dac_priv_dma_deinit();
    return ret;
}

esp_err_t dac_priv_dma_deinit(void)
{
    if (!s_ddp) {
        return ESP_OK;
    }

    if (s_ddp->intr_handle) {
        ESP_RETURN_ON_ERROR(esp_intr_disable(s_ddp->intr_handle), TAG, "Failed to disable DAC DMA interrupt");
        ESP_RETURN_ON_ERROR(esp_intr_free(s_ddp->intr_handle), TAG, "Failed to deregister DAC DMA interrupt");
        s_ddp->intr_handle = NULL;
    }

    if (s_ddp->dma_chan) {
        ESP_RETURN_ON_ERROR(spicommon_dma_chan_free(DAC_DMA_PERIPH_SPI_HOST), TAG, "Failed to free dma peripheral channel");
        s_ddp->dma_chan = 0;
    }

    if (s_ddp->periph_dev) {
        spi_ll_disable_intr(s_ddp->periph_dev, SPI_LL_INTR_OUT_DONE | SPI_LL_INTR_OUT_TOTAL_EOF);
        adc_apb_periph_free();
        ESP_RETURN_ON_FALSE(spicommon_periph_free(DAC_DMA_PERIPH_SPI_HOST), ESP_FAIL, TAG, "Failed to release DAC DMA peripheral");
        s_ddp->periph_dev = NULL;
    }

    if (s_ddp->clk_src) {
        ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)s_ddp->clk_src, false), TAG, "disable DAC digital clock source failed");
        s_ddp->clk_src = 0;
    }

    free(s_ddp);
    s_ddp = NULL;
    return ESP_OK;
}

static void s_dac_priv_dma_reset(void)
{
    spi_ll_dma_tx_reset(s_ddp->periph_dev, s_ddp->dma_chan);
    spi_ll_dma_tx_fifo_reset(s_ddp->periph_dev);
}

void dac_priv_dma_enable(void)
{
    s_dac_priv_dma_reset();
    dac_ll_digi_trigger_output(true);
    esp_intr_enable(s_ddp->intr_handle);
}

void dac_priv_dma_disable(void)
{
    s_dac_priv_dma_reset();
    spi_ll_dma_tx_stop(s_ddp->periph_dev, s_ddp->dma_chan);
    dac_ll_digi_trigger_output(false);
    esp_intr_disable(s_ddp->intr_handle);
}

void dac_priv_dma_trans_start(uintptr_t desc_addr)
{
    spi_ll_dma_tx_reset(s_ddp->periph_dev, s_ddp->dma_chan);
    spi_ll_dma_tx_fifo_reset(s_ddp->periph_dev);
    spi_ll_dma_tx_start(s_ddp->periph_dev, s_ddp->dma_chan, (lldesc_t *)desc_addr);
}

void dac_priv_dma_trans_stop(void)
{
    spi_ll_dma_tx_stop(s_ddp->periph_dev, s_ddp->dma_chan);
}

void dac_priv_dma_trans_append(void)
{
    spi_ll_dma_tx_restart(s_ddp->periph_dev, s_ddp->dma_chan);
}
