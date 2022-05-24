/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"


#include "hal/adc_ll.h"
#include "hal/i2s_ll.h"
#include "hal/i2s_types.h"
#include "soc/i2s_periph.h"
#include "../dac_dma.h"
#include "esp_private/i2s_platform.h"
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

extern portMUX_TYPE dac_spinlock; /* Global DAC spinlock */

static uint32_t dac_set_apll(uint32_t mclk)
{
    /* Calculate the expected APLL  */
    int div = (int)((SOC_APLL_MIN_HZ / mclk) + 1);
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
        ESP_LOGW(TAG, "APLL is occupied already, it is working at %d Hz", real_freq);
    }
    ESP_LOGD(TAG, "APLL expected frequency is %d Hz, real frequency is %d Hz", expt_freq, real_freq);
    return real_freq;
}

static esp_err_t dac_dma_periph_set_clock(uint32_t freq_hz, bool is_apll)
{
    /* Calculate clock coefficients */
    uint32_t bclk = freq_hz * I2S_LL_AD_BCK_FACTOR;
    uint32_t bclk_div = DAC_DMA_PERIPH_I2S_BIT_WIDTH;
    uint32_t mclk = bclk * bclk_div;
    uint32_t sclk; // use 160M PLL clock as default, minimun support freq: 19.6 KHz maximun support freq: 2.5 MHz
    if (is_apll) {
        sclk = dac_set_apll(mclk);
        ESP_RETURN_ON_FALSE(sclk, ESP_ERR_INVALID_ARG, TAG, "set APLL coefficients failed");
    } else {
        sclk = I2S_LL_BASE_CLK;
    }
    uint32_t mclk_div = sclk / mclk;

    /* Check if the configuration is correct */
    ESP_RETURN_ON_FALSE(sclk / (float)mclk > 1.99, ESP_ERR_INVALID_ARG, TAG, "Frequency is too large, the mclk division is below minimum value 2");
    ESP_RETURN_ON_FALSE(mclk_div < 256, ESP_ERR_INVALID_ARG, TAG, "Frequency is too small, the mclk division exceed the maximum value 255");
    ESP_LOGD(TAG, "[sclk] %d [mclk] %d [mclk_div] %d [bclk] %d [bclk_div] %d", sclk, mclk, mclk_div, bclk, bclk_div);

    portENTER_CRITICAL(&dac_spinlock);
    i2s_ll_tx_clk_set_src(s_ddp->periph_dev, is_apll ? I2S_CLK_SRC_APLL : I2S_CLK_SRC_DEFAULT);
    i2s_ll_tx_set_mclk(s_ddp->periph_dev, sclk, mclk, mclk_div);
    i2s_ll_tx_set_bck_div_num(s_ddp->periph_dev, bclk_div);
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
    ESP_RETURN_ON_ERROR(i2s_platform_acquire_occupation(DAC_DMA_PERIPH_I2S_NUM, "dac_dma"), TAG, "Failed to acquire DAC DMA peripheral");
    /* Allocate DAC DMA peripheral object */
    s_ddp = (dac_dma_periph_i2s_t *)calloc(1, sizeof(dac_dma_periph_i2s_t));
    ESP_GOTO_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, err, TAG, "No memory for DAC DMA object");
    s_ddp->periph_dev = (void *)I2S_LL_GET_HW(DAC_DMA_PERIPH_I2S_NUM);

    if (is_apll) {
        periph_rtc_apll_acquire();
        s_ddp->use_apll = true;
    }
    ESP_GOTO_ON_ERROR(dac_dma_periph_set_clock(freq_hz, is_apll), err, TAG, "Failed to set clock of DMA peripheral");

    portENTER_CRITICAL(&dac_spinlock);
    i2s_ll_enable_builtin_dac(s_ddp->periph_dev, true);
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_set_slave_mod(s_ddp->periph_dev, false);
    i2s_ll_tx_set_sample_bit(s_ddp->periph_dev, DAC_DMA_PERIPH_I2S_BIT_WIDTH, DAC_DMA_PERIPH_I2S_BIT_WIDTH);
    i2s_ll_tx_enable_mono_mode(s_ddp->periph_dev, !is_alternate);
    i2s_ll_tx_select_std_slot(s_ddp->periph_dev, 0x03, !is_alternate);
    i2s_ll_tx_enable_msb_shift(s_ddp->periph_dev, false);
    i2s_ll_tx_set_ws_width(s_ddp->periph_dev, DAC_DMA_PERIPH_I2S_BIT_WIDTH);
    i2s_ll_tx_enable_msb_right(s_ddp->periph_dev, false);
    i2s_ll_tx_enable_right_first(s_ddp->periph_dev, true);
    /* Should always enable fifo */
    i2s_ll_tx_force_enable_fifo_mod(s_ddp->periph_dev, true);
    portEXIT_CRITICAL(&dac_spinlock);

    return ret;
