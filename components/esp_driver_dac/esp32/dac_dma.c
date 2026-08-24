/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 *  Target-specific DAC DMA backend implementation
 *  Target: ESP32
 *  DAC DMA peripheral (data source): I2S0 (i.e. use I2S DMA to transmit data)
 *  DAC DMA interrupt source: I2S0
 *  DAC digital controller clock source: I2S ws signal (root clock: PLL_F160M or APLL)
 */

#include "dac_priv_common.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "hal/adc_ll.h"
#include "hal/i2s_hal.h"
#include "hal/i2s_types.h"
#include "hal/clk_tree_ll.h"
#include "hal/i2s_periph.h"
#include "dac_priv_dma.h"
#include "esp_private/i2s_platform.h"
#include "esp_clk_tree.h"
#include "esp_log.h"
#include "esp_check.h"

#define DAC_DMA_PERIPH_I2S_NUM          0
#define DAC_DMA_PERIPH_I2S_BIT_WIDTH    16      // Fixed bit width, only the high 8 bits take effect

#if CONFIG_DAC_ISR_IRAM_SAFE
#define DAC_DMA_INTR_ALLOC_FLAGS        (ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)
#else
#define DAC_DMA_INTR_ALLOC_FLAGS        (ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_INTRDISABLED)
#endif

typedef struct {
    void                *periph_dev;    /* DMA peripheral device address */
    intr_handle_t       intr_handle;    /* Interrupt handle */
    soc_periph_dac_digi_clk_src_t clk_src;  /* Acquired clock source; 0 means not enabled yet */
    dac_dma_event_callbacks_t cbs;      /* Event callbacks */
    void                *ctx;           /* Driver context for callbacks */
} dac_dma_periph_i2s_t;

static dac_dma_periph_i2s_t *s_ddp = NULL; // Static DAC DMA peripheral structure pointer

void dac_priv_dma_intr_handler(void *arg)
{
    dac_dma_periph_i2s_t *ddp = arg;
    bool need_yield = false;

    uint32_t status = i2s_ll_get_intr_status(ddp->periph_dev);
    if (status == 0) {
        // Avoid spurious interrupt
        return;
    }
    i2s_ll_clear_intr_status(ddp->periph_dev, status);

    if ((status & I2S_LL_EVENT_TX_DONE) && ddp->cbs.on_done) {
        need_yield |= ddp->cbs.on_done(ddp->ctx);
    }
    if ((status & I2S_LL_EVENT_TX_TEOF) && ddp->cbs.on_teof) {
        need_yield |= ddp->cbs.on_teof(ddp->ctx);
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static uint32_t s_dac_set_apll_freq(uint32_t mclk)
{
    /* Calculate the expected APLL  */
    int div = (int)((CLK_LL_APLL_MIN_HZ / mclk) + 1);
    /* apll_freq = mclk * div
     * when div = 1, hardware will still divide 2
     * when div = 0, hardware will divide 255
     * So the div here should be at least 2 */
    div = div < 2 ? 2 : div;
    uint32_t expt_freq = mclk * div;
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
 * @note  DAC frequency is decided by I2S WS frequency, the clock source of I2S is PLL_F160M or APLL on ESP32
 *        freq_hz = ws = bclk / I2S_LL_AD_BCK_FACTOR
 * @param clk_src    DAC digital controller clock source
 * @param freq_hz    DAC byte transmit frequency
 * @return
 *      - ESP_OK    config success
 *      - ESP_ERR_INVALID_ARG   invalid frequency
 */
static esp_err_t s_dac_priv_dma_set_clock(soc_periph_dac_digi_clk_src_t clk_src, uint32_t freq_hz)
{
    /* Calculate clock coefficients */
    uint32_t bclk = freq_hz * I2S_LL_AD_BCK_FACTOR;
    uint32_t bclk_div = DAC_DMA_PERIPH_I2S_BIT_WIDTH;
    uint32_t mclk = bclk * bclk_div;
    uint32_t sclk; // use 160M PLL clock as default, minimum support freq: 19.6 KHz maximum support freq: 2.5 MHz
    if (clk_src == DAC_DIGI_CLK_SRC_APLL) {
        sclk = s_dac_set_apll_freq(mclk);
        ESP_RETURN_ON_FALSE(sclk, ESP_ERR_INVALID_ARG, TAG, "set APLL coefficients failed");
    } else {
        ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sclk), TAG, "get clock source frequency failed");
    }
    uint32_t mclk_div = sclk / mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(sclk / (float)mclk > 1.99, ESP_ERR_INVALID_ARG, TAG, "Frequency is too large, the mclk division is below minimum value 2");
    ESP_RETURN_ON_FALSE(mclk_div < 256, ESP_ERR_INVALID_ARG, TAG, "Frequency is too small, the mclk division exceed the maximum value 255");
    ESP_LOGD(TAG, "[sclk] %"PRIu32" [mclk] %"PRIu32" [mclk_div] %"PRIu32" [bclk] %"PRIu32" [bclk_div] %"PRIu32, sclk, mclk, mclk_div, bclk, bclk_div);

    i2s_ll_tx_clk_set_src(s_ddp->periph_dev, (i2s_clock_src_t)clk_src);
    hal_utils_clk_div_t mclk_div_coeff = {};
    i2s_hal_calc_mclk_precise_division(sclk, mclk, &mclk_div_coeff);
    i2s_ll_tx_set_mclk(s_ddp->periph_dev, &mclk_div_coeff);
    i2s_ll_tx_set_bck_div_num(s_ddp->periph_dev, bclk_div);

    return ESP_OK;
}

esp_err_t dac_priv_dma_init(soc_periph_dac_digi_clk_src_t clk_src, uint32_t freq_hz, bool is_alternate,
                            const dac_dma_event_callbacks_t *cbs, void *ctx)
{
    ESP_RETURN_ON_FALSE(clk_src == DAC_DIGI_CLK_SRC_PLL_160M || clk_src == DAC_DIGI_CLK_SRC_APLL, ESP_ERR_INVALID_ARG, TAG, "invalid DAC digital clock source");
    DAC_NULL_POINTER_CHECK(cbs);

    esp_err_t ret = ESP_OK;

    /* Allocate DAC DMA peripheral object */
    s_ddp = (dac_dma_periph_i2s_t *)heap_caps_calloc(1, sizeof(dac_dma_periph_i2s_t), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, TAG, "No memory for DAC DMA object");

    /* Acquire DMA peripheral */
    ESP_GOTO_ON_ERROR(i2s_platform_acquire_occupation(I2S_CTLR_HP, DAC_DMA_PERIPH_I2S_NUM, "dac_dma"), err, TAG, "Failed to acquire DAC DMA peripheral");
    s_ddp->periph_dev = (void *)I2S_LL_GET_HW(DAC_DMA_PERIPH_I2S_NUM);

    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)clk_src, true), err, TAG, "enable DAC digital clock source failed");
    s_ddp->clk_src = clk_src;
    ESP_GOTO_ON_ERROR(s_dac_priv_dma_set_clock(clk_src, freq_hz), err, TAG, "Failed to set clock of DMA peripheral");

    i2s_ll_enable_builtin_adc_dac(s_ddp->periph_dev, true);
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_set_slave_mod(s_ddp->periph_dev, false);
    i2s_ll_tx_set_sample_bit(s_ddp->periph_dev, DAC_DMA_PERIPH_I2S_BIT_WIDTH, DAC_DMA_PERIPH_I2S_BIT_WIDTH);
    i2s_ll_tx_enable_mono_mode(s_ddp->periph_dev, !is_alternate);
    i2s_ll_tx_select_std_slot(s_ddp->periph_dev, I2S_STD_SLOT_BOTH, !is_alternate);
    i2s_ll_tx_enable_msb_shift(s_ddp->periph_dev, false);
    i2s_ll_tx_set_ws_width(s_ddp->periph_dev, DAC_DMA_PERIPH_I2S_BIT_WIDTH);
    i2s_ll_tx_enable_msb_right(s_ddp->periph_dev, false);
    i2s_ll_tx_enable_right_first(s_ddp->periph_dev, true);
    /* Should always enable fifo */
    i2s_ll_tx_force_enable_fifo_mod(s_ddp->periph_dev, true);
    i2s_ll_dma_enable_auto_write_back(s_ddp->periph_dev, true);

    s_ddp->cbs = *cbs;
    s_ddp->ctx = ctx;
    ESP_GOTO_ON_ERROR(esp_intr_alloc(i2s_periph_signal[DAC_DMA_PERIPH_I2S_NUM].irq, DAC_DMA_INTR_ALLOC_FLAGS, dac_priv_dma_intr_handler, s_ddp, &s_ddp->intr_handle),
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

    if (s_ddp->clk_src) {
        ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)s_ddp->clk_src, false), TAG, "disable DAC digital clock source failed");
        s_ddp->clk_src = 0;
    }

    if (s_ddp->periph_dev) {
        ESP_RETURN_ON_ERROR(i2s_platform_release_occupation(I2S_CTLR_HP, DAC_DMA_PERIPH_I2S_NUM), TAG, "Failed to release DAC DMA peripheral");
        s_ddp->periph_dev = NULL;
    }

    free(s_ddp);
    s_ddp = NULL;

    return ESP_OK;
}

