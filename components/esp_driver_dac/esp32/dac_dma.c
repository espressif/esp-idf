/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
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

#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "hal/adc_ll.h"
#include "hal/i2s_hal.h"
#include "hal/i2s_types.h"
#include "hal/clk_tree_ll.h"
#include "soc/i2s_periph.h"
#include "../dac_priv_dma.h"
#include "esp_private/i2s_platform.h"
#include "esp_private/esp_clk.h"
#include "clk_ctrl_os.h"
#if CONFIG_DAC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_check.h"
#include "esp_attr.h"

#define DAC_DMA_PERIPH_I2S_NUM          0
#define DAC_DMA_PERIPH_I2S_BIT_WIDTH    16      // Fixed bit width, only the high 8 bits take effect

typedef struct {
    void                *periph_dev;    /* DMA peripheral device address */
    intr_handle_t       intr_handle;    /* Interrupt handle */
    bool                use_apll;       /* Whether use APLL as clock source */
} dac_dma_periph_i2s_t;

static dac_dma_periph_i2s_t *s_ddp = NULL; // Static DAC DMA peripheral structure pointer

static const char *TAG = "DAC_DMA";

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
    esp_err_t ret = periph_rtc_apll_freq_set(expt_freq, &real_freq);
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
 * @note  DAC frequency is decided by I2S WS frequency, the clock source of I2S is D2PLL or APLL on ESP32
 *        freq_hz = ws = bclk / I2S_LL_AD_BCK_FACTOR
 * @param freq_hz    DAC byte transmit frequency
 * @return
 *      - ESP_OK    config success
 *      - ESP_ERR_INVALID_ARG   invalid frequency
 */
static esp_err_t s_dac_dma_periph_set_clock(uint32_t freq_hz, bool is_apll)
{
    /* Calculate clock coefficients */
    uint32_t bclk = freq_hz * I2S_LL_AD_BCK_FACTOR;
    uint32_t bclk_div = DAC_DMA_PERIPH_I2S_BIT_WIDTH;
    uint32_t mclk = bclk * bclk_div;
    uint32_t sclk; // use 160M PLL clock as default, minimum support freq: 19.6 KHz maximum support freq: 2.5 MHz
    if (is_apll) {
        sclk = s_dac_set_apll_freq(mclk);
        ESP_RETURN_ON_FALSE(sclk, ESP_ERR_INVALID_ARG, TAG, "set APLL coefficients failed");
    } else {
        // [clk_tree] TODO: replace the following clock by clk_tree API
        sclk = esp_clk_apb_freq() * 2; // D2PLL
    }
    uint32_t mclk_div = sclk / mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(sclk / (float)mclk > 1.99, ESP_ERR_INVALID_ARG, TAG, "Frequency is too large, the mclk division is below minimum value 2");
    ESP_RETURN_ON_FALSE(mclk_div < 256, ESP_ERR_INVALID_ARG, TAG, "Frequency is too small, the mclk division exceed the maximum value 255");
    ESP_LOGD(TAG, "[sclk] %"PRIu32" [mclk] %"PRIu32" [mclk_div] %"PRIu32" [bclk] %"PRIu32" [bclk_div] %"PRIu32, sclk, mclk, mclk_div, bclk, bclk_div);

    i2s_ll_tx_clk_set_src(s_ddp->periph_dev, is_apll ? I2S_CLK_SRC_APLL : I2S_CLK_SRC_DEFAULT);
    hal_utils_clk_div_t mclk_div_coeff = {};
    i2s_hal_calc_mclk_precise_division(sclk, mclk, &mclk_div_coeff);
    i2s_ll_tx_set_mclk(s_ddp->periph_dev, &mclk_div_coeff);
    i2s_ll_tx_set_bck_div_num(s_ddp->periph_dev, bclk_div);

    return ESP_OK;
}

