/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "esp_private/io_mux.h"
#include "hal/gpio_ll.h"

static portMUX_TYPE s_io_mux_spinlock = portMUX_INITIALIZER_UNLOCKED;
static soc_module_clk_t s_io_mux_clk_src = 0; // by default, the clock source is not set explicitly by any consumer (e.g. SDM, Filter)

esp_err_t io_mux_set_clock_source(soc_module_clk_t clk_src)
{
    bool clk_conflict = false;
    // check is the IO MUX has been set to another clock source
    portENTER_CRITICAL(&s_io_mux_spinlock);
    if (s_io_mux_clk_src != 0 && s_io_mux_clk_src != clk_src) {
        clk_conflict = true;
    } else {
        s_io_mux_clk_src = clk_src;
    }
    portEXIT_CRITICAL(&s_io_mux_spinlock);

    if (clk_conflict) {
        return ESP_ERR_INVALID_STATE;
    }

    gpio_ll_iomux_set_clk_src(clk_src);

    return ESP_OK;
}
