/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for ADC (esp32s2 specific part)

#include "hal/dac_hal.h"
#include "hal/adc_ll.h"
#include "hal/dac_types.h"

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

#define dac_ll_dma_clear_intr(dev, mask)               spi_ll_clear_intr(dev, mask)
#define dac_ll_dma_enable_intr(dev, mask)              spi_ll_enable_intr(dev, mask)
#define dac_ll_dma_fifo_reset(dev)                     spi_ll_dma_tx_fifo_reset(dev)
#define dac_ll_dma_disable_intr(dev, mask)             spi_ll_disable_intr(dev, mask)
#define dac_ll_dma_get_intr(dev, mask)                 spi_ll_get_intr(dev, mask)
#define dac_ll_dma_disable(dev)                        spi_dma_ll_tx_disable(dev)
#define dac_ll_dma_reset(dev, chan)                    spi_dma_ll_tx_reset(dev, chan)
#define dac_ll_dma_start(dev, chan, desc)              spi_dma_ll_tx_start(dev, chan, desc)

void dac_dma_hal_clr_intr(dac_hal_context_t *hal)
{
    spi_ll_clear_intr(hal->dev, DAC_DMA_HAL_INTR);
}

bool dac_dma_hal_get_intr_status(dac_hal_context_t *hal)
{
    return spi_ll_get_intr(hal->dev, DAC_DMA_HAL_INTR);
}


void dac_dma_hal_init(dac_hal_context_t *hal)
{
    dac_ll_dma_clear_intr(hal->dev, DAC_DMA_HAL_INTR);
    dac_ll_dma_enable_intr(hal->dev, DAC_DMA_HAL_INTR);

}

void dac_dma_hal_deinit(dac_hal_context_t *hal)
{
    dac_ll_digi_trigger_output(false);
    dac_ll_digi_enable_dma(false);
    dac_ll_dma_clear_intr(hal->dev, DAC_DMA_HAL_INTR);
    dac_ll_dma_disable(hal->dev);
}

void dac_dma_hal_trans_start(dac_hal_context_t *hal, lldesc_t *desc)
{
    dac_ll_dma_reset(hal->dev, hal->dma_chan);
    dac_ll_dma_fifo_reset(hal->dev);
    dac_ll_dma_start(hal->dev, hal->dma_chan, desc);
}

void dac_hal_digi_controller_configure(const dac_hal_ctrl_config_t *cfg)
{
    dac_ll_digi_clk_inv(true);
    dac_ll_digi_set_convert_mode(cfg->mode == DAC_CONV_ALTER);
    dac_ll_digi_set_trigger_interval(cfg->interval);
    adc_ll_digi_controller_clk_div(cfg->dig_clk.div_num, cfg->dig_clk.div_b, cfg->dig_clk.div_a);
    adc_ll_digi_clk_sel(cfg->dig_clk.use_apll);
}

void dac_hal_digi_start(void)
{
    dac_ll_digi_enable_dma(true);
    dac_ll_digi_trigger_output(true);
}

void dac_hal_digi_stop(void)
{
    dac_ll_digi_trigger_output(false);
    dac_ll_digi_enable_dma(false);
}

void __attribute__((deprecated)) dac_hal_digi_deinit(void)
{
    dac_ll_digi_trigger_output(false);
    dac_ll_digi_enable_dma(false);
    dac_ll_digi_fifo_reset();
    dac_ll_digi_reset();
}

void __attribute__((deprecated)) dac_hal_digi_controller_config(const dac_digi_config_t *cfg)
{
    dac_ll_digi_set_convert_mode(cfg->mode == DAC_CONV_ALTER);
    dac_ll_digi_set_trigger_interval(cfg->interval);
    adc_ll_digi_controller_clk_div(cfg->dig_clk.div_num, cfg->dig_clk.div_b, cfg->dig_clk.div_a);
    adc_ll_digi_controller_clk_enable(cfg->dig_clk.use_apll);
}

void __attribute__((deprecated)) dac_hal_digi_init(void)
{
    dac_ll_digi_clk_inv(true);
}
