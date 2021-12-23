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

void dac_hal_digi_init(void)
{
    dac_ll_digi_clk_inv(true);
}

void dac_hal_digi_deinit(void)
{
    dac_ll_digi_trigger_output(false);
    dac_ll_digi_enable_dma(false);
    dac_ll_digi_fifo_reset();
    dac_ll_digi_reset();
}

void dac_hal_digi_controller_config(const dac_digi_config_t *cfg)
{
    dac_ll_digi_set_convert_mode(cfg->mode);
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
