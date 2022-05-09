/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"


#include "hal/i2s_ll.h"
#include "hal/i2s_types.h"
#include "soc/i2s_periph.h"
#include "esp_private/dac_dma.h"
#include "esp_private/i2s_platform.h"

#include "esp_check.h"

#define DAC_DMA_PERIPH_I2S_NUM          0
#define DAC_DMA_PERIPH_I2S_BIT_WIDTH    8

typedef struct {
    void                *periph_dev;    /* DMA peripheral device address */
    intr_handle_t       intr_handle;    /* Interrupt handle */
} dac_dma_periph_i2s_t;

static dac_dma_periph_i2s_t *s_ddp = NULL; // Static DAC DMA peripheral structure pointer

static const char *TAG = "DAC_DMA";

extern portMUX_TYPE dac_spinlock; /* Global DAC spinlock */

static esp_err_t dac_dma_periph_set_clock(uint32_t freq_hz){
    /* Calculate clock coefficients */
    uint32_t bclk = freq_hz * I2S_LL_AD_BCK_FACTOR;
    uint32_t bclk_div = DAC_DMA_PERIPH_I2S_BIT_WIDTH;
    uint32_t mclk = bclk * bclk_div;
    uint32_t sclk = I2S_LL_BASE_CLK; // use PLL clock as default
    uint32_t mclk_div = sclk / mclk;

    /* Check if the configuration is correct */
    // TODO: expand the frequency range
    ESP_RETURN_ON_FALSE(sclk / (float)mclk > 1.99, ESP_ERR_INVALID_ARG, TAG, "Frequency is too large, the mclk division is below minimum value 2");
    ESP_RETURN_ON_FALSE(mclk_div < 256, ESP_ERR_INVALID_ARG, TAG, "Frequency is too small, the mclk division exceed the maximum value 255");

    portENTER_CRITICAL(&dac_spinlock);
    i2s_ll_tx_clk_set_src(s_ddp->periph_dev, I2S_CLK_D2CLK);
    i2s_ll_tx_set_mclk(s_ddp->periph_dev, sclk, mclk, mclk_div);
    i2s_ll_tx_set_bck_div_num(s_ddp->periph_dev, bclk_div);
    portEXIT_CRITICAL(&dac_spinlock);

    return ESP_OK;
}

esp_err_t dac_dma_periph_init(int chan_num, uint32_t freq_hz, bool is_alternate)
{
    esp_err_t ret = ESP_OK;
    /* Acquire DMA peripheral */
    ESP_RETURN_ON_ERROR(i2s_priv_register_object("dac_dma", DAC_DMA_PERIPH_I2S_NUM), TAG, "Failed to acquire DAC DMA peripheral");
    /* Allocate DAC DMA peripheral object */
    s_ddp = (dac_dma_periph_i2s_t *)calloc(1, sizeof(dac_dma_periph_i2s_t));
    ESP_GOTO_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, err, TAG, "No memory for DAC DMA object");
    s_ddp->periph_dev = (void *)I2S_LL_GET_HW(DAC_DMA_PERIPH_I2S_NUM);

    ESP_GOTO_ON_ERROR(dac_dma_periph_set_clock(freq_hz), err, TAG, "Failed to set clock of DMA peripheral");

    portENTER_CRITICAL(&dac_spinlock);
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_set_slave_mod(s_ddp->periph_dev, false);
    i2s_ll_tx_set_sample_bit(s_ddp->periph_dev, DAC_DMA_PERIPH_I2S_BIT_WIDTH, DAC_DMA_PERIPH_I2S_BIT_WIDTH);
    i2s_ll_tx_enable_mono_mode(s_ddp->periph_dev, !is_alternate);
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
    ESP_RETURN_ON_ERROR(i2s_priv_deregister_object(DAC_DMA_PERIPH_I2S_NUM), TAG, "Failed to release DAC DMA peripheral");
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
    /* Regigster interrupt */
    ESP_RETURN_ON_ERROR(esp_intr_alloc(i2s_periph_signal[DAC_DMA_PERIPH_I2S_NUM].irq, ESP_INTR_FLAG_LEVEL1,
                      intr_handler_func, user_ctx, &(s_ddp->intr_handle)),
                      TAG, "Failed to register DAC DMA interrupt");
    portENTER_CRITICAL(&dac_spinlock);
    i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_EOF, true);
    portEXIT_CRITICAL(&dac_spinlock);
    return ESP_OK;
}

esp_err_t dac_dma_periph_deregister_intr(void)
{
    ESP_RETURN_ON_FALSE(s_ddp, ESP_ERR_INVALID_STATE, TAG, "DAC DMA peripheral has not initialized yet");
    if (s_ddp->intr_handle) {
        portENTER_CRITICAL(&dac_spinlock);
        i2s_ll_enable_intr(s_ddp->periph_dev, I2S_LL_EVENT_TX_EOF, false);
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
    // i2s_ll_tx_start_link(s_ddp->periph_dev, (uint32_t)desc_addr);
    // i2s_ll_tx_start(s_ddp->periph_dev);
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
    portEXIT_CRITICAL(&dac_spinlock);
    /* Disable interrupt */
    esp_intr_disable(s_ddp->intr_handle);
}

bool inline dac_dma_periph_intr_is_triggered(void)
{
    uint32_t status = i2s_ll_get_intr_status(s_ddp->periph_dev);
    if (status == 0) {
        //Avoid spurious interrupt
        return false;
    }
    i2s_ll_clear_intr_status(s_ddp->periph_dev, status);
    return (status & I2S_LL_EVENT_TX_EOF) != 0;
}

uint32_t inline dac_dma_periph_intr_get_eof_desc(void)
{
    uint32_t finish_desc;
    i2s_ll_tx_get_eof_des_addr(s_ddp->periph_dev, &finish_desc);
    return finish_desc;
}

void inline dac_dma_periph_dma_trans_start(uint32_t desc_addr)
{
    portENTER_CRITICAL(&dac_spinlock);
    // TODO: check whether need to reset
    i2s_ll_tx_reset(s_ddp->periph_dev);
    i2s_ll_tx_reset_dma(s_ddp->periph_dev);
    i2s_ll_tx_reset_fifo(s_ddp->periph_dev);
    i2s_ll_tx_start_link(s_ddp->periph_dev, desc_addr);
    i2s_ll_tx_start(s_ddp->periph_dev);
    portEXIT_CRITICAL(&dac_spinlock);
}
