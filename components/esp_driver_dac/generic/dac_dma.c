/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 *  Generic DAC DMA backend implementation (based on GDMA)
 *
 *  Data path / interrupt source: LP-AHB GDMA TX channel
 *  Clock: DAC functional clock (DAC_HS_CLK)
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include "dac_priv_common.h"
#include "dac_priv_dma.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_private/gdma.h"
#include "freertos/FreeRTOS.h"
#include "hal/dac_ll.h"
#include "soc/clk_tree_defs.h"
#include "soc/dac_reg.h"

typedef struct {
    gdma_channel_handle_t tx_chan;      /* GDMA TX channel */
    dac_dma_event_callbacks_t cbs;      /* Event callbacks */
    void *user_data;                    /* User data for callbacks */
} dac_dma_periph_t;

static dac_dma_periph_t *s_ddp = NULL;

bool dac_priv_dma_gdma_done_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    (void)dma_chan;
    (void)event_data;
    dac_dma_periph_t *ddp = user_data;

    if (ddp && ddp->cbs.on_done) {
        return ddp->cbs.on_done(ddp->user_data);
    }
    return false;
}

bool dac_priv_dma_gdma_teof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    (void)dma_chan;
    (void)event_data;
    dac_dma_periph_t *ddp = user_data;

    if (ddp && ddp->cbs.on_teof) {
        return ddp->cbs.on_teof(ddp->user_data);
    }
    return false;
}

static esp_err_t s_dac_priv_dma_set_clock(soc_periph_dac_digi_clk_src_t clk_src, uint32_t freq_hz, bool is_alternate)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_ERROR(dac_priv_clk_acquire((soc_module_clk_t)clk_src, DAC_LS_CLK_CONSTRAINT_DEFAULT), TAG, "acquire DAC clock failed");
    uint32_t hs_clk_freq_hz = 0;
    ESP_GOTO_ON_ERROR(dac_priv_clk_get_hs_freq_hz(&hs_clk_freq_hz), err_clock, TAG, "get DAC HS clock frequency failed");

    /* Output rate = HS_CLK / max(timer_target, 1). Reject rates outside the timer range. */
    const uint64_t trans_freq_hz = (uint64_t)freq_hz * (is_alternate ? 2 : 1);
    ESP_GOTO_ON_FALSE(trans_freq_hz <= hs_clk_freq_hz && trans_freq_hz * DAC_PDMA_TIMER_TARGET_V >= hs_clk_freq_hz,
                      ESP_ERR_NOT_SUPPORTED, err_clock, TAG, "requested frequency out of supported range");

    /* Compare frequency errors for the floor and ceil divisors, rather than rounding the divisor itself. */
    uint32_t d1 = hs_clk_freq_hz / trans_freq_hz;
    uint32_t d2 = d1 + 1;
    uint64_t e1 = d2 * (hs_clk_freq_hz - trans_freq_hz * d1);
    uint64_t e2 = d1 * (trans_freq_hz * d2 - hs_clk_freq_hz);
    uint32_t timer_target = e1 < e2 ? d1 : d2;

    DAC_ENTER_CRITICAL();
    dac_ll_dma_set_timer_target(timer_target);
    dac_ll_dma_enable_alternate_mode(is_alternate);
    DAC_EXIT_CRITICAL();

    ESP_LOGD(TAG, "DAC HS clock: %"PRIu32" Hz, target frequency: %"PRIu64" Hz, actual frequency: %"PRIu32" Hz",
             hs_clk_freq_hz, trans_freq_hz, hs_clk_freq_hz / timer_target);
    return ESP_OK;

err_clock:
    dac_priv_clk_release(DAC_LS_CLK_CONSTRAINT_DEFAULT);
    return ret;
}