esp_err_t dac_dma_periph_init(uint32_t freq_hz, bool is_alternate, bool is_apll)
{
#if CONFIG_DAC_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    /* Acquire DMA peripheral */
    ESP_RETURN_ON_ERROR(i2s_platform_acquire_occupation(I2S_CTLR_HP, DAC_DMA_PERIPH_I2S_NUM, "dac_dma"), TAG, "Failed to acquire DAC DMA peripheral");
    /* Allocate DAC DMA peripheral object */
    s_ddp = (dac_dma_periph_i2s_t *)heap_caps_calloc(1, sizeof(dac_dma_periph_i2s_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, err, TAG, "No memory for DAC DMA object");
    s_ddp->periph_dev = (void *)I2S_LL_GET_HW(DAC_DMA_PERIPH_I2S_NUM);

    if (is_apll) {
        periph_rtc_apll_acquire();
        s_ddp->use_apll = true;
    }
    ESP_GOTO_ON_ERROR(s_dac_dma_periph_set_clock(freq_hz, is_apll), err, TAG, "Failed to set clock of DMA peripheral");

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
    /* Enable the interrupts */
    i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_EOF | I2S_LL_EVENT_TX_TEOF, true);

    return ret;
err:
    dac_dma_periph_deinit();
    return ret;
}

esp_err_t dac_dma_periph_deinit(void)
{
    if (!s_ddp) {
        return ESP_OK;
    }

    ESP_RETURN_ON_FALSE(s_ddp->intr_handle == NULL, ESP_ERR_INVALID_STATE, TAG, "The interrupt is not deregistered yet");
    ESP_RETURN_ON_ERROR(i2s_platform_release_occupation(I2S_CTLR_HP, DAC_DMA_PERIPH_I2S_NUM), TAG, "Failed to release DAC DMA peripheral");
    i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_EOF | I2S_LL_EVENT_TX_TEOF, false);
    if (s_ddp->use_apll) {
        periph_rtc_apll_release();
        s_ddp->use_apll = false;
    }
    free(s_ddp);
    s_ddp = NULL;

    return ESP_OK;
}

int dac_dma_periph_get_intr_signal(void)
{
    return i2s_periph_signal[DAC_DMA_PERIPH_I2S_NUM].irq;
}

static void s_dac_dma_periph_reset(void)
{
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_reset_dma(s_ddp->periph_dev);
    i2s_ll_tx_reset_fifo(s_ddp->periph_dev);
}

static void s_dac_dma_periph_start(void)
{
    i2s_ll_enable_dma(s_ddp->periph_dev, true);
    i2s_ll_tx_enable_intr(s_ddp->periph_dev);
    i2s_ll_tx_start(s_ddp->periph_dev);
    i2s_ll_dma_enable_eof_on_fifo_empty(s_ddp->periph_dev, true);
    i2s_ll_dma_enable_auto_write_back(s_ddp->periph_dev, true);
}

static void s_dac_dma_periph_stop(void)
{
    i2s_ll_tx_stop(s_ddp->periph_dev);
    i2s_ll_tx_stop_link(s_ddp->periph_dev);
    i2s_ll_tx_disable_intr(s_ddp->periph_dev);
    i2s_ll_enable_dma(s_ddp->periph_dev, false);
    i2s_ll_dma_enable_eof_on_fifo_empty(s_ddp->periph_dev, false);
    i2s_ll_dma_enable_auto_write_back(s_ddp->periph_dev, false);
}

void dac_dma_periph_enable(void)
{
    /* Reset */
    s_dac_dma_periph_reset();
    /* Start */
    s_dac_dma_periph_start();
}

void dac_dma_periph_disable(void)
{
    /* Reset */
    s_dac_dma_periph_reset();
    /* Stop */
    s_dac_dma_periph_stop();
}

uint32_t IRAM_ATTR dac_dma_periph_intr_is_triggered(void)
{
    uint32_t status = i2s_ll_get_intr_status(s_ddp->periph_dev);
    if (status == 0) {
        //Avoid spurious interrupt
        return false;
    }
    i2s_ll_clear_intr_status(s_ddp->periph_dev, status);
    uint32_t ret = 0;
    ret |= (status & I2S_LL_EVENT_TX_EOF) ? DAC_DMA_EOF_INTR : 0;
    ret |= (status & I2S_LL_EVENT_TX_TEOF) ? DAC_DMA_TEOF_INTR : 0;
    return ret;
}

uint32_t IRAM_ATTR dac_dma_periph_intr_get_eof_desc(void)
{
    uint32_t finish_desc;
    i2s_ll_tx_get_eof_des_addr(s_ddp->periph_dev, &finish_desc);
    return finish_desc;
}

void dac_dma_periph_dma_trans_start(uint32_t desc_addr)
{
    i2s_ll_tx_start_link(s_ddp->periph_dev, desc_addr);
}
