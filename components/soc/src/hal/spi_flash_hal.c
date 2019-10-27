// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <stdlib.h>
#include "hal/spi_flash_hal.h"
#include "string.h"
#include "hal/hal_defs.h"

#define APB_CYCLE_NS   (1000*1000*1000LL/APB_CLK_FREQ)

static const char TAG[] = "FLASH_HAL";

typedef struct {
    int freq;
    spi_flash_ll_clock_reg_t clock_reg_val;
} spi_flash_hal_clock_config_t;


static const spi_flash_hal_clock_config_t spi_flash_clk_cfg_reg[ESP_FLASH_SPEED_MAX] = {
    {5e6,  SPI_FLASH_LL_CLKREG_VAL_5MHZ},
    {10e6, SPI_FLASH_LL_CLKREG_VAL_10MHZ},
    {20e6, SPI_FLASH_LL_CLKREG_VAL_20MHZ},
    {26e6, SPI_FLASH_LL_CLKREG_VAL_26MHZ},
    {40e6, SPI_FLASH_LL_CLKREG_VAL_40MHZ},
    {80e6, SPI_FLASH_LL_CLKREG_VAL_80MHZ},
};

static inline int get_dummy_n(bool gpio_is_used, int input_delay_ns, int eff_clk)
{
    const int apbclk_kHz = APB_CLK_FREQ / 1000;
    //calculate how many apb clocks a period has
    const int apbclk_n = APB_CLK_FREQ / eff_clk;
    const int gpio_delay_ns = gpio_is_used ? (APB_CYCLE_NS * 2) : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns) * apbclk_kHz / 1000 / 1000;
    if (apb_period_n < 0) {
        apb_period_n = 0;
    }

    return apb_period_n / apbclk_n;
}

esp_err_t spi_flash_hal_init(spi_flash_memspi_data_t *data_out, const spi_flash_memspi_config_t *cfg)
{
    if (!esp_ptr_internal(data_out)) {
        return ESP_ERR_INVALID_ARG;
    }
    *data_out = (spi_flash_memspi_data_t) {
        .spi = spi_flash_ll_get_hw(cfg->host_id),
        .cs_num = cfg->cs_num,
        .extra_dummy = get_dummy_n(!cfg->iomux, cfg->input_delay_ns, spi_flash_clk_cfg_reg[cfg->speed].freq),
        .clock_conf = spi_flash_clk_cfg_reg[cfg->speed].clock_reg_val,
    };

    ESP_EARLY_LOGD(TAG, "extra_dummy: %d", data_out->extra_dummy);
    return ESP_OK;
}