err:
    dac_dma_periph_deinit();
    return ret;
}

esp_err_t dac_dma_periph_deinit(void)
{
    ESP_RETURN_ON_ERROR(i2s_platform_release_occupation(DAC_DMA_PERIPH_I2S_NUM), TAG, "Failed to release DAC DMA peripheral");
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
    /* Regigster interrupt */
    ESP_RETURN_ON_ERROR(esp_intr_alloc(i2s_periph_signal[DAC_DMA_PERIPH_I2S_NUM].irq, ESP_INTR_FLAG_LEVEL1,
                      intr_handler_func, user_ctx, &(s_ddp->intr_handle)),
                      TAG, "Failed to register DAC DMA interrupt");
    portENTER_CRITICAL(&dac_spinlock);
    i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_EOF | I2S_LL_EVENT_TX_TEOF, true);
    portEXIT_CRITICAL(&dac_spinlock);
    return ESP_OK;
}

esp_err_t dac_dma_periph_deregister_intr(void)
{
    ESP_RETURN_ON_FALSE(s_ddp, ESP_ERR_INVALID_STATE, TAG, "DAC DMA peripheral has not initialized yet");
    if (s_ddp->intr_handle) {
        portENTER_CRITICAL(&dac_spinlock);
        i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_EOF | I2S_LL_EVENT_TX_TEOF, false);
        portEXIT_CRITICAL(&dac_spinlock);
        esp_intr_free(s_ddp->intr_handle);
        s_ddp->intr_handle = NULL;
    }
    return ESP_OK;
}

void dac_dma_periph_enable(void)
{
    portENTER_CRITICAL(&dac_spinlock);
    /* Reset */
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_reset_dma(s_ddp->periph_dev);
    i2s_ll_tx_reset_fifo(s_ddp->periph_dev);
    /* Start */
    i2s_ll_enable_dma(s_ddp->periph_dev,true);
    i2s_ll_tx_enable_intr(s_ddp->periph_dev);
    i2s_ll_tx_start(s_ddp->periph_dev);
    i2s_ll_dma_enable_eof_on_fifo_empty(s_ddp->periph_dev, true);
    i2s_ll_dma_enable_auto_write_back(s_ddp->periph_dev, true);
    portEXIT_CRITICAL(&dac_spinlock);
    /* Enable interrupt */
    esp_intr_enable(s_ddp->intr_handle);
}

void dac_dma_periph_disable(void)
{
    portENTER_CRITICAL(&dac_spinlock);
    /* Reset */
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_reset_dma(s_ddp->periph_dev);
    i2s_ll_tx_reset_fifo(s_ddp->periph_dev);
    /* Stop */
    i2s_ll_tx_stop(s_ddp->periph_dev);
    i2s_ll_tx_stop_link(s_ddp->periph_dev);
    i2s_ll_tx_disable_intr(s_ddp->periph_dev);
    i2s_ll_enable_dma(s_ddp->periph_dev, false);
    i2s_ll_dma_enable_eof_on_fifo_empty(s_ddp->periph_dev, false);
    i2s_ll_dma_enable_auto_write_back(s_ddp->periph_dev, false);
    portEXIT_CRITICAL(&dac_spinlock);
    /* Disable interrupt */
    esp_intr_disable(s_ddp->intr_handle);
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

void inline dac_dma_periph_dma_trans_start(uint32_t desc_addr)
{
    portENTER_CRITICAL(&dac_spinlock);
    i2s_ll_tx_start_link(s_ddp->periph_dev, desc_addr);
    portEXIT_CRITICAL(&dac_spinlock);
}