static void s_dac_priv_dma_reset(void)
{
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_reset_dma(s_ddp->periph_dev);
    i2s_ll_tx_reset_fifo(s_ddp->periph_dev);
}

static void s_dac_priv_dma_start(void)
{
    i2s_ll_enable_dma(s_ddp->periph_dev, true);
    i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_DONE | I2S_LL_EVENT_TX_TEOF, true);
    i2s_ll_tx_start(s_ddp->periph_dev);
    i2s_ll_dma_enable_eof_on_fifo_empty(s_ddp->periph_dev, true);
    i2s_ll_dma_enable_auto_write_back(s_ddp->periph_dev, true);
}

static void s_dac_priv_dma_stop(void)
{
    i2s_ll_tx_stop(s_ddp->periph_dev);
    i2s_ll_tx_stop_link(s_ddp->periph_dev);
    i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_DONE | I2S_LL_EVENT_TX_TEOF, false);
    i2s_ll_enable_dma(s_ddp->periph_dev, false);
    i2s_ll_dma_enable_eof_on_fifo_empty(s_ddp->periph_dev, false);
    i2s_ll_dma_enable_auto_write_back(s_ddp->periph_dev, false);
}

void dac_priv_dma_enable(void)
{
    /* Reset */
    s_dac_priv_dma_reset();
    /* Start */
    s_dac_priv_dma_start();
    esp_intr_enable(s_ddp->intr_handle);
}

void dac_priv_dma_disable(void)
{
    /* Reset */
    s_dac_priv_dma_reset();
    /* Stop */
    s_dac_priv_dma_stop();
    esp_intr_disable(s_ddp->intr_handle);
}

void dac_priv_dma_trans_start(uintptr_t desc_addr)
{
    i2s_ll_tx_start_link(s_ddp->periph_dev, desc_addr);
}

void dac_priv_dma_trans_stop(void)
{
    i2s_ll_tx_stop_link(s_ddp->periph_dev);
}
