// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The HAL layer for ADC (esp32s2 specific part)

#include "hal/dac_hal.h"
#include "hal/adc_hal.h"
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
    adc_hal_digi_clk_config(&cfg->dig_clk);
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