esp_err_t dac_priv_dma_init(soc_periph_dac_digi_clk_src_t clk_src, uint32_t freq_hz, bool is_alternate,
                            const dac_dma_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CLK_SRC(clk_src, SOC_DAC_DIGI_CLKS), ESP_ERR_INVALID_ARG, TAG, "invalid DAC digital clock source");
    DAC_NULL_POINTER_CHECK(cbs);

    esp_err_t ret = ESP_OK;

    s_ddp = heap_caps_calloc(1, sizeof(dac_dma_periph_t), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(s_ddp, ESP_ERR_NO_MEM, TAG, "no memory for DAC DMA object");

    ESP_GOTO_ON_ERROR(s_dac_priv_dma_set_clock(clk_src, freq_hz, is_alternate), err_free, TAG, "failed to set DAC DMA clock");

    gdma_channel_alloc_config_t dma_chan_config = {
#if CONFIG_DAC_ISR_IRAM_SAFE
        .flags.isr_cache_safe = true,
#endif
    };
    ESP_GOTO_ON_ERROR(gdma_new_lp_ahb_channel(&dma_chan_config, &s_ddp->tx_chan, NULL),
                      err_clock, TAG, "failed to allocate GDMA TX channel");
    ESP_GOTO_ON_ERROR(gdma_connect(s_ddp->tx_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_DAC, 0)),
                      err_chan, TAG, "failed to connect GDMA to DAC");

    gdma_transfer_config_t transfer_config = {
        .access_ext_mem = false,    // TODO: IDF-16811
    };
    ESP_GOTO_ON_ERROR(gdma_config_transfer(s_ddp->tx_chan, &transfer_config),
                      err_connect, TAG, "failed to configure GDMA transfer");

    gdma_strategy_config_t strategy_config = {
        .owner_check = false,   // Descriptors are reused in cyclic mode and may also be reused in asynchronous mode.
        .auto_update_desc = true,
        .eof_till_data_popped = true,
    };
    ESP_GOTO_ON_ERROR(gdma_apply_strategy(s_ddp->tx_chan, &strategy_config),
                      err_connect, TAG, "failed to configure GDMA strategy");

    s_ddp->cbs = *cbs;
    s_ddp->user_data = user_data;
    gdma_tx_event_callbacks_t gdma_cbs = {
        .on_trans_done = dac_priv_dma_gdma_done_callback,
        .on_trans_total_eof = dac_priv_dma_gdma_teof_callback,
    };
    ESP_GOTO_ON_ERROR(gdma_register_tx_event_callbacks(s_ddp->tx_chan, &gdma_cbs, s_ddp),
                      err_connect, TAG, "failed to register GDMA callbacks");

    return ESP_OK;

err_connect:
    gdma_disconnect(s_ddp->tx_chan);
err_chan:
    gdma_del_channel(s_ddp->tx_chan);
err_clock:
    dac_priv_clk_release(DAC_LS_CLK_CONSTRAINT_DEFAULT);
err_free:
    free(s_ddp);
    s_ddp = NULL;
    return ret;
}

esp_err_t dac_priv_dma_deinit(void)
{
    if (!s_ddp) {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(gdma_disconnect(s_ddp->tx_chan), TAG, "disconnect GDMA from DAC failed");
    ESP_RETURN_ON_ERROR(gdma_del_channel(s_ddp->tx_chan), TAG, "delete GDMA TX channel failed");

    ESP_RETURN_ON_ERROR(dac_priv_clk_release(DAC_LS_CLK_CONSTRAINT_DEFAULT), TAG, "release DAC clock failed");

    free(s_ddp);
    s_ddp = NULL;
    return ESP_OK;
}

void dac_priv_dma_enable(void)
{
    dac_ll_dma_reset_fifo();
}

void dac_priv_dma_disable(void)
{
    dac_ll_dma_reset_fifo();
}

void dac_priv_dma_trans_start(uintptr_t desc_addr)
{
    gdma_start(s_ddp->tx_chan, (intptr_t)desc_addr);
    /**
     * @note Start GDMA first to fill the FIFO with samples, then enable the DAC's sample fetching and
     *       distribution mechanism to ensure the correct channel order.
     */
    dac_ll_dma_enable_timer(true);
}

void dac_priv_dma_trans_stop(void)
{
    dac_ll_dma_enable_timer(false);
    gdma_stop(s_ddp->tx_chan);
}

void dac_priv_dma_trans_append(void)
{
    gdma_append(s_ddp->tx_chan);
}
